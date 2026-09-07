#include "game.h"
#include "input.h"
#include "menu.h"
#include "render.h"
#include "scores.h"

#include <psp2/kernel/processmgr.h>

#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#define BUNDLED_LEVELS "app0:/levels"
#define CUSTOM_LEVELS "ux0:/data/VitOut/levels"
#define SCORE_PATH "ux0:/data/VitOut/scores.dat"
#define LEGACY_SCORE_PATH "ux0:/data/VitOut.save"

typedef struct {
    LevelCatalog catalog;
    Scores scores;
    Game game;
    Menu menu;
    Input input;
    bool playing;
    bool can_save;
    bool quit;
    char notice[LEVEL_MESSAGE_SIZE];
} App;

static void notify(App *app, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vsnprintf(app->notice, sizeof(app->notice), format, args);
    va_end(args);
    fprintf(stderr, "%s\n", app->notice);
}

static bool prepare_directories(App *app)
{
    static const char *const directories[] = {
        "ux0:/data", "ux0:/data/VitOut", CUSTOM_LEVELS
    };
    for (size_t i = 0; i < sizeof(directories) / sizeof(directories[0]); ++i) {
        if (mkdir(directories[i], 0777) != 0) {
            int error = errno;
            struct stat info;
            if (error == EEXIST) {
                if (stat(directories[i], &info) != 0) {
                    error = errno;
                } else if (S_ISDIR(info.st_mode)) {
                    continue;
                } else {
                    error = ENOTDIR;
                }
            }
            notify(app, "Cannot create %s: %s", directories[i], strerror(error));
            return false;
        }
    }
    return true;
}

static void reload_levels(App *app)
{
    char selected_id[LEVEL_ID_SIZE] = "";
    if (app->menu.level_index < app->catalog.count) {
        snprintf(selected_id, sizeof(selected_id), "%s",
                 app->catalog.levels[app->menu.level_index].id);
    }
    bool loaded = level_catalog_load(&app->catalog, BUNDLED_LEVELS, CUSTOM_LEVELS);
    app->menu.level_index = 0;
    for (size_t i = 0; i < app->catalog.count; ++i) {
        if (strcmp(app->catalog.levels[i].id, selected_id) == 0) {
            app->menu.level_index = i;
            break;
        }
    }
    if (app->catalog.message[0] != '\0') {
        notify(app, "%s", app->catalog.message);
    } else if (!loaded) {
        notify(app, "No playable levels. Add .lvl files to " CUSTOM_LEVELS " and press SELECT.");
    } else {
        notify(app, "Loaded %u levels. SELECT reloads " CUSTOM_LEVELS ".",
               (unsigned)app->catalog.count);
    }
}

static void start_level(App *app)
{
    if (app->menu.level_index >= app->catalog.count) {
        app->playing = false;
        notify(app, "No level selected. Add a valid .lvl file and reload with SELECT.");
        return;
    }
    const Level *level = &app->catalog.levels[app->menu.level_index];
    if (!game_start(&app->game, level, app->menu.difficulty,
                    (uint32_t)sceKernelGetProcessTimeWide())) {
        app->playing = false;
        notify(app, "Cannot start invalid level: %s", level->id);
        return;
    }
    app->playing = true;
    app->notice[0] = '\0';
    if (!app->can_save) {
        notify(app, "Score storage is unavailable. This run will not be saved.");
    }
}

static void record_win(App *app)
{
    if (!app->can_save) {
        notify(app, "Level cleared, but score storage is unavailable. Existing saves are untouched.");
        return;
    }
    const Level *level = &app->catalog.levels[app->menu.level_index];
    char error[LEVEL_MESSAGE_SIZE];
    double milliseconds = ceil((double)app->game.elapsed * 1000.0);
    uint32_t time_ms = milliseconds >= UINT32_MAX ? UINT32_MAX : (uint32_t)milliseconds;
    if (!scores_record(&app->scores, level->id, app->game.difficulty,
                       app->game.points, game_medal(&app->game, level),
                       app->game.lives, time_ms, error, sizeof(error)) ||
        !scores_save(&app->scores, SCORE_PATH, error, sizeof(error))) {
        notify(app, "Score not saved: %s", error);
    }
}

static void update_play(App *app, float seconds)
{
    uint32_t pressed = app->input.pressed;
    if (app->game.phase == GAME_WON || app->game.phase == GAME_LOST) {
        if (pressed & INPUT_CIRCLE) {
            app->playing = false;
        } else if (pressed & INPUT_TRIANGLE) {
            start_level(app);
        } else if (pressed & INPUT_CROSS) {
            if (app->game.phase == GAME_WON) {
                app->menu.level_index = (app->menu.level_index + 1) % app->catalog.count;
            }
            start_level(app);
        }
        return;
    }
    if (app->game.phase == GAME_PAUSED) {
        if (pressed & INPUT_CIRCLE) {
            app->playing = false;
        } else if (pressed & INPUT_TRIANGLE) {
            start_level(app);
        } else if (pressed & (INPUT_CROSS | INPUT_START)) {
            game_toggle_pause(&app->game);
        }
        return;
    }
    if (pressed & INPUT_START) {
        game_toggle_pause(&app->game);
        return;
    }
    GameInput input = {.move = app->input.move, .launch = (pressed & INPUT_CROSS) != 0};
    if (!game_update(&app->game, input, seconds)) {
        game_toggle_pause(&app->game);
        notify(app, "Invalid timing or controller input. START resumes the paused game.");
    } else if (app->game.phase == GAME_WON) {
        record_win(app);
    }
}

int main(void)
{
    static App app;
    if (!render_init()) {
        fprintf(stderr, "Could not initialize Vita graphics or the system font.\n");
        sceKernelExitProcess(1);
        return 1;
    }
    if (!input_init()) {
        fprintf(stderr, "Could not initialize Vita controller sampling.\n");
        render_shutdown();
        sceKernelExitProcess(1);
        return 1;
    }
    menu_init(&app.menu);
    reload_levels(&app);
    app.can_save = prepare_directories(&app);
    char error[LEVEL_MESSAGE_SIZE];
    if (!scores_load(&app.scores, SCORE_PATH, LEGACY_SCORE_PATH, error, sizeof(error))) {
        app.can_save = false;
        notify(&app, "Scores unavailable: %s", error);
    }
    uint64_t previous_time = sceKernelGetProcessTimeWide();
    bool input_failed = false;
    while (!app.quit) {
        uint64_t now = sceKernelGetProcessTimeWide();
        float seconds = (float)(now - previous_time) / 1000000.0f;
        previous_time = now;
        if (!input_read(&app.input)) {
            if (!input_failed) {
                notify(&app, "Controller read failed. Waiting for input; START resumes gameplay.");
            }
            if (app.playing && (app.game.phase == GAME_READY || app.game.phase == GAME_RUNNING)) {
                game_toggle_pause(&app.game);
            }
            input_failed = true;
        } else {
            if (input_failed) {
                app.notice[0] = '\0';
            }
            input_failed = false;
            if (app.playing) {
                update_play(&app, seconds);
            } else {
                switch (menu_update(&app.menu, &app.input, app.catalog.count)) {
                case MENU_ACTION_PLAY:
                    start_level(&app);
                    break;
                case MENU_ACTION_RELOAD:
                    reload_levels(&app);
                    break;
                case MENU_ACTION_QUIT:
                    app.quit = true;
                    break;
                case MENU_ACTION_NONE:
                    break;
                }
            }
        }
        if (!app.quit) {
            render_begin();
            if (app.playing) {
                render_game(&app.game, &app.catalog.levels[app.menu.level_index], app.notice);
            } else {
                render_menu(&app.menu, &app.catalog, &app.scores, app.notice);
            }
            render_end();
        }
    }
    render_shutdown();
    sceKernelExitProcess(0);
    return 0;
}
