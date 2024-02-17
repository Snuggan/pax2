# Example: how to do everything at once


Below is an example on how to calculate plot and raster metrics as well as creating individual point cloud files for all points in each plot. 

Say we have the following files:

- `plots.csv` is the input plots file.
- `points.laz` is the input point cloud file.
- `raster-metrics.tif` is the output raster metrics file.
- `plot-metrics.csv` is the output plot metrics file.
- `plot-points-files` is the output plot points *directory*.

We define a variable:

	PDAL_METRICS_PIPELINE := {	"pipeline":[ \
		{ \
			"type":"filters.raster_metrics", \
			"resolution":"12.5", \
			"metrics":"extra-allt", \
			"nilsson_level":"1.5", \
			"gdalopts":"BIGTIFF=IF_SAFER,COMPRESS=DEFLATE", \
			"data_type":"float" \
		},{ \
			"type":"filters.plot_metrics", \
			"plot_file":"plots.csv", \
			"metrics":"extra-allt", \
			"nilsson_level":"1.5" \
		},{ \
			"type":"filters.plot_points", \
			"plot_file":"plots.csv", \
			"dest_plot_points":"plot-points-files", \
			"max_distance":"12", \
			"id_column":"Descriptiontxt", \
			"dest_format":".laz" \
		} ] }

Then we can calculate raster metrics for `points.laz` and for all plots in `plots.csv` we calculate metrics and save individual point cloud files using the command:

		pdal translate \
			--json=$PDAL_METRICS_PIPELINE \
			--input=points.laz \
			--output=null.bull \
			--writer=writers.null \
			--filters.raster_metrics.dest=raster-metrics.tif \
			--filters.plot_metrics.dest_plot_metrics=plots-metrics.csv \
			--metadata=.json


## See also

- [pdal translate´](https://pdal.io/en/2.6.3/apps/translate.html)
- [filters.raster_metrics´](pdal-raster_metrics.md)
- [filters.plot_metrics´](pdal-plot_metrics.md)
- [filters.plot_points´](pdal-plot_points.md)
