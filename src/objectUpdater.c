#include "objectUpdater.h"
#include <math.h>

typedef enum tEnumHitPlane
{
    None,
    Top,
    Bottom,
    Left,
    Right
} tEnumHitPlane;

typedef struct tStIntercept
{
    float x;
    float y;
    tEnumHitPlane ePlane;
} tStIntercept;

float getElapsedtime()
{
    static SceRtcTick uliLast;
    SceRtcTick uliCurrent;
    sceRtcGetCurrentTick(&uliCurrent);
    float tDelta = (uliCurrent.tick - uliLast.tick) / (float)sceRtcGetTickResolution();
    uliLast = uliCurrent;
    return tDelta;
}

bool checkOverlap(tStObject *stBall, tStObject *stRect) // AABB (Axis Aligned Bounding Box)
{
    return  stBall->rX >= (stRect->rX - stBall->uiWidth) &&
            stBall->rX <= (stRect->rX + stRect->uiWidth + stBall->uiWidth) &&
            stBall->rY >= (stRect->rY - stBall->uiHeight) &&
            stBall->rY <= (stRect->rY + stBall->uiHeight + stRect->uiHeight);
}

tStIntercept intercept (float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, tEnumHitPlane ePlane)
{
    tStIntercept stIntercept = {0, 0, None};
    float denom = ((y4-y3) * (x2-x1)) - ((x4-x3) * (y2-y1));
    if (denom != 0)
    {
        float ua = (((x4-x3) * (y1-y3)) - ((y4-y3) * (x1-x3))) / denom;
        if ((ua >= 0) && (ua <= 1))
        {
            float ub = (((x2-x1) * (y1-y3)) - ((y2-y1) * (x1-x3))) / denom;
            if ((ub >= 0) && (ub <= 1))
            {
                stIntercept.x = x1 + (ua * (x2-x1));
                stIntercept.y = y1 + (ua * (y2-y1));
                stIntercept.ePlane = ePlane;
            }
        }
    }

    return stIntercept;
}

tStIntercept ballIntercept (tStObject *stBall, tStObject *stRect, float rDotX, float rDotY)
{
    tStIntercept stIntercept = {0, 0, None};

    if (rDotX < 0)
    {
            stIntercept = intercept(stBall->rX,
                                    stBall->rY,
                                    stBall->rX + rDotX,
                                    stBall->rY + rDotY,
                                    stRect->rX + stRect->uiWidth + stBall->uiWidth, // RIGHT
                                    stRect->rY - stBall->uiHeight, // TOP
                                    stRect->rX + stRect->uiWidth + stBall->uiWidth, // RIGHT
                                    stRect->rY + stRect->uiHeight + stBall->uiHeight, // BOTTOM
                                    Right);
    }
    else if (rDotX > 0)
    {
            stIntercept = intercept(stBall->rX,
                                    stBall->rY,
                                    stBall->rX + rDotX,
                                    stBall->rY + rDotY,
                                    stRect->rX - stBall->uiWidth, // LEFT
                                    stRect->rY - stBall->uiHeight, // TOP
                                    stRect->rX - stBall->uiWidth, // LEFT
                                    stRect->rY + stRect->uiHeight + stBall->uiHeight, // BOTTOM
                                    Left);
    }
    if (stIntercept.ePlane == None)
    {
        if (rDotY < 0)
        {
            stIntercept = intercept(stBall->rX,
                                    stBall->rY,
                                    stBall->rX + rDotX,
                                    stBall->rY + rDotY,
                                    stRect->rX - stBall->uiWidth, // LEFT
                                    stRect->rY + stRect->uiHeight + stBall->uiHeight, // BOTTOM
                                    stRect->rX + stRect->uiWidth + stBall->uiWidth, // RIGHT
                                    stRect->rY + stRect->uiHeight + stBall->uiHeight, // BOTTOM
                                    Bottom);
        }
        else if (rDotY > 0)
        {
            stIntercept = intercept(stBall->rX,
                                    stBall->rY,
                                    stBall->rX + rDotX,
                                    stBall->rY + rDotY,
                                    stRect->rX - stBall->uiWidth, // LEFT
                                    stRect->rY - stBall->uiHeight, // TOP
                                    stRect->rX + stRect->uiWidth + stBall->uiWidth, // RIGHT
                                    stRect->rY - stBall->uiHeight, // TOP
                                    Top);
        }
    }
    return stIntercept;
}

bool checkCollision(tStObject *stBall, tStObject *stFrame, tStObject *stPaddle, tStObject *stBrick, unsigned short uiNumberOfBricks, float rTime)
{
	bool xPaddleCollision = false;
    int iBrickHit = -1;
    float rDistance = 0.0;
    float rClosest = 9999.9;
    tStIntercept stIntercept = {0, 0, None};
    tStIntercept stClosestIntercept = {0, 0, None};

	float rNewBallDotX = stBall->rDotX*rTime;
	float rNewBallDotY = stBall->rDotY*rTime;

    // Check for closest wall collision
    for (int i=0; i<4; i++)
    {  
        if (stFrame[i].xVisible)
        {
            stIntercept = ballIntercept(stBall, &stFrame[i], rNewBallDotX, rNewBallDotY);

            if (stIntercept.ePlane != None)
            {
                rDistance = sqrtf(powf((stIntercept.x-stBall->rX), 2) + powf((stIntercept.y-stBall->rY), 2));
                if (rDistance < rClosest)
                {
                    rClosest = rDistance;
                    stClosestIntercept = stIntercept;
                }
            }
        }
    }

    // Check for paddle collision
    if (stPaddle->xVisible)
    {
        stIntercept = ballIntercept(stBall, stPaddle, rNewBallDotX, rNewBallDotY);

        if (stIntercept.ePlane != None)
        {
            rDistance = sqrtf(powf((stIntercept.x-stBall->rX), 2) + powf((stIntercept.y-stBall->rY), 2));
            if (rDistance < rClosest)
            {
                rClosest = rDistance;
                stClosestIntercept = stIntercept;
                xPaddleCollision = true;
            }
        }
    }

    // Check for closest brick collision
    for (int i=0; i<uiNumberOfBricks; i++)
    {  
        if (stBrick[i].xVisible)
        {
            stIntercept = ballIntercept(stBall, &stBrick[i], rNewBallDotX, rNewBallDotY);

            if (stIntercept.ePlane != None)
            {
                rDistance = sqrtf(powf((stIntercept.x-stBall->rX), 2) + powf((stIntercept.y-stBall->rY), 2));
                if (rDistance < rClosest)
                {
                    rClosest = rDistance;
                    stClosestIntercept = stIntercept;
                    iBrickHit = i;
                }
            }
        }
    }

    if (rClosest < 999.9) // Less than it's original value means it hit something
    {
        stBall->rX = stClosestIntercept.x;
        stBall->rY = stClosestIntercept.y;
        float rIntendedSpeed = sqrtf(powf(rNewBallDotX, 2) + powf(rNewBallDotY, 2));
        float rTimeRemaining = rTime - (rTime * (rClosest / rIntendedSpeed));

        if (xPaddleCollision && stClosestIntercept.ePlane == Top)
        {
            float rBallSpeed = sqrtf(powf(stBall->rDotX, 2) + powf(stBall->rDotY, 2));
            float rMaxLateralSpeed = 350;
            float rNormHit = ((stBall->rX - (stPaddle->rX + (float)stPaddle->uiWidth / 2))  / ((float)stPaddle->uiWidth / 2));
            stBall->rDotX = rMaxLateralSpeed * rNormHit;
            stBall->rDotY = sqrtf(powf(rBallSpeed,2) - powf(stBall->rDotX, 2));
        }
        if (iBrickHit >= 0 )
        {
            float rMaxBallSpeed = 1200;
            float rBallSpeed = sqrtf(powf(stBall->rDotX, 2) + powf(stBall->rDotY, 2));
            stBall->rDotX > 0 ? stBall->rDotX + (5 * (1 - (rBallSpeed / rMaxBallSpeed))) : stBall->rDotX - (5 * (1 - (rBallSpeed / rMaxBallSpeed)));
            stBall->rDotY > 0 ? stBall->rDotY + (5 * (1 - (rBallSpeed / rMaxBallSpeed))) : stBall->rDotY - (5 * (1 - (rBallSpeed / rMaxBallSpeed)));

            stBrick[iBrickHit].uiLives--;
            if (stBrick[iBrickHit].uiLives == 0)
            {
                stBrick[iBrickHit].xVisible^= 1;
            }
        }

        if (stClosestIntercept.ePlane == Left || stClosestIntercept.ePlane == Right)
        {
            stBall->rDotX*= -1;
        }
        else if (stClosestIntercept.ePlane == Top || stClosestIntercept.ePlane == Bottom)
        {
            stBall->rDotY*= -1;
        }

       return checkCollision(stBall, stFrame, stPaddle, stBrick, uiNumberOfBricks, rTimeRemaining); // call recursively but stay on the same depth
    }

    stBall->rX += rNewBallDotX;
    stBall->rY += rNewBallDotY;

    return false;
}

void paddleUpdate(stGamePad *stMcd, tStObject *stPaddle, tStObject *stFrame, float tDelta)
{
    float rMaxPaddleSpeed = 750;
    stPaddle->rDotX = 0;
    stPaddle->rDotY = 0;

    if (stMcd->stJoy[0].siX != 0)
    {
        stPaddle->rDotX = (float)stMcd->stJoy[0].siX * (rMaxPaddleSpeed / 127);
        stPaddle->rX += stPaddle->rDotX * tDelta;
        stPaddle->rX = limit(stFrame[1].rX-stPaddle->uiWidth, stFrame[0].rX+stFrame[0].uiWidth, stPaddle->rX);
    }

    // if (stMcd->stJoy[0].siY != 0)
    // {
    //     stPaddle->rDotY = (float)stMcd->stJoy[0].siY * (rMaxPaddleSpeed / 127);
    //     stPaddle->rY += stPaddle->rDotY;
    //     stPaddle->rY = limit(stFrame[3].rY-stPaddle->uiHeight, stFrame[2].rY+stFrame[2].uiHeight, stPaddle->rY);
    // }
}

void ballUpdate(stGamePad *stMcd, tStObject *stBall, tStObject *stPaddle, tStObject *stFrame, tStObject *stBrick, unsigned short uiNumberOfBricks, float tDelta)
{
    static unsigned short uiOldLives;

    // reset ball position
    if (stBall->rDotX == 0 && stBall->rDotY == 0)
    {
        stBall->rX = stPaddle->rX + (stPaddle->uiWidth / 2);
        stBall->rY = stPaddle->rY - stBall->uiHeight;

        if (stMcd->stButt[1].xTrigger)
        {
            stBall->rDotY = -400;
            stBall->rDotX = -100;
        }
    }

    checkCollision(stBall, stFrame, stPaddle, stBrick, uiNumberOfBricks, tDelta);

    // Check ball position
    if (stBall->rY > (stPaddle->rY + stPaddle->uiHeight + stBall->uiHeight))
    {
        stBall->uiLives--;
    }

    // reset ball speed
    if (stBall->uiLives < uiOldLives)
    {
        stBall->rDotX = 0;
        stBall->rDotY = 0;
    }

    uiOldLives = stBall->uiLives;
}

float brickUpdate(tEnumState eGameState, tStObject *stBrick, unsigned short uiNumberOfBricks, float tDelta)
{
    static float tCum;
    tCum += tDelta;

    if (eGameState == MainMenu && tCum > 0.2)
    {
        for (int i=0; i<uiNumberOfBricks/3; i++)
        {
            stBrick[i].uiLives++;

            if (stBrick[i].uiLives == 6)
            {
                stBrick[i].uiLives = 2;
            }
        }
        tCum = 0.0;
    }
    else if (eGameState == LevelSettings && tCum > 0.2)
    {
        for (int i=uiNumberOfBricks/3; i<uiNumberOfBricks-uiNumberOfBricks/3; i++)
        {
            stBrick[i].uiLives++;

            if (stBrick[i].uiLives == 6)
            {
                stBrick[i].uiLives = 2;
            }
        }
        tCum = 0.0;
    }
    else if (eGameState == Quitting && tCum > 0.2)
    {
        for (int i=uiNumberOfBricks-uiNumberOfBricks/3; i<uiNumberOfBricks; i++)
        {
            stBrick[i].uiLives++;

            if (stBrick[i].uiLives == 6)
            {
                stBrick[i].uiLives = 2;
            }
        }
        tCum = 0.0;
    }

    return tCum;
}

bool checkBricks(tStObject *stBrick, unsigned short uiNumberOfBricks)
{
    bool xAllBricksDestroyed = true;

    for (int i=0; i<uiNumberOfBricks; i++)
    {
        if (stBrick[i].xVisible)
        {
            xAllBricksDestroyed = false;
            break;
        }
    }

    return xAllBricksDestroyed;
}

unsigned char getHighScore(unsigned char usiLevel, unsigned short uiLives, float tGameDuration)
{
    unsigned char usiHighScore = 0;

    float rBronzeTime = 0.0;
    float rSilverTime = 0.0;
    float rGoldTime = 0.0;

    if (usiLevel == 1)
    {
        rBronzeTime = 700.0;
        rSilverTime = 500.0;
        rGoldTime = 300.0;
    }
    else if (usiLevel == 2)
    {
        rBronzeTime = 500.0;
        rSilverTime = 300.0;
        rGoldTime = 220.0;
    }
    else if (usiLevel == 3)
    {
        rBronzeTime = 800.0;
        rSilverTime = 500.0;
        rGoldTime = 400.0;
    }
    else if (usiLevel == 4)
    {
        rBronzeTime = 650.0;
        rSilverTime = 500.0;
        rGoldTime = 380.0;
    }
    else if (usiLevel == 5)
    {
        rBronzeTime = 600.0;
        rSilverTime = 500.0;
        rGoldTime = 430.0;
    }
    else if (usiLevel == 6)
    {
        rBronzeTime = 30.0;
        rSilverTime = 20.0;
        rGoldTime = 10.0;
    }

    if (tGameDuration <= rGoldTime)
    {
        usiHighScore = 20 + uiLives; // limit(250, 0, expf((float)uiLives / 100 * (rGoldTime - tGameDuration)));
    }
    else if (tGameDuration <= rSilverTime)
    {
        usiHighScore = 10 + uiLives;
    }
    else if (tGameDuration <= rBronzeTime)
    {
        usiHighScore = uiLives;
    }
 
    return usiHighScore;
}