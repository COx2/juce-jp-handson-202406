#!/bin/sh

set -e -u -o pipefail

realpath() {
  [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

SCRIPT_NAME="$(basename "$(realpath "$0")")"
SCRIPT_DIRECTORY="$(dirname "$(realpath "$0")")"

cd "${SCRIPT_DIRECTORY}/../"

echo "Configure from preset..."
cmake --preset xcode

echo "Build Debug version..."
cmake --build --preset xcode-debug

echo "Build Release version..."
cmake --build --preset xcode-release
