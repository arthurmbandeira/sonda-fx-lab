# Sonda FX Lab

Plugins de efeitos de áudio feitos com [JUCE](https://juce.com) 8.0.4 (C++17, CMake).

| Plugin | Descrição |
|---|---|
| [Simple Fuzz](plugins/fuzz/README.md) | Fuzz para guitarra/baixo com oversampling, modos Guitar/Bass e Mix. |
| [Bass VI Octaver](plugins/octaver/README.md) | Oitava abaixo (sub) com tracking monofónico, ideal para baixo/Bass VI. |

Formatos: VST3, AU e Standalone (macOS, arm64 + x86_64, macOS 11+).

## Build

Requisitos: Xcode command line tools e CMake ≥ 3.22. O JUCE é descarregado automaticamente.

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
2. Abrir **Live → Settings → Plug-ins** e ativar *Use Audio Units* e *Use VST3 Plug-in System Folders*.
3. Clicar em **Rescan** (manter `Option` premido para um rescan completo, se necessário).
4. Os plugins aparecem em *Plug-ins* no browser. Arrastar para uma pista de áudio.

Se o macOS bloquear o plugin (não está assinado), correr:

```sh
xattr -dr com.apple.quarantine ~/Library/Audio/Plug-Ins/VST3/*.vst3 ~/Library/Audio/Plug-Ins/Components/*.component
```

## Estrutura

```
CMakeLists.txt      projeto principal
plugins/fuzz        Simple Fuzz
plugins/octaver     Bass VI Octaver
```
