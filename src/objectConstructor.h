#ifndef _OBJECTCONSTRUCTOR_H_
#define _OBJECTCONSTRUCTOR_H_

#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define ROW 20
#define COL 30

static const char asLevelOne[ROW][COL] = 
      {
      "",
      "",
      "",
      "",
      "",
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

static const char asLevelTwo[ROW][COL] = 
	{
      "K",
      "KKK",
      "KKKKK",
      "KKKKKKK",
      "KKKKKKKKK",
      "KKKKKKKKKK",
      "KKKKKKKKKKK",
      "KKKKKKKKKKKKK",
      "KKKKKKKKKKKKKK",
      "KKKKKKKKKKKKKKK",
      "KKKKKKKKKKKKKKKK",
      "KKKKKKKKKKKKKKKKKK",
      "bbbbbbbbbbbbbbbbbbbbbb",
      "bbbbbbbbbbbbbbbbbbbbbbb",
      "bbbbbbbbbbbbbbbbbbbbbbbbb",
      "bbbbbbbbbbbbbbbbbbbbbbbbbb",
      "bbbbbbbbbbbbbbbbbbbbbbbbbbb",
      "bbbbbbbbbbbbbbbbbbbbbbbbbbbb",
      "bbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
      "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"};

static const char asLevelThree[ROW][COL] = 
	{
      "             bbb",
      "             bbb",
      "             bbb",
      "          KKKKKKKKK",
      "          KKKKKKKKK",
      "             bbb",
      "             bKb",
      "             bKb",
      "             bKb",
      "             bKb",
      "             bKb",
      "              bKb",
      "               bKb",
      "              bKb",
      "             bKb",
      "              bKb",
      "              bKb",
      "               b",
      "",
      ""};

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

typedef struct Position
{
	float rX;
	float rY;
}
Position;

typedef struct Velocity
{
	float rDotX;
	float rDotY;
}
Velocity;

typedef struct Characteristics
{
	unsigned short uiHeight;
	unsigned short uiWidth;
	unsigned short uiLives;
	bool xVisible;
}
Characteristics;

typedef struct Map
{
	unsigned short uiFrameThickness;
	unsigned short uiLeftBorder[2];
	unsigned short uiRightBorder[2];
	unsigned short uiBottomBorder[2];
	unsigned short uiTopBorder[2];
}
Map;

void frameConstructor(Map *stFrame, unsigned short uiScreenWidth, unsigned short uiScreenHeight);
void paddleConstructor(Position *stPadPos, Velocity *stPadVel, Characteristics *stPadChar);
void ballConstructor(Position *stBallPos, Velocity *stBallVel, Characteristics *stBallChar);
void levelConstructor(Position *stBlockPos, Characteristics *stBlockChar, Map *stFrame, unsigned short uiStartLevel, unsigned short uiNrOfBlocks);
void blockConstructor(Position *stBlockPos, Characteristics *stBlockChar, Map *stFrame, unsigned short auiPos[2], char uiBlockType);

#endif