# Sonda FX Lab

🇬🇧 English | [🇧🇷 Português](README.pt.md)

Audio effect plugins built with [JUCE](https://juce.com) 8.0.4 (C++17, CMake).

| Plugin | Description |
|---|---|
| [Simple Fuzz](plugins/fuzz/README.md) | Fuzz for guitar/bass with oversampling, Guitar/Bass modes and Mix. |
| [Bass VI Octaver](plugins/octaver/README.md) | Monophonic tracking sub-octave, ideal for bass / Bass VI. |

Formats: VST3, AU and Standalone (macOS, arm64 + x86_64, macOS 11+).

## Build

Requirements: Xcode command line tools and CMake ≥ 3.22. JUCE is downloaded automatically.

```sh
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j8
```

To reuse a local JUCE checkout:
`cmake -B build -DFETCHCONTENT_SOURCE_DIR_JUCE=/path/to/juce`

After the build, the plugins are copied to `~/Library/Audio/Plug-Ins/` (VST3 and Components).
Ready-to-use copies can be placed in `dist/` (not versioned).

## Using with Ableton Live 12 Lite

1. Make sure the plugins are in `~/Library/Audio/Plug-Ins/VST3` and/or `~/Library/Audio/Plug-Ins/Components`.
2. Open **Live → Settings → Plug-ins** and enable *Use Audio Units* and *Use VST3 Plug-in System Folders*.
3. Click **Rescan** (hold `Option` for a full rescan if needed).
4. The plugins show up under *Plug-ins* in the browser. Drag them onto an audio track.

If macOS blocks the plugin (it is unsigned), run:

```sh
xattr -dr com.apple.quarantine ~/Library/Audio/Plug-Ins/VST3/*.vst3 ~/Library/Audio/Plug-Ins/Components/*.component
```

## Layout

```
CMakeLists.txt      main project
plugins/fuzz        Simple Fuzz
plugins/octaver     Bass VI Octaver
```
