# include <stdbool.h>

#ifndef _INPUTHANDLER_H_
#define _INPUTHANDLER_H_

typedef struct Joystick
{
   signed char siY;
   signed char siX;
}Joystick;

typedef struct Touch
{
    char usiY;
    char usiX;
}Touch;

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

    Joystick stLeftJoy;
    Joystick stRightJoy;

    Touch stFrontTouch;
    Touch stRearTouch;
} Console;

void inputHandler(Console *stVita);

#endif