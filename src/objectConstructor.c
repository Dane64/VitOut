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

void levelConstructor(Position *stBlockPos, Characteristics *stBlockChar, Map *stFrame, unsigned short uiStartLevel, unsigned short uiNrOfBlocks){
    memset(stBlockChar, 0, uiNrOfBlocks * sizeof(*stBlockChar));
    unsigned short i = 0;
    unsigned short j = 0;
    char sObject = 0;
    unsigned short auiPos[2];
    char asLevelCopy[ROW][COL];

    switch (uiStartLevel)
    {
        case 1:
            for (i = 0; i < ROW; i++)
            {
                strcpy(asLevelCopy[i], asLevelOne[i]);
            }
            break;

        case 2:
            for (i = 0; i < ROW; i++)
            {
                strcpy(asLevelCopy[i], asLevelTwo[i]);
            }
            break;        
        
        case 3:
            for (i = 0; i < ROW; i++)
            {
                strcpy(asLevelCopy[i], asLevelThree[i]);
            }
            break;

        case 4:
            for (i = 0; i < ROW; i++)
            {
                strcpy(asLevelCopy[i], asLevelFour[i]);
            }
            break;
            
        default:
            break;
    }

    for (i = 0; i < ROW; i++)
    {
        for (j = 0; j < COL; j++)
        {
            sObject = asLevelCopy[i][j];
            if(sObject >= 32){
                auiPos[0] = i;
                auiPos[1] = j;
                blockConstructor(stBlockPos, stBlockChar, stFrame, auiPos,sObject);
            }
        }
    }
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
    stBallPos->rY = 490;
    stBallVel->rDotX = 0;
    stBallVel->rDotY = 0;
    stBallChar->uiLives = 3;
    stBallChar->uiWidth = 8;
    stBallChar->uiHeight = 8;
}

void blockConstructor(Position *stBlockPos, Characteristics *stBlockChar, Map *stFrame, unsigned short auiPos[2], char uiBlockType)
{
    unsigned short k = (auiPos[0] * COL) + auiPos[1];
    unsigned short uiBlockAlignment = 2;
    stBlockChar[k].uiWidth = (stFrame->uiRightBorder[1] - stFrame->uiLeftBorder[1] - stFrame->uiFrameThickness - (uiBlockAlignment * COL)) / COL;
    stBlockChar[k].uiHeight = 10;

    if(uiBlockType == 32){
        stBlockChar[k].xVisible = false;
        stBlockChar[k].uiLives = 0;
    }

    else if(uiBlockType == 66){
        stBlockChar[k].xVisible = true;
        stBlockChar[k].uiLives = 6;
    }

    else if(uiBlockType == 75){
        stBlockChar[k].xVisible = true;
        stBlockChar[k].uiLives = 1;
    }

    else if(uiBlockType == 87){
        stBlockChar[k].xVisible = true;
        stBlockChar[k].uiLives = 4;
    }

    else if(uiBlockType == 98){
        stBlockChar[k].xVisible = true;
        stBlockChar[k].uiLives = 2;
    }

    else if(uiBlockType == 112){
        stBlockChar[k].xVisible = true;
        stBlockChar[k].uiLives = 3;
    }

    else if(uiBlockType == 119){
        stBlockChar[k].xVisible = true;
        stBlockChar[k].uiLives = 5;
    }

    else
    {
        stBlockChar[k].xVisible = false;
        stBlockChar[k].uiLives = 0;
    }
    
    stBlockPos[k].rX = auiPos[1] * (stBlockChar[k].uiWidth + uiBlockAlignment) + stFrame->uiFrameThickness;
    stBlockPos[k].rY = auiPos[0] * (stBlockChar[k].uiHeight + uiBlockAlignment) + stFrame->uiFrameThickness;
}