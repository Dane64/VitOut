#ifndef VITOUT_GAME_H
#define VITOUT_GAME_H

#include "level.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define GAME_WIDTH 960
#define GAME_HEIGHT 544
#define GAME_LEFT 12.0f
#define GAME_RIGHT 948.0f
#define GAME_TOP 48.0f
#define GAME_PADDLE_Y 488.0f
#define GAME_PADDLE_HEIGHT 20.0f
#define GAME_BALL_RADIUS 7.0f
#define GAME_MAX_BALLS 3
#define GAME_MAX_DROPS 24
#define GAME_MAX_LIVES 5
#define GAME_STEP (1.0f / 120.0f)

typedef enum {
    DIFFICULTY_EASY,
    DIFFICULTY_NORMAL,
    DIFFICULTY_HARD,
    DIFFICULTY_COUNT
} Difficulty;

typedef struct {
    const char *name;
    unsigned lives;
    float paddle_width;
    float ball_speed;
    float max_ball_speed;
    unsigned drop_percent;
    unsigned score_multiplier;
} DifficultySettings;

extern const DifficultySettings game_difficulties[DIFFICULTY_COUNT];

typedef struct {
    float x;
    float y;
    float width;
    float height;
} GameRect;

typedef struct {
    GameRect rect;
    LevelCell cell;
} GameBrick;

typedef struct {
    float x;
    float y;
    float vx;
    float vy;
    bool active;
    bool attached;
} GameBall;

typedef struct {
    float x;
    float y;
    PowerupType type;
    bool active;
} GameDrop;

typedef enum {
    GAME_READY,
    GAME_RUNNING,
    GAME_PAUSED,
    GAME_WON,
    GAME_LOST
} GamePhase;

typedef struct {
    float move;
    bool launch;
} GameInput;

typedef struct {
    GamePhase phase;
    GamePhase resume_phase;
    Difficulty difficulty;
    GameRect paddle;
    GameBrick bricks[LEVEL_ROWS * LEVEL_COLS];
    GameBall balls[GAME_MAX_BALLS];
    GameDrop drops[GAME_MAX_DROPS];
    size_t remaining_bricks;
    unsigned lives;
    uint32_t points;
    uint32_t random_state;
    float ball_speed;
    float elapsed;
    float accumulator;
    float wide_seconds;
    float slow_seconds;
    bool launch_pending;
} Game;

bool game_start(Game *game, const Level *level, Difficulty difficulty, uint32_t seed);
bool game_update(Game *game, GameInput input, float seconds);
void game_toggle_pause(Game *game);
unsigned game_medal(const Game *game, const Level *level);
const char *game_powerup_name(PowerupType type);
const char *game_powerup_label(PowerupType type);

#endif
