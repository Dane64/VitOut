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
    stPadVel->rDotX = 0;
    stPadVel->rDotY = 0;

    if (stVita->stLeftJoy.siX >= rDeadzoneJoystick ||
        stVita->stLeftJoy.siX <= -rDeadzoneJoystick)
    {
        if (((stVita->stLeftJoy.siX < 0) & (stPadPos->rX > (stFrame->uiLeftBorder[1] + stFrame->uiFrameThickness))) ||
            ((stVita->stLeftJoy.siX > 0) & (stPadPos->rX < (stFrame->uiRightBorder[1] - stPadChar->uiWidth)))) //Right border already has frame thickness included because of drawing method
        {
            stPadVel->rDotX = (float)stVita->stLeftJoy.siX * (rMaxPaddleSpeed / rMaxRangeJoystick);
            stPadPos->rX += stPadVel->rDotX;
            stPadPos->rX = MAX((stFrame->uiLeftBorder[1] + stFrame->uiFrameThickness), MIN(stPadPos->rX, (stFrame->uiRightBorder[1] - stPadChar->uiWidth)));
        }
    }
}

bool ballUpdate(Position *stBallPos, Velocity *stBallVel, Characteristics *stBallChar, Map *stFrame, Position *stPadPos, Velocity *stPadVel, Characteristics *stPadChar, Position *stBlockPos, Characteristics *stBlockChar, Console *stVita, unsigned short uiNrOfBlocks)
{
    unsigned short i = 0;
    bool xAllBlocksDestroyed = true;

    // reset ball position
    if (stBallVel->rDotX == 0 && stBallVel->rDotY == 0)
    {
        stBallPos->rX = stPadPos->rX + (stPadChar->uiWidth / 2);
        stBallPos->rY = stPadPos->rY - stBallChar->uiHeight;

        if (stVita->xCross == true)
        {
            unsigned short uiMax = 2000;
            unsigned short uiMin = 100;
            stBallVel->rDotY = -10.0;
            stBallVel->rDotX = -0.2;
        //    srand(time(0));
        //    stBallVel->rDotY = (float)(rand()%(uiMax-uiMin+1)+uiMin)/1000;
        //    stBallVel->rDotX = (float)(rand()%(uiMax-uiMin+1)+uiMin)/1000;
        }
    }

    stBallPos->rX += stBallVel->rDotX;
    stBallPos->rY += stBallVel->rDotY;

    // Bounce off walls
    if (stBallPos->rY <= (stFrame->uiTopBorder[2] + stFrame->uiFrameThickness + stBallChar->uiHeight)) //||
        //stBallPos->rY >= (stFrame->uiBottomBorder[2] - stBallChar->uiHeight))
    {
        frameSound();
        stBallVel->rDotY *= -1;
    }

    if (stBallPos->rX < (stFrame->uiLeftBorder[1] + stFrame->uiFrameThickness + stBallChar->uiWidth) ||
        stBallPos->rX > (stFrame->uiRightBorder[1] - stBallChar->uiWidth)) 
    {
        frameSound();
        stBallVel->rDotX *= -1;
    }
    
    //Bounce off paddle
    xBaseCollision =
        stBallPos->rY > (stPadPos->rY - stBallChar->uiHeight) &&
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
        paddleSound();
        float rBallSpeed = sqrt(pow(stBallVel->rDotX, 2) + pow(stBallVel->rDotY, 2));
        float rMinLateralSpeed = 0.2;

        stBallVel->rDotX = MIN((rBallSpeed - rMinLateralSpeed), MAX((((stBallPos->rX - (stPadPos->rX + (float)stPadChar->uiWidth / 2))  / ((float)stPadChar->uiWidth / 2)) + (stPadVel->rDotX / 2)), (rBallSpeed*-1 + rMinLateralSpeed)));
        stBallVel->rDotY = sqrt(pow(rBallSpeed,2) - pow(stBallVel->rDotX, 2)) * -1;
    }

    // Check ball position
    if (stBallPos->rY > (stPadPos->rY + stPadChar->uiHeight + stBallChar->uiHeight))
    {
        stBallChar->uiLives--;
    }

    //Bounce off block
    for (i = 0; i < uiNrOfBlocks; i++)
    {
        if (stBlockChar[i].xVisible)
        {
            xAllBlocksDestroyed = false;
            if (stBallPos->rY >= (stBlockPos[i].rY - stBallChar->uiHeight) &&
                stBallPos->rY <= (stBlockPos[i].rY + stBlockChar[i].uiHeight + stBallChar->uiHeight) &&
                stBallPos->rX >= (stBlockPos[i].rX - stBallChar->uiWidth) &&
                stBallPos->rX <= (stBlockPos[i].rX + stBlockChar[i].uiWidth + stBallChar->uiWidth))
            {
                blockSound();
                stBlockChar[i].uiLives --;
                stBallVel->rDotY *= -1;

                if (stBlockChar[i].uiLives == 0)
                {
                    stBlockChar[i].xVisible = false;
                }
                break;
            }

            else if (stBallPos->rX <= (stBlockPos[i].rX + stBlockChar[i].uiWidth + stBallChar->uiWidth) &&
                stBallPos->rX >= (stBlockPos[i].rX - stBallChar->uiWidth) &&
                stBallPos->rY >= (stBlockPos[i].rY - stBallChar->uiHeight) &&
                stBallPos->rY <= (stBlockPos[i].rY + stBlockChar[i].uiHeight + stBallChar->uiHeight))
            {
                blockSound();
                stBlockChar[i].uiLives --;
                stBallVel->rDotX *= -1;

                if (stBlockChar[i].uiLives == 0)
                {
                    stBlockChar[i].xVisible = false;
                }
                break;
            }
        }
    }

    // reset ball charistics
    if (stBallChar->uiLives < uiOldLives)
    {
        stBallVel->rDotX = 0;
        stBallVel->rDotY = 0;
    }
    
    uiOldLives = stBallChar->uiLives;

    return xAllBlocksDestroyed;
}