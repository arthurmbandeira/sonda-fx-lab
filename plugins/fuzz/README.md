# Sardinha Fuzz

🇬🇧 English | [🇧🇷 Português](README.pt.md)

Oversampled fuzz for guitar and bass. Formats: VST3, AU, Standalone.

## Parameters

| Parameter | Range | Default | Function |
|---|---|---|---|
| Drive | 0–40 dB | 20 dB | Amount of gain/saturation before clipping. |
| Tone | 500–12000 Hz | 4000 Hz | Tone filter cutoff (lower = darker). |
| Level | −24 to +6 dB | −12 dB | Output volume. |
| Mode | Guitar / Bass | Guitar | *Bass* preserves the low end (200 Hz crossover) so you don't lose body. |
| Mix | 0–1 | 1 | Dry/wet balance. |

## Tips

- **Guitar:** *Guitar* mode, Drive 20–30 dB, Tone around 3–5 kHz. Lower Tone to tame harsh brightness.
- **Bass:** *Bass* mode with Mix 0.4–0.7 keeps low-end definition with grit on top.
- Drive is strong, so set **Level** last to match the clean signal's volume (start at −12 dB).
- Place before reverb/delay and after tuner/compressor.
- Placing it after an amp sim usually sounds worse; fuzz prefers a direct signal.
- Avoid feeding it an already clipping input signal.
