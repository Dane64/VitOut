#ifndef VITOUT_TEST_CTRL_H
#define VITOUT_TEST_CTRL_H

#include <stdint.h>

#define SCE_CTRL_SELECT UINT32_C(0x000001)
#define SCE_CTRL_START UINT32_C(0x000008)
#define SCE_CTRL_UP UINT32_C(0x000010)
#define SCE_CTRL_RIGHT UINT32_C(0x000020)
#define SCE_CTRL_DOWN UINT32_C(0x000040)
#define SCE_CTRL_LEFT UINT32_C(0x000080)
#define SCE_CTRL_LTRIGGER UINT32_C(0x000100)
#define SCE_CTRL_RTRIGGER UINT32_C(0x000200)
#define SCE_CTRL_TRIANGLE UINT32_C(0x001000)
#define SCE_CTRL_CIRCLE UINT32_C(0x002000)
#define SCE_CTRL_CROSS UINT32_C(0x004000)
#define SCE_CTRL_MODE_ANALOG_WIDE 2

typedef struct {
    uint32_t buttons;
    unsigned char lx;
} SceCtrlData;

int sceCtrlSetSamplingMode(int mode);
int sceCtrlPeekBufferPositive(int port, SceCtrlData *data, int count);

#endif
