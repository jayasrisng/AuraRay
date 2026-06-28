#!/bin/sh

set -eu

repository_root=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
renderer=${1:-"$repository_root/build/cmake/auraray"}

case "$renderer" in
    /*) ;;
    *) renderer="$repository_root/$renderer" ;;
esac

if [ ! -x "$renderer" ]; then
    echo "Renderer executable not found: $renderer" >&2
    exit 1
fi

output_directory=$(mktemp -d "${TMPDIR:-/tmp}/auraray-repro.XXXXXX")
trap 'rm -rf "$output_directory"' EXIT INT TERM

"$renderer" --output-dir "$output_directory"

if command -v shasum >/dev/null 2>&1; then
    hash_file() { shasum -a 256 "$1" | awk '{print $1}'; }
elif command -v sha256sum >/dev/null 2>&1; then
    hash_file() { sha256sum "$1" | awk '{print $1}'; }
else
    echo "Neither shasum nor sha256sum is available." >&2
    exit 1
fi

while read -r expected relative_path; do
    output_file="$output_directory/$(basename "$relative_path")"
    if [ ! -f "$output_file" ]; then
        echo "Missing expected render: $output_file" >&2
        exit 1
    fi

    actual=$(hash_file "$output_file")
    if [ "$actual" != "$expected" ]; then
        echo "Hash mismatch for $relative_path" >&2
        echo "  expected: $expected" >&2
        echo "  actual:   $actual" >&2
        exit 1
    fi
done < "$repository_root/tests/expected_render_hashes.sha256"

echo "AuraRay reproducibility check passed."
