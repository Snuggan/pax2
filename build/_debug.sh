#!/usr/bin/env bash

cmake 														\
	-DCMAKE_BUILD_TYPE=Debug								\
	-DCMAKE_INSTALL_RPATH_USE_LINK_PATH="ON"				\
	-D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_FAST	\
	-G Ninja												\
	../..
