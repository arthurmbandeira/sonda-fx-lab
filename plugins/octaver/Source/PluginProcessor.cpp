#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
    constexpr auto kOctave = "octave";
    constexpr auto kDry    = "dry";
    constexpr auto kTone   = "tone";
    constexpr auto kLevel  = "level";

    constexpr float trackCutoffHz = 500.0f; // only the fundamental should drive the tracker
    constexpr float subCutoffHz   = 250.0f;
    constexpr float gateLevel     = 0.002f; // below this the sub is silent
}

OctaverProcessor::OctaverProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", createLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout OctaverProcessor::createLayout()
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout l;
    l.add (std::make_unique<AudioParameterFloat> (ParameterID { kOctave, 1 }, "Octave",
                NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.8f));
    l.add (std::make_unique<AudioParameterFloat> (ParameterID { kDry, 1 }, "Dry",
                NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.6f));
    l.add (std::make_unique<AudioParameterFloat> (ParameterID { kTone, 1 }, "Tone",
                NormalisableRange<float> (400.0f, 8000.0f, 1.0f, 0.4f), 2500.0f, AudioParameterFloatAttributes().withLabel ("Hz")));
    l.add (std::make_unique<AudioParameterFloat> (ParameterID { kLevel, 1 }, "Level",
                NormalisableRange<float> (-24.0f, 6.0f, 0.1f), 0.0f, AudioParameterFloatAttributes().withLabel ("dB")));
    return l;
}

void OctaverProcessor::prepareToPlay (double sampleRate, int)
{
    sr = sampleRate;

    trackLowPass.coefficients = Coeffs::makeLowPass (sampleRate, trackCutoffHz);
    subLowPass1.coefficients  = Coeffs::makeLowPass (sampleRate, subCutoffHz);
    subLowPass2.coefficients  = Coeffs::makeLowPass (sampleRate, subCutoffHz);
    for (auto& f : toneLowPass)
        f.coefficients = Coeffs::makeLowPass (sampleRate, 2500.0f);
    lastToneHz = -1.0f;

    for (auto* f : { &trackLowPass, &subLowPass1, &subLowPass2, &toneLowPass[0], &toneLowPass[1] })
        f->reset();

    schmittHigh = false;
    flipFlop = false;
    envelope = 0.0f;
    envAttack  = 1.0f - std::exp (-1.0f / (0.002f * (float) sampleRate));
    envRelease = 1.0f - std::exp (-1.0f / (0.060f * (float) sampleRate));

    for (auto* s : { &octaveGain, &dryGain, &outGain })
        s->reset (sampleRate, 0.02);
    octaveGain.setCurrentAndTargetValue (apvts.getRawParameterValue (kOctave)->load());
    dryGain.setCurrentAndTargetValue    (apvts.getRawParameterValue (kDry)->load());
    outGain.setCurrentAndTargetValue    (juce::Decibels::decibelsToGain (apvts.getRawParameterValue (kLevel)->load()));
}

bool OctaverProcessor::isBusesLayoutSupported (const BusesLayout& l) const
{
    const auto out = l.getMainOutputChannelSet();
    return (out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo())
           && out == l.getMainInputChannelSet();
}

void OctaverProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int numCh = juce::jmin (buffer.getNumChannels(), 2);
    const int n = buffer.getNumSamples();

    octaveGain.setTargetValue (apvts.getRawParameterValue (kOctave)->load());
    dryGain.setTargetValue    (apvts.getRawParameterValue (kDry)->load());
    outGain.setTargetValue    (juce::Decibels::decibelsToGain (apvts.getRawParameterValue (kLevel)->load()));

    const float toneHz = apvts.getRawParameterValue (kTone)->load();
    if (! juce::approximatelyEqual (toneHz, lastToneHz))
    {
        for (auto& f : toneLowPass)
            f.coefficients = Coeffs::makeLowPass (sr, toneHz);
        lastToneHz = toneHz;
    }

    for (int i = 0; i < n; ++i)
    {
        float mono = 0.0f;
        for (int ch = 0; ch < numCh; ++ch)
            mono += buffer.getSample (ch, i);
        mono /= (float) numCh;

        const float tracked = trackLowPass.processSample (mono);

        const float a = std::abs (tracked);
        envelope += (a > envelope ? envAttack : envRelease) * (a - envelope);

        // hysteresis scales with the signal so noise does not retrigger the flip-flop
        const float threshold = juce::jmax (0.001f, 0.15f * envelope);
        if (! schmittHigh && tracked > threshold)
        {
            schmittHigh = true;
            flipFlop = ! flipFlop; // one toggle per input cycle = half the frequency
        }
        else if (schmittHigh && tracked < -threshold)
        {
            schmittHigh = false;
        }

        float sub = 0.0f;
        if (envelope > gateLevel)
            sub = (flipFlop ? 1.0f : -1.0f) * envelope * 0.7f;
        sub = subLowPass2.processSample (subLowPass1.processSample (sub));

        const float oct = octaveGain.getNextValue();
        const float dry = dryGain.getNextValue();
        const float out = outGain.getNextValue();

        for (int ch = 0; ch < numCh; ++ch)
        {
            const float mixed = buffer.getSample (ch, i) * dry + sub * oct;
            buffer.setSample (ch, i, toneLowPass[ch].processSample (mixed) * out);
        }
    }
}

juce::AudioProcessorEditor* OctaverProcessor::createEditor()
{
    return new OctaverEditor (*this);
}

void OctaverProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, dest);
}

void OctaverProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OctaverProcessor();
}
