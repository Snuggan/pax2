#!/usr/bin/env bash

cmake 									\
	-DCMAKE_BUILD_TYPE=RelWithDebInfo 	\
	-G Ninja							\
	../..
