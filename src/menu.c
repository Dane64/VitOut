#include "menu.h"

void menu_init(Menu *menu)
{
    *menu = (Menu){.selected = MENU_PLAY, .difficulty = DIFFICULTY_NORMAL};
}

MenuAction menu_update(Menu *menu, const Input *input, size_t level_count)
{
    uint32_t pressed = input->pressed;
    if (menu->level_index >= level_count) {
        menu->level_index = 0;
    }
    if (pressed & INPUT_UP) {
        menu->selected = (MenuItem)((menu->selected + MENU_ITEM_COUNT - 1) % MENU_ITEM_COUNT);
    } else if (pressed & INPUT_DOWN) {
        menu->selected = (MenuItem)((menu->selected + 1) % MENU_ITEM_COUNT);
    }
    int direction = 0;
    if (pressed & (INPUT_LEFT | INPUT_L)) {
        direction = -1;
    } else if (pressed & (INPUT_RIGHT | INPUT_R)) {
        direction = 1;
    }
    if ((pressed & (INPUT_L | INPUT_R)) || menu->selected == MENU_LEVEL) {
        if (level_count > 0 && direction != 0) {
            menu->level_index = direction > 0 ? (menu->level_index + 1) % level_count :
                (menu->level_index + level_count - 1) % level_count;
        }
    } else if (menu->selected == MENU_DIFFICULTY && direction != 0) {
        menu->difficulty = (Difficulty)((menu->difficulty + DIFFICULTY_COUNT + direction) %
                                        DIFFICULTY_COUNT);
    }
    if (pressed & INPUT_SELECT) {
        return MENU_ACTION_RELOAD;
    }
    if ((pressed & INPUT_START) && level_count > 0) {
        return MENU_ACTION_PLAY;
    }
    if (pressed & INPUT_CROSS) {
        switch (menu->selected) {
        case MENU_PLAY:
            return level_count > 0 ? MENU_ACTION_PLAY : MENU_ACTION_NONE;
        case MENU_LEVEL:
            if (level_count > 0) {
                menu->level_index = (menu->level_index + 1) % level_count;
            }
            break;
        case MENU_DIFFICULTY:
            menu->difficulty = (Difficulty)((menu->difficulty + 1) % DIFFICULTY_COUNT);
            break;
        case MENU_RELOAD:
            return MENU_ACTION_RELOAD;
        case MENU_QUIT:
            return MENU_ACTION_QUIT;
        case MENU_ITEM_COUNT:
            break;
        }
    }
    return MENU_ACTION_NONE;
}
