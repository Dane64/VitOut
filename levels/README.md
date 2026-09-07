# External levels and saved results

VitOut ships ten editable ASCII levels: six preserved classics, plus Powerup
Garden, Orbital Arcade, Fortress and Prism Rally. The classic layouts retain their
original colors, hit counts and medal thresholds. Original short rows are padded
on the **right**, not re-centered.

## Installing your own levels

1. Copy a `.lvl` file below as a starting point. Give your design a **new `id`**.
2. Open VitaShell and use USB mode, or start its FTP server and connect using
   the address VitaShell displays. Transfer the file to
   **`ux0:/data/VitOut/levels/`**. Create the directory if necessary.
3. Return to VitOut's menu and press **SELECT** to reload. No rebuild, reinstall
   or restart is necessary. Edit and reload from the menu, not during a round.
4. If a file is rejected, the menu reports the first problem and how many
   examined level files were rejected. Check the named file and line, fix it,
   then press SELECT again. Other valid levels remain playable.

Packaged levels come from `app0:/levels/` and appear first. Custom levels follow.
Within **each** directory, filenames are sorted bytewise (case-sensitive,
not natural-number order); prefixes such as `01-` and `02-` are useful.
Only regular files with a lowercase `.lvl` suffix are considered. Subdirectories
are not traversed. A missing custom directory is normal; other I/O errors are
reported even if some levels loaded successfully.

Every loaded level must have a unique ID. The first valid occurrence in the
above order wins; later duplicate files are **rejected**, never used to override
another level. In particular, a custom `classic-01` cannot replace the packaged
`classic-01`. Renaming a file or changing its display name does not change its
saved results. Changing its ID creates a different score key; do that when
publishing a substantially different challenge.

## Version 1 format

The first physical line is exactly:

```text
VITOUT_LEVEL 1
```

It is followed by these three required metadata fields, in any order, and the
exact marker `[bricks]`:

```text
id=classic-01
name=Classic Logo
times=300,500,700
[bricks]
```

* `id`: 1–47 characters, only lowercase ASCII `a`–`z`, digits, `-`, `_`.
* `name`: 1–63 printable ASCII characters, including spaces. Values are not
  trimmed. Use a meaningful, short display name.
* `times`: **gold,silver,bronze** thresholds in seconds. Values must be finite,
  within **0.001–86400** inclusive, and strictly increasing after conversion to
  the game's single-precision floats. Use decimal numbers (a fraction or decimal
  exponent is allowed); do not add units, spaces, signs before values or inline
  comments. Hexadecimal floating-point notation, `nan` and `inf` are not allowed.
* Unknown, duplicate or missing fields, or unsupported versions, reject the
  entire file. There is no optional metadata in version 1.

Immediately after `[bricks]` are **exactly 20 rows of exactly 30 characters**.
Do not indent, trim trailing spaces, insert comments, or leave out empty rows.
`.` is recommended for empty cells because editors tend to strip spaces.
At least one brick is required. A grid is never silently cropped or padded
by the loader.

Use ASCII text without a BOM. LF and CRLF are accepted, including mixed line
endings, and a final newline is optional. Bare CR, NUL, non-ASCII bytes and other
control characters are rejected. After the version line but **before** `[bricks]`,
and **after all 20 rows**, empty/space/tab-only lines and comment lines whose
first non-space/tab character is `#` are allowed. Tabs are only useful in those
ignored lines; they are invalid in names, metadata numbers and the grid.

### Symbols

Colors below are decimal RGB; nonempty cells have alpha 255. They are packed as
`r | (g << 8) | (b << 16) | (255u << 24)`, matching Vita2D `RGBA8`.
An empty cell also stores opaque black but is not drawn.

| Symbol | Hits | RGB | Guaranteed drop |
| --- | ---: | --- | --- |
| `.` or space | 0 | 0, 0, 0 (not drawn) | none |
| `1` | 1 | 64, 220, 255 — cyan | none |
| `2` | 2 | 100, 235, 110 — green | none |
| `3` | 3 | 255, 220, 70 — yellow | none |
| `4` | 4 | 255, 155, 65 — orange | none |
| `5` | 5 | 255, 100, 190 — pink | none |
| `6` | 6 | 175, 130, 255 — violet | none |
| `B` | 1 | 0, 0, 0 — legacy black | none |
| `K` | 2 | 255, 165, 0 — legacy orange | none |
| `R` | 2 | 255, 0, 0 — legacy red | none |
| `W` | 2 | 255, 255, 255 — legacy white | none |
| `b` | 1 | 139, 69, 19 — legacy brown | none |
| `p` | 2 | 255, 192, 203 — legacy pink | none |
| `w` | 3 | 245, 222, 179 — legacy wheat | none |
| `E` | 1 | 80, 230, 130 — green | wider paddle |
| `S` | 1 | 70, 190, 255 — blue | slower balls |
| `M` | 1 | 235, 120, 255 — purple | multiball |
| `L` | 1 | 255, 110, 125 — coral | extra life, up to five |

Uppercase and lowercase symbols are different. Guaranteed drops are released
when their brick breaks and must still be caught with the paddle. Other bricks
may drop random powerups according to the selected difficulty; that randomness
belongs to the game, not to the file parser.

### Complete example

Save as `11-four-gifts.lvl`. The blank-looking rows below each contain 30 dots.

```text
VITOUT_LEVEL 1
# A short warm-up with all four guaranteed powerups.
id=four-gifts
name=Four Gifts
times=30,60,120
[bricks]
..............................
..............................
..............................
..............................
..............................
..............................
..............................
..............................
..............................
..............................
..............................
..............................
..............................
..............................
..............................
..............................
..............................
..............................
..............................
.............ESML.............
```

### Bounds and diagnostics

* Maximum **128 usable levels** across both directories.
* Maximum **16,384 bytes per file** and **160 characters per physical line**,
  excluding its LF/CRLF terminator. Ignored comments still count toward limits.
* Maximum **255 bytes per filename**, **1,023 bytes per complete path**.
* Each directory considers at most **512 regular `.lvl` files**, keeping the
  lexicographically first 512 regardless of native directory enumeration order.
  Additional files are rejected. Invalid files within that set do not cause
  later, unconsidered files to be admitted.
* Each directory scan examines at most **4,096 non-dot directory entries**,
  including unrelated files. If there are more, **that whole directory is
  skipped**, rather than admitting an unpredictable enumeration-order subset.
  Move unrelated files elsewhere. Files already examined and rejected are
  counted; unexamined files cannot be counted. Directory I/O/scan-limit warnings
  are displayed even when the rejected-file count is zero.

The parser uses bounded buffers; catalog filenames use one fixed-capacity
allocation. Failed individual loads leave the previous `Level` untouched.
Reload rebuilds the catalog in place without making a megabyte-sized stack copy.

## Medals and score identity

Only completed wins are recorded. Gold, silver and bronze mean finishing at or
below the corresponding inclusive threshold; a slower completion has no medal.
The six original thresholds are:

| ID | Name | Gold | Silver | Bronze |
| --- | --- | ---: | ---: | ---: |
| `classic-01` | Classic Logo | 300 | 500 | 700 |
| `classic-02` | Stairway | 220 | 300 | 500 |
| `classic-03` | Stripes | 400 | 500 | 800 |
| `classic-04` | Cat | 380 | 500 | 650 |
| `classic-05` | Cruiser | 430 | 500 | 600 |
| `classic-06` | Last Brick | 10 | 20 | 30 |

Scores are keyed by **stable level ID and difficulty**, not the menu's ordinal
index. Easy, Normal and Hard never overwrite one another. Best points, best
medal (then most remaining lives for that medal), and fastest nonzero completion
time are merged **independently**; they can represent three different wins.

`ux0:/data/VitOut/scores.dat` is a strict ASCII versioned file. For example:

```text
VITOUT_SCORES 1
count=2
classic-01 1 2400 3 4 285000
powerup-garden 0 900 2 5 185000
END
```

Records have six fields separated by exactly one space:
`id difficulty points medal lives time_ms`. Difficulty is `0`/`1`/`2` for
Easy/Normal/Hard; medal is `0`/`1`/`2`/`3` for none/bronze/silver/gold.
Points and time are unsigned 32-bit integers; lives are 1–5. New results require
positive time. There may be at most 512 records, 65,536 bytes per score file and
128 characters per line. `count` and `END` detect truncated saves, including
missing complete record lines. Blank lines, comments, extra fields and duplicate
keys are not permitted. LF/CRLF and a missing final newline are supported.

If the new save is missing, an exact six-byte legacy `ux0:/data/VitOut.save`
can be migrated to `classic-01`…`classic-06` on **Normal**: bytes `1`–`5` are
bronze, `11`–`15` silver, `21`–`25` gold, with the last digit giving lives.
Zero means no score. Migrated records alone may have zero points and zero time;
the legacy file is never deleted. A malformed current save or existing legacy
save is an explicit error, not an excuse to silently clear it.

Saving uses an exclusively created sibling `scores.dat.tmp`, checks all writes
and close, moves the old save to `scores.dat.bak`, then installs the new save.
This avoids Vita newlib's `rename()` behavior of unlinking an existing
destination **before** attempting its rename. Failed installation restores the
old save when possible; if rollback also fails, the old bytes remain in `.bak`.
A missing main save automatically loads a valid `.bak`, and the next successful
save finishes that recovery. This is a single-writer protocol, not a guarantee
against storage failure or power loss during filesystem operations.

Do not edit score files while the game is running. If a stale `.tmp` blocks
saving, back up the files and remove only that `.tmp`. If both the main save
and `.bak` exist, the main save is used, but saving refuses to overwrite the
backup: copy it somewhere safe and move it aside. If a current save is corrupt,
the game disables saving rather than overwriting it; preserve a copy before
repairing or replacing it. Score paths are limited to 1,019 bytes so their
four-character transaction suffixes always fit.
