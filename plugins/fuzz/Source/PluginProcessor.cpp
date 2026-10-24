#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
    constexpr auto kDrive = "drive";
    constexpr auto kTone  = "tone";
    constexpr auto kLevel = "level";
    constexpr auto kMix   = "mix";
    constexpr auto kMode  = "mode";
    constexpr float bassCrossoverHz = 200.0f;
}

FuzzProcessor::FuzzProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMS", createLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout FuzzProcessor::createLayout()
{
    using namespace juce;
    AudioProcessorValueTreeState::ParameterLayout l;
    l.add (std::make_unique<AudioParameterFloat> (ParameterID { kDrive, 1 }, "Drive",
                NormalisableRange<float> (0.0f, 40.0f, 0.1f), 20.0f, AudioParameterFloatAttributes().withLabel ("dB")));
    l.add (std::make_unique<AudioParameterFloat> (ParameterID { kTone, 1 }, "Tone",
                NormalisableRange<float> (500.0f, 12000.0f, 1.0f, 0.4f), 4000.0f, AudioParameterFloatAttributes().withLabel ("Hz")));
    l.add (std::make_unique<AudioParameterFloat> (ParameterID { kLevel, 1 }, "Level",
                NormalisableRange<float> (-24.0f, 6.0f, 0.1f), -12.0f, AudioParameterFloatAttributes().withLabel ("dB")));
    l.add (std::make_unique<AudioParameterChoice> (ParameterID { kMode, 1 }, "Mode",
                StringArray { "Guitar", "Bass" }, 0));
    l.add (std::make_unique<AudioParameterFloat> (ParameterID { kMix, 1 }, "Mix",
                NormalisableRange<float> (0.0f, 1.0f, 0.01f), 1.0f));
    return l;
}

void FuzzProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sr = sampleRate;
    const juce::dsp::ProcessSpec spec { sampleRate, (juce::uint32) samplesPerBlock, 2 };

    oversampling.initProcessing ((size_t) samplesPerBlock);
    oversampling.reset();
    const int latency = (int) std::round (oversampling.getLatencyInSamples());
    setLatencySamples (latency);

    dryDelay.prepare (spec);
    dryDelay.setDelay ((float) latency);
    lowDelay.prepare (spec);
    lowDelay.setDelay ((float) latency);

    crossover.prepare (spec);
    crossover.setCutoffFrequency (bassCrossoverHz);
    crossover.reset();
    lowBuffer.setSize (2, samplesPerBlock);

    inputHighPass.prepare (spec);
    // *state = *new copies the values into the shared object the per-channel filters already use
    *inputHighPass.state = *Coeffs::makeHighPass (sampleRate, 80.0f);
    toneLowPass.prepare (spec);

    dryBuffer.setSize (2, samplesPerBlock);

    drive.reset (sampleRate * 4.0, 0.02); // advanced at the oversampled rate
    level.reset (sampleRate, 0.02);
    mix.reset   (sampleRate, 0.02);
    drive.setCurrentAndTargetValue (juce::Decibels::decibelsToGain (apvts.getRawParameterValue (kDrive)->load()));
    level.setCurrentAndTargetValue (juce::Decibels::decibelsToGain (apvts.getRawParameterValue (kLevel)->load()));
    mix.setCurrentAndTargetValue   (apvts.getRawParameterValue (kMix)->load());
}

bool FuzzProcessor::isBusesLayoutSupported (const BusesLayout& l) const
{
    const auto out = l.getMainOutputChannelSet();
    return (out == juce::AudioChannelSet::mono() || out == juce::AudioChannelSet::stereo())
           && out == l.getMainInputChannelSet();
}

void FuzzProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int numCh = buffer.getNumChannels();
    const int n = buffer.getNumSamples();

    drive.setTargetValue (juce::Decibels::decibelsToGain (apvts.getRawParameterValue (kDrive)->load()));
    level.setTargetValue (juce::Decibels::decibelsToGain (apvts.getRawParameterValue (kLevel)->load()));
    mix.setTargetValue   (apvts.getRawParameterValue (kMix)->load());
    *toneLowPass.state = *Coeffs::makeLowPass (sr, apvts.getRawParameterValue (kTone)->load());

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> ctx (block);

    // dry copy, delayed to line up with the oversampled wet path
    dryBuffer.setSize (numCh, n, false, false, true);
    for (int ch = 0; ch < numCh; ++ch)
        dryBuffer.copyFrom (ch, 0, buffer, ch, 0, n);
    juce::dsp::AudioBlock<float> dryBlock (dryBuffer);
    dryDelay.process (juce::dsp::ProcessContextReplacing<float> (dryBlock));

    const bool bassMode = apvts.getRawParameterValue (kMode)->load() > 0.5f;

    // Bass: a 4th-order Linkwitz-Riley crossover sends the lows around the fuzz (kept clean)
    // and only the highs through it. It runs in both modes so its state stays continuous.
    lowBuffer.setSize (numCh, n, false, false, true);
    for (int ch = 0; ch < numCh; ++ch)
    {
        auto* x = buffer.getWritePointer (ch);
        auto* lo = lowBuffer.getWritePointer (ch);
        for (int i = 0; i < n; ++i)
        {
            float low = 0.0f, high = 0.0f;
            crossover.processSample (ch, x[i], low, high);
            lo[i] = low;
            if (bassMode)
                x[i] = high;
        }
    }
    juce::dsp::AudioBlock<float> lowBlock (lowBuffer);
    lowDelay.process (juce::dsp::ProcessContextReplacing<float> (lowBlock)); // match oversampling latency

    if (! bassMode)
        inputHighPass.process (ctx); // guitar: cut mud before clipping

    auto osBlock = oversampling.processSamplesUp (block);
    const int osN = (int) osBlock.getNumSamples();
    constexpr float bias = 0.15f; // asymmetry adds even harmonics
    const float biasOffset = std::tanh (bias);

    for (int i = 0; i < osN; ++i)
    {
        const float g = drive.getNextValue();
        for (size_t ch = 0; ch < osBlock.getNumChannels(); ++ch)
        {
            auto* d = osBlock.getChannelPointer (ch);
            d[i] = juce::jlimit (-0.7f, 0.7f, std::tanh (d[i] * g + bias) - biasOffset) * 1.4f;
        }
    }

    oversampling.processSamplesDown (block);
    toneLowPass.process (ctx);

    if (bassMode)
        for (int ch = 0; ch < numCh; ++ch)
            buffer.addFrom (ch, 0, lowBuffer, ch, 0, n);

    for (int i = 0; i < n; ++i)
    {
        const float lv = level.getNextValue();
        const float mx = mix.getNextValue();
        for (int ch = 0; ch < numCh; ++ch)
            buffer.setSample (ch, i, buffer.getSample (ch, i) * lv * mx
                                         + dryBuffer.getSample (ch, i) * (1.0f - mx));
    }
}

juce::AudioProcessorEditor* FuzzProcessor::createEditor()
{
    return new FuzzEditor (*this);
}

void FuzzProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, dest);
}

void FuzzProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FuzzProcessor();
}
