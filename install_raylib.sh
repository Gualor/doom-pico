#!/bin/bash

RAYLIB_DIR="raylib"
RAYLIB_VERSION="4.5.0"
BASE_URL="https://github.com/raysan5/raylib/releases/download/${RAYLIB_VERSION}"
RAYLIB_ARCHIVE=""

rm -rf "${RAYLIB_DIR}"
if [[ "${MSYSTEM}" == "MINGW"* ]]; then
	echo "Downloading Raylib Mingw release..."
	RAYLIB_ARCHIVE="raylib-${RAYLIB_VERSION}_win64_mingw-w64.zip"
	curl -LO "${BASE_URL}/${RAYLIB_ARCHIVE}"
	unzip "${RAYLIB_ARCHIVE}"
	mv "${RAYLIB_ARCHIVE%.zip}" "${RAYLIB_DIR}"
elif [ "$(uname -s)" == "Linux" ]; then
	echo "Downloading Raylib Linux release..."
	RAYLIB_ARCHIVE="raylib-${RAYLIB_VERSION}_linux_amd64.tar.gz"
	curl -LO "${BASE_URL}/${RAYLIB_ARCHIVE}"
	tar -xzf "${RAYLIB_ARCHIVE}"
	mv "${RAYLIB_ARCHIVE%.tar.gz}" "${RAYLIB_DIR}"
else
	echo "Platform currently not supported."
	exit 1
fi
rm -rf "${RAYLIB_ARCHIVE}"
