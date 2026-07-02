# PvZ XenonTV Recompiled

[English](./README.md) | **简体中文**

从零开始逐步重建 Plants vs. Zombies Android TV 版。
无包装、无原始二进制依赖、无黑箱。

## 反编译方法

本项目通过渐进式替换来反编译 `libGameMain.so` (ARM32)：

1. **静态分析**：使用 Hex-Rays IDA 9.3 反编译二进制文件。
   原始输出位于 [`pseudocode/libGameMain.so.c`](../pseudocode/libGameMain.so.c)
   （约 82.3 万行），作为原始逻辑的参考。

2. **符号映射**：每个二进制函数在
   [`Symbols.h`](../app/src/main/cpp/PvZ/include/PvZ/Symbols.h) 中声明为
   `void *` 指针。地址在运行时通过 `libGameMain.so` 基址偏移解析。

3. **Hook 机制**：[`Homura`](../app/src/main/cpp/Homura) 框架提供
   `HookFunction<T>()` 来拦截二进制调用。每个 hook 将执行重定向到
   干净的 C++ 重新实现。若需原始行为，可调用 hook 前保存的 `old_*` 指针。

4. **渐进式重实现**：每个子系统从 *stub*（带有待解析 `Addr` 的骨架）开始。
   反编译完成后，替换为不依赖原始二进制的原生 C++ 代码。

5. **验证**：重新实现必须与原始逻辑 1:1 一致。只改变架构，不改变游戏玩法。

Hook 函数典型流程：
```
调用二进制 → hook 拦截 → 执行原生 C++ → （可选）调用 old_* 以使用原始函数
```

## 构建

```sh
./gradlew assembleDebug
```

需要 Android SDK Platform 34、NDK r27c、CMake 3.20+。
游戏资源文件（`app/src/main/assets/`）不包含在此仓库中。

## 编码风格 (C++)

### 命名约定

| 元素 | 约定 | 示例 |
|------|------|------|
| 函数 / 类型 / 概念 | PascalCase | `DrawShadow`, `GetCost`, `GameObject` |
| 变量 | camelCase | `mSeedType`, `theGridX`, `gKeyDown` |
| 命名空间 | snake_case | `sexy`, `homura`, `audiere`, `pvzstl` |
| 宏 / 常量 / 枚举成员 / 非类型模板参数 | UPPER_CASE | `BOARD_WIDTH`, `ADVICE_NONE`, `PVZ_SYMBOLS_H` |

#### 附加约定

- **Include guards**：`PVZ_` + 路径的 UPPER_SNAKE_CASE
  （例如 `PVZ_LAWN_BOARD_GAME_OBJECT_H`）
- **原始函数指针**：前缀 `old_` + PascalCase
  （例如 `old_PlaySample`, `old_Plant_Draw`）
- **符号地址**：后缀 `Addr` + PascalCase
  （例如 `Board_UpdateAddr`, `TodAnimateCurveAddr`）

### 格式

见 [`.clang-format`](../.clang-format)。
建议在每次提交前使用 clang-format 格式化代码。

### 提交

遵循[约定式提交](https://www.conventionalcommits.org/zh-hans/v1.0.0/)。

### 拉取请求

发送 PR 到 `dev` 分支。

## 许可协议

GPL-3.0。本项目与渡维科技、宝开或艺电无关，也未获得他们的认可。
