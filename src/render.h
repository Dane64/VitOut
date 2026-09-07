#ifndef VITOUT_RENDER_H
#define VITOUT_RENDER_H

#include "game.h"
#include "menu.h"
#include "scores.h"

bool render_init(void);
void render_begin(void);
void render_menu(const Menu *menu, const LevelCatalog *catalog,
                 const Scores *scores, const char *notice);
void render_game(const Game *game, const Level *level, const char *notice);
void render_end(void);
void render_shutdown(void);

#endif
