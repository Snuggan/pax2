# Processing las/laz files with the pax tools

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

- ``make -j8 -k filter`` will filter the point cloud files in ``2-pc-source`` and also calculate raster metrics from them. Resulting files will be put into ``3-pc-filtered`` and ``4-raster-metrics`.
- ``make -j8 -k raster`` will produce raster metrics files from already point cloud files in ``3-pc-filtered``. Resulting files will be put into ``4-raster-metrics``.
- ``make -j8 -k plots``will calculate metrics for all plot files in ``2-plots-source``. It will first make sure that all point cloud files are filtered and then read all point cloud files in ``3-pc-filtered``, so if you files that also have copies, the copies will also be used. The result will be in ``4-plots-metric.csv``.
- ``make -j8 -k estimate``will do all the above and also calculate the forest variables.
- ``make -j8 -k`` will do all the above. 

Parameters:
- ``-j8`` processes the files in parallel, using 8 threads. Use a different number, if you so prefer. (Eventually the discs' speed will be the limiting factor, not the number of threads.) 
- ``-k`` will continue the processing even if there are errors. It is not rare with problematic las/laz files and ``-k`` will continue the processing. 

All the specific processing parameters are defined in the ``Makefile`` file. If you feel adventurous, you can open the file and change. If you want to see the details of a specific tool, run it with the ``--help`` parameter, e.g. ``pax-pc --help``.

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

### ruta_meta.csv

This file contains metadata on the areas covered by the point cloud files. It has a header line followed by data on one area per line. Required columns (header names) are:
- ``east`` is the eastern coordinate in the geographical reference system used in rasters and point cloud files.
- ``north`` is the northern coordinate in the geographical reference system used in rasters and point cloud files.
- ``id`` The basename of the files (no directory part, no suffix). 
- ``leaveson`` was the plot measured during the summer season, values are 1. If not, values are 0. 
- ``scanner_type`` what scanner type was used to scan the plot (Leica ALS80 is 0, Leica TerrainMapper is 1). 

## The really nerdy details

The [Docker](https://www.docker.com/products/docker-desktop) container is based on [Ubuntu](https://ubuntu.com), presently Ubuntu 20.10. The tools are written in [C++17](https://en.cppreference.com/w/cpp/17) and compiled with [GCC](https://gcc.gnu.org). A number of libraries are used: [Boost](http://www.boost.org), [GDAL](http://www.gdal.org), [\{fmt\}](https://github.com/fmtlib/fmt), [LASzip](http://www.laszip.org), [libLAS](http://www.liblas.org), [pdal](http://www.pdal.io), and [taoJSON](https://github.com/taocpp/json). The testing environment is [``doctest``](https://github.com/onqtam/doctest) and the building environment is [``cmake``](https://cmake.org). All are excellent open source software with generous licences. For more details than you want, run ``pax --meta``.

### Version history

- 2019.07 Can filter and calculate raster metrics. It has been reasonably tested.
- 2019.09 Added plot metrics. It has not yet been much tested. 
- 2019.10.01 Interim version.
- 2019.10.10 interim version.
- 2019.10.17 Safer make script. Somewhat more informative crash reports. 
- 2020-02-17 General code updates. Point cloud filtering now keeps points of class 0 (unclassified) -- sorry for any inconvenience. 
- 2020-02-21 General code updates. Point cloud filtering now ignores point return number 0, instead of throwing. 
- 2020-03-09 Regression now a part of the process. The cmd tool ``pax`` now reports a lot more metadata on both las/laz-files and text files. More capable and stable makefiles for running the process, where las-files are also welcome (not fuylly tested yet, though). General code improvements. Version is now 2.3, run 'pax' to see what version you are using.
- 2020-05-15 Regression seems to work, although metrics are calculated without any Nilsson filtering. Reasonable estimates that will become better in the next release, presumably. 
- 2020-05-19 Rebased the docker image on Ubuntu 20.04 LTS (was Ubuntu 19.10).
- 2020-06-08 Estimation now works -- you can run the complite process. The estimation result looks ok but is not yet fully evaluated. 
- 2020-06-08b Renamed the basic_linear modelling to photogrammetry and created a new basic_linear modelling for processing lidar point clouds.
- 2020-06-09 Generalized the makefile scripts so that they are more orthogonal. They are now more simple and more general, but somewhat slower in certain cases. 
- 2020-06-10 Oj! I forgot to include p80 in the metrics calculations, so the system chokes at estimating for lack of metric... 
- 2020-06-10b Oj! I wrote Diameter where it should have been Dgv... 
- 2020-06-11 Fixed an issue with empty filtered las files having no extent. 
- 2020-06-25 Fixed a few bugs. It seems that R CMD build works in a slightly new way after an update to R, which prevented the R-code of this system to be updated. 
- 2020-06-30 Suppressed warnings from the estimation part of the process.
- 2020-08-20 Updated models and general improvements to the base C++ code.
- 2020-09-03 Now saves plot_metrics.csv in a dir, for consistency. In the make scripåt, added a step that mosaics the variable rasters.
- 2020-09-04 The mosaic step now also creates overviews (pyramids).
- 2020-09-24 Added a number of metrics to the pax-pc and pax-plots tools (general counters and first return counters). The basic_linear modelling now uses first return proportion, instead of all returns. 
- 2020-11-03 Started the transition to pdal-based tools, the point cloud file filter is now implemented in the pdal environment. Various small fixes. 
- 2020-11-06 Now based on Ubuntu 20.10. This means that a more recent version of pdal is included.
- 2020-11-09 Library stuff now hopefully placed correctly. Updated makefiles.
- 2020-11-10 Raster metrics now aligned with the resolution. Filtering now done to temporary file and then moved, safe for interrupts. 
- 2020-12-08 Filtering now done by pdal (using home-grown filter). 
- 2020-12-14 Implemented work-arounds to avoid problems with pdal's handling of empty point files. All files now contains at least one [dummy] point  -- and point files with fewer than two points are ignored. I must say, pdal requires a lot of work-arounds to work in the production chain...
- 2020-12-18 Raster metrics now done by pdal (using home-grown filter). Creates mosaics (and pyramids) for raster metrics p95 and prop_first, in addition to the estimated variables. 
- 2021-01-12 Whoops! The pdal metrics filter happened to generate rasters upside-down... Should be fixed, all tests are go, nut I haven't had time to check it in docker. 
- 2021-01-25 The Proj4 library is in the process of changing the textual format for spacial references and this update is to keep compatibility with these changes. It fixes a crash in the estimating part of processing. 
- 2021-01-27 In estimation, potential plots must now have the same ``scanner_type`` as the ruta processed. 
- 2021-02-24 Only plots that are entirely inside a ruta get metrics, others will not be used. Changed metric name from "std_dev" to "sample_std_dev". Employs a timestamp to only update plots with points from updated las-files. A slightly bigger update than usual. 
- 2021-02-27 Bugfix, as the new plot metrics was flawed when updating some but not all plots. 
- 2021-02-28 Improved metadata reporting in plot metrics. 
- 2021-03-01 Plot metrics now ignores almost empty point cloud files as they may have strange bounding boxes and generate questionable metrics anyhow.
- 2021-03-08 I renamed a few metrics to conform to a more consistent naming system, but forgot a few places – sorry! You need to throw away all previous raster metrics, plot metrics, and estimations and rerun. Really sorry. 
- 2021-03-19 I reorganised and renamed metrics to be really generic and systematic. A lot of improvements under the hood that might not be visible to a user, but makes the code a lot more maintainable and flexible. Due to renaming, you need to throw all previous raster and plot metrics and rerun. Sorry! I'll try not to change the names again soon... You also need to update old Makefile:s, there is a new one in /usr/local/etc/makefiles. 
- 2021-03-24 Small improvements. 
- 2021-05-01 A few memory bugs have been fixed that could make las/laz-reading tools to crash in specific circumstances. Now ``pdal`` does all point cloud handling, so ``liblas`` is no longer required nor included. This means that both the the C++ code and the docker generation is significantly simplified and the package is easier to maintain and more future proof. It also means that all tools now can handle all point cloud formats that ``pdal`` can handle, with the exception of  the tool ``pax-plots``. ``pax-plots`` can not be implemented as a ``pdal.filter`` due to the kind of input files it requires, but it uses ``pdal`` internally to read las/laz files. I hope to generalise this in the future, so that all ``pdal`` point cloud formats can be handled also by ``pax-plots``.
- 2021-05-27 Bug-fix in R outlier detection.
- 2021-05-31 Changed from raster to terra for handling rasters in R.
- 2021-06-03 More flexibility in the outlier reduction phase via two variables defined at top of the makefile (don't forget to replace old makefiles!).
- 2021-06-14 Changed the outlier filtering to having two boundary lines expressed as hgv = m + k*p95 (from having a maximum distance abs(hgv - p95).
- 2021-10-05 Lots of internal changes that hopefully will not be noticed (code simplifications). Now produces a number of Volume variable rasters – these are temporary for evaluation purposes only. This whole version is for evaluation purposes only as we are trying out new models. 

Have fun!

Peder Axensten, Nyåker 2021
