# How to estimate forest variables in an automated manner  


*Processing point cloud files en-masse with the pax2 tools.*



## The short story

The pax tools are a number of command line programs that are intended to work together to process point cloud files. Each tool is intended for a specific task and processes one file. Specifically, ``pax-pc`` filters a las/laz file and calculate its raster metrics and ``pax-plots`` calculate metrics for plots. To process many files in an efficient manner, the Unix tool ``make`` is used. You can run all these tools on any platform from the Docker image [``axensten/slu``](https://cloud.docker.com/repository/docker/axensten/slu). 

If you have problematic raster-, las/laz-, csv/text-files, (or if you just want detailed metadata info on such files), run ``pax <filename>``. 


## The detailed story

### 1. Install Docker and tools

You find Docker here: [Get Docker.](https://docs.docker.com/install/)
(If you are interested, you may have a look at [Get started.](https://docs.docker.com/get-started/))


### 2. Start a Docker session

Run ``docker run -it --rm -v your-las-file-directory:/process axensten/slu`` to start a session.

- ``-it`` makes you run interactively. 
- ``--rm`` removes the Docker volume once you are done with it. It is better to create a new clean one at each occasion, as this is very quick. Removing the Docker volume does not remove the Docker image axensten/slu, nor your las/laz files as they are in your file system.
- ``-v your-las-file-directory:/process`` mounts your las file directory in the Docker volume file hierarchy. You change ``your-las-file-directory`` to the path where in your file system you want to process your las/laz files, of course. 
- ``axensten/slu`` is the Docker image name. The first time you run the command, it will be downloaded from the Docker Hub. Thereafter it is on your system until you remove or update it. It is about 1.5 Gbytes in size. 

The first time you run it on a file directory, you are asked if your files are from Lidar or photogrammetry, as different filtering algorithms and metrics are used. The appropriate ``Makefile`` is copied. A number of empty directories are also created -- don't change their names:

- ``0-project`` will need to contain a ground model file ``dem.tif`` and  ``species_type.tif`` (that cover the area of the point cloud files to be filtered) and ``plots.csv`` and ``ruta_meta.csv``, that are described under “Format of csv-files”, below. A file called ``4-plots-metric.csv`` will be produced in the base directory, containing the same data as ``plots.csv`` but with the addition of required metrics. 
- ``1-rar-source``: put rar-compressed las/laz files here and they will be decompressed. 
- ``2-pc-source``: put your uncompressed las/laz files here. (Any rar-compressed files will be decompressed into this directory.)

After processing, resulting files will be found in:

- ``3-pc-filtered``: point cloud files end up here after they have been filtered.
- ``4-plot-metrics``: home of the plot_metrics.csv file.
- ``4-raster-metrics``: raster metric files end up here.
- ``5-estimates``: raster value files end up here. 
- ``6-mosaic``: mosaics of the raster value files in ``5-estimates``. 


### 3. Move the files to the designated directories

- Copy/move ``dem.tif``, ``plots.csv``, and ``ruta_meta.csv`` to the ``0-project`` directory.
- Copy/move your las/laz files into ``2-pc-source``.

If at a later stage you get more files to process, just add them in the appropriate directories. Only files that have changed will be processed when you rerun ``make``.  

### 4. Process

To process files efficiently the tool [``make``](https://en.wikipedia.org/wiki/Make_(software)) is used. It maintains a dependency tree of the files to be processed and makes sure that missing files are created and old files are updated. An old file is a file where any of the files that it depends on is newer. Put simply: it only does what is necessary – if there are no new files and all existing files are up to date, it will do nothing. 

- ``make -j8 -k filter`` will filter the point cloud files in ``2-pc-source``. Resulting files are put into ``3-pc-filtered``.
- ``make -j8 -k raster`` will do ``filter`` above and then produce raster metrics files from the (normalized)  point cloud files in ``3-pc-filtered``. Resulting files are put into ``4-raster-metrics``.
- ``make -j8 -k plots`` will do ``filter`` above and then update the metrics for the plots in ``4-plots-metrics/plots-metrics.csv`` (if this file is missing, ``0-project/plots.csv`` is copied and used).  It will read all point cloud files in ``3-pc-filtered``,  so if you have files there that overlap, these files will all contribute to the metrics. 
- ``make -j8 -k estimate`` will do all the above and also calculate the forest variables.
- ``make -j8 -k mosaic`` will do all the above and also mosaic the variable rasters and a few of the metric rasters.
- ``make -j8 -k`` will do all the above. 

All the specific processing parameters are defined in the ``Makefile`` file. If you feel adventurous, you can open the file and change their values, they are defined and documented in the beginning of the file. If you want to see the details of a specific tool, run it with the ``--help`` parameter, e.g. ``pax-pc --help``.

If there are errors, run ``make -k`` and you will get a clean list of the problem files with a (possibly cryptic) description of the problem.

To end the Docker volume session, execute ``exit``.


## Format of csv-files

These files are text files with the first line containing the headers and then one data item on each line. Line delimiters might be Windows or Linux/MacOS. Column delimiters must be '``;``'. All lines must have the same number of columns (column delimiters). What columns they are required to contain is defined below. Header names are case sensitive. The order of columns is not important and the file might have other columns, as long as they follow the formatting. Do not use “strange” characters, such as `*#\åäö`` etc. 

These files should be located in the folder ``0-project``.

### plots.csv

This file contains field measurements, ground data. It has a header line followed by data from one plot per line. Required columns (header names) are:
- ``east`` is the eastern coordinate in the geographical reference system used in rasters and point cloud files.
- ``north`` is the northern coordinate in the geographical reference system used in rasters and point cloud files.
- ``radius`` in the unit used by geographical reference system used in rasters and point cloud files. Most probably meters. 
- ``leaveson`` was the plot measured during the summer season, values are 1. If not, values are 0. 
- ``scanner_type`` what scanner type was used to scan the plot (Leica ALS80 is 0, Leica TerrainMapper is 1). 
- ``variables``* Measured values for the variables that are to be estimated. Typically they are: . ``Hgv``, ``Volume``, ``Biomass``, ``Basal_area``, and ``Dgv``. 

The included ``pax.regression`` R library includes a function ``check_required_plot_cols("path/to/plots.csv")``, that can be used to check if a file contains the required columns. 

### ruta_meta.csv

This file contains metadata on the areas covered by the point cloud files. It has a header line followed by data on one area per line. Required columns (header names) are:
- ``east`` is the eastern coordinate in the geographical reference system used in rasters and point cloud files.
- ``north`` is the northern coordinate in the geographical reference system used in rasters and point cloud files.
- ``id`` The basename of the files (no directory part, no suffix). 
- ``leaveson`` was the plot measured during the summer season, values are 1. If not, values are 0. 
- ``scanner_type`` what scanner type was used to scan the plot (Leica ALS80 is 0, Leica TerrainMapper is 1). 

The included ``pax.regression`` R library includes a function ``check_required_ruta_metadata_cols("path/to/metadata.csv")``, that can be used to check if a file contains the pre-calculated columns. 

## The really nerdy details

The [Docker](https://www.docker.com/products/docker-desktop) container is based on [Ubuntu](https://ubuntu.com), presently Ubuntu 20.10. The tools are written in [C++17](https://en.cppreference.com/w/cpp/17) and compiled with [GCC](https://gcc.gnu.org). A number of libraries are used: [Boost](http://www.boost.org), [GDAL](http://www.gdal.org), [\{fmt\}](https://github.com/fmtlib/fmt), [LASzip](http://www.laszip.org), [libLAS](http://www.liblas.org), [pdal](http://www.pdal.io), and [taoJSON](https://github.com/taocpp/json). The testing environment is [``doctest``](https://github.com/onqtam/doctest) and the building environment is [``cmake``](https://cmake.org). All are excellent open source software with generous licences. For more details than you want, run ``pax --meta``.
