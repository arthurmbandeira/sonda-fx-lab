#include "PluginEditor.h"

namespace
{
    const juce::Colour tolex   { 0xff17181a };
    const juce::Colour chrome1 { 0xfff2f3f5 };
    const juce::Colour chrome2 { 0xff8d9399 };
    const juce::Colour cream   { 0xfff1e4bd };
    const juce::Colour burst   { 0xffc8481f };
    const juce::Colour ink     { 0xff25282c };
}

void ChromeKnobLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float pos,
                                              float startAngle, float endAngle, juce::Slider&)
{
    auto b = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (6.0f);
    const float d = juce::jmin (b.getWidth(), b.getHeight());
    auto c = juce::Rectangle<float> (d, d).withCentre (b.getCentre());
    const float angle = startAngle + pos * (endAngle - startAngle);

    g.setColour (juce::Colours::black.withAlpha (0.5f));
    g.fillEllipse (c.translated (1.5f, 3.0f));

    g.setColour (ink.withAlpha (0.8f));
    for (int i = 0; i <= 10; ++i)
    {
        const float a = startAngle + (float) i / 10.0f * (endAngle - startAngle);
        const auto dir = juce::Point<float> (std::sin (a), -std::cos (a));
        g.drawLine ({ c.getCentre() + dir * (d * 0.5f + 2.0f), c.getCentre() + dir * (d * 0.5f + 6.0f) }, 1.4f);
    }

    g.setGradientFill (juce::ColourGradient (chrome1, c.getTopLeft(), chrome2, c.getBottomRight(), false));
    g.fillEllipse (c);

    const auto cap = c.reduced (d * 0.12f);
    g.setGradientFill (juce::ColourGradient (cream.brighter (0.2f), cap.getX(), cap.getY(),
                                             cream.darker (0.4f), cap.getRight(), cap.getBottom(), false));
    g.fillEllipse (cap);
    g.setColour (ink);
    g.drawEllipse (cap, 1.0f);

    juce::Path p;
    p.addRoundedRectangle (-2.0f, -cap.getHeight() * 0.46f, 4.0f, cap.getHeight() * 0.34f, 2.0f);
    g.setColour (ink);
    g.fillPath (p, juce::AffineTransform::rotation (angle).translated (c.getCentre()));
}

OctaverEditor::OctaverEditor (OctaverProcessor& p)
    : AudioProcessorEditor (&p), octaver (p)
{
    addKnob (octave, "octave", "OCTAVE", "",    2);
    addKnob (dry,    "dry",    "DRY",    "",    2);
    addKnob (tone,   "tone",   "TONE",   " Hz", 0);
    addKnob (level,  "level",  "LEVEL",  " dB", 1);
    setSize (520, 300);
}

OctaverEditor::~OctaverEditor()
{
    for (auto* k : { &octave, &dry, &tone, &level })
        k->slider.setLookAndFeel (nullptr);
}

void OctaverEditor::addKnob (Knob& k, const juce::String& id, const juce::String& title,
                             const juce::String& suffix, int decimals)
{
    k.title = title;
    k.slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    k.slider.setRotaryParameters (juce::degreesToRadians (225.0f), juce::degreesToRadians (495.0f), true);
    k.slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 18);
    k.slider.setTextValueSuffix (suffix);
    k.slider.setNumDecimalPlacesToDisplay (decimals);
    k.slider.setColour (juce::Slider::textBoxTextColourId, ink);
    k.slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    k.slider.setLookAndFeel (&lnf);
    addAndMakeVisible (k.slider);
    k.attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (octaver.apvts, id, k.slider);
}

void OctaverEditor::resized()
{
    plateBounds = getLocalBounds().toFloat().reduced (24.0f, 20.0f);

    auto row = getLocalBounds().reduced (44, 0);
    row = row.removeFromBottom (150).withTrimmedBottom (34);
    const int cw = row.getWidth() / 4;
    for (auto* k : { &octave, &dry, &tone, &level })
        k->slider.setBounds (row.removeFromLeft (cw));
}

void OctaverEditor::paint (juce::Graphics& g)
{
    g.fillAll (tolex);

    // tolex grain
    juce::Random rng (7);
    g.setColour (juce::Colours::white.withAlpha (0.035f));
    for (int i = 0; i < 1400; ++i)
        g.fillRect (rng.nextFloat() * (float) getWidth(), rng.nextFloat() * (float) getHeight(), 1.5f, 1.5f);

    // chrome plate
    g.setColour (juce::Colours::black.withAlpha (0.5f));
    g.fillRoundedRectangle (plateBounds.translated (0.0f, 3.0f), 14.0f);
    g.setGradientFill (juce::ColourGradient (chrome1, plateBounds.getTopLeft(), chrome2, plateBounds.getBottomRight(), false));
    g.fillRoundedRectangle (plateBounds, 14.0f);
    g.setColour (ink);
    g.drawRoundedRectangle (plateBounds, 14.0f, 1.5f);

    // screws
    for (auto pt : { plateBounds.getTopLeft() + juce::Point<float> (14.0f, 14.0f),
                     plateBounds.getTopRight() + juce::Point<float> (-14.0f, 14.0f),
                     plateBounds.getBottomLeft() + juce::Point<float> (14.0f, -14.0f),
                     plateBounds.getBottomRight() + juce::Point<float> (-14.0f, -14.0f) })
    {
        g.setColour (chrome2.darker (0.3f));
        g.fillEllipse (juce::Rectangle<float> (10.0f, 10.0f).withCentre (pt));
        g.setColour (ink);
        g.drawLine ({ pt.x - 3.5f, pt.y - 1.5f, pt.x + 3.5f, pt.y + 1.5f }, 1.2f);
    }

    // title
    auto top = plateBounds.reduced (28.0f, 16.0f);
    g.setColour (ink);
    g.setFont (juce::Font (juce::FontOptions ("Georgia", 32.0f, juce::Font::bold)));
    g.drawText ("BASS VI OCTAVER", top.removeFromTop (38.0f), juce::Justification::centred);

    g.setColour (burst);
    g.fillRect (top.removeFromTop (4.0f).reduced (60.0f, 0.0f));
    g.setColour (ink.withAlpha (0.85f));
    g.setFont (juce::Font (juce::FontOptions ("Georgia", 13.0f, juce::Font::italic)));
    g.drawText ("one octave down", top.removeFromTop (22.0f), juce::Justification::centred);

    g.setColour (ink);
    g.setFont (juce::Font (juce::FontOptions ("Helvetica Neue", 13.0f, juce::Font::bold)));
    // titles sit above the knob scale ticks so the knob doesn't cover them
    constexpr int titleOffset = 20;
    for (auto* k : { &octave, &dry, &tone, &level })
        g.drawText (k->title, k->slider.getBounds().withHeight (16).translated (0, -titleOffset), juce::Justification::centred);
}
