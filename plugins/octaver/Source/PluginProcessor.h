#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class OctaverProcessor : public juce::AudioProcessor
{
public:
    OctaverProcessor();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout&) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Bass VI Octaver"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();

    using Filter = juce::dsp::IIR::Filter<float>;
    using Coeffs = juce::dsp::IIR::Coefficients<float>;

    // Pitch tracker state: a Schmitt trigger feeding a flip-flop that divides the frequency by 2
    Filter trackLowPass;
    bool schmittHigh = false;
    bool flipFlop = false;
    float envelope = 0.0f;
    float envAttack = 0.0f, envRelease = 0.0f;

    // Smooths the divided square wave into a rounder sub tone (two cascaded 2nd-order stages)
    Filter subLowPass1, subLowPass2;

    // Master tone, one filter per channel
    Filter toneLowPass[2];
    float lastToneHz = -1.0f;

    juce::SmoothedValue<float> octaveGain, dryGain, outGain;
    double sr = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OctaverProcessor)
};
