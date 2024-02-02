#!/usr/bin/env bash

echo "The relative directory is \"$(dirname "$0")\""

HERE=$(cd "$(dirname "$0")" && pwd)    # absolutized and normalized path to this script
echo "The absolute directory is \"$HERE\""
