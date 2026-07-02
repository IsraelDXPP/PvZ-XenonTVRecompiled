# PvZ XenonTV Recompiled

Reconstrucción progresiva de Plants vs. Zombies Android TV desde cero.
Sin wrappers, sin dependencias del binario original, sin humo.

## Metodología de descompilación

Este proyecto descompila `libGameMain.so` (ARM32) mediante un proceso de
reemplazo progresivo:

1. **Análisis estático**: El binario se descompila con Hex-Rays IDA 9.3.
   El output crudo está en [`pseudocode/libGameMain.so.c`](../pseudocode/libGameMain.so.c)
   (~823K líneas). Sirve como referencia de la lógica original.

2. **Mapeo de símbolos**: Cada función del binario se declara como puntero
   (`void *`) en [`Symbols.h`](../app/src/main/cpp/PvZ/include/PvZ/Symbols.h).
   Las direcciones se resuelven en tiempo de ejecución offseteando la base
   de `libGameMain.so`.

3. **Hooking**: El framework [`Homura`](../app/src/main/cpp/Homura) provee
   `HookFunction<T>()` para interceptar llamadas del binario. Cada hook
   redirige la ejecución a una reimplementación en C++ limpio. Si el hook
   necesita comportamiento original, puede llamar al puntero `old_*`
   guardado antes del hook.

4. **Reimplementación progresiva**: Cada subsistema comienza como un *stub*
   (esqueleto con `Addr` pendiente de resolución). Cuando se descompila, se
   sustituye por código C++ nativo sin dependencias del binario. El orden
   sigue la matriz del README principal.

5. **Verificación**: La reimplementación debe ser 1:1 con la lógica original.
   No se introducen cambios de gameplay, solo de arquitectura.

Flujo típico de una función hookeada:
```
llamada al binario → hook intercepta → ejecuta C++ nativo → (opcional) llama old_* si necesita el original
```

## Build

```sh
./gradlew assembleDebug
```

Requiere Android SDK Platform 34, NDK r27c, CMake 3.20+.
Assets del juego en `app/src/main/assets/` no incluidos.

## Coding Style (C++)

### Name Convention

| Elemento | Convención | Ejemplo |
|----------|-----------|---------|
| Functions / types / concepts | PascalCase | `DrawShadow`, `GetCost`, `GameObject` |
| Variables | camelCase | `mSeedType`, `theGridX`, `gKeyDown` |
| Namespaces | snake_case | `sexy`, `homura`, `audiere`, `pvzstl` |
| Macros / constants / enumerators / non-type template parameters | UPPER_CASE | `BOARD_WIDTH`, `ADVICE_NONE`, `PVZ_SYMBOLS_H` |

#### Convenciones adicionales

- **Include guards**: `PVZ_` + path en UPPER_SNAKE_CASE
  (ej. `PVZ_LAWN_BOARD_GAME_OBJECT_H`)
- **Punteros a función original**: prefijo `old_` + PascalCase
  (ej. `old_PlaySample`, `old_Plant_Draw`)
- **Direcciones de símbolos**: sufijo `Addr` + PascalCase
  (ej. `Board_UpdateAddr`, `TodAnimateCurveAddr`)

### Format

Ver [`.clang-format`](../.clang-format).
Se recomienda formatear con clang-format antes de cada commit.

### Commits

[Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/).

### Pull Requests

Enviar PR a la rama `dev`.

## Licencia

GPL-3.0. Proyecto no afiliado con Transmension, PopCap Games ni Electronic Arts.
