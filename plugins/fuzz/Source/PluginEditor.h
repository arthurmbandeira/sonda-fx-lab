#pragma once

#include "PluginProcessor.h"

// Brushed-metal bottle-cap style knob
class TinKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics&, int x, int y, int w, int h, float pos,
                           float startAngle, float endAngle, juce::Slider&) override;

    // Analog-style toggle switch: GUITAR on the left, BASS on the right
    void drawToggleButton (juce::Graphics&, juce::ToggleButton&, bool highlighted, bool down) override;
};

class FuzzEditor : public juce::AudioProcessorEditor
{
public:
    explicit FuzzEditor (FuzzProcessor&);
    ~FuzzEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct Knob
    {
        juce::Slider slider;
        juce::String title;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };

    void addKnob (Knob&, const juce::String& id, const juce::String& title, const juce::String& suffix);

    FuzzProcessor& fuzz;
    TinKnobLookAndFeel lnf;
    Knob drive, tone, level, mix;
    juce::ToggleButton modeSwitch;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> modeAttachment;
    juce::Rectangle<float> tinBounds, labelBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (FuzzEditor)
};
