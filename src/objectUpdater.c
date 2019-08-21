#include "objectUpdater.h"

#define PI 3.14159265
#define Deg2Rad PI/180

void paddleUpdate(Base *stPaddle, Console *stVita)
{
    if (stVita->stLeftJoy.siX > 10 | stVita->stLeftJoy.siX < -10) {
        stPaddle->rXPos += stVita->stLeftJoy.siX; 
    }
}

void ballUpdate(Base *stBall, Map *stFrame)
{
    float rAngle = (float)(stBall->iAngle) * Deg2Rad;
    stBall->rXPos += stBall->iSpeed * cosf(rAngle);
    stBall->rYPos += stBall->iSpeed * sinf(rAngle);

    if (stBall->rYPos < stFrame->uiTopBorder[2]) 
    {
        stBall->iSpeed *= -1;
        stBall->iAngle += 90;
    }
    else if (stBall->rYPos > stFrame->uiBottomBorder[2])
    {
        stBall->iSpeed *= -1;
        stBall->iAngle += 90;
    }

    if (stBall->rXPos <= stFrame->uiLeftBorder[1])
    {
        stBall->iSpeed *= -1;
        stBall->iAngle += 90;
    }
    else if (stBall->rXPos > stFrame->uiRightBorder[1])
    {
        stBall->iSpeed *= -1;
        stBall->iAngle += 90;
    }
}