#!/usr/bin/env bash

HERE=$(cd "$(dirname "$0")" && pwd)    # absolutized and normalized path to this script

# There are three options to the -v flag:
# consistent: perfect consistency (host and container have an identical view of the mount at all times)
# cached:     the host's view is authoritative (permit delays before updates on the host appear in the container)
# delegated:  the container's view is authoritative (permit delays before updates on the container appear in the host)

docker run -it --rm -v $HERE/..:/pax:cached axensten/slu
