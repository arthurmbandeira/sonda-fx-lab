# Bass VI Octaver

Octaver monofónico que gera uma oitava abaixo (sub) a partir do sinal, com tracking da fundamental. Formatos: VST3, AU, Standalone.

## Parâmetros

| Parâmetro | Gama | Padrão | Função |
|---|---|---|---|
| Octave | 0–1 | 0.8 | Volume da oitava abaixo. |
| Dry | 0–1 | 0.6 | Volume do sinal original. |
| Tone | 400–8000 Hz | 2500 Hz | Filtro de tom do sinal da oitava. |
| Level | −24 a +6 dB | 0 dB | Volume de saída. |

## Dicas

- **Tocar uma nota de cada vez:** o tracking é monofónico; acordes e notas sobrepostas geram artefactos.
- Funciona melhor com sinal limpo, sem distorção, e com notas bem articuladas. Tocar mais perto do braço/ponte altera o ataque do tracking.
- O sub fica em silêncio abaixo de um limiar de nível, por isso notas muito suaves podem não ativar a oitava.
- Para um som de sub grosso: Octave 0.8–1.0, Dry 0.4–0.6, Tone 1–2 kHz.
- Para efeito subtil de engrossar: Octave 0.3–0.5, Dry 0.8–1.
- Cuidado com sistemas que não reproduzem frequências muito graves; usar auscultadores/monitores adequados.
- Colocar o octaver **antes** de distorção/fuzz (por exemplo, antes do Simple Fuzz) para o tracking ser limpo.
