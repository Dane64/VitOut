#include "render.h"

#include <vita2d.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define BACKGROUND UINT32_C(0xff211a13)
#define PANEL UINT32_C(0xff34281e)
#define SELECTED UINT32_C(0xff655234)
#define WHITE UINT32_C(0xfff5f1e9)
#define MUTED UINT32_C(0xffb6a699)
#define ACCENT UINT32_C(0xffd9db5b)
#define BORDER UINT32_C(0xff554433)

static vita2d_pgf *font;

static void text(int x, int y, uint32_t color, float scale, const char *value)
{
    vita2d_pgf_draw_text(font, x, y, color, scale, value);
}

static void textf(int x, int y, uint32_t color, float scale, const char *format, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    text(x, y, color, scale, buffer);
}

static void fitted_text(int x, int y, int width, uint32_t color, float scale,
                        const char *value)
{
    int actual = vita2d_pgf_text_width(font, scale, value);
    if (actual > width) {
        scale *= (float)width / (float)actual;
    }
    text(x, y, color, scale, value);
}

static void centered_text(int y, uint32_t color, float scale, const char *value)
{
    int width = vita2d_pgf_text_width(font, scale, value);
    text((GAME_WIDTH - width) / 2, y, color, scale, value);
}

static uint32_t darken(uint32_t color, unsigned divisor)
{
    uint32_t r = (color & 255u) / divisor;
    uint32_t g = ((color >> 8) & 255u) / divisor;
    uint32_t b = ((color >> 16) & 255u) / divisor;
    return r | (g << 8) | (b << 16) | UINT32_C(0xff000000);
}

static uint32_t powerup_color(PowerupType type)
{
    static const uint32_t colors[POWERUP_COUNT] = {
        MUTED, UINT32_C(0xff96dc4f), UINT32_C(0xffefa04c),
        UINT32_C(0xffeb87d7), UINT32_C(0xff56d9ef)
    };
    return (unsigned)type < POWERUP_COUNT ? colors[type] : MUTED;
}

static const char *medal_name(unsigned medal)
{
    static const char *const names[] = {"No medal", "Bronze", "Silver", "Gold"};
    return medal < 4 ? names[medal] : names[0];
}

static void draw_notice(const char *notice)
{
    if (notice[0] == '\0') {
        return;
    }
    size_t length = strlen(notice);
    size_t first = length < 96 ? length : 96;
    char line[97];
    memcpy(line, notice, first);
    line[first] = '\0';
    fitted_text(24, 511, 912, ACCENT, 0.58f, line);
    if (length > first) {
        fitted_text(24, 534, 912, ACCENT, 0.58f, notice + first);
    }
}

static void draw_cell(float x, float y, float width, float height,
                      const LevelCell *cell, bool label)
{
    uint32_t outline = cell->color == UINT32_C(0xff000000) ? MUTED : cell->color;
    vita2d_draw_rectangle(x, y, width, height, outline);
    vita2d_draw_rectangle(x + 1.0f, y + 1.0f, width - 2.0f, height - 2.0f, cell->color);
    if (cell->hits > 1) {
        vita2d_draw_rectangle(x + width * 0.1f, y + height * 0.15f,
                              width * 0.8f, height * 0.7f, darken(cell->color, cell->hits));
    }
    if (label && cell->powerup != POWERUP_NONE) {
        text((int)x + 10, (int)y + 12, BACKGROUND, 0.48f, game_powerup_label(cell->powerup));
    }
}

bool render_init(void)
{
    if (vita2d_init() <= 0) {
        return false;
    }
    vita2d_set_clear_color(BACKGROUND);
    vita2d_set_vblank_wait(1);
    font = vita2d_load_default_pgf();
    if (!font) {
        vita2d_fini();
        return false;
    }
    return true;
}

void render_begin(void)
{
    vita2d_start_drawing();
    vita2d_clear_screen();
}

void render_menu(const Menu *menu, const LevelCatalog *catalog,
                 const Scores *scores, const char *notice)
{
    text(32, 58, WHITE, 1.55f, "VitOut");
    text(34, 86, ACCENT, 0.65f, "BREAK BRICKS. BUILD LEVELS.");
    static const char *const items[MENU_ITEM_COUNT] = {
        "Play", "Level", "Difficulty", "Reload custom levels", "Quit"
    };
    for (unsigned i = 0; i < MENU_ITEM_COUNT; ++i) {
        int y = 140 + (int)i * 54;
        if ((unsigned)menu->selected == i) {
            vita2d_draw_rectangle(24.0f, (float)y - 31.0f, 372.0f, 46.0f, SELECTED);
            vita2d_draw_rectangle(24.0f, (float)y - 31.0f, 4.0f, 46.0f, ACCENT);
        }
        text(40, y, WHITE, 0.8f, items[i]);
        if (i == MENU_LEVEL) {
            textf(214, y, ACCENT, 0.7f, "< %u / %u >",
                  catalog->count ? (unsigned)menu->level_index + 1 : 0,
                  (unsigned)catalog->count);
        } else if (i == MENU_DIFFICULTY) {
            textf(214, y, ACCENT, 0.7f, "< %s >", game_difficulties[menu->difficulty].name);
        }
    }
    vita2d_draw_rectangle(422.0f, 109.0f, 514.0f, 294.0f, PANEL);
    if (menu->level_index < catalog->count) {
        const Level *level = &catalog->levels[menu->level_index];
        fitted_text(442, 137, 474, WHITE, 0.85f, level->name);
        for (size_t i = 0; i < LEVEL_ROWS * LEVEL_COLS; ++i) {
            const LevelCell *cell = &level->cells[i];
            if (cell->hits > 0) {
                float x = 447.0f + 15.5f * (float)(i % LEVEL_COLS);
                float y = 154.0f + 8.0f * (float)(i / LEVEL_COLS);
                draw_cell(x, y, 15.0f, 7.5f, cell, false);
            }
        }
        const ScoreRecord *best = scores_find(scores, level->id, menu->difficulty);
        if (best) {
            textf(442, 338, ACCENT, 0.65f, "%s / %u lives   |   Best: %u points",
                  medal_name(best->medal), best->lives, (unsigned)best->points);
            if (best->time_ms > 0) {
                textf(442, 362, MUTED, 0.62f, "Fastest clear: %.1f seconds",
                      (double)best->time_ms / 1000.0);
            } else {
                text(442, 362, MUTED, 0.62f, "Imported classic medal");
            }
        } else {
            text(442, 338, MUTED, 0.65f, "No clear recorded at this difficulty");
        }
        textf(442, 386, MUTED, 0.58f, "%s level  |  Gold %.0fs / Silver %.0fs / Bronze %.0fs",
              level->external ? "Custom" : "Bundled", (double)level->medal_times[0],
              (double)level->medal_times[1], (double)level->medal_times[2]);
    } else {
        text(442, 181, WHITE, 0.8f, "No playable levels found");
        text(442, 224, MUTED, 0.65f, "Copy .lvl files to:");
        text(442, 254, ACCENT, 0.6f, "ux0:/data/VitOut/levels");
        text(442, 296, MUTED, 0.65f, "Press SELECT to reload.");
    }
    const DifficultySettings *settings = &game_difficulties[menu->difficulty];
    textf(32, 426, MUTED, 0.62f, "%s: %u lives  |  %.0f px paddle  |  %.0f px/s ball  |  x%u points",
          settings->name, settings->lives, (double)settings->paddle_width,
          (double)settings->ball_speed, settings->score_multiplier);
    text(32, 451, ACCENT, 0.6f, "POWERUPS:  E Wide (14s)   S Slow (10s)   M Multiball   L Extra life");
    text(32, 480, WHITE, 0.6f,
         "D-pad Select/Change   CROSS Confirm   START Play   L/R Level   SELECT Reload");
    draw_notice(notice);
}

void render_game(const Game *game, const Level *level, const char *notice)
{
    unsigned ball_count = 0;
    for (size_t i = 0; i < GAME_MAX_BALLS; ++i) {
        ball_count += game->balls[i].active;
    }
    fitted_text(20, 25, 575, WHITE, 0.73f, level->name);
    textf(615, 25, ACCENT, 0.65f, "%s   Lives %u   Balls %u",
          game_difficulties[game->difficulty].name, game->lives, ball_count);
    textf(20, 42, MUTED, 0.53f, "%u points   |   %.1fs   |   %u bricks",
          (unsigned)game->points, (double)game->elapsed, (unsigned)game->remaining_bricks);
    text(610, 42, MUTED, 0.53f, "Stick / D-pad Move   CROSS Launch   START Pause");
    vita2d_draw_rectangle(0.0f, 44.0f, GAME_WIDTH, 4.0f, BORDER);
    vita2d_draw_rectangle(0.0f, GAME_TOP, GAME_LEFT, GAME_HEIGHT - GAME_TOP, BORDER);
    vita2d_draw_rectangle(GAME_RIGHT, GAME_TOP, GAME_WIDTH - GAME_RIGHT,
                          GAME_HEIGHT - GAME_TOP, BORDER);
    for (size_t i = 0; i < LEVEL_ROWS * LEVEL_COLS; ++i) {
        if (game->bricks[i].cell.hits > 0) {
            const GameBrick *brick = &game->bricks[i];
            draw_cell(brick->rect.x, brick->rect.y, brick->rect.width, brick->rect.height,
                      &brick->cell, true);
        }
    }
    const GameRect *p = &game->paddle;
    uint32_t paddle_color = game->wide_seconds > 0.0f ? powerup_color(POWERUP_WIDE) : ACCENT;
    vita2d_draw_rectangle(p->x, p->y, p->width, p->height, paddle_color);
    vita2d_draw_rectangle(p->x + 8.0f, p->y + 5.0f, p->width - 16.0f, 10.0f,
                          darken(paddle_color, 3));
    for (size_t i = 0; i < GAME_MAX_DROPS; ++i) {
        const GameDrop *drop = &game->drops[i];
        if (drop->active) {
            vita2d_draw_rectangle(drop->x - 9.0f, drop->y - 9.0f, 18.0f, 18.0f,
                                  powerup_color(drop->type));
            text((int)drop->x - 5, (int)drop->y + 5, BACKGROUND, 0.6f,
                 game_powerup_label(drop->type));
        }
    }
    for (size_t i = 0; i < GAME_MAX_BALLS; ++i) {
        if (game->balls[i].active) {
            vita2d_draw_fill_circle(game->balls[i].x, game->balls[i].y, GAME_BALL_RADIUS,
                                   game->slow_seconds > 0.0f ? powerup_color(POWERUP_SLOW) : WHITE);
        }
    }
    if (game->phase == GAME_READY) {
        centered_text(443, WHITE, 0.8f, "Position the paddle, then press CROSS to launch");
    }
    if (notice[0] == '\0') {
        textf(24, 534, MUTED, 0.6f, "E Wide: %.0fs   S Slow: %.0fs   |   Catch falling E / S / M / L capsules",
              (double)game->wide_seconds, (double)game->slow_seconds);
    }
    if (game->phase == GAME_PAUSED || game->phase == GAME_WON || game->phase == GAME_LOST) {
        vita2d_draw_rectangle(154.0f, 163.0f, 652.0f, 233.0f, PANEL);
        vita2d_draw_rectangle(154.0f, 163.0f, 652.0f, 4.0f, ACCENT);
        if (game->phase == GAME_PAUSED) {
            centered_text(226, WHITE, 1.3f, "PAUSED");
            centered_text(275, MUTED, 0.7f, "The clock and all powerups are frozen.");
            centered_text(330, ACCENT, 0.72f, "CROSS / START Resume");
            centered_text(366, WHITE, 0.65f, "TRIANGLE Restart    CIRCLE Level menu");
        } else {
            centered_text(220, WHITE, 1.2f,
                          game->phase == GAME_WON ? "LEVEL CLEARED" : "OUT OF LIVES");
            char summary[128];
            snprintf(summary, sizeof(summary), "%u points   |   %.1fs   |   %u lives",
                     (unsigned)game->points, (double)game->elapsed, game->lives);
            centered_text(264, MUTED, 0.72f, summary);
            if (game->phase == GAME_WON) {
                centered_text(302, ACCENT, 0.9f, medal_name(game_medal(game, level)));
            }
            centered_text(338, ACCENT, 0.7f,
                          game->phase == GAME_WON ? "CROSS Next level" : "CROSS Retry");
            centered_text(372, WHITE, 0.65f, "TRIANGLE Retry    CIRCLE Level menu");
        }
    }
    draw_notice(notice);
}

void render_end(void)
{
    vita2d_end_drawing();
    vita2d_swap_buffers();
}

void render_shutdown(void)
{
    vita2d_wait_rendering_done();
    vita2d_free_pgf(font);
    font = NULL;
    vita2d_fini();
}
