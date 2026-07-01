# PvZ XenonTV Recompiled

Decompilación progresiva del sistema de audio de Plants vs. Zombies para Android ARM nativo.

## Objetivo

Reemplazar todo el sistema de audio propietario del binario original (AudiereSoundManager, AudiereSoundInstance, codecs Ogg/Vorbis, backend OpenAL) con una reimplementación en C++ limpia y descompilada, usando OpenSL ES como backend nativo de Android.

## Estado actual

- ✅ AudiereSoundManager (256 slots, 32 canales) — descompilado 1:1
- ✅ AudiereSoundInstance (volumen, pitch, pan, play/stop/release) — descompilado
- ✅ OpenSL ES backend (buffer queue doble, 2048 frames) — funcional
- ✅ MixerDevice / MixerStream / Resampler / WAVSource — pipeline de audio completo
- ✅ PlaySample / PlaySampleSingle hooks (4 puntos de entrada) — redirigidos a nuestro SM
- ✅ TodFoley migrado a llamadas directas (sin vtable del binario)
- ✅ Importación de PCM desde el binario (decodeBuffer y WAVInputStream)
- ❌ Sonido se corta / calidad pobre — en depuración

## Próximos pasos

1. Corregir la reproducción completa de los sonidos (se cortan antes de terminar)
2. Sincronización de volúmenes en tiempo real
3. Mezcla de Foley (varias instancias simultáneas del mismo sonido)
4. Soporte Ogg Vorbis nativo (sin depender del decodeBuffer del binario)

## Build

```sh
./gradlew assembleDebug
```

Requiere Android SDK Platform 34, NDK r27c, CMake 3.20+.

Recursos del juego (assets/) no incluidos — copiar desde la instalación original de PvZ Android TV.

## Licencia

GPL-3.0. Proyecto no afiliado con Transmension, PopCap o Electronic Arts.
