#include "splashScreen.h"

void splash()
{
    unsigned short uiCounter = 0;
    vita2d_texture *logo = vita2d_load_PNG_file("app0:/icons/psv.png");
    
    while(1) {
        updateVisualizer();
        vita2d_draw_texture(logo, 0, 135);
        uiCounter++;

        if (uiCounter == 100) {
            break;
        }

        closeVisualizer();
    }
}