# pdal module raster_metrics

Create rasters with specified metrics (statistics). calculated for each pixel. 

Execute `pdal --options filters.raster_metrics` for a list of available parameters and metrics.


## Parameters

**`dest`**  
Destination directory and file name template for the metric raster files. 
(An empty file with the file name template and the name of the metric as the suffix is created with the metric files. 
It can be used by 'make' and similar tools as a target file.) 

**`resolution`**  
Size of pixels in the metric rasters. 

**`metrics`**  
What metrics to calculate (see [here](metrics-how-to-specify.md)).

**`nilsson_level`**  
For some metrics (see [here](metrics-how-to-specify.md)), ignore *z*-values below this value.

**`alignment`**  
Raster alignment, raster corner will be aligned. 

**`gdaldriver`**  
GDAL writer driver name.

**`gdalopts`**  
GDAL driver options (name=value,name=value...).

**`data_type`**  
Data type for output raster (\"int8\", \"uint64\", \"float\", etc.).

**`nodata`**  
No data value, a sentinal value to say that no value was set for nodata.


## Example

	pdal translate input.laz null.laz \
		-w writers.null \
		-f filters.raster_metrics \
		--filters.raster_metrics.dest="raster-metrics/result.tif" \
		--filters.raster_metrics.resolution="20" \
		--filters.raster_metrics.metrics="basic-linear" \
		--filters.raster_metrics.nilsson_level="1.85" \
		--filters.raster_metrics.gdalopts="BIGTIFF=IF_SAFER,COMPRESS=DEFLATE" \
		--filters.raster_metrics.data_type="float"


## See also

- [How to specify metrics.](metrics-how-to-specify.md)
