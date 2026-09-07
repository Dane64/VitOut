#!/usr/bin/env bash
set -euo pipefail

if (( $# != 1 )); then
    printf 'Usage: %s OUTPUT_DIRECTORY\n' "$0" >&2
    exit 2
fi

output_dir="$1"
channel_base="${VITASDK_CHANNEL_BASE_URL:-https://vitasdk.org/channels}"
bootstrap_url="${VITASDK_BOOTSTRAP_SCRIPT_URL:-https://raw.githubusercontent.com/vitasdk/vdpm/HEAD/bootstrap-vitasdk.sh}"
mkdir -p "$output_dir"

download() {
    curl --fail --silent --show-error --location --retry 3 --output "$2" "$1"
}

download "$bootstrap_url" "$output_dir/bootstrap-vitasdk.sh"
download "$channel_base/index.json" "$output_dir/index.json"
channel=$(jq -er '
    .channels | to_entries
    | map(select(.value.status == "supported" and (.value.world // "vita") == "vita"))
    | if length == 0 then error("No supported VitaSDK series for the vita ABI")
      else sort_by(.key) | last | .key end
    | if test("^[0-9]{4}\\.[0-9]{2}$") then .
      else error("Invalid supported VitaSDK series name") end
' "$output_dir/index.json")

# These are cache inputs; the upstream bootstrap verifies signatures before installation.
download "$channel_base/$channel.json" "$output_dir/channel.json"
download "$channel_base/$channel.json.sig" "$output_dir/channel.json.sig"
