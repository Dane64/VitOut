#include "objectConstructor.h"

void frameConstructor(Map *stFrame, unsigned short uiScreenWidth, unsigned short uiScreenHeight)
{
    stFrame->uiFrameThickness = 10;

    stFrame->uiLeftBorder[1] = 0;
    stFrame->uiLeftBorder[2] = 0;

    stFrame->uiRightBorder[1] = uiScreenWidth - stFrame->uiFrameThickness;
    stFrame->uiRightBorder[2] = 0;

    stFrame->uiBottomBorder[1] = stFrame->uiFrameThickness;
    stFrame->uiBottomBorder[2] = uiScreenHeight - stFrame->uiFrameThickness;

    stFrame->uiTopBorder[1] = stFrame->uiFrameThickness;
    stFrame->uiTopBorder[2] = 0;
}

void paddleConstructor(Base *stPaddle)
{
    stPaddle->rXPos = 460;
    stPaddle->rYPos = 500;
    stPaddle->uiPsiPos = 0;
}

void ballConstructor(Base *stBall)
{
    stBall->rXPos = 460;
    stBall->rYPos = 256;
    stBall->uiPsiPos = 0;
    stBall->iSpeed = 1;
    stBall->iAngle = 210;
}