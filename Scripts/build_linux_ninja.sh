#!/bin/bash

set -e -u -o pipefail

realpath() {
  [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

SCRIPT_NAME="$(basename "$(realpath "$0")")"
SCRIPT_DIRECTORY="$(dirname "$(realpath "$0")")"

cd "${SCRIPT_DIRECTORY}/../"

echo "Debug Configure from preset..."
cmake --preset ninja-single-debug

echo "Build Debug version..."
cmake --build --preset ninja-single-debug

echo "Release Configure from preset..."
cmake --preset ninja-single-release

echo "Build Release version..."
cmake --build --preset ninja-single-release
