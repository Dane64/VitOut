#include "menu.h"

#define BLACK   RGBA8(  0,   0,   0, 255)
#define GREY    RGBA8(111, 111, 111, 255)
#define WHITE   RGBA8(255, 255, 255, 255)
#define RED     RGBA8(255,   0,   0, 255)
#define ORANGE  RGBA8(255, 165,   0, 255)
#define YELLOW  RGBA8(255, 255,   0, 255)
#define GREEN   RGBA8(  0, 255,   0, 255)
#define CYAN    RGBA8(  0, 255, 255, 255)
#define BLUE    RGBA8(  0,   0, 255, 255)
#define INDIGO  RGBA8(111,   0, 255, 255)
#define MAGENTA RGBA8(255,   0, 255, 255)

#define uiLevels 10

unsigned short uiBoxWidth = 300;
unsigned short uiBoxHeight = 45;
bool xWaitForLevelSelection;

vita2d_texture *frame;

void startVisualizer()
{
    vita2d_init();
    font = vita2d_load_font_file("app0:Roboto-Regular.ttf");
    frame = vita2d_load_PNG_file("app0:/icons/textFrame.png");
}

void updateVisualizer()
{
	vita2d_start_drawing();
	vita2d_clear_screen();
}

int drawMenuItems(Console *stVita, unsigned short uiScreenWidth, unsigned short uiScreenHeight)
{

    unsigned short uiCentre = uiScreenWidth/2 - uiBoxWidth/2;
    unsigned short uiMiddle = uiScreenHeight/(NROFBOXES + 1);
    unsigned short uiBoxPosition = 0 + uiMiddle;
	unsigned short uiStartLevel = 0;

    if (xWaitForLevelSelection)
    {
        uiStartLevel = selectLevel(stVita, uiScreenWidth, uiScreenHeight);

        if (uiStartLevel > 0)
        {
           xWaitForLevelSelection = false;
        }
        
        return uiStartLevel;
    }

    vita2d_draw_texture(frame, uiCentre, uiBoxPosition);
    vita2d_font_draw_textf(font, uiCentre + 80, uiBoxPosition+uiBoxHeight, WHITE, uiBoxHeight, "START");

    if (stVita->stFrontTouch.uiX > uiCentre && stVita->stFrontTouch.uiX < uiCentre + uiBoxWidth &&
        stVita->stFrontTouch.uiY > uiBoxPosition && stVita->stFrontTouch.uiY < uiBoxPosition + uiBoxHeight &&
        stVita->stFrontTouch.xRtrig)
    {
        xWaitForLevelSelection = true;
    }

    uiBoxPosition += uiMiddle;

    vita2d_draw_texture(frame, uiCentre, uiBoxPosition);
    vita2d_font_draw_textf(font, uiCentre + 45, uiBoxPosition+uiBoxHeight, WHITE, uiBoxHeight, "SETTINGS");

    uiBoxPosition += uiMiddle;

    vita2d_draw_texture(frame, uiCentre, uiBoxPosition);
    vita2d_font_draw_textf(font, uiCentre + 20, uiBoxPosition+uiBoxHeight, WHITE, uiBoxHeight, "HIGH SCORE");

    uiBoxPosition += uiMiddle;

    vita2d_draw_texture(frame, uiCentre, uiBoxPosition);
    vita2d_font_draw_textf(font, uiCentre + 55, uiBoxPosition+uiBoxHeight, WHITE, uiBoxHeight, "CREDITS");

    return 0;
}

int selectLevel(Console *stVita, unsigned short uiScreenWidth, unsigned short uiScreenHeight)
{
    unsigned short uiLevelSpacing = 15;
    unsigned short uiStartPositionX = uiLevelSpacing;
    unsigned short uiStartPositionY = 25;
    unsigned short uiLevelsPerLine = uiScreenWidth / (uiBoxWidth + uiLevelSpacing);
    unsigned short uiSelectedLevel = 0;

    static const char asLevelText[uiLevels][10] = 
      {
      "ONE",
      "TWO",
      "THREE",
      "FOUR",
      "FIVE",
      "SIX",
      "SEVEN",
      "EIGHT",
      "NINE",
      "TEN"};

    for (unsigned short i = 0; i < uiLevels; i++)
    {
        if (i % uiLevelsPerLine == 0)
        {
            uiStartPositionY += (uiBoxHeight + uiLevelSpacing + 13); // +13 original height of frame is 58 (58-45 = 13)
            uiStartPositionX = uiLevelSpacing;
        }

        vita2d_draw_texture(frame, uiStartPositionX, uiStartPositionY);
        vita2d_font_draw_textf(font, uiStartPositionX+uiLevelSpacing, uiStartPositionY+uiBoxHeight, WHITE, uiBoxHeight, asLevelText[i]);

        if (stVita->stFrontTouch.uiX > uiStartPositionX && stVita->stFrontTouch.uiX < uiStartPositionX + uiBoxWidth &&
            stVita->stFrontTouch.uiY > uiStartPositionY && stVita->stFrontTouch.uiY < uiStartPositionY + uiBoxHeight &&
            stVita->stFrontTouch.xRtrig)
        {
            uiSelectedLevel = i + 1;
            break;
        }

        uiStartPositionX += uiBoxWidth + uiLevelSpacing;
    }

    return uiSelectedLevel;
}

void closeVisualizer()
{
    vita2d_wait_rendering_done();
	vita2d_end_drawing();
	vita2d_swap_buffers();
	sceDisplayWaitVblankStart();
}

void finishVisualizer()
{
    vita2d_fini();
}