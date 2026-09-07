#include "input.h"
#include <psp2/ctrl.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>

static SceCtrlData sample = {.lx = 128};
static int sampling_result;
static int read_result = 1;

int sceCtrlSetSamplingMode(int mode)
{
    assert(mode == SCE_CTRL_MODE_ANALOG_WIDE);
    return sampling_result;
}

int sceCtrlPeekBufferPositive(int port, SceCtrlData *data, int count)
{
    assert(port == 0 && count == 1);
    *data = sample;
    return read_result;
}

int main(void)
{
    Input input = {0};
    assert(input_init());
    sampling_result = -1;
    assert(!input_init());
    assert(input_read(&input));
    assert(input.held == 0 && input.pressed == 0 && input.move == 0.0f);
    sample.buttons = SCE_CTRL_CROSS | SCE_CTRL_UP;
    assert(input_read(&input));
    assert(input.pressed == (INPUT_CROSS | INPUT_UP));
    assert(input_read(&input));
    assert(input.pressed == 0);
    sample.buttons = SCE_CTRL_CROSS | SCE_CTRL_DOWN;
    assert(input_read(&input));
    assert(input.pressed == INPUT_DOWN);
    sample.buttons = 0;
    sample.lx = 144;
    assert(input_read(&input) && input.move == 0.0f);
    sample.lx = 112;
    assert(input_read(&input) && input.move == 0.0f);
    sample.lx = 255;
    assert(input_read(&input) && fabsf(input.move - 1.0f) < 0.001f);
    sample.lx = 0;
    assert(input_read(&input) && fabsf(input.move + 1.0f) < 0.001f);
    sample.buttons = SCE_CTRL_RIGHT;
    assert(input_read(&input) && input.move == 1.0f);
    sample.buttons = SCE_CTRL_LEFT | SCE_CTRL_RIGHT;
    assert(input_read(&input) && input.move == 0.0f);
    sample.buttons = SCE_CTRL_LEFT;
    read_result = -1;
    assert(!input_read(&input));
    assert(input.pressed == 0 && input.move == 0.0f);
    read_result = 1;
    assert(input_read(&input));
    assert(input.pressed == 0 && input.move == -1.0f);
    sample.buttons = SCE_CTRL_SELECT | SCE_CTRL_START | SCE_CTRL_TRIANGLE |
                     SCE_CTRL_CIRCLE | SCE_CTRL_LTRIGGER | SCE_CTRL_RTRIGGER;
    assert(input_read(&input));
    assert(input.pressed == (INPUT_SELECT | INPUT_START | INPUT_TRIANGLE |
                             INPUT_CIRCLE | INPUT_L | INPUT_R));
    puts("Controller edge, deadzone and failure cases passed.");
    return 0;
}
