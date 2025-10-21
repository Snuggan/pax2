FROM	ubuntu:25.10
LABEL	url="https://hub.docker.com/repository/docker/axensten/slu" \
		organiusation="SLU, Swedish University of Agricultural Sciences" \
		maintainer="Peder Axensten <peder.axensten@slu.se>"

ENV		TZ=Europe/Stockholm

RUN		echo "\n\n--- Main stuff ---" \
		&&	ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone \
		&&	apt-get update \
		&&	apt-get install -y --no-install-recommends \
				git \
				curl \
			# Compilation and related tools:
				xz-utils \
				build-essential \
				gdb \
				gfortran \
				cmake \
				ninja-build \
				doctest-dev \
				libboost-all-dev \
			# Special libraries, necessary for the pax tools and plugins:
				libgdal-dev gdal-bin \
				### pdal libpdal-dev libpdal-plugins \
			# R core of statistical computation and graphics system + needed packages:
				r-base-core \
				r-cran-codetools \
				r-cran-robustbase \
				r-cran-mass \
				r-cran-getopt \
				r-cran-raster \
				r-cran-sf \
				r-cran-terra \
		# # Make g++-15 the default:
		# 		gcc-15 \
		# 		g++-15 \
		# &&	update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-15 50 \
		# &&	update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-15 50 \
		# Remove the apt-get cache, as it is needed no longer:
		&&	rm -Rf /var/lib/apt/lists/* \
		&&	ldconfig 


RUN		echo "\n\n--- PDAL ---\n\n" \
		&&	git clone https://github.com/PDAL/PDAL.git \
		&&	cd PDAL \
		# The latest version (2.9.n) has a bug and don't compile with gcc++-15...
		&&	git checkout 2.8.4 \
		&&	mkdir build \
		&&	cd build \
		&&	cmake -G Ninja .. \
		&&	ninja install \
		&&	ln -s /usr/lib/x86_64-linux-gnu/libgdal.so /usr/local/lib/libgdal.so \
		&&	hash pdal \
		&&	cd ../.. \
		&&	rm -rf PDAL


# Copy stuff related to running the pax tools.
COPY	usr/ /usr/
COPY	tmp-post/ /tmp/
RUN		echo "\n\n---- The pax stuff ---" \
		# Install the pax.regression package.
		&&	R -q -e 'install.packages( pkgs="tmp/pax.regression.tar.gz", repos=NULL )' \
		&&	rm -rf /tmp/pax.regression.tar.gz \
		&&	echo "Done install.packages( pax.regression )" \
		&&	R -q -e 'sessionInfo()' \
		&&	R -q -e 'library()$results[ , 1 ]' \
		# Run pax-setup.sh when you start the docker volume. The '.' is needed to automatically change the directory.
		&&	echo ". paxsetup.sh" >> /root/.bashrc \
		&&	echo "\n-------------------------------------------------------------\nDone!\n"

# Start from a Bash prompt, after running pax-setup.sh. 
CMD [ "/bin/bash" ]
