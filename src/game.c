#include "game.h"

#include <math.h>
#include <string.h>

const DifficultySettings game_difficulties[DIFFICULTY_COUNT] = {
    {"Easy", 5, 180.0f, 330.0f, 750.0f, 25, 1},
    {"Normal", 3, 140.0f, 420.0f, 960.0f, 18, 2},
    {"Hard", 2, 108.0f, 540.0f, 1150.0f, 12, 3}
};

typedef enum {
    HIT_NONE,
    HIT_WALL,
    HIT_PADDLE,
    HIT_BRICK
} HitKind;

typedef struct {
    float time;
    float nx;
    float ny;
    HitKind kind;
    size_t brick;
} Hit;

static float clamp(float value, float low, float high)
{
    return fminf(high, fmaxf(low, value));
}

static uint32_t next_random(Game *game)
{
    uint32_t value = game->random_state;
    value ^= value << 13;
    value ^= value >> 17;
    value ^= value << 5;
    game->random_state = value;
    return value;
}

static float effective_speed(const Game *game)
{
    return game->ball_speed * (game->slow_seconds > 0.0f ? 0.7f : 1.0f);
}

static void set_velocity(GameBall *ball, float vx, float vy, float speed)
{
    float length = hypotf(vx, vy);
    ball->vx = vx / length * speed;
    ball->vy = vy / length * speed;
}

static void refresh_speeds(Game *game)
{
    for (size_t i = 0; i < GAME_MAX_BALLS; ++i) {
        GameBall *ball = &game->balls[i];
        if (ball->active && !ball->attached) {
            set_velocity(ball, ball->vx, ball->vy, effective_speed(game));
        }
    }
}

static void resize_paddle(Game *game)
{
    float width = game_difficulties[game->difficulty].paddle_width;
    if (game->wide_seconds > 0.0f) {
        width *= 1.5f;
    }
    game->paddle.x += (game->paddle.width - width) * 0.5f;
    game->paddle.width = width;
    game->paddle.x = clamp(game->paddle.x, GAME_LEFT, GAME_RIGHT - width);
}

static void prepare_serve(Game *game)
{
    memset(game->balls, 0, sizeof(game->balls));
    memset(game->drops, 0, sizeof(game->drops));
    game->wide_seconds = 0.0f;
    game->slow_seconds = 0.0f;
    game->ball_speed = game_difficulties[game->difficulty].ball_speed;
    game->paddle.width = game_difficulties[game->difficulty].paddle_width;
    game->paddle.x = (GAME_LEFT + GAME_RIGHT - game->paddle.width) * 0.5f;
    game->balls[0] = (GameBall){
        .x = game->paddle.x + game->paddle.width * 0.5f,
        .y = game->paddle.y - GAME_BALL_RADIUS - 0.5f,
        .active = true,
        .attached = true
    };
    game->phase = GAME_READY;
    game->accumulator = 0.0f;
    game->launch_pending = false;
}

bool game_start(Game *game, const Level *level, Difficulty difficulty, uint32_t seed)
{
    if (!game || !level || (unsigned)difficulty >= DIFFICULTY_COUNT) {
        return false;
    }
    size_t count = 0;
    for (size_t i = 0; i < LEVEL_ROWS * LEVEL_COLS; ++i) {
        if (level->cells[i].hits > 6 ||
            (unsigned)level->cells[i].powerup >= POWERUP_COUNT) {
            return false;
        }
        count += level->cells[i].hits > 0;
    }
    for (size_t i = 0; i < 3; ++i) {
        if (!isfinite(level->medal_times[i]) || level->medal_times[i] <= 0.0f ||
            (i > 0 && level->medal_times[i] <= level->medal_times[i - 1])) {
            return false;
        }
    }
    if (count == 0) {
        return false;
    }
    *game = (Game){
        .difficulty = difficulty,
        .remaining_bricks = count,
        .lives = game_difficulties[difficulty].lives,
        .random_state = seed ? seed : UINT32_C(0x6d2b79f5),
        .paddle = {.y = GAME_PADDLE_Y, .height = GAME_PADDLE_HEIGHT}
    };
    for (size_t i = 0; i < LEVEL_ROWS * LEVEL_COLS; ++i) {
        game->bricks[i] = (GameBrick){
            .rect = {15.0f + 31.0f * (float)(i % LEVEL_COLS),
                     64.0f + 16.0f * (float)(i / LEVEL_COLS), 30.0f, 15.0f},
            .cell = level->cells[i]
        };
    }
    prepare_serve(game);
    return true;
}

static void consider_hit(Hit *hit, float time, float nx, float ny,
                         HitKind kind, size_t brick)
{
    if (time >= -0.000001f && time <= 1.0f && time < hit->time) {
        *hit = (Hit){fmaxf(time, 0.0f), nx, ny, kind, brick};
    }
}

static void intersect_brick(Hit *hit, const GameBall *ball, const GameRect *rect,
                            float dx, float dy, size_t index)
{
    float left = rect->x - GAME_BALL_RADIUS;
    float right = rect->x + rect->width + GAME_BALL_RADIUS;
    float top = rect->y - GAME_BALL_RADIUS;
    float bottom = rect->y + rect->height + GAME_BALL_RADIUS;
    float near_x = -INFINITY;
    float far_x = INFINITY;
    float near_y = -INFINITY;
    float far_y = INFINITY;

    if (fabsf(dx) < 0.000001f) {
        if (ball->x < left || ball->x > right) {
            return;
        }
    } else {
        float a = (left - ball->x) / dx;
        float b = (right - ball->x) / dx;
        near_x = fminf(a, b);
        far_x = fmaxf(a, b);
    }
    if (fabsf(dy) < 0.000001f) {
        if (ball->y < top || ball->y > bottom) {
            return;
        }
    } else {
        float a = (top - ball->y) / dy;
        float b = (bottom - ball->y) / dy;
        near_y = fminf(a, b);
        far_y = fmaxf(a, b);
    }
    float entry = fmaxf(near_x, near_y);
    if (entry > fminf(far_x, far_y)) {
        return;
    }
    float nx = near_x >= near_y - 0.000001f ? (dx > 0.0f ? -1.0f : 1.0f) : 0.0f;
    float ny = near_y >= near_x - 0.000001f ? (dy > 0.0f ? -1.0f : 1.0f) : 0.0f;
    consider_hit(hit, entry, nx, ny, HIT_BRICK, index);
}

static Hit find_hit(const Game *game, const GameBall *ball, float dx, float dy)
{
    Hit hit = {.time = 2.0f, .kind = HIT_NONE};
    if (dx < 0.0f) {
        consider_hit(&hit, (GAME_LEFT + GAME_BALL_RADIUS - ball->x) / dx,
                     1.0f, 0.0f, HIT_WALL, 0);
    } else if (dx > 0.0f) {
        consider_hit(&hit, (GAME_RIGHT - GAME_BALL_RADIUS - ball->x) / dx,
                     -1.0f, 0.0f, HIT_WALL, 0);
    }
    if (dy < 0.0f) {
        consider_hit(&hit, (GAME_TOP + GAME_BALL_RADIUS - ball->y) / dy,
                     0.0f, 1.0f, HIT_WALL, 0);
    } else if (dy > 0.0f) {
        float time = (game->paddle.y - GAME_BALL_RADIUS - ball->y) / dy;
        float x = ball->x + dx * time;
        if (x >= game->paddle.x - GAME_BALL_RADIUS &&
            x <= game->paddle.x + game->paddle.width + GAME_BALL_RADIUS) {
            consider_hit(&hit, time, 0.0f, -1.0f, HIT_PADDLE, 0);
        }
    }
    for (size_t i = 0; i < LEVEL_ROWS * LEVEL_COLS; ++i) {
        if (game->bricks[i].cell.hits > 0) {
            intersect_brick(&hit, ball, &game->bricks[i].rect, dx, dy, i);
        }
    }
    return hit;
}

static void spawn_drop(Game *game, const GameBrick *brick)
{
    PowerupType type = brick->cell.powerup;
    if (type == POWERUP_NONE) {
        if (next_random(game) % 100 >= game_difficulties[game->difficulty].drop_percent) {
            return;
        }
        type = (PowerupType)(POWERUP_WIDE + next_random(game) % (POWERUP_COUNT - 1));
    }
    for (size_t i = 0; i < GAME_MAX_DROPS; ++i) {
        if (!game->drops[i].active) {
            game->drops[i] = (GameDrop){
                .x = brick->rect.x + brick->rect.width * 0.5f,
                .y = brick->rect.y + brick->rect.height * 0.5f,
                .type = type,
                .active = true
            };
            break;
        }
    }
}

static void move_ball(Game *game, GameBall *ball)
{
    float remaining = GAME_STEP;
    /* Bound repeated contacts without ever moving unchecked through a solid. */
    for (unsigned impacts = 0; impacts < 8 && remaining > 0.000001f; ++impacts) {
        float dx = ball->vx * remaining;
        float dy = ball->vy * remaining;
        Hit hit = find_hit(game, ball, dx, dy);
        if (hit.kind == HIT_NONE) {
            ball->x += dx;
            ball->y += dy;
            break;
        }
        ball->x += dx * hit.time;
        ball->y += dy * hit.time;
        remaining *= 1.0f - hit.time;
        if (hit.kind == HIT_PADDLE) {
            float offset = clamp((ball->x - game->paddle.x - game->paddle.width * 0.5f) /
                                 (game->paddle.width * 0.5f), -1.0f, 1.0f);
            if (fabsf(offset) < 0.12f) {
                offset = ball->vx < 0.0f ? -0.12f : 0.12f;
            }
            float lateral = offset * 0.85f;
            set_velocity(ball, lateral, -sqrtf(1.0f - lateral * lateral),
                         effective_speed(game));
        } else {
            if (hit.nx != 0.0f) {
                ball->vx = -ball->vx;
            }
            if (hit.ny != 0.0f) {
                ball->vy = -ball->vy;
            }
        }
        ball->x += hit.nx * 0.01f;
        ball->y += hit.ny * 0.01f;
        if (hit.kind == HIT_BRICK) {
            GameBrick *brick = &game->bricks[hit.brick];
            unsigned multiplier = game_difficulties[game->difficulty].score_multiplier;
            --brick->cell.hits;
            game->points += 10 * multiplier;
            if (brick->cell.hits == 0) {
                --game->remaining_bricks;
                game->points += 40 * multiplier;
                spawn_drop(game, brick);
            }
            game->ball_speed = fminf(game->ball_speed + 6.0f,
                                    game_difficulties[game->difficulty].max_ball_speed);
            refresh_speeds(game);
            if (game->remaining_bricks == 0) {
                return;
            }
        }
    }
    if (ball->y > GAME_HEIGHT + GAME_BALL_RADIUS) {
        ball->active = false;
    }
}

static void multiball(Game *game)
{
    GameBall source = {0};
    for (size_t i = 0; i < GAME_MAX_BALLS; ++i) {
        if (game->balls[i].active && !game->balls[i].attached) {
            source = game->balls[i];
            break;
        }
    }
    if (!source.active) {
        return;
    }
    float angle = atan2f(source.vy, source.vx);
    float offset = -0.45f;
    for (size_t i = 0; i < GAME_MAX_BALLS; ++i) {
        if (!game->balls[i].active) {
            GameBall *ball = &game->balls[i];
            *ball = source;
            float vx = cosf(angle + offset);
            float vy = sinf(angle + offset);
            if (fabsf(vy) < 0.35f) {
                vy = source.vy < 0.0f ? -0.35f : 0.35f;
            }
            set_velocity(ball, vx, vy, effective_speed(game));
            offset = 0.45f;
        }
    }
}

static void collect_drop(Game *game, PowerupType type)
{
    switch (type) {
    case POWERUP_WIDE:
        game->wide_seconds = 14.0f;
        resize_paddle(game);
        break;
    case POWERUP_SLOW:
        game->slow_seconds = 10.0f;
        refresh_speeds(game);
        break;
    case POWERUP_MULTIBALL:
        multiball(game);
        break;
    case POWERUP_LIFE:
        if (game->lives < GAME_MAX_LIVES) {
            ++game->lives;
        }
        break;
    case POWERUP_NONE:
    case POWERUP_COUNT:
        break;
    }
    game->points += 25 * game_difficulties[game->difficulty].score_multiplier;
}

static void update_drops(Game *game)
{
    for (size_t i = 0; i < GAME_MAX_DROPS; ++i) {
        GameDrop *drop = &game->drops[i];
        if (!drop->active) {
            continue;
        }
        float old_y = drop->y;
        drop->y += 150.0f * GAME_STEP;
        if (drop->x + 9.0f >= game->paddle.x &&
            drop->x - 9.0f <= game->paddle.x + game->paddle.width &&
            old_y - 9.0f <= game->paddle.y + game->paddle.height &&
            drop->y + 9.0f >= game->paddle.y) {
            drop->active = false;
            collect_drop(game, drop->type);
        } else if (drop->y > GAME_HEIGHT + 9.0f) {
            drop->active = false;
        }
    }
}

static void step_game(Game *game, float movement)
{
    if (game->phase == GAME_RUNNING) {
        game->wide_seconds = fmaxf(0.0f, game->wide_seconds - GAME_STEP);
        game->slow_seconds = fmaxf(0.0f, game->slow_seconds - GAME_STEP);
    }
    resize_paddle(game);
    game->paddle.x = clamp(game->paddle.x + movement * 750.0f * GAME_STEP,
                           GAME_LEFT, GAME_RIGHT - game->paddle.width);
    if (game->phase == GAME_READY) {
        GameBall *ball = &game->balls[0];
        ball->x = game->paddle.x + game->paddle.width * 0.5f;
        if (!game->launch_pending) {
            return;
        }
        ball->attached = false;
        set_velocity(ball, 0.24f, -1.0f, effective_speed(game));
        game->phase = GAME_RUNNING;
    }
    game->launch_pending = false;
    game->elapsed += GAME_STEP;
    update_drops(game);
    refresh_speeds(game);
    size_t active = 0;
    for (size_t i = 0; i < GAME_MAX_BALLS; ++i) {
        if (game->balls[i].active) {
            move_ball(game, &game->balls[i]);
            active += game->balls[i].active;
        }
        if (game->remaining_bricks == 0) {
            game->phase = GAME_WON;
            game->accumulator = 0.0f;
            return;
        }
    }
    if (active == 0) {
        --game->lives;
        if (game->lives == 0) {
            game->phase = GAME_LOST;
            game->accumulator = 0.0f;
        } else {
            prepare_serve(game);
        }
    }
}

bool game_update(Game *game, GameInput input, float seconds)
{
    if (!game || !isfinite(seconds) || seconds < 0.0f || !isfinite(input.move)) {
        return false;
    }
    if (game->phase != GAME_READY && game->phase != GAME_RUNNING) {
        return true;
    }
    game->launch_pending |= input.launch;
    /* Discard long suspend/stall gaps rather than fast-forwarding through a life. */
    game->accumulator += fminf(seconds, 0.1f);
    while (game->accumulator + 0.0000001f >= GAME_STEP) {
        game->accumulator = fmaxf(0.0f, game->accumulator - GAME_STEP);
        step_game(game, clamp(input.move, -1.0f, 1.0f));
        if (game->phase != GAME_READY && game->phase != GAME_RUNNING) {
            break;
        }
    }
    return true;
}

void game_toggle_pause(Game *game)
{
    if (game->phase == GAME_READY || game->phase == GAME_RUNNING) {
        game->resume_phase = game->phase;
        game->phase = GAME_PAUSED;
    } else if (game->phase == GAME_PAUSED) {
        game->phase = game->resume_phase;
    }
    game->accumulator = 0.0f;
    game->launch_pending = false;
}

unsigned game_medal(const Game *game, const Level *level)
{
    if (game->phase == GAME_WON) {
        for (unsigned i = 0; i < 3; ++i) {
            if (game->elapsed <= level->medal_times[i]) {
                return 3 - i;
            }
        }
    }
    return 0;
}

const char *game_powerup_name(PowerupType type)
{
    static const char *const names[POWERUP_COUNT] = {
        "None", "Wide paddle", "Slow ball", "Multiball", "Extra life"
    };
    return (unsigned)type < POWERUP_COUNT ? names[type] : "Unknown";
}

const char *game_powerup_label(PowerupType type)
{
    static const char *const labels[POWERUP_COUNT] = {"", "E", "S", "M", "L"};
    return (unsigned)type < POWERUP_COUNT ? labels[type] : "?";
}
