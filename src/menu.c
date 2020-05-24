#include "menu.h"

unsigned char selectLevel(stGamePad *stMcd)
{
    static unsigned char usiLevel;
    if (stMcd->stDpad[0].xRight && stMcd->stDpad[0].xTrigger)
    {
        usiLevel--;
    }
    else if (stMcd->stDpad[0].xLeft && stMcd->stDpad[0].xTrigger)
    {
        usiLevel++;
    }

    usiLevel = limit(MAXLEVEL, 1, usiLevel); // Limits level between 1 and MAXLEVEL

    return usiLevel;
}

void mainMenu(tEnumState *eGameState, stGamePad *stMcd, unsigned char *usiHighScore)
{
    static unsigned char usiLevel = 1;
    
    if (*eGameState == MainMenu)
    {
        if (stMcd->stDpad[0].xDown && stMcd->stDpad[0].xTrigger)
        {
            *eGameState = LevelSettings;
        }
        else if (stMcd->stDpad[0].xUp && stMcd->stDpad[0].xTrigger)
        {
            *eGameState = Quitting;
        }

        if (stMcd->stButt[1].xTrigger || stMcd->stButt[7].xTrigger)
        {
            *eGameState = 10 + usiLevel;
        }
    }
    else if (*eGameState == LevelSettings)
    {
        if (stMcd->stDpad[0].xDown && stMcd->stDpad[0].xTrigger)
        {
            *eGameState = Quitting;
        }
        else if (stMcd->stDpad[0].xUp && stMcd->stDpad[0].xTrigger)
        {
            *eGameState = MainMenu;
        }

        usiLevel = selectLevel(stMcd);
    }
    else if (*eGameState == Quitting)
    {
        if (stMcd->stDpad[0].xDown && stMcd->stDpad[0].xTrigger)
        {
            *eGameState = MainMenu;
        }
        else if (stMcd->stDpad[0].xUp && stMcd->stDpad[0].xTrigger)
        {
            *eGameState = LevelSettings;
        }

        if (stMcd->stButt[1].xTrigger)
        {
            *eGameState = Quit;
        }
    }

    if (*eGameState < Playing)
    {
        showDifficulty(usiLevel);
        showHighscore(usiHighScore[usiLevel-1]);
    }
    
}