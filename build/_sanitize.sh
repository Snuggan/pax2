#!/usr/bin/env bash

cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DPAX_USE_SANITIZE=1 ../..
