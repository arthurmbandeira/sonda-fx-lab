# Simple Fuzz

[🇬🇧 English](README.md) | 🇧🇷 Português

Fuzz com oversampling para guitarra e baixo. Formatos: VST3, AU, Standalone.

## Parâmetros

| Parâmetro | Gama | Padrão | Função |
|---|---|---|---|
| Drive | 0–40 dB | 20 dB | Quantidade de ganho/saturação antes do clipping. |
| Tone | 500–12000 Hz | 4000 Hz | Frequência de corte do filtro de tom (mais baixo = mais escuro). |
| Level | −24 a +6 dB | −12 dB | Volume de saída. |
| Mode | Guitar / Bass | Guitar | *Bass* preserva os graves (crossover a 200 Hz) para não perder corpo. |
| Mix | 0–1 | 1 | Balanço seco/molhado. |

## Dicas

- **Guitarra:** modo *Guitar*, Drive 20–30 dB, Tone ~3–5 kHz. Baixar Tone para suavizar o brilho agressivo.
- **Baixo:** modo *Bass* e Mix 0.4–0.7 mantêm a definição do grave com sujeira por cima.
- Como o Drive é forte, ajustar **Level** por último para igualar o volume com o sinal limpo (começar em −12 dB).
- Colocar antes de reverb/delay e depois de afinador/compressor.
- Colocar depois de um amp sim costuma soar pior; o fuzz prefere sinal direto.
- Em faixas mono, o plugin funciona em stereo; use um sinal sem clipping à entrada.
