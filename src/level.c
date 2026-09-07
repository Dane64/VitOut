#include "level.h"

#include <dirent.h>
#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define LEVEL_FILE_LIMIT 16384u
#define LEVEL_LINE_LIMIT 160u
#define LEVEL_PATH_SIZE 1024u
#define LEVEL_FILENAME_SIZE 256u
#define LEVEL_DIRECTORY_LIMIT 4096u
#define LEVEL_CANDIDATE_LIMIT 512u
#define LEVEL_MIN_TIME 0.001f
#define LEVEL_MAX_TIME 86400.0f
#define CATALOG_FIRST_SIZE (LEVEL_MESSAGE_SIZE - 48)
#define COLOR(r, g, b) \
    ((uint32_t)(r) | ((uint32_t)(g) << 8) | ((uint32_t)(b) << 16) | UINT32_C(0xff000000))

typedef struct {
    FILE *file;
    const char *path;
    size_t bytes;
    size_t line;
    char *error;
    size_t error_size;
} LevelReader;

typedef struct {
    char name[LEVEL_FILENAME_SIZE];
} LevelFilename;

static size_t bounded_length(const char *text, size_t limit)
{
    size_t length = 0;
    while (length < limit && text[length] != '\0') {
        ++length;
    }
    return length;
}

static void level_error(char *error, size_t size, const char *path, size_t line,
                        const char *format, ...)
{
    char reason[112];
    va_list arguments;
    const char *label = path != NULL ? path : "level";
    size_t length = bounded_length(label, LEVEL_PATH_SIZE);

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

static bool path_valid(const char *path)
{
    return path != NULL && path[0] != '\0' &&
           bounded_length(path, LEVEL_PATH_SIZE) < LEVEL_PATH_SIZE;
}

bool level_decode_symbol(char symbol, LevelCell *cell)
{
    static const uint32_t colors[6] = {
        COLOR(64, 220, 255), COLOR(100, 235, 110), COLOR(255, 220, 70),
        COLOR(255, 155, 65), COLOR(255, 100, 190), COLOR(175, 130, 255)
    };
    LevelCell decoded = {COLOR(0, 0, 0), 0, POWERUP_NONE};

    if (cell == NULL) {
        return false;
    }
    if (symbol >= '1' && symbol <= '6') {
        decoded.hits = (unsigned char)(symbol - '0');
        decoded.color = colors[(unsigned)(symbol - '1')];
    } else {
        switch (symbol) {
        case '.':
        case ' ':
            break;
        case 'B':
            decoded.hits = 1;
            break;
        case 'K':
            decoded.hits = 2;
            decoded.color = COLOR(255, 165, 0);
            break;
        case 'R':
            decoded.hits = 2;
            decoded.color = COLOR(255, 0, 0);
            break;
        case 'W':
            decoded.hits = 2;
            decoded.color = COLOR(255, 255, 255);
            break;
        case 'b':
            decoded.hits = 1;
            decoded.color = COLOR(139, 69, 19);
            break;
        case 'p':
            decoded.hits = 2;
            decoded.color = COLOR(255, 192, 203);
            break;
        case 'w':
            decoded.hits = 3;
            decoded.color = COLOR(245, 222, 179);
            break;
        case 'E':
            decoded.hits = 1;
            decoded.color = COLOR(80, 230, 130);
            decoded.powerup = POWERUP_WIDE;
            break;
        case 'S':
            decoded.hits = 1;
            decoded.color = COLOR(70, 190, 255);
            decoded.powerup = POWERUP_SLOW;
            break;
        case 'M':
            decoded.hits = 1;
            decoded.color = COLOR(235, 120, 255);
            decoded.powerup = POWERUP_MULTIBALL;
            break;
        case 'L':
            decoded.hits = 1;
            decoded.color = COLOR(255, 110, 125);
            decoded.powerup = POWERUP_LIFE;
            break;
        default:
            return false;
        }
    }
    *cell = decoded;
    return true;
}

static int level_read_byte(LevelReader *reader)
{
    int byte = fgetc(reader->file);
    if (byte == EOF) {
        if (ferror(reader->file)) {
            level_error(reader->error, reader->error_size, reader->path,
                        reader->line, "read failed: %s", strerror(errno));
            return -2;
        }
        return EOF;
    }
    ++reader->bytes;
    if (reader->bytes > LEVEL_FILE_LIMIT) {
        level_error(reader->error, reader->error_size, reader->path,
                    reader->line, "file exceeds %u bytes", LEVEL_FILE_LIMIT);
        return -2;
    }
    return byte;
}

static int level_read_line(LevelReader *reader, char line[LEVEL_LINE_LIMIT + 1])
{
    size_t length = 0;
    ++reader->line;
    for (;;) {
        int byte = level_read_byte(reader);
        if (byte == -2) {
            return -1;
        }
        if (byte == EOF || byte == '\n') {
            line[length] = '\0';
            return byte == EOF && length == 0 ? 0 : 1;
        }
        if (byte == '\r') {
            byte = level_read_byte(reader);
            if (byte == -2) {
                return -1;
            }
            if (byte != '\n') {
                level_error(reader->error, reader->error_size, reader->path,
                            reader->line, "bare CR; use LF or CRLF line endings");
                return -1;
            }
            line[length] = '\0';
            return 1;
        }
        if ((byte < 32 && byte != '\t') || byte > 126) {
            level_error(reader->error, reader->error_size, reader->path,
                        reader->line, "non-ASCII or binary byte 0x%02x", (unsigned)byte);
            return -1;
        }
        if (length == LEVEL_LINE_LIMIT) {
            level_error(reader->error, reader->error_size, reader->path,
                        reader->line, "line exceeds %u characters", LEVEL_LINE_LIMIT);
            return -1;
        }
        line[length++] = (char)byte;
    }
}

static bool ignored_line(const char *line)
{
    while (*line == ' ' || *line == '\t') {
        ++line;
    }
    return *line == '\0' || *line == '#';
}

static bool identifier_valid(const char *id)
{
    size_t length = bounded_length(id, LEVEL_ID_SIZE);
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

static bool name_valid(const char *name)
{
    size_t length = bounded_length(name, LEVEL_NAME_SIZE);
    if (length == 0 || length == LEVEL_NAME_SIZE) {
        return false;
    }
    for (size_t i = 0; i < length; ++i) {
        if ((unsigned char)name[i] < 32 || (unsigned char)name[i] > 126) {
            return false;
        }
    }
    return true;
}

static bool times_valid(const char *text, float times[3])
{
    const char *cursor = text;
    for (const char *p = text; *p != '\0'; ++p) {
        if (!((*p >= '0' && *p <= '9') || *p == '.' || *p == ',' ||
              *p == 'e' || *p == 'E' || *p == '+' || *p == '-')) {
            return false;
        }
    }
    for (size_t i = 0; i < 3; ++i) {
        char *end = NULL;
        if (!((*cursor >= '0' && *cursor <= '9') || *cursor == '.')) {
            return false;
        }
        errno = 0;
        times[i] = strtof(cursor, &end);
        if (end == cursor || errno == ERANGE || !isfinite(times[i]) ||
            times[i] < LEVEL_MIN_TIME || times[i] > LEVEL_MAX_TIME ||
            (i != 0 && times[i] <= times[i - 1])) {
            return false;
        }
        if (i < 2) {
            if (*end != ',') {
                return false;
            }
            cursor = end + 1;
        } else if (*end != '\0') {
            return false;
        }
    }
    return true;
}

static bool level_parse(LevelReader *reader, Level *level)
{
    char line[LEVEL_LINE_LIMIT + 1];
    unsigned fields = 0;
    size_t bricks = 0;
    int status = level_read_line(reader, line);
    if (status < 0) {
        return false;
    }
    if (status == 0 || strcmp(line, "VITOUT_LEVEL 1") != 0) {
        level_error(reader->error, reader->error_size, reader->path, reader->line,
                    "expected VITOUT_LEVEL 1");
        return false;
    }
    for (;;) {
        status = level_read_line(reader, line);
        if (status < 0) {
            return false;
        }
        if (status == 0) {
            level_error(reader->error, reader->error_size, reader->path, reader->line,
                        "missing [bricks] and grid");
            return false;
        }
        if (ignored_line(line)) {
            continue;
        }
        if (strcmp(line, "[bricks]") == 0) {
            break;
        }
        if (strncmp(line, "id=", 3) == 0) {
            if ((fields & 1u) != 0 || !identifier_valid(line + 3)) {
                level_error(reader->error, reader->error_size, reader->path,
                            reader->line, "invalid or duplicate id (1..47 a-z, 0-9, -, _)");
                return false;
            }
            (void)strcpy(level->id, line + 3);
            fields |= 1u;
        } else if (strncmp(line, "name=", 5) == 0) {
            if ((fields & 2u) != 0 || !name_valid(line + 5)) {
                level_error(reader->error, reader->error_size, reader->path,
                            reader->line, "invalid or duplicate name (1..63 printable ASCII)");
                return false;
            }
            (void)strcpy(level->name, line + 5);
            fields |= 2u;
        } else if (strncmp(line, "times=", 6) == 0) {
            if ((fields & 4u) != 0 || !times_valid(line + 6, level->medal_times)) {
                level_error(reader->error, reader->error_size, reader->path,
                            reader->line, "invalid/duplicate times; need 0.001 <= gold < silver < bronze <= 86400");
                return false;
            }
            fields |= 4u;
        } else {
            level_error(reader->error, reader->error_size, reader->path, reader->line,
                        "unknown header field; use id, name, times");
            return false;
        }
    }
    if (fields != 7u) {
        level_error(reader->error, reader->error_size, reader->path, reader->line,
                    "missing required id, name or times");
        return false;
    }
    for (size_t row = 0; row < LEVEL_ROWS; ++row) {
        status = level_read_line(reader, line);
        if (status < 0) {
            return false;
        }
        if (status == 0) {
            level_error(reader->error, reader->error_size, reader->path, reader->line,
                        "expected 20 brick rows; found %zu", row);
            return false;
        }
        if (strlen(line) != LEVEL_COLS) {
            level_error(reader->error, reader->error_size, reader->path, reader->line,
                        "brick row %zu must have exactly 30 columns", row + 1);
            return false;
        }
        for (size_t column = 0; column < LEVEL_COLS; ++column) {
            LevelCell *cell = &level->cells[row * LEVEL_COLS + column];
            if (!level_decode_symbol(line[column], cell)) {
                level_error(reader->error, reader->error_size, reader->path,
                            reader->line, "invalid brick '%c' in column %zu",
                            line[column], column + 1);
                return false;
            }
            if (cell->hits != 0) {
                ++bricks;
            }
        }
    }
    while ((status = level_read_line(reader, line)) > 0) {
        if (!ignored_line(line)) {
            level_error(reader->error, reader->error_size, reader->path, reader->line,
                        "unexpected data after 20 brick rows");
            return false;
        }
    }
    if (status < 0) {
        return false;
    }
    if (bricks == 0) {
        level_error(reader->error, reader->error_size, reader->path, reader->line,
                    "empty level; add at least one brick");
        return false;
    }
    return true;
}

bool level_load_file(const char *path, Level *level, char *error, size_t error_size)
{
    struct stat info;
    Level parsed = {0};
    LevelReader reader = {NULL, path, 0, 0, error, error_size};
    bool success;

    if (error != NULL && error_size != 0) {
        error[0] = '\0';
    }
    if (level == NULL || !path_valid(path)) {
        level_error(error, error_size, path, 0, "invalid output or path (maximum 1023 bytes)");
        return false;
    }
    if (stat(path, &info) != 0) {
        level_error(error, error_size, path, 0, "cannot stat file: %s", strerror(errno));
        return false;
    }
    if (!S_ISREG(info.st_mode)) {
        level_error(error, error_size, path, 0, "not a regular file");
        return false;
    }
    reader.file = fopen(path, "rb");
    if (reader.file == NULL) {
        level_error(error, error_size, path, 0, "cannot open file: %s", strerror(errno));
        return false;
    }
    success = level_parse(&reader, &parsed);
    if (fclose(reader.file) != 0 && success) {
        level_error(error, error_size, path, reader.line, "close failed: %s", strerror(errno));
        success = false;
    }
    if (success) {
        *level = parsed;
    }
    return success;
}

static void catalog_note(LevelCatalog *catalog, const char *message)
{
    if (catalog->message[0] == '\0') {
        (void)snprintf(catalog->message, CATALOG_FIRST_SIZE, "%.*s",
                       CATALOG_FIRST_SIZE - 1, message);
    }
}

static bool level_extension(const char *name, size_t length)
{
    return length > 4 && memcmp(name + length - 4, ".lvl", 4) == 0;
}

static void catalog_scan(LevelCatalog *catalog, const char *path, bool custom)
{
    DIR *directory;
    LevelFilename *files;
    size_t count = 0;
    size_t entries = 0;
    bool overflow = false;
    char error[LEVEL_MESSAGE_SIZE];
    char filename[LEVEL_PATH_SIZE];

    if (!path_valid(path)) {
        level_error(error, sizeof(error), path, 0, "invalid directory path (maximum 1023 bytes)");
        catalog_note(catalog, error);
        return;
    }
    directory = opendir(path);
    if (directory == NULL) {
        if (custom && errno == ENOENT) {
            return;
        }
        level_error(error, sizeof(error), path, 0, "cannot open directory: %s", strerror(errno));
        catalog_note(catalog, error);
        return;
    }
    files = malloc(LEVEL_CANDIDATE_LIMIT * sizeof(*files));
    if (files == NULL) {
        level_error(error, sizeof(error), path, 0, "not enough memory to scan levels");
        catalog_note(catalog, error);
        (void)closedir(directory);
        return;
    }
    for (;;) {
        struct dirent *entry;
        struct stat info;
        size_t length;
        size_t position;
        int written;

        errno = 0;
        entry = readdir(directory);
        if (entry == NULL) {
            if (errno != 0) {
                level_error(error, sizeof(error), path, 0, "directory read failed: %s", strerror(errno));
                catalog_note(catalog, error);
            }
            break;
        }
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        if (++entries > LEVEL_DIRECTORY_LIMIT) {
            level_error(error, sizeof(error), path, 0,
                        "over %u directory entries; directory skipped; move unrelated files",
                        LEVEL_DIRECTORY_LIMIT);
            catalog_note(catalog, error);
            overflow = true;
            break;
        }
        length = strlen(entry->d_name);
        if (!level_extension(entry->d_name, length)) {
            continue;
        }
        if (length >= LEVEL_FILENAME_SIZE) {
            ++catalog->rejected;
            level_error(error, sizeof(error), path, 0, ".lvl filename exceeds 255 bytes");
            catalog_note(catalog, error);
            continue;
        }
        written = snprintf(filename, sizeof(filename), "%s/%s", path, entry->d_name);
        if (written < 0 || (size_t)written >= sizeof(filename)) {
            ++catalog->rejected;
            level_error(error, sizeof(error), path, 0, ".lvl path exceeds 1023 bytes");
            catalog_note(catalog, error);
            continue;
        }
        if (stat(filename, &info) != 0) {
            ++catalog->rejected;
            level_error(error, sizeof(error), filename, 0, "cannot stat level: %s", strerror(errno));
            catalog_note(catalog, error);
            continue;
        }
        if (!S_ISREG(info.st_mode)) {
            continue;
        }
        position = 0;
        while (position < count && strcmp(files[position].name, entry->d_name) < 0) {
            ++position;
        }
        if (count == LEVEL_CANDIDATE_LIMIT) {
            ++catalog->rejected;
            level_error(error, sizeof(error), path, 0,
                        "over %u .lvl files; only first %u filenames considered",
                        LEVEL_CANDIDATE_LIMIT, LEVEL_CANDIDATE_LIMIT);
            catalog_note(catalog, error);
            if (position == count) {
                continue;
            }
            --count;
        }
        if (position < count) {
            memmove(&files[position + 1], &files[position],
                    (count - position) * sizeof(*files));
        }
        (void)strcpy(files[position].name, entry->d_name);
        ++count;
    }
    if (closedir(directory) != 0) {
        level_error(error, sizeof(error), path, 0, "directory close failed: %s", strerror(errno));
        catalog_note(catalog, error);
    }
    if (overflow) {
        catalog->rejected += count;
        free(files);
        return;
    }
    for (size_t i = 0; i < count; ++i) {
        Level *level;
        bool duplicate = false;
        int written = snprintf(filename, sizeof(filename), "%s/%s", path, files[i].name);
        if (written < 0 || (size_t)written >= sizeof(filename)) {
            ++catalog->rejected;
            level_error(error, sizeof(error), path, 0, ".lvl path exceeds 1023 bytes");
            catalog_note(catalog, error);
            continue;
        }
        if (catalog->count == LEVEL_MAX_COUNT) {
            ++catalog->rejected;
            level_error(error, sizeof(error), filename, 0, "128-level limit; remove unused .lvl files");
            catalog_note(catalog, error);
            continue;
        }
        level = &catalog->levels[catalog->count];
        if (!level_load_file(filename, level, error, sizeof(error))) {
            ++catalog->rejected;
            catalog_note(catalog, error);
            continue;
        }
        for (size_t j = 0; j < catalog->count; ++j) {
            if (strcmp(catalog->levels[j].id, level->id) == 0) {
                duplicate = true;
                break;
            }
        }
        if (duplicate) {
            ++catalog->rejected;
            level_error(error, sizeof(error), filename, 0,
                        "duplicate id '%s'; give this level a unique id", level->id);
            catalog_note(catalog, error);
            continue;
        }
        level->external = custom;
        ++catalog->count;
    }
    free(files);
}

bool level_catalog_load(LevelCatalog *catalog, const char *bundled_dir,
                        const char *custom_dir)
{
    if (catalog == NULL) {
        return false;
    }
    catalog->count = 0;
    catalog->rejected = 0;
    catalog->message[0] = '\0';
    catalog_scan(catalog, bundled_dir, false);
    if (custom_dir != NULL) {
        catalog_scan(catalog, custom_dir, true);
    }
    if (catalog->count == 0 && catalog->message[0] == '\0') {
        (void)snprintf(catalog->message, CATALOG_FIRST_SIZE, "No usable .lvl files; add levels to %.80s",
                       custom_dir != NULL ? custom_dir : (bundled_dir != NULL ? bundled_dir : "levels"));
    }
    if (catalog->message[0] != '\0') {
        size_t used = strlen(catalog->message);
        (void)snprintf(catalog->message + used, sizeof(catalog->message) - used,
                       "; %zu file(s) rejected", catalog->rejected);
    }
    return catalog->count != 0;
}
