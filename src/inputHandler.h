#ifndef _INPUTHANDLER_H_
#define _INPUTHANDLER_H_

# include <stdbool.h>

typedef struct Joystick
{
   signed char siY;
   signed char siX;
} Joystick;

typedef struct Touch
{
    unsigned short uiY;
    unsigned short uiX;
    bool xRtrig;
    bool xFtrig;
} Touch;

typedef struct Console
{
    bool xCross;
    bool xTriangle;
    bool xCircle;
    bool xSquare;

    bool xUp;
    bool xDown;
    bool xLeft;
    bool xRight;

    bool xStart;
    bool xSelect;

    bool xRightTrigger;
    bool xLeftTrigger;

    Joystick stLeftJoy;
    Joystick stRightJoy;

    Touch stFrontTouch;
    Touch stRearTouch;
} Console;

void startInput();
void inputRead(Console *stVita);

#endif