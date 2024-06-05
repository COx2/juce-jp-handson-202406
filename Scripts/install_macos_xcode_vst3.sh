#!/bin/sh

set -e -u -o pipefail

realpath() {
  [[ $1 = /* ]] && echo "$1" || echo "$PWD/${1#./}"
}

SCRIPT_NAME="$(basename "$(realpath "$0")")"
SCRIPT_DIRECTORY="$(dirname "$(realpath "$0")")"

cd "${SCRIPT_DIRECTORY}/../"

echo "Install Release version..."
cmake --install "${SCRIPT_DIRECTORY}/../builds/xcode" --prefix Release --component ReactInstrument_VST3
