# PvZ XenonTV Recompiled

<p align="center">
  <img src="https://img.shields.io/badge/status-en%20desarrollo-yellow" alt="Status">
  <img src="https://img.shields.io/badge/plataforma-Android%20ARM-blue" alt="Platform">
  <img src="https://img.shields.io/badge/licencia-GPL--3.0-green" alt="License">
</p>

<p align="center">
  <b>Reconstrucción progresiva de Plants vs. Zombies Android TV desde cero.</b><br>
  <i>Sin wrappers, sin dependencias del binario original, sin humo.</i>
</p>

---

## 🎯 Objetivo

Descompilar **todo** el juego — no solo parchear, no solo wrappear, no solo el audio. Reimplementar cada subsistema del binario original en C++ limpio y legible, manteniendo compatibilidad 1:1 con la lógica original, hasta que el binario deje de ser necesario.

| Componente | Estado |
|-----------|--------|
| 🎵 Sistema de audio | ✅ Descompilado (OpenSL ES) |
| 🎮 GLSurfaceView / GameView | 🔄 En progreso |
| 🧵 Game thread / loop | ❌ Pendiente |
| 📦 Gestión de assets | ❌ Pendiente |
| 🖥️ Render pipeline | ❌ Pendiente |
| 📝 Input / touch | ❌ Pendiente |

## 🏗️ Estructura del proyecto

```
app/src/main/cpp/PvZ/
├── include/PvZ/SexyAppFramework/   # Headers descompilados
├── src/SexyAppFramework/           # Implementaciones nativas (audio, etc.)
├── src/Android/                    # Backend Android (OpenSL ES, JNI)
├── src/HookInit.cpp                # Punto de entrada de hooks al binario
└── src/TodFoley.cpp                # Foley descompilado sin vtable
```

## 🛠️ Build

```sh
./gradlew assembleDebug
```

**Requisitos:**
- Android SDK Platform 34
- NDK r27c
- CMake 3.20+

Los assets del juego (`app/src/main/assets/`) deben copiarse desde una instalación original de PvZ Android TV — no se distribuyen en este repositorio.

## 📜 Licencia

GPL-3.0. Proyecto no afiliado con Transmension, PopCap Games ni Electronic Arts.
