#!/usr/bin/env bash

### YOU MUST BE IN docker/slu ###
# TO UPDATE THE DOCKER HUB (https://hub.docker.com/repository/docker/axensten/slu)
# 1. Compile for Linux:
#		Run _develop.sh to start up docker-develop
#		Compile
#		exit
# 2. docker login -u axensten (password starts with qotr...)
# 3. Run _create.sh (this document) and input the tag name (default is todays date)
# 4. docker push axensten/slu --all-tags


# Stop, at first error.
set -e

# Set tag to use (default is todays date).
PAX_TAG=`date +"%Y.%m.%d"`
echo "What tag to use? (default is '${PAX_TAG}')"
read NEW_PAX_TAG
if [ "$NEW_PAX_TAG" != "" ]; then
	PAX_TAG=${NEW_PAX_TAG}
fi
echo "Using tag '${PAX_TAG}'"

# Ask if we should update dockerhub too.
UPDATE=""
while true; do
	read -p "Do you want to update dockerhub too? (y/n/q) " yn
	case $yn in 
		[yY] ) echo "Ok, we will update dockerhub."
			UPDATE="y"
			break;;
		[nN] ) echo "We will not update dockerhub."
			UPDATE="n"
			break;;
		[qQ] ) echo "Quitting..."
			exit;;
		* ) echo "Invalid response...";;
	esac
done


#SLU_DIR=.
SLU_DIR=$(cd "$(dirname "$0")" && pwd)/slu    # absolutized and normalized path to the slu dir in this script's dir
PAX_DIR="${SLU_DIR}/../.."
BIN_DIR="${PAX_DIR}/build/linux"
R_DIR="${PAX_DIR}/R"

mkdir -p ${BIN_DIR}
mkdir -p ${SLU_DIR}/usr/local/lib
mkdir -p ${SLU_DIR}/usr/local/bin
mkdir -p ${SLU_DIR}/usr/local/etc/makefiles

# Copy the pax* tools.
cp	-fpXv												\
	${BIN_DIR}/pax-concat-tables						\
	${BIN_DIR}/pax-metrics								\
	${SLU_DIR}/usr/local/bin
#	${BIN_DIR}/pax										\
#	${BIN_DIR}/pax-time									\

# Copy libpax* and libpdal_* stuff
echo "aa"
cp	-fpXv												\
	${BIN_DIR}/lib*										\
	${SLU_DIR}/usr/local/lib
echo "bb"

# Copy the pax-regression tool into the docker create sandbox.
mkdir -p ${SLU_DIR}/usr/local/bin
cp  -fpXv												\
	${R_DIR}/pax.regression/inst/tools/pax-regression	\
	${SLU_DIR}/usr/local/bin/pax-regression

# Create the pax.regression package and remove the version number from the file name.
R CMD build ${R_DIR}/pax.regression
mkdir -p ${SLU_DIR}/tmp-post
mv -f pax.regression*.tar.gz ${SLU_DIR}/tmp-post/pax.regression.tar.gz


# Build the docker image (add argument "--progress plain" for more info):
COMMAND="docker build --progress plain -t axensten/slu:${PAX_TAG} -t axensten/slu:latest -t axensten/slu:stable ${SLU_DIR}"
echo ${COMMAND}
${COMMAND}

# Possibly, update dockerhub.
if [ "$UPDATE" == "y" ]; then
	echo "Updating dockerhub!"
	docker push axensten/slu:${PAX_TAG}
	docker push axensten/slu:latest
	docker push axensten/slu:stable
	git tag "${PAX_TAG}" -m "Updated the docker document to ${PAX_TAG}."
	open --url https://hub.docker.com/repository/docker/axensten/slu
else
	echo "Not updating dockerhub..."
fi
