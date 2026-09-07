#include "render.h"

#include <psp2/ctrl.h>
#include <psp2/kernel/processmgr.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static unsigned frame;
static unsigned first_result;
static unsigned second_result;
static bool second_run;
static bool reload_observed;
static float paused_elapsed;

static void add_level(void)
{
    FILE *file = fopen("ux0:/data/VitOut/levels/00-added.lvl", "w");
    assert(file);
    assert(fputs("VITOUT_LEVEL 1\nid=hot-added\nname=Hot Added\n"
                 "times=10,20,30\n[bricks]\n", file) >= 0);
    for (size_t row = 0; row < LEVEL_ROWS; ++row) {
        for (size_t column = 0; column < LEVEL_COLS; ++column) {
            assert(fputc(row == 3 && column == 17 ? '1' : '.', file) != EOF);
        }
        assert(fputc('\n', file) != EOF);
    }
    assert(fclose(file) == 0);
}

int sceCtrlSetSamplingMode(int mode)
{
    assert(mode == SCE_CTRL_MODE_ANALOG_WIDE);
    return 0;
}

int sceCtrlPeekBufferPositive(int port, SceCtrlData *data, int count)
{
    static const uint32_t opening[] = {
        SCE_CTRL_DOWN, SCE_CTRL_RIGHT, SCE_CTRL_DOWN, SCE_CTRL_RIGHT,
        SCE_CTRL_SELECT, SCE_CTRL_START, 0, SCE_CTRL_START,
        0, SCE_CTRL_CROSS, 0, SCE_CTRL_CROSS, 0, SCE_CTRL_START, 0, SCE_CTRL_CROSS
    };
    assert(port == 0 && count == 1 && frame < 1000);
    *data = (SceCtrlData){.lx = 128};
    if (frame == 4) {
        add_level();
    }
    if (frame < sizeof(opening) / sizeof(opening[0])) {
        data->buttons = opening[frame];
    } else if (second_result > 0) {
        unsigned elapsed = frame - second_result;
        if (elapsed == 2) {
            data->buttons = SCE_CTRL_CIRCLE;
        } else if (elapsed == 3 || elapsed == 5) {
            data->buttons = SCE_CTRL_DOWN;
        } else if (elapsed == 6) {
            data->buttons = SCE_CTRL_CROSS;
        }
    } else if (first_result > 0) {
        unsigned elapsed = frame - first_result;
        if (elapsed == 3) {
            data->buttons = SCE_CTRL_CIRCLE;
        } else if (elapsed == 4) {
            data->buttons = SCE_CTRL_RIGHT;
        } else if (elapsed == 5) {
            second_run = true;
            data->buttons = SCE_CTRL_START;
        } else if (elapsed == 7) {
            data->buttons = SCE_CTRL_CROSS;
        }
    }
    return 1;
}

uint64_t sceKernelGetProcessTimeWide(void)
{
    static uint64_t microseconds = UINT64_C(900000000000);
    microseconds += frame == 14 ? UINT64_C(60000000) : UINT64_C(16667);
    return microseconds;
}

int sceKernelExitProcess(int result)
{
    assert(result == 0 && first_result > 0 && second_result > 0 && reload_observed);
    Scores scores;
    char error[LEVEL_MESSAGE_SIZE];
    assert(scores_load(&scores, "ux0:/data/VitOut/scores.dat", NULL, error, sizeof(error)));
    assert(scores.count == 2);
    const ScoreRecord *hard = scores_find(&scores, "fixture-second", DIFFICULTY_HARD);
    const ScoreRecord *easy = scores_find(&scores, "fixture-second", DIFFICULTY_EASY);
    assert(hard && easy);
    assert(hard->medal == 3 && easy->medal == 3);
    assert(hard->lives == 2 && easy->lives == 5);
    assert(hard->points == 150 && easy->points == 50);
    assert(hard->time_ms > 0 && hard->time_ms < 2000);
    assert(easy->time_ms > 0 && easy->time_ms < 2000);
    assert(!scores_find(&scores, "hot-added", DIFFICULTY_HARD));
    puts("App hot reload, selection identity, pause/resume, wins and repeated saves passed.");
    return 0;
}

bool render_init(void)
{
    return true;
}

void render_begin(void)
{
}

void render_menu(const Menu *menu, const LevelCatalog *catalog,
                 const Scores *scores, const char *notice)
{
    (void)scores;
    assert(strstr(notice, "unavailable") == NULL);
    if (frame < 4) {
        assert(catalog->count == 2);
    } else {
        assert(catalog->count == 3);
        assert(menu->level_index == 2);
        assert(strcmp(catalog->levels[menu->level_index].id, "fixture-second") == 0);
        reload_observed = true;
    }
    if (frame == 3) {
        assert(menu->difficulty == DIFFICULTY_HARD);
    }
}

void render_game(const Game *game, const Level *level, const char *notice)
{
    assert(notice[0] == '\0');
    assert(strcmp(level->id, "fixture-second") == 0);
    assert(game->difficulty == (second_run ? DIFFICULTY_EASY : DIFFICULTY_HARD));
    if (frame == 5 || frame == 9) {
        assert(game->phase == GAME_READY);
        assert(game->elapsed == 0.0f);
    } else if (frame == 7) {
        assert(game->phase == GAME_PAUSED);
        assert(game->elapsed == 0.0f);
    } else if (frame == 11) {
        assert(game->phase == GAME_RUNNING);
        assert(game->elapsed > 0.0f);
    } else if (frame == 13) {
        assert(game->phase == GAME_PAUSED);
        paused_elapsed = game->elapsed;
    } else if (frame == 14 || frame == 15) {
        assert(game->phase == (frame == 14 ? GAME_PAUSED : GAME_RUNNING));
        assert(fabsf(game->elapsed - paused_elapsed) < 0.0001f);
    }
    assert(game->phase != GAME_LOST);
    if (game->phase == GAME_WON) {
        if (second_run && second_result == 0) {
            second_result = frame;
        } else if (!second_run && first_result == 0) {
            first_result = frame;
        }
    }
}

void render_end(void)
{
    ++frame;
}

void render_shutdown(void)
{
    assert(first_result > 0 && second_result > 0);
}
