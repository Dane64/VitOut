#include "objectConstructor.h"
#include "objectVisualizer.h"
#include "vitasdk.h"
#include <stdlib.h>
#include <string.h>

static const char asLevelOne[ROW][COL] = 
	{
      "             W                ",
      "             WW               ",
      "             WWW              ",
      "             WWWW             ",
      "             WWWWWWW          ",
      "             WWWW  WWWW       ",
      "             WWWW  WWWWW      ",
      "             WWWW  WWWWWW     ",
      "             WWWW  WWWWW      ",
      "             WWWW  WWWW       ",
      "         WWW WWWW  WWW        ",
      "       WWWWW WWWW             ",
      "     WWWWWWW WWWW    W        ",
      "   WWWWW     WWWW  WWWWW      ",
      "   WW        WWWW WWWWWWWW    ",
      "    WWWWWWWW WWWW        WW   ",
      "      WWWWW  WWWW     WWWWW   ",
      "        W     WWW WWWWWWW     ",
      "               WW WWWWW       ",
      "                W WWW         "};

static const char asLevelTwo[ROW][COL] = 
	{
      "K                             ",
      "KKK                           ",
      "KKKKK                         ",
      "KKKKKKK                       ",
      "KKKKKKKKK                     ",
      "KKKKKKKKKK                    ",
      "KKKKKKKKKKK                   ",
      "KKKKKKKKKKKKK                 ",
      "KKKKKKKKKKKKKK                ",
      "KKKKKKKKKKKKKKK               ",
      "KKKKKKKKKKKKKKKK              ",
      "KKKKKKKKKKKKKKKKKK            ",
      "bbbbbbbbbbbbbbbbbbbbbb        ",
      "bbbbbbbbbbbbbbbbbbbbbbb       ",
      "bbbbbbbbbbbbbbbbbbbbbbbbb     ",
      "bbbbbbbbbbbbbbbbbbbbbbbbbb    ",
      "bbbbbbbbbbbbbbbbbbbbbbbbbbb   ",
      "bbbbbbbbbbbbbbbbbbbbbbbbbbbb  ",
      "bbbbbbbbbbbbbbbbbbbbbbbbbbbbb ",
      "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"};

static const char asLevelThree[ROW][COL] = 
      {
      "                              ",
      "                              ",
      "                              ",
      "                              ",
      "                              ",
      "bbbbbKKKKKbbbbbKKKKKbbbbbKKKKK",
      "bbbbbKKKKKbbbbbKKKKKbbbbbKKKKK",
      "bbbbbKKKKKbbbbbKKKKKbbbbbKKKKK",
      "bbbbbKKKKKbbbbbKKKKKbbbbbKKKKK",
      "bbbbbKKKKKbbbbbKKKKKbbbbbKKKKK",
      "bbbbbKKKKKbbbbbKKKKKbbbbbKKKKK",
      "bbbbbKKKKKbbbbbKKKKKbbbbbKKKKK",
      "bbbbbKKKKKbbbbbKKKKKbbbbbKKKKK",
      "bbbbbKKKKKbbbbbKKKKKbbbbbKKKKK",
      "bbbbbKKKKKbbbbbKKKKKbbbbbKKKKK",
      "bbbbbKKKKKbbbbbKKKKKbbbbbKKKKK",
      "bbbbbKKKKKbbbbbKKKKKbbbbbKKKKK",
      "bbbbbKKKKKbbbbbKKKKKbbbbbKKKKK",
      "bbbbbKKKKKbbbbbKKKKKbbbbbKKKKK",
      "bbbbbKKKKKbbbbbKKKKKbbbbbKKKKK"};

static const char asLevelFour[ROW][COL] = 
	{
      " bbbb                    bbbb ",
      " bpppbbbbb          bbbbbpppb ",
      " bppppppppbb      bbppppppppb ",
      " bppppppppppbbbbbbppppppppppb ",
      " bppppppwwwwwwwwwwwwwwppppppb ",
      "  bpppwwwwwwwwwwwwwwwwwwpppb  ",
      "  wpwwwwwwwwwwWWwwwwwwwwwwpw  ",
      " wwwwwwwwwwwwWWWWwwwwwwwwwwww ",
      " wwwwwwwKKKKKWWWWKKKKKwwwwwww ",
      " wwwwwwwKKBKKWWWWKKBKKwwwwwww ",
      " wwwwwwwKKKKKWWWWKKKKKwwwwwww ",
      "wwwwwwwwwwwWWWWWWWWwwwwwwwwwww",
      "wwwwwwwwwwWWWWWWWWWWwwwwwwwwww",
      "wwwwwwwwwWWWWWppWWWWWwwwwwwwww",
      "wwwwwwwwwWWWppppppWWWwwwwwwwww",
      " wwwwwwwWWWWWWppWWWWWWwwwwwww ",
      "   wwwwWWWWWWWBBWWWWWWWwwww   ",
      "     wWWWWWBBBBBBBBWWWWWw     ",
      "       wWWWWWWWWWWWWWWw       ",
      "      wWWWWWWWWWWWWWWWWw      "};

static const char asLevelFive[ROW][COL] = 
	{
      "              BB              ",
      "         WWWWWBBWWWWW         ",
      "        WWWWWWWWWWWWWW        ",
      "       WW            WW       ",
      "      WW              WW      ",
      "     WW                WW     ",
      "  WWWW                  WWWW  ",
      " WWWW                    WWWW ",
      "  WWWWWWWWWWWWWWWWWWWWWWWWWW  ",
      " WBKKKKWWWWWWWWWWWWWWWWKKKKBW ",
      "WWBKKKKKWWWWWWWWWWWWWWKKKKKBWW",
      "WWWKKKKKKWWWBBBBBBWWWKKKKKKWWW",
      "WWWWKKKKKKWBBBBBBBBWKKKKKKWWWW",
      " WWWWWWWWWWWWWWWWWWWWWWWWWWWW ",
      " BWWWWWWBBBBwwwwwwBBBBWWWWWWB ",
      " BWWWWWBBBBBwwwwwwBBBBBWWWWWB ",
      " BWWWWBBWBBBBBBBBBBBBWBBWWWWB ",
      " BBWWWWWW            WWWWWWBB ",
      " BBBBB                  BBBBB ",
      " BBBBB                  BBBBB "};

static const char asLevelSix[ROW][COL] = 
	{
      "                              ",
      "                              ",
      "                              ",
      "                              ",
      "                              ",
      "                              ",
      "                              ",
      "                              ",
      "                              ",
      "                              ",
      "                              ",
      "                              ",
      "                              ",
      "                              ",
      "                              ",
      "                              ",
      "                              ",
      "                              ",
      "                              ",
      "              R               "};

static const char asMainMenu[ROW][COL] = 
      {
      "                              ",
      "  RRR RRRRR  RR   RRRR  RRRRR ",
      "  R     R   R  R  R   R   R   ",
      "  RRR   R  RRRRRR RRRR    R   ",
      "    R   R  R    R R  R    R   ",
      "  RRR   R  R    R R   R   R   ",
      "                              ",
      "                              ",
      " K    KKKKK K    K KKKKK K    ",
      " K    K     K    K K     K    ",
      " K    KKKK   K  K  KKKKK K    ",
      " K    K      K  K  K     K    ",
      " KKKK KKKKK   KK   KKKKK KKKK ",
      "                              ",
      "                              ",
      "    WWWW  W   W WWWWW WWWWW   ",
      "    W  W  W   W   W     W     ",
      "    W  W  W   W   W     W     ",
      "    W  W  W   W   W     W     ",
      "    WWWWW WWWWW WWWWW   W     "};


void frameConstructor(tStObject *stFrame)
{
    stFrame[0].uiWidth = 10; // LEFT FRAME
    stFrame[0].uiHeight = HEIGHT;
    stFrame[0].rX = 0;
    stFrame[0].rY = 0;
    stFrame[0].xVisible = true;
    stFrame[0].luiColor = DARKGREY;

    stFrame[1].uiWidth = 10; // RIGHT FRAME
    stFrame[1].uiHeight = HEIGHT;
    stFrame[1].rX = WIDTH-stFrame[1].uiWidth;
    stFrame[1].rY = 0;
    stFrame[1].xVisible = true;
    stFrame[1].luiColor = DARKGREY;

    stFrame[2].uiHeight = 10;  // TOP FRAME
    stFrame[2].uiWidth = WIDTH;
    stFrame[2].rX = 0;
    stFrame[2].rY = 0;
    stFrame[2].xVisible = true;
    stFrame[2].luiColor = DARKGREY;

    stFrame[3].uiHeight = 10; // BOTTOM FRAME
    stFrame[3].uiWidth = WIDTH;
    stFrame[3].rX = 0;
    stFrame[3].rY = HEIGHT-stFrame[3].uiHeight;
    stFrame[3].xVisible = false;
    stFrame[3].luiColor = DARKGREY;
}

void paddleConstructor(tStObject *stPaddle)
{
    stPaddle->rX = WIDTH/2-180/2;
    stPaddle->rY = 500.0;
    stPaddle->rDotX = 0.0;
    stPaddle->rDotY = 0.0;
    stPaddle->uiWidth = 180;
    stPaddle->uiHeight = 30;
    stPaddle->xVisible = true;
    stPaddle->luiColor = RED;
}

void ballConstructor(tStObject *stBall)
{
    stBall->rX = WIDTH/2;
    stBall->rY = 492;
    stBall->rDotX = 0;
    stBall->rDotY = 0;
    stBall->uiLives = 3;
    stBall->uiWidth = 8;
    stBall->uiHeight = 8;
    stBall->luiColor = MAGENTA;
}

void brickConstructor(tStObject *stFrame, tStObject *stBrick, unsigned short i, unsigned short j, char s)
{
    unsigned short uiBrickAlignment = 1;
    unsigned short k = (i * COL) + j;
    unsigned short uiCenterOffset = 5;
    stBrick[k].uiWidth = 30; // 30 COLS fixed width
    stBrick[k].uiHeight = 15; // 20 ROWS fixed height

    if(s == ' '){
        stBrick[k].xVisible = false;
        stBrick[k].uiLives = 0;
    }
    else if(s == 'B'){
        stBrick[k].xVisible = true;
        stBrick[k].uiLives = 1;
        stBrick[k].luiColor = BLACK;
    }
    else if(s == 'K'){
        stBrick[k].xVisible = true;
        stBrick[k].uiLives = 2;
        stBrick[k].luiColor = ORANGE;
    }
    else if(s == 'R'){
        stBrick[k].xVisible = true;
        stBrick[k].uiLives = 2;
        stBrick[k].luiColor = RED;
    }
    else if(s == 'W'){
        stBrick[k].xVisible = true;
        stBrick[k].uiLives = 2;
        stBrick[k].luiColor = WHITE;
    }
    else if(s == 'b'){
        stBrick[k].xVisible = true;
        stBrick[k].uiLives = 1;
        stBrick[k].luiColor = BROWN;
    }
    else if(s == 'p'){
        stBrick[k].xVisible = true;
        stBrick[k].uiLives = 2;
        stBrick[k].luiColor = PINK;
    }
    else if(s == 'w'){
        stBrick[k].xVisible = true;
        stBrick[k].uiLives = 3;
        stBrick[k].luiColor = WHEAT;
    }

    stBrick[k].rX = j * (stBrick[k].uiWidth + uiBrickAlignment) + stFrame->uiWidth + uiCenterOffset;
    stBrick[k].rY = i * (stBrick[k].uiHeight + uiBrickAlignment) + stFrame->uiWidth;
}

void levelConstructor(unsigned char usiLevel, tStObject *stFrame, tStObject *stBrick, unsigned short uiNumberOfBricks)
{
    memset(stBrick, 0, uiNumberOfBricks * sizeof(tStObject));
    char asLevelCopy[ROW][COL];

    switch(usiLevel)
    {
        case 1:
            memcpy(*asLevelCopy, *asLevelOne, (sizeof(unsigned char) * ROW * COL));
            break;

        case 2:
            memcpy(*asLevelCopy, *asLevelTwo, (sizeof(unsigned char) * ROW * COL));
            break;

        case 3:
            memcpy(*asLevelCopy, *asLevelThree, (sizeof(unsigned char) * ROW * COL));
            break;

         case 4:
            memcpy(*asLevelCopy, *asLevelFour, (sizeof(unsigned char) * ROW * COL));
            break;

        case 5:
            memcpy(*asLevelCopy, *asLevelFive, (sizeof(unsigned char) * ROW * COL));
            break;

        case 6:
            memcpy(*asLevelCopy, *asLevelSix, (sizeof(unsigned char) * ROW * COL));
            break;

        default:
            memcpy(*asLevelCopy, *asMainMenu, (sizeof(unsigned char) * ROW * COL));
            break;
    }

    for (unsigned short i = 0; i < ROW; i++)
    {
        for (unsigned short j = 0; j < COL; j++)
        {
            char sObject = asLevelCopy[i][j];
            if(sObject > 31){
                brickConstructor(stFrame, stBrick, i, j, sObject);
            }
        }
    }   
}

bool loadGame(unsigned char *usiHighScore)
{
    SceUID fd = sceIoOpen("ux0:data/VitOut.save", SCE_O_RDONLY , 0777);
    if (fd < 0)
    {
        return 0;
    }

    sceIoRead(fd, usiHighScore, MAXLEVEL);
    sceIoClose(fd);

    return 1;
}

bool saveGame(unsigned char *usiHighScore)
{
    SceUID fd = sceIoOpen("ux0:data/VitOut.save", SCE_O_TRUNC | SCE_O_WRONLY | SCE_O_CREAT, 0777);
    if (fd < 0)
    {
        return 0;
    }

    sceIoWrite(fd, usiHighScore, MAXLEVEL);
    sceIoClose(fd);

    return 1;   
}