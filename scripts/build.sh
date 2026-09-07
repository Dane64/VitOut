#!/usr/bin/env bash
set -euo pipefail

if (( $# > 1 )) || { (( $# == 1 )) && [[ "$1" != "--metadata" ]]; }; then
    printf 'Usage: %s [--metadata]\n' "$0" >&2
    exit 2
fi

release_tag="${RELEASE_TAG:-}"
filename=VitOut.vpk
vita_version=
prerelease=false

if [[ -n "$release_tag" ]]; then
    tag_pattern='^v?(0|[1-9][0-9]?)\.(0|[1-9][0-9]?)(-[0-9A-Za-z-]+(\.[0-9A-Za-z-]+)*)?$'
    if [[ ! "$release_tag" =~ $tag_pattern ]]; then
        printf 'Invalid release tag: %s. Use [v]MAJOR.MINOR[-PRERELEASE], with numbers from 0 to 99 and no leading zeros.\n' "$release_tag" >&2
        exit 1
    fi
    printf -v vita_version '%02d.%02d' "${BASH_REMATCH[1]}" "${BASH_REMATCH[2]}"
    filename="VitOut-${release_tag#v}.vpk"
    if [[ -n "${BASH_REMATCH[3]}" ]]; then
        prerelease=true
    fi
fi

if (( $# == 1 )); then
    printf 'filename=%s\nvita-version=%s\nprerelease=%s\n' "$filename" "$vita_version" "$prerelease"
    exit 0
fi

source_dir="${SOURCE_DIR:-/src}"
build_dir="${BUILD_DIR:-/build}"
output_dir="${OUTPUT_DIR:-/out}"

# Clear a previous tag's version when reusing a local build directory.
cmake_args=(
    -S "$source_dir" -B "$build_dir" -G Ninja
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_C_COMPILER_LAUNCHER=ccache
    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
    -U VITA_VERSION
)
if [[ -n "$vita_version" ]]; then
    cmake_args+=("-DVITA_VERSION=$vita_version")
fi

ccache --zero-stats
cmake "${cmake_args[@]}"
cmake --build "$build_dir" --parallel
unzip -t "$build_dir/VitOut.vpk"
install -Dm644 "$build_dir/VitOut.vpk" "$output_dir/$filename"
(
    cd "$output_dir"
    sha256sum "$filename" > SHA256SUMS
)
ccache --show-stats
