# Estimate forest variables in an automated manner  


*Processing point cloud files en-masse with the pax2 tools.*



## Overview

At the Forest Remote Sensing section of the Swedish University of Agricultural Sciences (SLU) we have developed a [set of tools](../readme.md) that we use to produce wall-to-wall rasters of Sweden for a set of forest variables. The tools are command line tools as well as filter modules for the command line tool [PDAL](https://pdal.io/). The tools are packaged in a Docker image [`axensten/slu`](https://cloud.docker.com/repository/docker/axensten/slu) in a ready-to-run environment that also contains `pdal`, `gdal`, *etc.* The example presented here is specific for Sweden nad the input files that we have, but could be adapted for other use cases.

Each tool is intended for a specific task and processes one file as specified [here](../readme.md). To run everything in an automated manner there is a [`make` script](../docker/slu/usr/local/etc/makefiles/Makefile), it updates the file structure and estimates the forest variables. All input files need to be organised in a specific file structure. 

I hope you find the image useful.

Peder Axensten, Nyåker 2024


## Processing *en-masse*

### 1. Install Docker and tools

You find Docker here: [Get Docker.](https://docs.docker.com/install/)
(If you are interested, you may have a look at [Get started.](https://docs.docker.com/get-started/)). 


### 2. Create your file structure

In an otherwise empty directory, create the following directories and put the input files in them:

	0-project/
		dem.tif
		plots.csv
		ruta-meta.csv
		species_type.tif
	2-pc-source/
		<non-normalized point-cloud files here>
	3-pc-filtered/
		<normalized point-cloud files here>

- The `dem.tif` is the ground base model to subtract from non-normalised points to obtain normalised *z*-values. 
- The `plots.csv` contains data on the plots (`east`, `north`, and `radius`) as well as field measurements needed for the estimations.
- The `ruta-meta.csv` contains other data specific for the Swedish laser scanning. 
- The `species_type.tif` is specific for the estimations we use for Sweden. 


### 3. Start a Docker session

Run `docker run -it --rm -v your-las-file-directory:/process axensten/slu` to start a session.

The first time you run it on a file directory, a `Makefile` is copied to it. 
After processing, resulting files will be found in:

- `3-pc-filtered`: point cloud files end up here after they have been filtered and noramised.
- `4-plot-metrics`: home of the plot_metrics.csv file.
- `4-raster-metrics`: raster metric files end up here.
- `5-estimates`: raster value files end up here. 
- `6-mosaic`: mosaics of the raster value files in `5-estimates`. 


### 4. Process

To process files efficiently the tool [`make`](https://en.wikipedia.org/wiki/Make_(software)) is used. It maintains a dependency tree of the files to be processed and makes sure that missing files are produced and that old files are updated (an old file is a file that needs to be updated: where any of the files that it is produced from is newer). The `make`script only does what is necessary, if everything is up-to-date it does nothing. 

To start the process, `cd` to your processing directory and issue:

	make -j12 -k

and you are processing. The flag `-j12` means that you will use 12 threads for processing and `-k` means that it will not stop if there are errors, that it will process all files that it can. If you want to process up to a certain level, you can:

	make -j12 -k mertrics

This will stop processing once all metrics are calculated – it will not do any estimation.

All the specific processing parameters are defined in the `Makefile` file. If you feel adventurous, you can open the file and change their values, they are defined and documented in the beginning of the file.

If there are errors, execute `make -k` and you will get a clean list of the problem files with a description of the problem.

To end the Docker volume session, execute `exit`.


## Format of csv-files

These files are text files with the first line containing the headers and then one data item on each line. Line delimiters might be either Windows (`\r\n`) or Linux/MacOS (`\n`). Column delimiters must be '`;`'. All lines must have the same number of columns. Required columns are defined below. Header names are case sensitive. The order of columns is not important and the file might have other columns. Do not use “strange” characters, such as `*#\åäö` etc. 

These files should be located in the folder `0-project`.

### `plots.csv`

This file contains plot-related data such as coordinates, radius, field measurements, *etc.* It has a header line followed by data from one plot per line. The regular tools, the `pdal` filter modules, require these columns:

- `east` is the eastern coordinate in the geographical reference system used in rasters and point cloud files.
- `north` is the northern coordinate in the geographical reference system used in rasters and point cloud files.
- `radius` in the unit used by geographical reference system used in rasters and point cloud files. Most probably meters. 

Then there are also a number of required columns for the estimation we employ for Sweden:

- `leaveson` was the plot measured during the summer season, values are 1. If not, values are 0. 
- `scanner_type` what scanner type was used to scan the plot (Leica ALS80 is 0, Leica TerrainMapper is 1). 
- `variables`* Measured values for the variables that are to be estimated. Typically they are: . `Hgv`, `Volume`, `Biomass`, `Basal_area`, and `Dgv`. 


### `ruta_meta.csv`

This file contains metadata on the areas covered by the point cloud files. It has a header line followed by data on one area per line. Required columns (header names) are:
- `east` is the eastern coordinate in the geographical reference system used in rasters and point cloud files.
- `north` is the northern coordinate in the geographical reference system used in rasters and point cloud files.
- `id` The basename of the files (no directory part, no suffix). 
- `leaveson` was the plot measured during the summer season, values are 1. If not, values are 0. 
- `scanner_type` what scanner type was used to scan the plot (Leica ALS80 is 0, Leica TerrainMapper is 1). 


## Nerdy details

The [Docker](https://www.docker.com/products/docker-desktop) container is based on [Ubuntu](https://ubuntu.com). 
The tools are written in [C++23](https://en.cppreference.com/w/cpp/23) and compiled with [GCC](https://gcc.gnu.org). 
A number of libraries are used, mainly: [PDAL](https://pdal.io/), [GDAL](http://www.gdal.org), and [\{fmt\}](https://github.com/fmtlib/fmt). 
The testing environment is [`doctest`](https://github.com/onqtam/doctest) and the building environment is [`cmake`](https://cmake.org) – all excellent open source software with generous licences. 
