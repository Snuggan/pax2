#!/usr/bin/env bash

#BASE=/Users/peder/Desktop/Linus-setup-test

SOURCE="${BASE}/usr/local/"

PROCESS="${BASE}/process"

PAX="${BASE}/pax"

if [[ -d "${PROCESS}" ]]; then
	# We are in processing mode.
	cd ${PROCESS}
	
	# If there is no Makefile, add one and run it to create the processing directories.
	if ! [[ -f "${PROCESS}/Makefile" ]]; then

		# Copy a default makefile into the "process" directory as there is none already.
		cp -fp "${SOURCE}/etc/makefiles/Makefile" "${PROCESS}/Makefile"

		# finished=0
		# while [[ ${finished} -eq 0 ]]; do
		# 	# Ask for lidar or photogrammetry and copy the appropriate Makefile
		# 	echo ""
		# 	read -p "What kind of las files: lidar (l) or photogrammetric (p) or ignore ()? "  filetype
		# 	echo ""
		# 	if [[ "${filetype}" == "l" ]]; then
		# 		finished=2
		# 		echo "Setting up for processing Lidar las files."
		# 		cp -fp "${SOURCE}/etc/makefiles/Makefile-lidar" "${PROCESS}/Makefile"
		# 	elif [[ "${filetype}" == "p" ]]; then
		# 		finished=2
		# 		echo "Setting up for processing photogrametric las files."
		# 		cp -fp "${SOURCE}/etc/makefiles/Makefile-photogrammetry" "${PROCESS}/Makefile"
		# 	elif [[ $filetype == "" ]]; then
		# 		finished=1
		# 	else
		# 		echo "Unrecognized option: ${filetype}"
		# 	fi
		# done
		# if [[ ${finished} -eq 2 ]]; then
		# 	# Run make silently, also supressing errors (there are none).
		# 	make >/dev/null 2>/dev/null
		# 	echo ""
		# 	echo "To process your las files:"
		# 	echo "  1. put them in 2-pc-source/"
		# 	echo "  2. run 'make -j8 -k'"
		# 	echo "     run 'make -j8 -k filter' if you only want to filter the las files"
		# 	echo "     run 'make -j8 -k metrics' if you only want to filter the las files and calculate metric rasters"
		# 	echo "     (-j8 will process in 8 threads and -k will continue to process in case of an error)."
		# fi
	fi

	echo ""

elif [[ -d "${PAX}" ]]; then
	# We are in developing mode.
	cd ${PAX}/build/Linux

	# Setup cmake for a release version.
	"${PAX}/build/_release.sh"

else
	echo "Neither ${PROCESS} nor ${PAX} were attached."
fi
