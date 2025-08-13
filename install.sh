#!/usr/bin/env bash
set -e

REPO="your-github-username/pebble-vm"
VERSION=${1:-"latest"}

if [ "$VERSION" = "latest" ]; then
    VERSION=$(curl -s https://api.github.com/repos/$REPO/releases/latest | grep '"tag_name":' | cut -d'"' -f4)
fi

OS=$(uname -s)
ARCH=$(uname -m)

case "$OS" in
    Linux) FILE="pebble-ubuntu-latest" ;;
    Darwin) FILE="pebble-macos-latest" ;;
    MINGW*|MSYS*|CYGWIN*) echo "Please download the Windows installer from GitHub Releases"; exit 0 ;;
    *) echo "Unsupported OS: $OS"; exit 1 ;;
esac

URL="https://github.com/$REPO/releases/download/$VERSION/$FILE"

echo "Downloading Pebble $VERSION for $OS..."
curl -L "$URL" -o pebble

chmod +x pebble
sudo mv pebble /usr/local/bin/

echo "Pebble installed! Run: pebble examples/hello.peb"
