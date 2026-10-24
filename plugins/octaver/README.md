# Bass VI Octaver

🇬🇧 English | [🇧🇷 Português](README.pt.md)

Monophonic octaver that generates a sub-octave by tracking the fundamental. Formats: VST3, AU, Standalone.

## Parameters

| Parameter | Range | Default | Function |
|---|---|---|---|
| Octave | 0–1 | 0.8 | Volume of the octave-down signal. |
| Dry | 0–1 | 0.6 | Volume of the original signal. |
| Tone | 400–8000 Hz | 2500 Hz | Tone filter for the octave signal. |
| Level | −24 to +6 dB | 0 dB | Output volume. |

## Tips

- **Play one note at a time:** tracking is monophonic; chords and overlapping notes cause artifacts.
- Works best with a clean, undistorted signal and well-articulated notes.
- The sub is silent below a level threshold, so very soft notes may not trigger the octave.
- For a thick sub sound: Octave 0.8–1.0, Dry 0.4–0.6, Tone 1–2 kHz.
- For a subtle thickening: Octave 0.3–0.5, Dry 0.8–1.
- Be careful with systems that can't reproduce very low frequencies; use suitable headphones/monitors.
- Put the octaver **before** distortion/fuzz (e.g. before Sardinha Fuzz) for cleaner tracking.
