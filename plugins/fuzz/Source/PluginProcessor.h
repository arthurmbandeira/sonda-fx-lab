#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class FuzzProcessor : public juce::AudioProcessor
{
public:
    FuzzProcessor();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout&) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Simple Fuzz"; }
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

    juce::dsp::Oversampling<float> oversampling { 2, 2, // 2 channels, 4x
        juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR };
    juce::dsp::ProcessorDuplicator<Filter, Coeffs> inputHighPass, toneLowPass;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::None> dryDelay { 1024 };

    juce::dsp::LinkwitzRileyFilter<float> crossover;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::None> lowDelay { 1024 };

    juce::SmoothedValue<float> drive, level, mix;
    juce::AudioBuffer<float> dryBuffer, lowBuffer;
    double sr = 44100.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FuzzProcessor)
};
