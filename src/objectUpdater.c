#include "objectUpdater.h"

#define MAX(a, b) ( ( a > b) ? a : b )
#define MIN(a, b) ( ( a < b) ? a : b )
#define PI 3.14159265
#define Deg2Rad PI/180

void paddleUpdate(Position *stPadPos, Velocity *stPadVel, Characteristics *stPadChar, Console *stVita, Map *stFrame)
{
    float rMaxRangeJoystick = 128;
    float rDeadzoneJoystick = 10;
    float rMaxPaddleSpeed = 10;
    if (stVita->stLeftJoy.siX >= rDeadzoneJoystick ||
        stVita->stLeftJoy.siX <= -rDeadzoneJoystick)
    {
        if (((stVita->stLeftJoy.siX < 0) & (stPadPos->rX > (stFrame->uiLeftBorder[1] + stFrame->uiFrameThickness))) ||
            ((stVita->stLeftJoy.siX > 0) & (stPadPos->rX < (stFrame->uiRightBorder[1] - stPadChar->uiWidth)))) //Right border already has frame thickness included
        {
            stPadVel->rDotX = ((float)stVita->stLeftJoy.siX - rDeadzoneJoystick) * (rMaxPaddleSpeed / rMaxRangeJoystick);
            stPadPos->rX += stPadVel->rDotX;
            stPadPos->rX = MAX((stFrame->uiLeftBorder[1] + stFrame->uiFrameThickness), MIN(stPadPos->rX, (stFrame->uiRightBorder[1] - stPadChar->uiWidth)));
        }
    }
}

void ballUpdate(Position *stBallPos, Velocity *stBallVel, Characteristics *stBallChar, Map *stFrame, Position *stPadPos, Velocity *stPadVel, Characteristics *stPadChar, Position *stBlockPos, Characteristics *stBlockChar, unsigned short uiNrOfBlocks)
{
    unsigned short i = 0;
    stBallPos->rX += stBallVel->rDotX;
    stBallPos->rY += stBallVel->rDotY;

    // Bounce off walls
    if (stBallPos->rY <= (stFrame->uiTopBorder[2] + stFrame->uiFrameThickness + stBallChar->uiHeight) ||
        stBallPos->rY >= (stFrame->uiBottomBorder[2] - stBallChar->uiHeight))
    {
        stBallVel->rDotY *= -1;
    }

    if (stBallPos->rX <= (stFrame->uiLeftBorder[1] + stFrame->uiFrameThickness + stBallChar->uiWidth) ||
        stBallPos->rX >= (stFrame->uiRightBorder[1] - stBallChar->uiWidth)) 
    {
        stBallVel->rDotX *= -1;
    }
    
    //Bounce off paddle
    xBaseCollision =
        stBallPos->rY >= (stPadPos->rY - stBallChar->uiHeight) &&
        stBallPos->rX >= (stPadPos->rX - stBallChar->uiWidth) &&
        stBallPos->rX <= (stPadPos->rX + stPadChar->uiWidth + stBallChar->uiWidth);
    
    if (xBaseCollision && !xBaseCollisionOld)
    {
        xBaseCollisionTrigger = true;
    }
    else
    {
        xBaseCollisionTrigger = false;
    }

    xBaseCollisionOld = xBaseCollision;

    if (xBaseCollision &&
        xBaseCollisionTrigger)
    {
        float rBallSpeed = sqrt(pow(stBallVel->rDotX, 2) + pow(stBallVel->rDotY, 2));
        float rMinLateralSpeed = 0.2;

        stBallVel->rDotX = MIN((rBallSpeed - rMinLateralSpeed), MAX((((stBallPos->rX - (stPadPos->rX + (float)stPadChar->uiWidth / 2))  / ((float)stPadChar->uiWidth / 2)) +(stPadVel->rDotY / 2)), (rBallSpeed*-1 + rMinLateralSpeed)));
        stBallVel->rDotY = sqrt(pow(rBallSpeed,2) - pow(stBallVel->rDotX, 2)) * -1;        
    }

    //Bounce off block
    for (i = 0; i < uiNrOfBlocks; i++)
    {
        if (stBlockChar[i].xVisible)
        {
            if (stBallPos->rY >= (stBlockPos[i].rY - stBallChar->uiHeight) &&
                stBallPos->rY <= (stBlockPos[i].rY + stBlockChar[i].uiHeight + stBallChar->uiHeight) &&
                stBallPos->rX >= (stBlockPos[i].rX) &&
                stBallPos->rX <= (stBlockPos[i].rX + stBlockChar[i].uiWidth))
            {
                stBlockChar[i].xVisible = false;
                stBallVel->rDotY *= -1;
            }

            if (stBallPos->rX <= (stBlockPos[i].rX + stBlockChar[i].uiWidth + stBallChar->uiWidth) &&
                stBallPos->rX >= (stBlockPos[i].rX - stBallChar->uiWidth) &&
                stBallPos->rY >= (stBlockPos[i].rY) &&
                stBallPos->rY <= (stBlockPos[i].rY + stBlockChar[i].uiHeight))
            {
                stBlockChar[i].xVisible = false;
                stBallVel->rDotX *= -1;
            }
        }
    }
}