#!/bin/sh
# Helper script to download a prebuilt piper binary for the current architecture.
# Uses the GitHub releases API to find the latest version.
# Usage: ./fetch_piper.sh

set -eu

ARCH=$(uname -m)
case "$ARCH" in
    x86_64)
        asset_name="piper_linux_x86_64.tar.gz" ;;
    aarch64|arm64)
        asset_name="piper_linux_aarch64.tar.gz" ;;
    armv7l)
        asset_name="piper_linux_armv7.tar.gz" ;;
    *)
        echo "unsupported architecture: $ARCH" >&2
        exit 1
        ;;
esac

# fetch latest release information from GitHub API
api_url="https://api.github.com/repos/rhasspy/piper/releases/latest"
echo "querying GitHub for latest piper release..."

asset_url=$(curl -s "$api_url" \
    | grep -oP '"browser_download_url":\s*"\K[^\"]+' \
    | grep "$asset_name" \
    | head -n1)

if [ -z "$asset_url" ]; then
    echo "could not find asset for $asset_name in latest release" >&2
    exit 1
fi

echo "fetching $asset_url..."

curl -L -o piper.tar.gz "$asset_url"

echo "extracting..."

tar xf piper.tar.gz
rm -f piper.tar.gz

# if extraction created a directory named "piper" containing the binary,
# move the binary to the workspace root and leave supporting files in the
# subdirectory (they may be required at runtime).  Otherwise, if the
# archive directly produced a `piper` executable, do nothing.
if [ -d ./piper ] && [ -x ./piper/piper ]; then
    mv ./piper/piper ./piper.bin
    chmod +x ./piper.bin
    echo "moved inner binary to ./piper.bin"
    # keep the rest of the directory in case libraries are needed; users
    # can add it to LD_LIBRARY_PATH if necessary
elif [ -x ./piper ]; then
    chmod +x ./piper
fi

echo
if [ -x ./piper ] || [ -x ./piper.bin ]; then
    echo "download complete; you can run \"./piper\" (or ./piper.bin)"
else
    echo "extraction failed, piper binary not found" >&2
    exit 1
fi

# optionally fetch a TTS model and its metadata
# default to the amy-low model from HuggingFace; override by setting
# MODEL_URL and MODEL_JSON_URL environment variables.
MODEL_URL=${MODEL_URL:-"https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/amy/low/en_US-amy-low.onnx?download=true"}
MODEL_JSON_URL=${MODEL_JSON_URL:-"https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/amy/low/en_US-amy-low.onnx.json?download=true"}

# derive filenames
model_file=$(basename "${MODEL_URL}" | sed 's/?.*//')
meta_file=$(basename "${MODEL_JSON_URL}" | sed 's/?.*//')

# download if not already present
if [ ! -f "$model_file" ]; then
    echo "fetching model $model_file..."
    curl -L -o "$model_file" "$MODEL_URL"
fi
if [ ! -f "$meta_file" ]; then
    echo "fetching metadata $meta_file..."
    curl -L -o "$meta_file" "$MODEL_JSON_URL"
fi

echo "model files are saved as $model_file and $meta_file";

