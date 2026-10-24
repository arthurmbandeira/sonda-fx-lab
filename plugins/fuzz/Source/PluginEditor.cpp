#include "PluginEditor.h"

namespace
{
    const juce::Colour tinLight   { 0xffe9ecef };
    const juce::Colour tinMid     { 0xffb4bac1 };
    const juce::Colour tinDark    { 0xff7b838c };
    const juce::Colour labelTop   { 0xff1f4e79 };
    const juce::Colour labelBot   { 0xff122c47 };
    const juce::Colour cream      { 0xfff3e6c4 };
    const juce::Colour redAccent  { 0xffd9432f };

    void drawSardine (juce::Graphics& g, juce::Rectangle<float> r, juce::Colour body)
    {
        const float w = r.getWidth(), h = r.getHeight();
        const auto at = [&] (float fx, float fy) { return juce::Point<float> (r.getX() + fx * w, r.getY() + fy * h); };

        juce::Path fish;
        fish.startNewSubPath (at (0.0f, 0.5f));
        fish.cubicTo (at (0.08f, 0.1f), at (0.45f, 0.0f), at (0.78f, 0.38f));
        fish.lineTo (at (1.0f, 0.12f));
        fish.lineTo (at (0.94f, 0.5f));
        fish.lineTo (at (1.0f, 0.88f));
        fish.lineTo (at (0.78f, 0.62f));
        fish.cubicTo (at (0.45f, 1.0f), at (0.08f, 0.9f), at (0.0f, 0.5f));
        fish.closeSubPath();

        g.setGradientFill (juce::ColourGradient (body.brighter (0.5f), at (0.5f, 0.1f),
                                                 body.darker (0.4f), at (0.5f, 0.95f), false));
        g.fillPath (fish);
        g.setColour (body.darker (0.7f));
        g.strokePath (fish, juce::PathStrokeType (1.2f));

        // gill, eye, stripes
        g.setColour (body.darker (0.6f));
        g.drawLine ({ at (0.2f, 0.28f), at (0.2f, 0.72f) }, 1.2f);
        g.setColour (juce::Colours::black);
        g.fillEllipse (juce::Rectangle<float> (w * 0.045f, w * 0.045f).withCentre (at (0.1f, 0.45f)));
        g.setColour (juce::Colours::white.withAlpha (0.35f));
        g.drawLine ({ at (0.3f, 0.3f), at (0.68f, 0.38f) }, 1.0f);
        g.drawLine ({ at (0.3f, 0.5f), at (0.72f, 0.5f) }, 1.0f);
    }
}

void TinKnobLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float pos,
                                           float startAngle, float endAngle, juce::Slider&)
{
    auto b = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (4.0f);
    const float d = juce::jmin (b.getWidth(), b.getHeight());
    auto c = juce::Rectangle<float> (d, d).withCentre (b.getCentre());
    const float angle = startAngle + pos * (endAngle - startAngle);

    // drop shadow
    g.setColour (juce::Colours::black.withAlpha (0.45f));
    g.fillEllipse (c.translated (1.5f, 3.0f));

    // scale ticks
    g.setColour (cream.withAlpha (0.8f));
    for (int i = 0; i <= 10; ++i)
    {
        const float a = startAngle + (float) i / 10.0f * (endAngle - startAngle);
        const auto dir = juce::Point<float> (std::sin (a), -std::cos (a));
        g.drawLine ({ c.getCentre() + dir * (d * 0.5f + 1.0f), c.getCentre() + dir * (d * 0.5f + 5.0f) }, 1.4f);
    }

    // knurled cap edge
    const auto cap = c.reduced (2.0f);
    g.setGradientFill (juce::ColourGradient (tinLight, cap.getTopLeft(), tinDark, cap.getBottomRight(), false));
    g.fillEllipse (cap);
    g.setColour (tinDark.darker (0.4f));
    for (int i = 0; i < 36; ++i)
    {
        const float a = (float) i / 36.0f * juce::MathConstants<float>::twoPi;
        const auto dir = juce::Point<float> (std::sin (a), -std::cos (a));
        g.drawLine ({ cap.getCentre() + dir * (cap.getWidth() * 0.5f - 3.0f),
                      cap.getCentre() + dir * (cap.getWidth() * 0.5f) }, 1.0f);
    }

    // inner dome
    const auto dome = cap.reduced (cap.getWidth() * 0.14f);
    g.setGradientFill (juce::ColourGradient (tinLight.brighter (0.2f), dome.getX(), dome.getY(),
                                             tinMid.darker (0.2f), dome.getRight(), dome.getBottom(), true));
    g.fillEllipse (dome);
    g.setColour (tinDark);
    g.drawEllipse (dome, 1.0f);

    // pointer
    juce::Path p;
    p.addRoundedRectangle (-2.0f, -dome.getHeight() * 0.46f, 4.0f, dome.getHeight() * 0.3f, 2.0f);
    g.setColour (redAccent);
    g.fillPath (p, juce::AffineTransform::rotation (angle).translated (c.getCentre()));
}

void TinKnobLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& b, bool, bool)
{
    const auto area = b.getLocalBounds().toFloat();
    const bool bass = b.getToggleState();
    const auto centre = area.getCentre();

    g.setFont (juce::Font (juce::FontOptions ("Helvetica Neue", 13.0f, juce::Font::bold)));
    auto left = area.withWidth (area.getWidth() * 0.5f - 26.0f);
    auto right = area.withTrimmedLeft (area.getWidth() * 0.5f + 26.0f);
    g.setColour (bass ? cream.withAlpha (0.45f) : cream);
    g.drawText ("GUITAR", left, juce::Justification::centredRight);
    g.setColour (bass ? cream : cream.withAlpha (0.45f));
    g.drawText ("BASS", right, juce::Justification::centredLeft);

    // chrome nut / base plate
    const auto pivot = juce::Point<float> (centre.x, centre.y + 9.0f);
    const auto nut = juce::Rectangle<float> (26.0f, 26.0f).withCentre (pivot);
    g.setColour (juce::Colours::black.withAlpha (0.5f));
    g.fillEllipse (nut.translated (1.0f, 2.0f));
    g.setGradientFill (juce::ColourGradient (tinLight, nut.getTopLeft(), tinDark, nut.getBottomRight(), false));
    g.fillEllipse (nut);
    g.setColour (tinDark.darker (0.5f));
    g.drawEllipse (nut, 1.0f);
    g.setColour (tinDark.darker (0.3f));
    g.fillEllipse (nut.reduced (7.0f));

    // lever
    const float angle = bass ? 0.75f : -0.75f;
    const auto tip = pivot + juce::Point<float> (std::sin (angle), -std::cos (angle)) * 26.0f;
    g.setColour (juce::Colours::black.withAlpha (0.4f));
    g.drawLine ({ pivot.x + 1.5f, pivot.y + 2.0f, tip.x + 1.5f, tip.y + 2.0f }, 6.0f);
    g.setColour (tinMid);
    g.drawLine ({ pivot, tip }, 6.0f);
    g.setColour (tinLight);
    g.drawLine ({ pivot.x - 1.0f, pivot.y, tip.x - 1.0f, tip.y }, 2.0f);
    const auto ball = juce::Rectangle<float> (12.0f, 12.0f).withCentre (tip);
    g.setGradientFill (juce::ColourGradient (tinLight, ball.getTopLeft(), tinDark, ball.getBottomRight(), false));
    g.fillEllipse (ball);
    g.setColour (tinDark.darker (0.5f));
    g.drawEllipse (ball, 1.0f);
}

FuzzEditor::FuzzEditor (FuzzProcessor& p)
    : AudioProcessorEditor (&p), fuzz (p)
{
    addKnob (drive, "drive", "DRIVE", " dB");
    addKnob (tone,  "tone",  "TONE",  " Hz");
    addKnob (level, "level", "LEVEL", " dB");
    addKnob (mix,   "mix",   "MIX",   "");

    modeSwitch.setLookAndFeel (&lnf);
    addAndMakeVisible (modeSwitch);
    modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (fuzz.apvts, "mode", modeSwitch);

    setSize (560, 400);
}

FuzzEditor::~FuzzEditor()
{
    modeSwitch.setLookAndFeel (nullptr);
    for (auto* k : { &drive, &tone, &level, &mix })
        k->slider.setLookAndFeel (nullptr);
}

void FuzzEditor::addKnob (Knob& k, const juce::String& id, const juce::String& title, const juce::String& suffix)
{
    k.title = title;
    k.slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    k.slider.setRotaryParameters (juce::degreesToRadians (225.0f), juce::degreesToRadians (495.0f), true);
    k.slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 80, 18);
    k.slider.setTextValueSuffix (suffix);
    k.slider.setNumDecimalPlacesToDisplay (id == "mix" ? 2 : (id == "tone" ? 0 : 1));
    k.slider.setColour (juce::Slider::textBoxTextColourId, cream);
    k.slider.setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    k.slider.setLookAndFeel (&lnf);
    addAndMakeVisible (k.slider);
    k.attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (fuzz.apvts, id, k.slider);
}

void FuzzEditor::resized()
{
    tinBounds = getLocalBounds().toFloat().reduced (6.0f);
    labelBounds = tinBounds.reduced (22.0f);

    auto area = getLocalBounds().reduced (40);
    area.removeFromBottom (14);             // margin so the value text stays inside the label border
    auto row = area.removeFromBottom (128); // was 140: a shorter row means smaller knobs
    const int cw = row.getWidth() / 4;
    for (auto* k : { &drive, &tone, &level, &mix })
        k->slider.setBounds (row.removeFromLeft (cw).withTrimmedTop (16));

    modeSwitch.setBounds (getWidth() / 2 - 110, 170, 220, 44);
}

void FuzzEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff2a2d31));

    // tin body and rim
    g.setColour (juce::Colours::black.withAlpha (0.5f));
    g.fillRoundedRectangle (tinBounds.translated (0.0f, 2.0f), 38.0f);
    g.setGradientFill (juce::ColourGradient (tinLight, tinBounds.getTopLeft(), tinDark, tinBounds.getBottomRight(), false));
    g.fillRoundedRectangle (tinBounds, 38.0f);
    g.setColour (tinDark.darker (0.3f));
    g.drawRoundedRectangle (tinBounds, 38.0f, 1.5f);

    // stamped ridge
    const auto ridge = tinBounds.reduced (11.0f);
    g.setColour (tinDark.withAlpha (0.8f));
    g.drawRoundedRectangle (ridge, 30.0f, 2.0f);
    g.setColour (tinLight);
    g.drawRoundedRectangle (ridge.translated (0.0f, 1.5f), 30.0f, 1.0f);

    // painted label
    g.setGradientFill (juce::ColourGradient (labelTop, labelBounds.getTopLeft(), labelBot, labelBounds.getBottomLeft(), false));
    g.fillRoundedRectangle (labelBounds, 22.0f);
    g.setColour (cream);
    g.drawRoundedRectangle (labelBounds.reduced (7.0f), 16.0f, 1.5f);

    // title
    auto top = labelBounds.reduced (24.0f, 14.0f);
    g.setColour (cream);
    g.setFont (juce::Font (juce::FontOptions ("Georgia", 34.0f, juce::Font::bold)));
    g.drawText ("SARDINHA FUZZ", top.removeFromTop (40.0f), juce::Justification::centred);
    g.setColour (redAccent);
    g.setFont (juce::Font (juce::FontOptions ("Georgia", 13.0f, juce::Font::italic)));
    g.drawText ("pickled distortion  -  packed in overdrive oil", top.removeFromTop (18.0f), juce::Justification::centred);

    // fish
    auto fishRow = top.removeFromTop (70.0f).reduced (30.0f, 8.0f);
    const float fw = 96.0f;
    const float gap = (fishRow.getWidth() - 3.0f * fw) / 2.0f;
    for (int i = 0; i < 3; ++i)
    {
        auto r = juce::Rectangle<float> (fishRow.getX() + (float) i * (fw + gap), fishRow.getY() + (i == 1 ? 6.0f : 0.0f),
                                         fw, 40.0f);
        drawSardine (g, r, i == 1 ? juce::Colour (0xffcfd8e0) : juce::Colour (0xffaebccb));
    }

    // knob titles: moved up by titleOffset pixels (was 2) so they clear the knob
    constexpr int titleOffset = 20;
    g.setColour (cream);
    g.setFont (juce::Font (juce::FontOptions ("Helvetica Neue", 13.0f, juce::Font::bold)));
    for (auto* k : { &drive, &tone, &level, &mix })
    {
        auto b = k->slider.getBounds().withHeight (16).translated (0, -titleOffset);
        g.drawText (k->title, b, juce::Justification::centred);
    }

    // pull-tab ring, top-right
    const auto ringC = juce::Point<float> (tinBounds.getRight() - 52.0f, tinBounds.getY() + 36.0f);
    g.setColour (juce::Colours::black.withAlpha (0.35f));
    g.drawEllipse (juce::Rectangle<float> (34.0f, 26.0f).withCentre (ringC + juce::Point<float> (1.5f, 2.5f)), 5.0f);
    g.setColour (tinLight);
    g.drawEllipse (juce::Rectangle<float> (34.0f, 26.0f).withCentre (ringC), 5.0f);
    g.setColour (tinDark);
    g.drawEllipse (juce::Rectangle<float> (42.0f, 34.0f).withCentre (ringC), 1.0f);
}
