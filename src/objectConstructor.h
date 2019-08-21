#ifndef _OBJECTCONSTRUCTOR_H_
#define _OBJECTCONSTRUCTOR_H_

typedef struct Base
{
	float rXPos;
	float rYPos;
    unsigned short uiPsiPos;
	signed short iJerk;
	signed short iAccel;
	signed short iSpeed;
    signed short iAngle;
} Base;

typedef struct Map
{
	unsigned short uiFrameThickness;
	unsigned short uiLeftBorder[2];
	unsigned short uiRightBorder[2];
	unsigned short uiBottomBorder[2];
	unsigned short uiTopBorder[2];
} Map;

void frameConstructor(Map *stFrame, unsigned short uiScreenWidth, unsigned short uiScreenHeight);
void paddleConstructor(Base *stPaddle);
void ballConstructor(Base *stBall);
void blockConstructor();

#endif