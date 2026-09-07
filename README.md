# VitOut

[<img src="https://img.shields.io/github/downloads/Dane64/VitOut/total">](https://github.com/Dane64/VitOut/releases)
[<img src="https://img.shields.io/github/v/release/Dane64/Vitout">](https://github.com/Dane64/VitOut/releases/latest)

## Gameplay

Clear every brick while keeping as many lives as possible. Darker brick cores
indicate additional hit points. Choose a level and difficulty in the menu;
each combination has its own best score, medal, remaining lives, and clear time.
Medal thresholds belong to the level, and only active play counts toward the
timer: aiming a new serve and pausing do not consume time.

| Difficulty | Starting lives | Paddle width | Starting ball speed | Points |
|---|---:|---:|---:|---:|
| Easy | 5 | 180 px | 330 px/s | x1 |
| Normal | 3 | 140 px | 420 px/s | x2 |
| Hard | 2 | 108 px | 540 px/s | x3 |

The ball accelerates as it damages bricks, with a difficulty-specific speed
limit. Catch falling capsules with the paddle to activate powerups. Marked
bricks guarantee the indicated capsule; other destroyed bricks have a 25%,
18%, or 12% drop chance on Easy, Normal, or Hard.

| Capsule | Effect |
|---|---|
| E | Wide paddle: 50% wider for 14 seconds |
| S | Slow ball: all balls move at 70% speed for 10 seconds |
| M | Multiball: split into up to three balls |
| L | Extra life, up to a maximum of five |

Collecting E or S again refreshes its timer instead of stacking the effect.
You lose a life only when the last ball falls out of play. Losing a life clears
capsules and temporary effects and prepares a fresh serve. Pausing freezes the
entire simulation, including powerups.

| Screen | Controls |
|---|---|
| Menu | D-pad Up/Down selects; Left/Right changes level or difficulty; Cross confirms; Start plays |
| Level selection | L/R changes levels from any menu row; Select reloads level files |
| Playing | Left stick or D-pad moves; Cross launches; Start pauses |
| Paused | Cross/Start resumes; Triangle restarts; Circle returns to the menu |
| Results | Cross plays the next level after a win, or retries after a loss; Triangle retries; Circle opens the menu |

## External levels

The six classic layouts and four new powerup-focused levels are shipped as
plain-text `.lvl` files, not compiled arrays. Add more without rebuilding or
reinstalling the game:

1. Copy a bundled file from [`levels/`](levels/) and give it a new `id` and `name`.
2. Edit its 20-by-30 brick grid using a text editor or an external generator.
3. Transfer it with VitaShell USB or FTP to `ux0:/data/VitOut/levels/`.
4. Press **Select** in the game's menu (or choose **Reload custom levels**).

See the [level format and authoring guide](levels/README.md) for metadata,
symbols, examples, and limits. Bundled files load from `app0:/levels/`; custom
files follow them in filename order. Up to 128 total levels are supported.
Malformed files and duplicate IDs are reported and skipped without preventing
valid levels from loading.

Scores live in `ux0:/data/VitOut/scores.dat` and use the stable level ID and
difficulty, not the file's position in the menu. Renaming or reordering a file
therefore does not mix up scores. Use a new ID for a substantially changed
layout. An unreadable or corrupt score file is reported and is not overwritten.

## Historical screenshots

These animations show the original interface; the current version has a
text-based menu, level previews, a gameplay HUD, and pause/results screens.

<img src="Screenshots/MainMenu.gif"><br>
<img src="Screenshots/LevelSelect.gif"><br>
<img src="Screenshots/Game.gif"><br>

## Building

The builder follows `ubuntu:latest`, current Ubuntu build-tool packages, and the
newest supported VitaSDK release series for the existing Vita ABI. VitaSDK's
upstream bootstrap verifies signed channel metadata and installs matching
compiler and library packages. No image digests, package versions, or dependency
checksums are pinned in this repository. Only `libvita2d` and its dependencies
are installed, rather than compiling the SDK or installing every VitaSDK package.
The container targets `linux/amd64`.

The gameplay and content modules are platform-independent C11. The Vita target
uses target-scoped GNU C11 settings, strict prototypes, and `-fno-common`, rather
than relying on the compiler's changing default language standard. CMake 3.16
or newer, including CMake 4.x, is supported. Simulation uses a bounded 120 Hz
fixed step with swept collisions; input, presentation, and monotonic timing are
kept in the Vita-specific layer.

### Podman

Build the image, then run the tests, compiler, and packager in the container:

```sh
podman build --pull=always --no-cache --tag vitout-builder .
mkdir -p build dist .cache/ccache
podman run --rm --network none --userns=keep-id \
  --volume "$PWD:/src:ro" \
  --volume "$PWD/build:/build" \
  --volume "$PWD/dist:/out" \
  --volume "$PWD/.cache/ccache:/ccache" \
  vitout-builder
```

Podman's `--userns=keep-id` makes rootless build outputs belong to the current
user. Podman does not provide BuildKit's per-stage `--no-cache-filter`, so
`--no-cache` refreshes all image-build stages. Keep the mounted build and ccache
directories to reuse compiled game objects.

### Docker / BuildKit

For selective dependency-metadata refresh while retaining installation layers:

```sh
docker buildx build --pull --platform linux/amd64 \
  --no-cache-filter package-index,sdk-index \
  --load --tag vitout-builder .
mkdir -p build dist .cache/ccache
docker run --rm --network none \
  --user "$(id -u):$(id -g)" \
  --volume "$PWD:/src:ro" \
  --volume "$PWD/build:/build" \
  --volume "$PWD/dist:/out" \
  --volume "$PWD/.cache/ccache:/ccache" \
  vitout-builder
```

The output is `dist/VitOut.vpk` and `dist/SHA256SUMS`. Builds without a release
tag retain the version declared in `CMakeLists.txt`. To build a release locally
without publishing it, add `--env RELEASE_TAG=v2.1` before the image name; this
produces `VitOut-2.1.vpk` with Vita package version `02.01`.

Keep the mounted build and compiler-cache directories for incremental local
builds. Run the image-build command again to pick up upstream updates. `--pull`
refreshes the base image; `--no-cache-filter` refreshes the package and SDK metadata
probes without discarding unchanged installation layers. Keep both options when
building locally so cached layers cannot indefinitely hide dependency updates.
Checksum files are ignored by Git; generated release checksums are still included
in workflow artifacts and GitHub Releases.

### Gameplay and content tests

Every container build of the game runs the Python `unittest` suite before
cross-compiling. The image includes a native compiler for portable engine,
controller, menu, level parser, and score persistence regression tests. Tests
also simulate the bundled levels at every difficulty, and exercise frame-rate
independence, collisions, life loss, pause/resume, and all four powerups.

Run just the tests in Podman:

```sh
podman run --rm --network none --userns=keep-id \
  --env PYTHONDONTWRITEBYTECODE=1 \
  --volume "$PWD:/src:ro" \
  --entrypoint python3 vitout-builder \
  -m unittest discover -s /src/tests -p 'test_*.py'
```

Add `--env VITOUT_SANITIZERS=1` to enable AddressSanitizer and
UndefinedBehaviorSanitizer for the native C tests. Temporary test files stay
inside the container; no SDK, native compiler, or Python install is needed on
the host.

## CI and releases

Every branch push and pull request runs the same containerized Release build
used for publishing, including gameplay/content tests and VPK archive integrity
checks. Branch builds are
available as the `VitOut-vpk` workflow artifact for seven days; they never create
a GitHub Release or derive a version from the branch name. CI can also be run
manually from the Actions tab.

The shared workflow caches builder-image layers with BuildKit and compiler
results with ccache. Dependency metadata is refreshed on every run, while
installation layers are reused when the resolved versions have not changed.
Source and artwork changes do not invalidate those layers. Compiler-cache keys
include the installed toolchain versions and source inputs, so unchanged
translation units can be reused across commits, subject to GitHub's branch cache
visibility. CI uses a fresh build directory and always packages the current
artwork and metadata instead of restoring a potentially stale VPK. Superseded
branch runs are cancelled; release runs are not.

Runners use `ubuntu-latest`, and actions follow their upstream `main` or `master`
branches rather than fixed commits. Upstream changes are therefore adopted
automatically; builds of an older source commit can change as dependencies evolve.

To publish a release, commit and push the release-ready branch, then push a new
tag on that commit:

```sh
git push origin HEAD
git tag -a v2.1 -m "VitOut 2.1"
git push origin v2.1
```

Tags use `[v]MAJOR.MINOR[-PRERELEASE]`, with each number between 0 and 99 and no
leading zeros. The optional `v` preserves compatibility with existing tags.
For example, `v2.1` and `2.1` both produce Vita version `02.01`; `v2.2-rc.1`
produces `02.02` and is marked as a GitHub prerelease. Vita's version field is
limited to `NN.NN`, so three-component tags such as `v2.1.3` are rejected rather
than silently discarding a patch version.

After the tagged build succeeds, the release workflow publishes the versioned
VPK and `SHA256SUMS` with generated release notes. Only the publishing job has
repository write permission, and it uses the built artifact without rebuilding.
No registry credentials or personal access token are required. Existing releases
are not overwritten; use a new version tag for a new release.

## Special Thanks to:

Vitadev Package manager for releasing the SDK \
I would like to thank the developers of Vita3K to help me create my content at a faster pace.
