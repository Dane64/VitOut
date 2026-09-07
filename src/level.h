#ifndef VITOUT_LEVEL_H
#define VITOUT_LEVEL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define LEVEL_ROWS 20
#define LEVEL_COLS 30
#define LEVEL_MAX_COUNT 128
#define LEVEL_ID_SIZE 48
#define LEVEL_NAME_SIZE 64
#define LEVEL_MESSAGE_SIZE 192

typedef enum {
    POWERUP_NONE,
    POWERUP_WIDE,
    POWERUP_SLOW,
    POWERUP_MULTIBALL,
    POWERUP_LIFE,
    POWERUP_COUNT
} PowerupType;

typedef struct {
    uint32_t color;
    unsigned char hits;
    PowerupType powerup;
} LevelCell;

typedef struct {
    char id[LEVEL_ID_SIZE];
    char name[LEVEL_NAME_SIZE];
    float medal_times[3];
    LevelCell cells[LEVEL_ROWS * LEVEL_COLS];
    bool external;
} Level;

typedef struct {
    Level levels[LEVEL_MAX_COUNT];
    size_t count;
    size_t rejected;
    char message[LEVEL_MESSAGE_SIZE];
} LevelCatalog;

/* Failed decoding/loading leaves the supplied cell/level untouched. */
bool level_decode_symbol(char symbol, LevelCell *cell);
bool level_load_file(const char *path, Level *level, char *error, size_t error_size);
/* Rebuild in place; message is nonempty on partial success as well as failure. */
bool level_catalog_load(LevelCatalog *catalog, const char *bundled_dir,
                        const char *custom_dir);

#endif
