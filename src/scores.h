#ifndef VITOUT_SCORES_H
#define VITOUT_SCORES_H

#include "game.h"

#define SCORE_MAX_RECORDS 512

typedef struct {
    char level_id[LEVEL_ID_SIZE];
    Difficulty difficulty;
    uint32_t points;
    unsigned medal;
    unsigned lives;
    uint32_t time_ms;
} ScoreRecord;

typedef struct {
    ScoreRecord records[SCORE_MAX_RECORDS];
    size_t count;
} Scores;

/* Failed loads/records leave scores untouched. Missing files are empty success.
   A successful load may report recovery from the protected .bak sibling. */
bool scores_load(Scores *scores, const char *path, const char *legacy_path,
                 char *error, size_t error_size);
/* Uses exclusive .tmp and protected .bak siblings; one writer per save path. */
bool scores_save(const Scores *scores, const char *path, char *error, size_t error_size);
const ScoreRecord *scores_find(const Scores *scores, const char *level_id,
                               Difficulty difficulty);
bool scores_record(Scores *scores, const char *level_id, Difficulty difficulty,
                   uint32_t points, unsigned medal, unsigned lives, uint32_t time_ms,
                   char *error, size_t error_size);

#endif
