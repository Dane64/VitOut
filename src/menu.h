#ifndef _MENU_H_
#define _MENU_H_

#include <psp2/display.h>
#include <vita2d.h>
#include "inputHandler.h"

#define NROFBOXES 4

vita2d_font *font;

void startVisualizer();
void updateVisualizer();
int drawMenuItems(Console *stVita, unsigned short uiScreenWidth, unsigned short uiScreenHeight);
int selectLevel(Console *stVita, unsigned short uiScreenWidth, unsigned short uiScreenHeight);
void closeVisualizer();
void finishVisualizer();

#endif