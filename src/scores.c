#include "scores.h"

#include <errno.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define SCORE_PATH_SIZE 1024u
#define SCORE_LINE_LIMIT 128u
#define SCORE_FILE_LIMIT 65536u
#define LEGACY_LEVEL_COUNT 6u

typedef struct {
    FILE *file;
    const char *path;
    size_t bytes;
    size_t line;
    char *error;
    size_t error_size;
} ScoreReader;

static size_t bounded_length(const char *text, size_t limit)
{
    size_t length = 0;
    while (length < limit && text[length] != '\0') {
        ++length;
    }
    return length;
}

static void score_error(char *error, size_t size, const char *path, size_t line,
                        const char *format, ...)
{
    const char *label = path != NULL ? path : "scores";
    size_t length = bounded_length(label, SCORE_PATH_SIZE);
    char reason[112];
    va_list arguments;

    if (error == NULL || size == 0) {
        return;
    }
    va_start(arguments, format);
    (void)vsnprintf(reason, sizeof(reason), format, arguments);
    va_end(arguments);
    if (length > 64) {
        label += length - 64;
    }
    if (line != 0) {
        (void)snprintf(error, size, "%.64s:%zu: %s", label, line, reason);
    } else {
        (void)snprintf(error, size, "%.64s: %s", label, reason);
    }
}

static bool identifier_valid(const char *id)
{
    size_t length;
    if (id == NULL) {
        return false;
    }
    length = bounded_length(id, LEVEL_ID_SIZE);
    if (length == 0 || length == LEVEL_ID_SIZE) {
        return false;
    }
    for (size_t i = 0; i < length; ++i) {
        char byte = id[i];
        if (!((byte >= 'a' && byte <= 'z') ||
              (byte >= '0' && byte <= '9') || byte == '-' || byte == '_')) {
            return false;
        }
    }
    return true;
}

static bool difficulty_valid(Difficulty difficulty)
{
    return (unsigned)difficulty < (unsigned)DIFFICULTY_COUNT;
}

static bool record_valid(const ScoreRecord *record)
{
    if (!identifier_valid(record->level_id) || !difficulty_valid(record->difficulty) ||
        record->medal > 3 || record->lives < 1 || record->lives > GAME_MAX_LIVES) {
        return false;
    }
    if (record->time_ms != 0) {
        return true;
    }
    return record->difficulty == DIFFICULTY_NORMAL && record->points == 0 &&
           record->medal != 0 && strlen(record->level_id) == 10 &&
           strncmp(record->level_id, "classic-0", 9) == 0 &&
           record->level_id[9] >= '1' && record->level_id[9] <= '6';
}

static bool scores_valid(const Scores *scores, const char *path,
                         char *error, size_t error_size)
{
    if (scores == NULL || scores->count > SCORE_MAX_RECORDS) {
        score_error(error, error_size, path, 0, "invalid score table (maximum 512 records)");
        return false;
    }
    for (size_t i = 0; i < scores->count; ++i) {
        if (!record_valid(&scores->records[i])) {
            score_error(error, error_size, path, 0, "invalid score record %zu", i + 1);
            return false;
        }
        for (size_t j = 0; j < i; ++j) {
            if (scores->records[i].difficulty == scores->records[j].difficulty &&
                strcmp(scores->records[i].level_id, scores->records[j].level_id) == 0) {
                score_error(error, error_size, path, 0, "duplicate score key '%s' difficulty %u",
                            scores->records[i].level_id, (unsigned)scores->records[i].difficulty);
                return false;
            }
        }
    }
    return true;
}

const ScoreRecord *scores_find(const Scores *scores, const char *level_id,
                               Difficulty difficulty)
{
    if (scores == NULL || scores->count > SCORE_MAX_RECORDS ||
        !identifier_valid(level_id) || !difficulty_valid(difficulty)) {
        return NULL;
    }
    for (size_t i = 0; i < scores->count; ++i) {
        const ScoreRecord *record = &scores->records[i];
        if (record->difficulty == difficulty && record_valid(record) &&
            strcmp(record->level_id, level_id) == 0) {
            return record;
        }
    }
    return NULL;
}

bool scores_record(Scores *scores, const char *level_id, Difficulty difficulty,
                   uint32_t points, unsigned medal, unsigned lives, uint32_t time_ms,
                   char *error, size_t error_size)
{
    ScoreRecord *record = NULL;
    if (error != NULL && error_size != 0) {
        error[0] = '\0';
    }
    if (!identifier_valid(level_id) || !difficulty_valid(difficulty) ||
        medal > 3 || lives < 1 || lives > GAME_MAX_LIVES || time_ms == 0) {
        score_error(error, error_size, NULL, 0,
                    "invalid result; need valid id/difficulty, medal 0..3, lives 1..5 and time > 0");
        return false;
    }
    if (!scores_valid(scores, NULL, error, error_size)) {
        return false;
    }
    for (size_t i = 0; i < scores->count; ++i) {
        if (scores->records[i].difficulty == difficulty &&
            strcmp(scores->records[i].level_id, level_id) == 0) {
            record = &scores->records[i];
            break;
        }
    }
    if (record == NULL) {
        if (scores->count == SCORE_MAX_RECORDS) {
            score_error(error, error_size, NULL, 0, "512 score records full; cannot add another level/difficulty");
            return false;
        }
        record = &scores->records[scores->count];
        memset(record, 0, sizeof(*record));
        (void)strcpy(record->level_id, level_id);
        record->difficulty = difficulty;
        record->points = points;
        record->medal = medal;
        record->lives = lives;
        record->time_ms = time_ms;
        ++scores->count;
        return true;
    }
    if (points > record->points) {
        record->points = points;
    }
    if (medal > record->medal || (medal == record->medal && lives > record->lives)) {
        record->medal = medal;
        record->lives = lives;
    }
    if (record->time_ms == 0 || time_ms < record->time_ms) {
        record->time_ms = time_ms;
    }
    return true;
}

static int score_read_byte(ScoreReader *reader)
{
    int byte = fgetc(reader->file);
    if (byte == EOF) {
        if (ferror(reader->file)) {
            score_error(reader->error, reader->error_size, reader->path, reader->line,
                        "read failed: %s", strerror(errno));
            return -2;
        }
        return EOF;
    }
    if (++reader->bytes > SCORE_FILE_LIMIT) {
        score_error(reader->error, reader->error_size, reader->path, reader->line,
                    "score file exceeds %u bytes", SCORE_FILE_LIMIT);
        return -2;
    }
    return byte;
}

static int score_read_line(ScoreReader *reader, char line[SCORE_LINE_LIMIT + 1])
{
    size_t length = 0;
    ++reader->line;
    for (;;) {
        int byte = score_read_byte(reader);
        if (byte == -2) {
            return -1;
        }
        if (byte == EOF || byte == '\n') {
            line[length] = '\0';
            return byte == EOF && length == 0 ? 0 : 1;
        }
        if (byte == '\r') {
            byte = score_read_byte(reader);
            if (byte == -2) {
                return -1;
            }
            if (byte != '\n') {
                score_error(reader->error, reader->error_size, reader->path, reader->line,
                            "bare CR; use LF or CRLF line endings");
                return -1;
            }
            line[length] = '\0';
            return 1;
        }
        if (byte < 32 || byte > 126) {
            score_error(reader->error, reader->error_size, reader->path, reader->line,
                        "non-ASCII or binary byte 0x%02x", (unsigned)byte);
            return -1;
        }
        if (length == SCORE_LINE_LIMIT) {
            score_error(reader->error, reader->error_size, reader->path, reader->line,
                        "line exceeds %u characters", SCORE_LINE_LIMIT);
            return -1;
        }
        line[length++] = (char)byte;
    }
}

static bool unsigned_number(const char *text, uint32_t maximum, uint32_t *result)
{
    uint32_t value = 0;
    if (*text == '\0') {
        return false;
    }
    for (; *text != '\0'; ++text) {
        uint32_t digit;
        if (*text < '0' || *text > '9') {
            return false;
        }
        digit = (uint32_t)(*text - '0');
        if (value > maximum / 10 ||
            (value == maximum / 10 && digit > maximum % 10)) {
            return false;
        }
        value = value * 10 + digit;
    }
    *result = value;
    return true;
}

static bool parse_record(char *line, ScoreRecord *record)
{
    char *parts[6];
    uint32_t difficulty;
    uint32_t medal;
    uint32_t lives;
    char *cursor = line;

    for (size_t i = 0; i < 6; ++i) {
        char *separator;
        parts[i] = cursor;
        separator = strchr(cursor, ' ');
        if (i == 5) {
            if (separator != NULL) {
                return false;
            }
        } else {
            if (separator == NULL) {
                return false;
            }
            *separator = '\0';
            cursor = separator + 1;
        }
    }
    if (!identifier_valid(parts[0]) ||
        !unsigned_number(parts[1], DIFFICULTY_COUNT - 1, &difficulty) ||
        !unsigned_number(parts[2], UINT32_MAX, &record->points) ||
        !unsigned_number(parts[3], 3, &medal) ||
        !unsigned_number(parts[4], GAME_MAX_LIVES, &lives) ||
        !unsigned_number(parts[5], UINT32_MAX, &record->time_ms)) {
        return false;
    }
    (void)strcpy(record->level_id, parts[0]);
    record->difficulty = (Difficulty)difficulty;
    record->medal = (unsigned)medal;
    record->lives = (unsigned)lives;
    return record_valid(record);
}

static bool parse_scores(ScoreReader *reader, Scores *scores)
{
    char line[SCORE_LINE_LIMIT + 1];
    uint32_t count;
    int status = score_read_line(reader, line);

    if (status < 0) {
        return false;
    }
    if (status == 0 || strcmp(line, "VITOUT_SCORES 1") != 0) {
        score_error(reader->error, reader->error_size, reader->path, reader->line,
                    "expected VITOUT_SCORES 1; save is corrupt or unsupported");
        return false;
    }
    status = score_read_line(reader, line);
    if (status < 0) {
        return false;
    }
    if (status == 0 || strncmp(line, "count=", 6) != 0 ||
        !unsigned_number(line + 6, SCORE_MAX_RECORDS, &count)) {
        score_error(reader->error, reader->error_size, reader->path, reader->line,
                    "expected count=0..512");
        return false;
    }
    for (uint32_t i = 0; i < count; ++i) {
        ScoreRecord record = {0};
        status = score_read_line(reader, line);
        if (status < 0) {
            return false;
        }
        if (status == 0 || !parse_record(line, &record)) {
            score_error(reader->error, reader->error_size, reader->path, reader->line,
                        "invalid/truncated record; expected id difficulty points medal lives time_ms");
            return false;
        }
        for (size_t j = 0; j < scores->count; ++j) {
            if (record.difficulty == scores->records[j].difficulty &&
                strcmp(record.level_id, scores->records[j].level_id) == 0) {
                score_error(reader->error, reader->error_size, reader->path, reader->line,
                            "duplicate score key '%s' difficulty %u",
                            record.level_id, (unsigned)record.difficulty);
                return false;
            }
        }
        scores->records[scores->count++] = record;
    }
    status = score_read_line(reader, line);
    if (status < 0) {
        return false;
    }
    if (status == 0 || strcmp(line, "END") != 0) {
        score_error(reader->error, reader->error_size, reader->path, reader->line,
                    "missing END; save is truncated or has extra records");
        return false;
    }
    status = score_read_line(reader, line);
    if (status < 0) {
        return false;
    }
    if (status != 0) {
        score_error(reader->error, reader->error_size, reader->path, reader->line,
                    "unexpected data after END");
        return false;
    }
    return true;
}

static bool score_path_valid(const char *path)
{
    return path != NULL && path[0] != '\0' &&
           bounded_length(path, SCORE_PATH_SIZE - 4) < SCORE_PATH_SIZE - 4;
}

static bool sibling_path(const char *path, const char *suffix, char output[SCORE_PATH_SIZE],
                         char *error, size_t error_size)
{
    int written;
    if (!score_path_valid(path)) {
        score_error(error, error_size, path, 0, "invalid path (maximum 1019 bytes)");
        return false;
    }
    written = snprintf(output, SCORE_PATH_SIZE, "%s%s", path, suffix);
    if (written < 0 || (size_t)written >= SCORE_PATH_SIZE) {
        score_error(error, error_size, path, 0, "score path too long");
        return false;
    }
    return true;
}

static int score_file_state(const char *path, char *error, size_t error_size)
{
    struct stat info;
    if (stat(path, &info) != 0) {
        if (errno == ENOENT) {
            return 0;
        }
        score_error(error, error_size, path, 0, "cannot stat save: %s", strerror(errno));
        return -1;
    }
    if (!S_ISREG(info.st_mode)) {
        score_error(error, error_size, path, 0, "save path is not a regular file");
        return -1;
    }
    return 1;
}

static bool read_scores(Scores *scores, const char *path, bool legacy,
                        char *error, size_t error_size)
{
    FILE *file = fopen(path, "rb");
    bool success;
    if (file == NULL) {
        score_error(error, error_size, path, 0, "cannot open save: %s", strerror(errno));
        return false;
    }
    if (legacy) {
        unsigned char values[LEGACY_LEVEL_COUNT];
        size_t read = fread(values, 1, sizeof(values), file);
        int extra = fgetc(file);
        success = read == sizeof(values) && extra == EOF && !ferror(file);
        if (!success) {
            score_error(error, error_size, path, 0, "legacy save must contain exactly six bytes");
        }
        for (size_t i = 0; success && i < LEGACY_LEVEL_COUNT; ++i) {
            unsigned value = values[i];
            unsigned lives = value % 10;
            unsigned medal = value / 10 + 1;
            ScoreRecord *record;
            if (value == 0) {
                continue;
            }
            if (lives < 1 || lives > GAME_MAX_LIVES || medal > 3) {
                score_error(error, error_size, path, 0, "invalid legacy score byte %zu", i + 1);
                success = false;
                break;
            }
            record = &scores->records[scores->count++];
            (void)snprintf(record->level_id, sizeof(record->level_id), "classic-%02u", (unsigned)i + 1);
            record->difficulty = DIFFICULTY_NORMAL;
            record->points = 0;
            record->medal = medal;
            record->lives = lives;
            record->time_ms = 0;
        }
    } else {
        ScoreReader reader = {file, path, 0, 0, error, error_size};
        success = parse_scores(&reader, scores);
    }
    if (fclose(file) != 0 && success) {
        score_error(error, error_size, path, 0, "save close failed: %s", strerror(errno));
        success = false;
    }
    return success;
}

bool scores_load(Scores *scores, const char *path, const char *legacy_path,
                 char *error, size_t error_size)
{
    char backup[SCORE_PATH_SIZE];
    const char *source = path;
    bool legacy = false;
    bool recovered = false;
    bool success = true;
    Scores *loaded;
    int state;

    if (error != NULL && error_size != 0) {
        error[0] = '\0';
    }
    if (scores == NULL) {
        score_error(error, error_size, path, 0, "missing output score table");
        return false;
    }
    if (!sibling_path(path, ".bak", backup, error, error_size)) {
        return false;
    }
    state = score_file_state(path, error, error_size);
    if (state < 0) {
        return false;
    }
    if (state == 0) {
        source = backup;
        state = score_file_state(backup, error, error_size);
        if (state < 0) {
            return false;
        }
        recovered = state != 0;
        if (state == 0 && legacy_path != NULL) {
            if (!score_path_valid(legacy_path)) {
                score_error(error, error_size, legacy_path, 0, "invalid legacy path");
                return false;
            }
            source = legacy_path;
            legacy = true;
            state = score_file_state(source, error, error_size);
            if (state < 0) {
                return false;
            }
        }
    }
    loaded = calloc(1, sizeof(*loaded));
    if (loaded == NULL) {
        score_error(error, error_size, path, 0, "not enough memory to load scores");
        return false;
    }
    if (state != 0) {
        success = read_scores(loaded, source, legacy, error, error_size);
    }
    if (success) {
        *scores = *loaded;
        if (recovered) {
            score_error(error, error_size, backup, 0, "recovered previous save; next save completes recovery");
        }
    }
    free(loaded);
    return success;
}

static bool write_scores(FILE *file, const Scores *scores)
{
    if (fprintf(file, "VITOUT_SCORES 1\ncount=%zu\n", scores->count) < 0) {
        return false;
    }
    for (size_t i = 0; i < scores->count; ++i) {
        const ScoreRecord *record = &scores->records[i];
        if (fprintf(file, "%s %u %" PRIu32 " %u %u %" PRIu32 "\n",
                    record->level_id, (unsigned)record->difficulty, record->points,
                    record->medal, record->lives, record->time_ms) < 0) {
            return false;
        }
    }
    return fputs("END\n", file) >= 0 && fflush(file) == 0 && !ferror(file);
}

static void discard_temporary(const char *path, char *error, size_t error_size)
{
    if (remove(path) != 0 && errno != ENOENT && error != NULL && error_size != 0) {
        size_t used = bounded_length(error, error_size);
        if (used < error_size) {
            (void)snprintf(error + used, error_size - used, "; remove stale .tmp");
        }
    }
}

static bool recovery_valid(const char *backup, char *error, size_t error_size)
{
    Scores *old = calloc(1, sizeof(*old));
    bool success;
    if (old == NULL) {
        score_error(error, error_size, backup, 0, "not enough memory to verify recovery save");
        return false;
    }
    success = read_scores(old, backup, false, error, error_size);
    free(old);
    return success;
}

bool scores_save(const Scores *scores, const char *path, char *error, size_t error_size)
{
    char temporary[SCORE_PATH_SIZE];
    char backup[SCORE_PATH_SIZE];
    FILE *file;
    bool written;
    int current_state;
    int backup_state;
    int saved_errno;

    if (error != NULL && error_size != 0) {
        error[0] = '\0';
    }
    if (!scores_valid(scores, path, error, error_size) ||
        !sibling_path(path, ".tmp", temporary, error, error_size) ||
        !sibling_path(path, ".bak", backup, error, error_size)) {
        return false;
    }
    current_state = score_file_state(path, error, error_size);
    backup_state = score_file_state(backup, error, error_size);
    if (current_state < 0 || backup_state < 0) {
        return false;
    }
    if (current_state != 0 && backup_state != 0) {
        score_error(error, error_size, backup, 0,
                    "backup already exists; keep current save and move this backup aside");
        return false;
    }
    if (backup_state != 0 && !recovery_valid(backup, error, error_size)) {
        return false;
    }
    file = fopen(temporary, "wbx");
    if (file == NULL) {
        score_error(error, error_size, temporary, 0,
                    "cannot create save staging file: %s; check free space/stale .tmp", strerror(errno));
        return false;
    }
    written = write_scores(file, scores);
    saved_errno = errno;
    if (fclose(file) != 0) {
        saved_errno = errno;
        written = false;
    }
    if (!written) {
        score_error(error, error_size, temporary, 0, "write/close failed: %s", strerror(saved_errno));
        discard_temporary(temporary, error, error_size);
        return false;
    }
    /* Vita newlib rename() unlinks an existing destination before attempting the
       rename. Move the old save aside first; never use it as a rename target. */
    current_state = score_file_state(path, error, error_size);
    backup_state = score_file_state(backup, error, error_size);
    if (current_state < 0 || backup_state < 0 ||
        (current_state != 0 && backup_state != 0)) {
        if (current_state > 0 && backup_state > 0) {
            score_error(error, error_size, backup, 0, "backup appeared during save; no files replaced");
        }
        discard_temporary(temporary, error, error_size);
        return false;
    }
    if (current_state != 0) {
        if (rename(path, backup) != 0) {
            score_error(error, error_size, path, 0, "cannot protect old save: %s", strerror(errno));
            discard_temporary(temporary, error, error_size);
            return false;
        }
        backup_state = 1;
    }
    if (rename(temporary, path) != 0) {
        saved_errno = errno;
        if (backup_state != 0) {
            int target_state = score_file_state(path, NULL, 0);
            if (target_state == 0 && rename(backup, path) == 0) {
                score_error(error, error_size, path, 0,
                            "save install failed; previous save restored: %s", strerror(saved_errno));
            } else {
                score_error(error, error_size, backup, 0,
                            "save install failed; previous save kept here; restore it: %s",
                            strerror(saved_errno));
            }
        } else {
            score_error(error, error_size, path, 0, "cannot install save: %s", strerror(saved_errno));
        }
        discard_temporary(temporary, error, error_size);
        return false;
    }
    if (backup_state != 0 && remove(backup) != 0) {
        score_error(error, error_size, backup, 0,
                    "new save written; old backup cleanup failed: %s; move it aside", strerror(errno));
        return false;
    }
    return true;
}
