# Sonda FX Lab

[🇬🇧 English](README.md) | 🇧🇷 Português

Plugins de efeitos de áudio feitos com [JUCE](https://juce.com) 8.0.4 (C++17, CMake).

| Plugin | Descrição |
|---|---|
| [Sardinha Fuzz](plugins/fuzz/README.pt.md) | Fuzz para guitarra/baixo com oversampling, modos Guitar/Bass e Mix. |
| [Bass VI Octaver](plugins/octaver/README.pt.md) | Oitava abaixo (sub) com tracking monofônico, ideal para baixo/Bass VI. |

Formatos: VST3, AU e Standalone (macOS, arm64 + x86_64, macOS 11+).

## Build

Requisitos: Xcode command line tools e CMake ≥ 3.22. O JUCE é baixado automaticamente.

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j8
```

Para reutilizar um checkout local do JUCE:
`cmake -B build -DFETCHCONTENT_SOURCE_DIR_JUCE=/caminho/para/juce`

Após o build, os plugins são copiados para `~/Library/Audio/Plug-Ins/` (VST3 e Components).
Cópias prontas ficam em `dist/` (não versionada).

## Usar no Ableton Live 12 Lite

1. Garantir que os plugins estão em `~/Library/Audio/Plug-Ins/VST3` e/ou `~/Library/Audio/Plug-Ins/Components`.
2. Abra **Live → Settings → Plug-ins** e ativar *Use Audio Units* e *Use VST3 Plug-in System Folders*.
3. Clique em **Rescan** (segure `Option` para um rescan completo, se necessário).
4. Os plugins aparecem em *Plug-ins* no navegador. Arraste para uma faixa de áudio.

Se o macOS bloquear o plugin (não está assinado), correr:

```sh
xattr -dr com.apple.quarantine ~/Library/Audio/Plug-Ins/VST3/*.vst3 ~/Library/Audio/Plug-Ins/Components/*.component
```

## Estrutura

```
CMakeLists.txt      projeto principal
plugins/fuzz        Sardinha Fuzz
plugins/octaver     Bass VI Octaver
```
