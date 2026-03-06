#!/bin/sh
# Helper script to download cJSON library

echo "Creating cJSON directory..."
mkdir -p cJSON

echo "Downloading cJSON source..."
curl -L -o cJSON/cJSON.h https://raw.githubusercontent.com/DaveGamble/cJSON/master/cJSON.h
curl -L -o cJSON/cJSON.c https://raw.githubusercontent.com/DaveGamble/cJSON/master/cJSON.c

echo "Done. cJSON located in ./cJSON/"