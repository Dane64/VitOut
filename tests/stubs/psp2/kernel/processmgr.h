#ifndef VITOUT_TEST_PROCESSMGR_H
#define VITOUT_TEST_PROCESSMGR_H

#include <stdint.h>

int sceKernelExitProcess(int result);
uint64_t sceKernelGetProcessTimeWide(void);

#endif
