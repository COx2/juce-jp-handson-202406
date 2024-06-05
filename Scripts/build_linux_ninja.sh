#!/bin/bash

set -e -u -o pipefail

realpath() {
  [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

SCRIPT_NAME="$(basename "$(realpath "$0")")"
SCRIPT_DIRECTORY="$(dirname "$(realpath "$0")")"

cd "${SCRIPT_DIRECTORY}/../"

echo "Configure from preset..."
cmake --preset ninja-multi

echo "Build Debug version..."
cmake --build --preset ninja-debug

echo "Build Release version..."
cmake --build --preset ninja-release
