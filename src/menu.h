#ifndef VITOUT_MENU_H
#define VITOUT_MENU_H

#include "game.h"
#include "input.h"

typedef enum {
    MENU_PLAY,
    MENU_LEVEL,
    MENU_DIFFICULTY,
    MENU_RELOAD,
    MENU_QUIT,
    MENU_ITEM_COUNT
} MenuItem;

typedef enum {
    MENU_ACTION_NONE,
    MENU_ACTION_PLAY,
    MENU_ACTION_RELOAD,
    MENU_ACTION_QUIT
} MenuAction;

typedef struct {
    MenuItem selected;
    size_t level_index;
    Difficulty difficulty;
} Menu;

void menu_init(Menu *menu);
MenuAction menu_update(Menu *menu, const Input *input, size_t level_count);

#endif
