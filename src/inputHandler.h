#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <stdbool.h>

#define max(a, b) ( ( a > b) ? a : b )
#define min(a, b) ( ( a < b) ? a : b )
#define limit(u, l, v) (max(l, min(v, u)))

typedef enum tEnumPlatformSelector
{
    NoConsole,
    Vita,
    New3DS
} ePlatformSelector;

#define PLATFORM Vita

#if (PLATFORM == Vita)
    #define JOYSTICKS 2
    #define BUTTONS 8
    #define TOUCHSCREENS 2
    #define TRIGGERS 0
    #define DPADS 1
#elif (PLATFORM == New3DS)
    #define JOYSTICKS 1
    #define BUTTONS 11
    #define TOUCHSCREENS 1
    #define TRIGGERS 0
    #define DPADS 1
#else
    #define JOYSTICKS 0
    #define BUTTONS 0
    #define TOUCHSCREENS 0
    #define TRIGGERS 0
    #define DPADS 0
#endif

typedef struct tStJoystick
{
   signed char siY;
   signed char siX;
} stJoystick;

typedef struct tStButton
{
   bool xHold;
   bool xTrigger;
} stButton;

typedef struct tStTouchScreen
{
    unsigned short uiY;
    unsigned short uiX;
    bool xTrigger;
} stTouchScreen;

typedef struct tStAnalogTigger
{
   unsigned char usiPos;
} stAnalogTrigger;

typedef struct tStDirectionalPad
{
    bool xUp;
    bool xDown;
    bool xLeft;
    bool xRight;
    bool xTrigger;
} stDirectionalPad;

typedef struct Gamepad
{
    stJoystick stJoy[JOYSTICKS];
    stButton stButt[BUTTONS];
    stTouchScreen stTouch[TOUCHSCREENS];
    stAnalogTrigger stAna[TRIGGERS];
    stDirectionalPad stDpad[DPADS];

} stGamePad;

void startInput();
void inputRead(stGamePad *stMcd);

#endif