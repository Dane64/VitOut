#include "menu.h"

unsigned char selectLevel(stGamePad *stMcd)
{
    static unsigned char usiLevel;
    if (stMcd->stDpad[0].xDown && stMcd->stDpad[0].xTrigger)
    {
        usiLevel--;
    }
    else if (stMcd->stDpad[0].xUp && stMcd->stDpad[0].xTrigger)
    {
        usiLevel++;
    }

    usiLevel = limit(5, 1, usiLevel); // Limits level between 1 and 5

    return usiLevel;
}

void mainMenu(tEnumState *eGameState, stGamePad *stMcd)
{
    if (*eGameState == MainMenu)
    {
        unsigned char usiLevel = selectLevel(stMcd);
        showDifficulty(usiLevel);
        if (stMcd->stButt[1].xTrigger)
        {
            *eGameState = 10 + usiLevel;
        }
    }
}