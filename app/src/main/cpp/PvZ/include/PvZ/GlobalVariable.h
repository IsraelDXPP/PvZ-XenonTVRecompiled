#ifndef PVZ_GLOBAL_VARIABLE_H
#define PVZ_GLOBAL_VARIABLE_H

#include "PvZ/Lawn/Board/Board.h"
#include "PvZ/Lawn/Common/ConstEnums.h"
#include "PvZ/TodLib/Common/TodFoley.h"
#include "PvZ/Lawn/Widget/LeaderboardsWidget.h"
#include "PvZ/Lawn/Widget/ZombatarWidget.h"

#include <atomic>

inline uintptr_t gLibGameMainBaseAddr;

inline bool isMainMenu = true;
inline int mBackgroundType = -1;
inline bool requestPause;
inline bool isKeyboardTwoPlayerMode;
inline bool doKeyboardTwoPlayerDialog;
inline bool requestDrawShovelInCursor;
inline GameButton *gBoardMenuButton;
inline GameButton *gBoardStoreButton;
inline bool requestDrawButterInCursor;
inline bool m1PChoosingSeeds = true;

inline bool gKeyDown = false;
inline bool gButtonDown = false;
inline bool gButtonDownP1 = false;
inline bool gButtonDownP2 = false;
inline bool gButtonDownSeedChooser = false;
inline bool gButtonDownVSSetup = false;
inline Sexy::GamepadButton gButtonCode = Sexy::GamepadButton::GAMEPAD_BUTTON_NONE;
inline Sexy::GamepadButton gButtonCodeP1 = Sexy::GamepadButton::GAMEPAD_BUTTON_NONE;
inline Sexy::GamepadButton gButtonCodeP2 = Sexy::GamepadButton::GAMEPAD_BUTTON_NONE;
inline int gGamePlayerIndex = -1;
inline int gGamepad1ToPlayerIndex = -1;

inline int speedUpMode;
inline int speedUpCounter;

inline std::atomic_bool gHasInputContent;
inline std::string gInputString;

inline float angle1 = 0;
inline float angle2 = 1;

inline float gGamepadP1VelX;
inline float gGamepadP1VelY;
inline float gGamepadP2VelX;
inline float gGamepadP2VelY;

inline bool keyboardMode;

inline int xx, yy, xw, yh;
inline int xx1, yy1, xw1, yh1;

#endif
