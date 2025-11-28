#!/usr/bin/env bash

cmake	-DCMAKE_BUILD_TYPE=RelWithDebInfo						\
		-D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_FAST	\
		../..
