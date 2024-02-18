# pdal module plot_metrics

Calculate metrics (statistics) for all points within each plot. 

Execute `pdal --options filters.plot_metrics` for a list of available parameters and metrics.


## Parameters

**`plot_file`**  
File path to a csv-type file with at least the required columns `north`, `east`, `radius`, and `id`. 
Plots not entirely within the point cloud bounding box are ignored. 

**`dest_plot_metrics`**  
Destination file path for the resulting plot metric file.

**`metrics`**  
What metrics to calculate (see [here](metrics-how-to-specify.md)).

**`nilsson_level`**  
For some metrics (see [here](metrics-how-to-specify.md)), ignore *z*-values below this value.


## Example

	pdal translate input.laz null.laz \
		-w writers.null \
		-f filters.plot_metrics \
		--filters.plot_metrics.plot_file="plots.csv" \
		--filters.plot_metrics.dest_plot_metrics="plots+metrics.csv" \
		--filters.plot_metrics.metrics="basic-linear" \
		--filters.plot_metrics.nilsson_level="1.85"


## See also

- [How to specify metrics.](metrics-how-to-specify.md)
- [`pax-concat-files`](pax-concat-files.md) concatenates csv-like textual table files into one.
