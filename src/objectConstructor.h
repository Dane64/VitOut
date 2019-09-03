#ifndef _OBJECTCONSTRUCTOR_H_
#define _OBJECTCONSTRUCTOR_H_

# include <stdbool.h>

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
void blockConstructor(Position *stBlockPos, Characteristics *stBlockChar, Map *stFrame, unsigned short uiNrOfBlocks);

#endif