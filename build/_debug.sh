#!/usr/bin/env bash

cmake 											\
	-DCMAKE_BUILD_TYPE=Debug					\
	-DCMAKE_INSTALL_RPATH_USE_LINK_PATH="ON"	\
	-G Ninja									\
	../..
