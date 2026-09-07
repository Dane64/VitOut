#include "input.h"

#include <psp2/ctrl.h>

bool input_init(void)
{
    return sceCtrlSetSamplingMode(SCE_CTRL_MODE_ANALOG_WIDE) >= 0;
}

bool input_read(Input *input)
{
    static const struct {
        uint32_t vita;
        InputButton button;
    } bindings[] = {
        {SCE_CTRL_UP, INPUT_UP}, {SCE_CTRL_DOWN, INPUT_DOWN},
        {SCE_CTRL_LEFT, INPUT_LEFT}, {SCE_CTRL_RIGHT, INPUT_RIGHT},
        {SCE_CTRL_CROSS, INPUT_CROSS}, {SCE_CTRL_CIRCLE, INPUT_CIRCLE},
        {SCE_CTRL_TRIANGLE, INPUT_TRIANGLE}, {SCE_CTRL_START, INPUT_START},
        {SCE_CTRL_SELECT, INPUT_SELECT}, {SCE_CTRL_LTRIGGER, INPUT_L},
        {SCE_CTRL_RTRIGGER, INPUT_R}
    };
    SceCtrlData pad = {0};
    input->pressed = 0;
    input->move = 0.0f;
    if (sceCtrlPeekBufferPositive(0, &pad, 1) <= 0) {
        return false;
    }
    uint32_t held = 0;
    for (unsigned i = 0; i < sizeof(bindings) / sizeof(bindings[0]); ++i) {
        if (pad.buttons & bindings[i].vita) {
            held |= bindings[i].button;
        }
    }
    input->pressed = held & ~input->held;
    input->held = held;
    int axis = (int)pad.lx - 128;
    if (axis > 16) {
        input->move = (float)(axis - 16) / 111.0f;
    } else if (axis < -16) {
        input->move = (float)(axis + 16) / 112.0f;
    }
    if (held & (INPUT_LEFT | INPUT_RIGHT)) {
        input->move = ((held & INPUT_RIGHT) ? 1.0f : 0.0f) -
                      ((held & INPUT_LEFT) ? 1.0f : 0.0f);
    }
    return true;
}
