# pdal module plot_metrics

Calculate metrics (statistics) for all points within each plot. 


## Parameters

### `--plot_file`
File path to a csv-type file with at least the required columns `north`, `east`, `radius`, and `id`. 
Plots not entirely within the point cloud bounding box are ignored. 

### `--dest_plot_metrics`
Destination file path for the resulting plot metric file.

### `--metrics`
What metrics to calculate (see [here](metrics.md)).

### `--nilsson_level`
For some metrics (see [here](metrics.md)), ignore *z*-values below this value.


## See also

- [`pax-concat-files`](pax-concat-files.md) concatenates csv-like textual table files into one.
