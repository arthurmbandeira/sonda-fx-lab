#pragma once

#include "PluginProcessor.h"

class ChromeKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h, float pos,
                           float startAngle, float endAngle, juce::Slider&) override;
};

class OctaverEditor : public juce::AudioProcessorEditor
{
public:
    explicit OctaverEditor (OctaverProcessor&);
    ~OctaverEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct Knob
    {
        juce::Slider slider;
        juce::String title;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };

    void addKnob (Knob&, const juce::String& id, const juce::String& title, const juce::String& suffix, int decimals);

    OctaverProcessor& octaver;
    ChromeKnobLookAndFeel lnf;
    Knob octave, dry, tone, level;
    juce::Rectangle<float> plateBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OctaverEditor)
};
