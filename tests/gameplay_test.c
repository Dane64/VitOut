#include "game.h"
#include "menu.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

static void near(float actual, float expected)
{
    assert(isfinite(actual));
    assert(fabsf(actual - expected) < 0.02f);
}

static Level fixture(void)
{
    Level level = {
        .id = "fixture",
        .name = "Fixture",
        .medal_times = {10.0f, 20.0f, 30.0f}
    };
    level.cells[0] = (LevelCell){.hits = 2, .color = UINT32_C(0xffeeeeee)};
    level.cells[LEVEL_ROWS * LEVEL_COLS - 1] = (LevelCell){.hits = 1};
    return level;
}

static Game start(const Level *level, Difficulty difficulty)
{
    Game game;
    assert(game_start(&game, level, difficulty, 42));
    return game;
}

static void tick(Game *game)
{
    assert(game_update(game, (GameInput){0}, GAME_STEP));
}

static void fly(Game *game, size_t index, float x, float y, float vx, float vy)
{
    game->phase = GAME_RUNNING;
    game->balls[index] = (GameBall){
        .x = x, .y = y, .vx = vx, .vy = vy, .active = true
    };
}

static void strike(Game *game, size_t brick)
{
    GameRect rect = game->bricks[brick].rect;
    fly(game, 0, rect.x + rect.width * 0.5f,
        rect.y + rect.height + GAME_BALL_RADIUS + 1.0f, 0.0f, -1.0f);
    tick(game);
}

static size_t active_balls(const Game *game)
{
    size_t active = 0;
    for (size_t i = 0; i < GAME_MAX_BALLS; ++i) {
        active += game->balls[i].active;
    }
    return active;
}

static void catch_powerup(Game *game, PowerupType type)
{
    game->drops[0] = (GameDrop){
        .x = game->paddle.x + game->paddle.width * 0.5f,
        .y = game->paddle.y - 10.0f,
        .type = type,
        .active = true
    };
    tick(game);
    assert(!game->drops[0].active);
}

static void test_start_and_difficulty(void)
{
    Level level = fixture();
    for (Difficulty difficulty = DIFFICULTY_EASY; difficulty < DIFFICULTY_COUNT; ++difficulty) {
        Game game = start(&level, difficulty);
        assert(game.phase == GAME_READY);
        assert(game.lives == game_difficulties[difficulty].lives);
        assert(game.remaining_bricks == 2);
        near(game.paddle.width, game_difficulties[difficulty].paddle_width);
        near(game.ball_speed, game_difficulties[difficulty].ball_speed);
        near(game.elapsed, 0.0f);
        assert(active_balls(&game) == 1);
        assert(game.balls[0].attached);
        assert(game_update(&game, (GameInput){.move = 1.0f}, 0.1f));
        near(game.balls[0].x, game.paddle.x + game.paddle.width * 0.5f);
        near(game.elapsed, 0.0f);
    }
    assert(game_difficulties[DIFFICULTY_EASY].lives >
           game_difficulties[DIFFICULTY_NORMAL].lives);
    assert(game_difficulties[DIFFICULTY_NORMAL].lives >
           game_difficulties[DIFFICULTY_HARD].lives);
    assert(game_difficulties[DIFFICULTY_EASY].ball_speed <
           game_difficulties[DIFFICULTY_NORMAL].ball_speed);
    assert(game_difficulties[DIFFICULTY_NORMAL].ball_speed <
           game_difficulties[DIFFICULTY_HARD].ball_speed);
    Game game = start(&level, DIFFICULTY_NORMAL);
    game.points = 99;
    assert(!game_start(&game, &level, (Difficulty)-1, 1));
    assert(game.points == 99);
    level.cells[0].hits = 7;
    assert(!game_start(&game, &level, DIFFICULTY_NORMAL, 1));
    level = fixture();
    level.medal_times[0] = NAN;
    assert(!game_start(&game, &level, DIFFICULTY_NORMAL, 1));
    level = (Level){.medal_times = {1.0f, 2.0f, 3.0f}};
    assert(!game_start(&game, &level, DIFFICULTY_NORMAL, 1));
    assert(!game_start(NULL, &level, DIFFICULTY_NORMAL, 1));
}

static void test_timing_and_pause(void)
{
    Level level = fixture();
    Game game = start(&level, DIFFICULTY_NORMAL);
    assert(!game_update(&game, (GameInput){0}, -1.0f));
    assert(!game_update(&game, (GameInput){0}, NAN));
    assert(!game_update(&game, (GameInput){.move = INFINITY}, GAME_STEP));
    assert(game_update(&game, (GameInput){.launch = true}, 0.0f));
    assert(game.phase == GAME_READY);
    tick(&game);
    assert(game.phase == GAME_RUNNING);
    near(hypotf(game.balls[0].vx, game.balls[0].vy), 420.0f);
    game.wide_seconds = 8.0f;
    game.slow_seconds = 6.0f;
    game.drops[0] = (GameDrop){.x = 200.0f, .y = 200.0f, .active = true, .type = POWERUP_LIFE};
    game_toggle_pause(&game);
    float x = game.balls[0].x;
    float y = game.balls[0].y;
    float elapsed = game.elapsed;
    assert(game_update(&game, (GameInput){.move = 1.0f, .launch = true}, 3600.0f));
    near(game.balls[0].x, x);
    near(game.balls[0].y, y);
    near(game.elapsed, elapsed);
    near(game.wide_seconds, 8.0f);
    near(game.slow_seconds, 6.0f);
    near(game.drops[0].y, 200.0f);
    game_toggle_pause(&game);
    assert(game.phase == GAME_RUNNING);
    assert(game_update(&game, (GameInput){0}, 3600.0f));
    assert(game.elapsed - elapsed <= 0.101f);
    assert(game.balls[0].y < y);
    assert(game.drops[0].y > 200.0f);
    game = start(&level, DIFFICULTY_NORMAL);
    game_toggle_pause(&game);
    game_toggle_pause(&game);
    tick(&game);
    assert(game.phase == GAME_READY);
}

static void test_frame_rate_independence(void)
{
    Level level = fixture();
    Game reference = start(&level, DIFFICULTY_NORMAL);
    for (unsigned i = 0; i < 240; ++i) {
        assert(game_update(&reference, (GameInput){.move = 0.3f, .launch = i == 0},
                           GAME_STEP));
    }
    const unsigned rates[] = {30, 60, 144};
    for (size_t rate = 0; rate < sizeof(rates) / sizeof(rates[0]); ++rate) {
        Game game = start(&level, DIFFICULTY_NORMAL);
        for (unsigned i = 0; i < rates[rate] * 2; ++i) {
            assert(game_update(&game, (GameInput){.move = 0.3f, .launch = i == 0},
                               1.0f / (float)rates[rate]));
        }
        near(game.elapsed, reference.elapsed);
        near(game.paddle.x, reference.paddle.x);
        near(game.balls[0].x, reference.balls[0].x);
        near(game.balls[0].y, reference.balls[0].y);
        assert(game.points == reference.points);
        assert(game.lives == reference.lives);
        assert(game.random_state == reference.random_state);
    }
}

static void test_collisions(void)
{
    Level level = fixture();
    Game game = start(&level, DIFFICULTY_NORMAL);
    strike(&game, 0);
    assert(game.bricks[0].cell.hits == 1);
    assert(game.balls[0].vy > 0.0f);
    assert(game.points == 20);
    tick(&game);
    assert(game.bricks[0].cell.hits == 1);
    strike(&game, 0);
    assert(game.bricks[0].cell.hits == 0);
    assert(game.remaining_bricks == 1);
    assert(game.points == 120);

    level.cells[0].hits = 6;
    game = start(&level, DIFFICULTY_HARD);
    game.ball_speed = game_difficulties[DIFFICULTY_HARD].max_ball_speed;
    GameRect rect = game.bricks[0].rect;
    fly(&game, 0, rect.x + rect.width * 0.5f,
        rect.y + rect.height + GAME_BALL_RADIUS + 1.0f, 0.0f, -1.0f);
    assert(game_update(&game, (GameInput){0}, 0.1f));
    assert(game.bricks[0].cell.hits == 5);
    assert(game.balls[0].vy > 0.0f);
    near(game.ball_speed, 1150.0f);

    game = start(&level, DIFFICULTY_NORMAL);
    fly(&game, 0, GAME_LEFT + GAME_BALL_RADIUS + 0.5f, 430.0f, -1.0f, -1.0f);
    tick(&game);
    assert(game.balls[0].vx > 0.0f);
    assert(game.balls[0].x >= GAME_LEFT + GAME_BALL_RADIUS);
    fly(&game, 0, GAME_RIGHT - GAME_BALL_RADIUS - 0.5f, 430.0f, 1.0f, -1.0f);
    tick(&game);
    assert(game.balls[0].vx < 0.0f);
    assert(game.balls[0].x <= GAME_RIGHT - GAME_BALL_RADIUS);
    fly(&game, 0, 480.0f, GAME_TOP + GAME_BALL_RADIUS + 0.5f, 1.0f, -1.0f);
    tick(&game);
    assert(game.balls[0].vy > 0.0f);
    fly(&game, 0, GAME_RIGHT - GAME_BALL_RADIUS - 0.5f,
        GAME_TOP + GAME_BALL_RADIUS + 0.5f, 1.0f, -1.0f);
    tick(&game);
    assert(game.balls[0].vx < 0.0f && game.balls[0].vy > 0.0f);

    game.slow_seconds = 5.0f;
    fly(&game, 0, game.paddle.x - 6.5f, game.paddle.y - GAME_BALL_RADIUS - 1.0f,
        0.0f, 1.0f);
    tick(&game);
    assert(game.balls[0].vx < 0.0f && game.balls[0].vy < 0.0f);
    near(hypotf(game.balls[0].vx, game.balls[0].vy), 294.0f);
}

static void test_lives_and_restart(void)
{
    Level level = fixture();
    Game game = start(&level, DIFFICULTY_NORMAL);
    fly(&game, 0, 50.0f, GAME_HEIGHT + GAME_BALL_RADIUS + 1.0f, 0.0f, 1.0f);
    fly(&game, 1, 480.0f, 430.0f, 0.2f, -1.0f);
    tick(&game);
    assert(game.lives == 3 && active_balls(&game) == 1);
    game.slow_seconds = 5.0f;
    game.wide_seconds = 6.0f;
    fly(&game, 1, 50.0f, GAME_HEIGHT + GAME_BALL_RADIUS + 1.0f, 0.0f, 1.0f);
    tick(&game);
    assert(game.lives == 2 && game.phase == GAME_READY);
    assert(game.balls[0].attached && active_balls(&game) == 1);
    near(game.slow_seconds, 0.0f);
    near(game.wide_seconds, 0.0f);
    near(game.paddle.width, 140.0f);
    assert(game.elapsed > 0.0f);
    for (unsigned i = 0; i < 100; ++i) {
        tick(&game);
        assert(game.lives == 2);
    }
    for (unsigned i = 0; i < 2; ++i) {
        fly(&game, 0, 50.0f, GAME_HEIGHT + GAME_BALL_RADIUS + 1.0f, 0.0f, 1.0f);
        tick(&game);
    }
    assert(game.phase == GAME_LOST && game.lives == 0);
    for (unsigned i = 0; i < 100; ++i) {
        assert(game_update(&game, (GameInput){.launch = true}, 0.1f));
        assert(game.phase == GAME_LOST && game.lives == 0);
    }
    assert(game_medal(&game, &level) == 0);
    assert(game_start(&game, &level, DIFFICULTY_EASY, 0));
    assert(game.lives == 5 && game.phase == GAME_READY);
    assert(game.points == 0 && game.random_state != 0);
    near(game.elapsed, 0.0f);
}

static void test_powerups(void)
{
    Level level = fixture();
    level.cells[0].hits = 1;
    level.cells[0].powerup = POWERUP_WIDE;
    Game game = start(&level, DIFFICULTY_NORMAL);
    strike(&game, 0);
    assert(game.drops[0].active && game.drops[0].type == POWERUP_WIDE);
    game.drops[0].active = false;
    fly(&game, 0, 480.0f, 430.0f, 0.2f, -1.0f);
    game.paddle.x = GAME_LEFT;
    catch_powerup(&game, POWERUP_WIDE);
    near(game.paddle.width, 210.0f);
    near(game.paddle.x, GAME_LEFT);
    near(game.wide_seconds, 14.0f);
    game.wide_seconds = 5.0f;
    catch_powerup(&game, POWERUP_WIDE);
    near(game.paddle.width, 210.0f);
    near(game.wide_seconds, 14.0f);
    catch_powerup(&game, POWERUP_SLOW);
    near(game.slow_seconds, 10.0f);
    near(hypotf(game.balls[0].vx, game.balls[0].vy), game.ball_speed * 0.7f);
    catch_powerup(&game, POWERUP_SLOW);
    near(game.slow_seconds, 10.0f);
    near(hypotf(game.balls[0].vx, game.balls[0].vy), game.ball_speed * 0.7f);
    catch_powerup(&game, POWERUP_MULTIBALL);
    assert(active_balls(&game) == 3);
    for (size_t i = 0; i < GAME_MAX_BALLS; ++i) {
        near(hypotf(game.balls[i].vx, game.balls[i].vy), game.ball_speed * 0.7f);
        assert(fabsf(game.balls[i].vy) > game.ball_speed * 0.7f * 0.3f);
    }
    assert(fabsf(game.balls[0].vx - game.balls[1].vx) > 1.0f);
    assert(fabsf(game.balls[1].vx - game.balls[2].vx) > 1.0f);
    catch_powerup(&game, POWERUP_MULTIBALL);
    assert(active_balls(&game) == 3);
    for (unsigned i = 0; i < 5; ++i) {
        catch_powerup(&game, POWERUP_LIFE);
    }
    assert(game.lives == GAME_MAX_LIVES);
    game.wide_seconds = GAME_STEP * 0.5f;
    game.slow_seconds = GAME_STEP * 0.5f;
    tick(&game);
    near(game.paddle.width, 140.0f);
    near(game.wide_seconds, 0.0f);
    near(game.slow_seconds, 0.0f);
    for (size_t i = 0; i < GAME_MAX_BALLS; ++i) {
        near(hypotf(game.balls[i].vx, game.balls[i].vy), game.ball_speed);
    }
    game = start(&level, DIFFICULTY_NORMAL);
    for (size_t i = 0; i < GAME_MAX_DROPS; ++i) {
        game.drops[i] = (GameDrop){.x = 200.0f, .y = 200.0f, .active = true,
                                    .type = POWERUP_LIFE};
    }
    strike(&game, 0);
    for (size_t i = 0; i < GAME_MAX_DROPS; ++i) {
        assert(game.drops[i].active && game.drops[i].type == POWERUP_LIFE);
    }
    game.drops[0].y = GAME_HEIGHT + 10.0f;
    tick(&game);
    assert(!game.drops[0].active);
}

static void test_win_medals_and_seed(void)
{
    Level level = fixture();
    level.cells[0].hits = 1;
    Game first = start(&level, DIFFICULTY_NORMAL);
    Game second = start(&level, DIFFICULTY_NORMAL);
    strike(&first, 0);
    strike(&second, 0);
    assert(first.random_state == second.random_state);
    assert(first.drops[0].active == second.drops[0].active);
    assert(first.drops[0].type == second.drops[0].type);
    strike(&first, LEVEL_ROWS * LEVEL_COLS - 1);
    assert(first.phase == GAME_WON && first.remaining_bricks == 0);
    assert(game_medal(&first, &level) == 3);
    uint32_t points = first.points;
    tick(&first);
    assert(first.phase == GAME_WON && first.points == points);
    first.elapsed = 10.0f;
    assert(game_medal(&first, &level) == 3);
    first.elapsed = 15.0f;
    assert(game_medal(&first, &level) == 2);
    first.elapsed = 25.0f;
    assert(game_medal(&first, &level) == 1);
    first.elapsed = 31.0f;
    assert(game_medal(&first, &level) == 0);
    first.phase = GAME_LOST;
    first.elapsed = 1.0f;
    assert(game_medal(&first, &level) == 0);
}

static MenuAction press(Menu *menu, uint32_t buttons, size_t count)
{
    Input input = {.pressed = buttons, .held = buttons};
    return menu_update(menu, &input, count);
}

static void test_menu(void)
{
    Menu menu;
    menu_init(&menu);
    assert(menu.difficulty == DIFFICULTY_NORMAL && menu.level_index == 0);
    assert(press(&menu, INPUT_CROSS, 10) == MENU_ACTION_PLAY);
    assert(press(&menu, INPUT_UP, 10) == MENU_ACTION_NONE);
    assert(menu.selected == MENU_QUIT);
    assert(press(&menu, INPUT_CROSS, 10) == MENU_ACTION_QUIT);
    press(&menu, INPUT_DOWN, 10);
    assert(menu.selected == MENU_PLAY);
    press(&menu, INPUT_DOWN, 10);
    assert(menu.selected == MENU_LEVEL);
    press(&menu, INPUT_LEFT, 10);
    assert(menu.level_index == 9);
    press(&menu, INPUT_RIGHT, 10);
    assert(menu.level_index == 0);
    press(&menu, INPUT_CROSS, 10);
    assert(menu.level_index == 1);
    press(&menu, INPUT_DOWN, 10);
    assert(menu.selected == MENU_DIFFICULTY);
    press(&menu, INPUT_RIGHT, 10);
    assert(menu.difficulty == DIFFICULTY_HARD);
    press(&menu, INPUT_CROSS, 10);
    assert(menu.difficulty == DIFFICULTY_EASY);
    press(&menu, INPUT_LEFT, 10);
    assert(menu.difficulty == DIFFICULTY_HARD);
    press(&menu, INPUT_R, 10);
    assert(menu.level_index == 2 && menu.difficulty == DIFFICULTY_HARD);
    assert(press(&menu, INPUT_START, 10) == MENU_ACTION_PLAY);
    assert(press(&menu, INPUT_SELECT, 10) == MENU_ACTION_RELOAD);
    menu.selected = MENU_RELOAD;
    assert(press(&menu, INPUT_CROSS, 0) == MENU_ACTION_RELOAD);
    menu.selected = MENU_PLAY;
    assert(press(&menu, INPUT_CROSS, 0) == MENU_ACTION_NONE);
    assert(press(&menu, INPUT_START, 0) == MENU_ACTION_NONE);
    menu.selected = MENU_LEVEL;
    press(&menu, INPUT_LEFT | INPUT_R, 0);
    assert(menu.level_index == 0);
    menu.level_index = 100;
    press(&menu, 0, 3);
    assert(menu.level_index == 0);
}

static void check_game(const Game *game)
{
    assert(isfinite(game->elapsed) && game->elapsed >= 0.0f);
    assert(game->lives <= GAME_MAX_LIVES);
    assert(game->paddle.x >= GAME_LEFT - 0.02f);
    assert(game->paddle.x + game->paddle.width <= GAME_RIGHT + 0.02f);
    size_t bricks = 0;
    for (size_t i = 0; i < LEVEL_ROWS * LEVEL_COLS; ++i) {
        assert(game->bricks[i].cell.hits <= 6);
        bricks += game->bricks[i].cell.hits > 0;
    }
    assert(bricks == game->remaining_bricks);
    for (size_t i = 0; i < GAME_MAX_BALLS; ++i) {
        const GameBall *ball = &game->balls[i];
        if (ball->active) {
            assert(isfinite(ball->x) && isfinite(ball->y));
            assert(isfinite(ball->vx) && isfinite(ball->vy));
            assert(ball->x >= GAME_LEFT + GAME_BALL_RADIUS - 0.02f);
            assert(ball->x <= GAME_RIGHT - GAME_BALL_RADIUS + 0.02f);
            assert(ball->y >= GAME_TOP + GAME_BALL_RADIUS - 0.02f);
            if (!ball->attached) {
                near(hypotf(ball->vx, ball->vy),
                     game->ball_speed * (game->slow_seconds > 0.0f ? 0.7f : 1.0f));
            }
        }
    }
    assert(game->phase == GAME_LOST || active_balls(game) > 0);
}

static void test_bundled_simulation(const char *directory, const char *custom)
{
    LevelCatalog *catalog = calloc(1, sizeof(*catalog));
    assert(catalog);
    assert(level_catalog_load(catalog, directory, custom));
    assert(catalog->count >= 10 && catalog->rejected == 0);
    for (size_t i = 0; i < catalog->count; ++i) {
        for (Difficulty difficulty = DIFFICULTY_EASY; difficulty < DIFFICULTY_COUNT; ++difficulty) {
            Game game = start(&catalog->levels[i], difficulty);
            for (unsigned frame = 0; frame < 2400; ++frame) {
                float target = game.paddle.x + game.paddle.width * 0.5f;
                for (size_t ball = 0; ball < GAME_MAX_BALLS; ++ball) {
                    if (game.balls[ball].active) {
                        target = game.balls[ball].x;
                        break;
                    }
                }
                float movement = (target - game.paddle.x - game.paddle.width * 0.5f) /
                                  (750.0f * GAME_STEP);
                assert(game_update(&game, (GameInput){.move = movement, .launch = true},
                                   GAME_STEP));
                if (frame % 32 == 0) {
                    check_game(&game);
                }
                if (game.phase == GAME_WON || game.phase == GAME_LOST) {
                    break;
                }
            }
            check_game(&game);
        }
    }
    free(catalog);
}

int main(int argc, char **argv)
{
    assert(argc == 3);
    test_start_and_difficulty();
    test_timing_and_pause();
    test_frame_rate_independence();
    test_collisions();
    test_lives_and_restart();
    test_powerups();
    test_win_medals_and_seed();
    test_menu();
    test_bundled_simulation(argv[1], argv[2]);
    puts("Gameplay, physics, powerups, menus and bundled simulations passed.");
    return 0;
}
