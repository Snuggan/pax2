# pax2

Pdal filters and command line tools to calculate *z*-metrics from point clouds. 

There is a docker image available containing all the stuff of `pax2` as well as the environment needed to run it (pdal, gdal, *etc.*) at [`dockerhub´](https://hub.docker.com/repository/docker/axensten/slu).


## pdal filters

- [plot_metrics](documentation/pdal-plot_metrics.md) calculates metrics (statistics) for *z*-values of all points within each plot. 
- [plot_points](documentation/pdal-plot_points.md) saves all points within plots (plus an optional buffer) to individual files, one for each plot. 
- [raster_metrics](documentation/pdal-raster_metrics.md) creates rasters with specified metrics (statistics). calculated for each pixel.
- [remove_overlap](documentation/pdal-remove_overlap.md) removes overlap created by multiple flights by only accepting points with the same source id as the pixel's source id. 
- [slu_lm](documentation/pdal-slu_lm.md) filters points according to *z*-values and point type.


## Command line tools

- [`pax-concat-files`](documentation/pax-concat-files.md) concatenates csv-like textual table files into one.
- [`pax-metrics`](documentation/pax-metrics.md) lists specified metrics. Given a set of metric and metric set ids, it returns a sorted list of metrics.


## Examples

- [How to do everything at once:](documentation/example-1.md) How to calculate plot and raster metrics as well as creating individual point cloud files for all points in each plot. 


## Reference

- [How to specify metrics.](documentation/metrics-how-to-specify.md)