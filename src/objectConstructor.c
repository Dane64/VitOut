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

void paddleConstructor(Position *stPadPos, Velocity *stPadVel, Characteristics *stPadChar)
{
    stPadPos->rX = 460;
    stPadPos->rY = 500;
    stPadVel->rDotX = 0;
    stPadVel->rDotY = 0;
    stPadChar->uiWidth = 180;
    stPadChar->uiHeight = 30;
}

void ballConstructor(Position *stBallPos, Velocity *stBallVel, Characteristics *stBallChar)
{
    stBallPos->rX = 460;
    stBallPos->rY = 256;
    stBallVel->rDotX = 2;
    stBallVel->rDotY = 2;
    stBallChar->uiWidth = 8;
    stBallChar->uiHeight = 8;
}

void blockConstructor(Position *stBlockPos, Characteristics *stBlockChar, Map *stFrame, unsigned short uiNrOfBlocks)
{
    unsigned short i = 0;
    unsigned short uiBlockAlignment = 2;
    unsigned short uiBlocksPerRow = 20;
    stBlockChar[i].uiWidth = (stFrame->uiRightBorder[1] - stFrame->uiLeftBorder[1] - stFrame->uiFrameThickness - (uiBlockAlignment * uiBlocksPerRow)) / uiBlocksPerRow;
    stBlockChar[i].uiHeight = 10;
    stBlockChar[i].xVisible = true;
    stBlockPos[i].rX = 0 + stFrame->uiFrameThickness;
    stBlockPos[i].rY = 0 + stFrame->uiFrameThickness;

    for (i = 1; i < uiNrOfBlocks; i++)
    {
        stBlockChar[i].xVisible = true;
        stBlockChar[i].uiWidth = stBlockChar[0].uiWidth;
        stBlockChar[i].uiHeight = stBlockChar[0].uiHeight;
        stBlockPos[i].rX = stBlockPos[i-1].rX + stBlockChar[i-1].uiWidth + uiBlockAlignment;
        
        if (stBlockPos[i].rX < (stFrame->uiRightBorder[1] - stBlockChar[i].uiWidth))
        {
            stBlockPos[i].rY = stBlockPos[i-1].rY;
        }
        else
        {
            stBlockPos[i].rX = 0 + stFrame->uiFrameThickness;
            stBlockPos[i].rY = stBlockPos[i-1].rY + stBlockChar[i-1].uiHeight + uiBlockAlignment;
        }
    }
}