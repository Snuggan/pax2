#!/usr/bin/env bash

#PROCESSING="/Users/pederaxensten/Desktop/processing"
PROCESSING="/Volumes/pax-testdata/lidar-big/"

# There are three options to the -v flag:
# consistent: perfect consistency (host and container have an identical view of the mount at all times)
# cached:     the host's view is authoritative (permit delays before updates on the host appear in the container)
# delegated:  the container's view is authoritative (permit delays before updates on the container appear in the host)
docker run -it --rm -v $PROCESSING:/process:delegated axensten/slu bash
