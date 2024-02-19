# Pax2 Docker Image




At the Forest Remote Sensing section of the Swedish University of Agricultural Sciences (SLU) we have developed a set of tools that we use to produce wall-to-wall rasters of Sweden for a set of forest variables. The tools are command line tools as well as filter modules for the command line tool [PDAL](https://pdal.io/). The tools are packaged in this Docker image in a ready-to-run environment that also contains `pdal`, `gdal`, *etc.* 

- For more information on the `pax2` tools and how you can use them, see the project’s [githup page](https://github.com/Snuggan/pax2?tab=readme-ov-file). More documentation, the code, and a more detailed version history is available here. 
- For more information on how to use the `pax2` tools to process files *en-masse*, see [here](https://github.com/Snuggan/pax2/blob/main/documentation/example-en-mass.md).

I hope you find the image useful.

Peder Axensten, Nyåker 2024


## Recent version history

- 2024-02-09 Small fixes. It all seems to work now. 
- 2024-02-06 A reorganisation of the code. Needs a bit of internal testing before general use. Taged as "stable" by misstake -- sorry!
- 2023-03-02 Now handles ``Biomass_above`` as well as ``Biomass_below``. Still a test version using species information as stated in the ``2023-02-24``, above. 
- 2023-10-23 A bit of a rewrite, now entirely based on pdal in terms of point cloud handling. There are three main pdal modules: for raster metrics, for plot metrics, and for exporting points of plots. This version should be more flexible and faster. Upped the internal version number to 3.0.0. 
- 2023-07-25 Significant update. All point cloud related processing now use PDAL. Supplied in this Docker image are PDAL plug-ins for producing plot metrics, raster metrics, individual point clouds per each plot, to remove overlap due to different flight, and a special filter that we use at SLU to process point clouds from Lantmäteriet. The plot metrics calculation has also been opimized, so it should be considerable quicker. I plan to rewrite this page and focus it on the PDAL plug-ins, the make script to process large amounts of data is really secondary. I also plan to make tho code public on git-hub, but I need to remove unnecessary library dependencies first.
- 2023-05-11 A new generalised system to define and calculate metrics. Now calculates bot ``Biomas_above`` and ``Biomass_below``. 
- 2023-02-24 NOTE: experimental! Just to test it we are using species information in all models. It should not make estimations worse, but the question is if it improves estimations? Maybe for some variables. 
- 2023-01-25 Added a makefile variant ``'extra-allt'`` that does what 'basic-linear' does, but also output a lot of extra metrics. If you only want to estimate variables as quickly as possible, use ``'basic-linear'``. If you want a lot of metrics to experiment with, use  ``'extra-allt'``. 
- 2022-05-03-ubuntu.21-10 The most updated docker, based on Ubuntu 21.10. Next will be 22.04. 
- 2022-05-08 Now using Ubuntu 22.04 and newer libraries are used. Other small fixes. 
- 2022-03-28 The previous release (2022-03-22) did not include pdal plug-ins. Fixed. 
- 2022-03-22 Rewrote code to enable more flexibility in combining metrics with filters. Improvements in the ``pax --stat`` tool (see ``pax --help``).
