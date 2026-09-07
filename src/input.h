#ifndef VITOUT_INPUT_H
#define VITOUT_INPUT_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    INPUT_UP = 1u << 0,
    INPUT_DOWN = 1u << 1,
    INPUT_LEFT = 1u << 2,
    INPUT_RIGHT = 1u << 3,
    INPUT_CROSS = 1u << 4,
    INPUT_CIRCLE = 1u << 5,
    INPUT_TRIANGLE = 1u << 6,
    INPUT_START = 1u << 7,
    INPUT_SELECT = 1u << 8,
    INPUT_L = 1u << 9,
    INPUT_R = 1u << 10
} InputButton;

typedef struct {
    uint32_t held;
    uint32_t pressed;
    float move;
} Input;

bool input_init(void);
bool input_read(Input *input);

#endif
