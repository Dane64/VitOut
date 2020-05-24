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
    // if (rTime <= 0)
    // {
    //     return true;
    // }

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
        if (iBrickHit >= 0)
        {
            stBrick[iBrickHit].uiLives--;
        }
        stBall->rX = stClosestIntercept.x;
        stBall->rY = stClosestIntercept.y;
        float rIntendedSpeed = sqrtf(powf(rNewBallDotX,2) + powf(rNewBallDotY,2));
        float rTimeRemaining = rTime - (rTime * (rClosest / rIntendedSpeed));

        if (xPaddleCollision && stClosestIntercept.ePlane == Top)
        {
            float rBallSpeed = sqrtf(powf(stBall->rDotX, 2) + powf(stBall->rDotY, 2));
            float rMaxLateralSpeed = 350;
            float rNormHit = ((stBall->rX - (stPaddle->rX + (float)stPaddle->uiWidth / 2))  / ((float)stPaddle->uiWidth / 2));
            stBall->rDotX = rMaxLateralSpeed * rNormHit;//(stPaddle->rDotX / 2) + rNormHit < 0 ? rNormHit * rMaxLateralSpeed - 20 : rNormHit * rMaxLateralSpeed + 20;
            stBall->rDotY = sqrtf(powf(rBallSpeed,2) - powf(stBall->rDotX, 2));
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

void paddleUpdate(stGamePad *stMcd, tStObject *stPaddle, tStObject *stFrame)
{
    static SceRtcTick uliLast;
    SceRtcTick uliCurrent;
    sceRtcGetCurrentTick(&uliCurrent);
    float tDelta = (uliCurrent.tick - uliLast.tick) / (float)sceRtcGetTickResolution();
    uliLast = uliCurrent;

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

void ballUpdate(stGamePad *stMcd, tStObject *stBall, tStObject *stPaddle, tStObject *stFrame, tStObject *stBrick, unsigned short uiNumberOfBricks)
{
    static SceRtcTick uliLast;
    SceRtcTick uliCurrent;
    sceRtcGetCurrentTick(&uliCurrent);
    float tDelta = (uliCurrent.tick - uliLast.tick) / (float)sceRtcGetTickResolution();
    uliLast = uliCurrent;

    // // reset ball position
    // if (stBall->rDotX == 0 && stBall->rDotY == 0)
    // {
    //     stBall->rX = stPaddle->rX + (stPaddle->uiWidth / 2);
    //     stBall->rY = stPaddle->rY - stBall->uiHeight;

    //     if (stMcd->stButt[1].xTrigger)
    //     {
    //         unsigned short lfsr = 0xACE1u;
    //         unsigned bit;
    //         bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
    //         lfsr = (lfsr >> 1) | (bit << 15);
    //         stBall->rDotY = -(lfsr%1000);
    //         stBall->rDotX = -(lfsr%100);
    //     }
    // }

        if (stMcd->stButt[1].xTrigger){
            stBall->rDotY = -400;
            stBall->rDotX = -100;       
        }

    // stBall->rDotX = stMcd->stJoy[1].siX;
    // stBall->rDotY = stMcd->stJoy[1].siY;

    checkCollision(stBall, stFrame, stPaddle, stBrick, uiNumberOfBricks, tDelta);
}

void brickUpdate(tStObject *stBrick, unsigned short uiNumberOfBricks)
{
    for (int i=0; i<uiNumberOfBricks; i++)
    {
        if (stBrick[i].uiLives == 0 && stBrick[i].xVisible)
        {
            stBrick[i].xVisible^= 1;
        }
    }
}