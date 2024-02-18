#!/usr/bin/env bash


# Below is an example on how to calculate plot and raster metrics as 
# well as creating individual point cloud files for all points in each plot. 
# 
# Say we have the following files:

INPUT_PLOTS_FILE="the-input-plots-file.csv"
INPUT_POINTS_FILE="the-input-point-cloud-file.laz"
OUTPUT_RASTER_METRICS="the-output-raster-metrics/template.tif"
OUTPUT_PLOTS_METRICS="the-output-plot-metrics-file.csv"
OUTPUT_PLOTS_POINTS_DIR="the-output-plot-points-directory/"

PDAL_METRICS_PIPELINE='{ "pipeline":[
	{
		"type":"filters.raster_metrics",
		"resolution":"12.5",
		"metrics":"extra-allt",
		"nilsson_level":"1.5",
		"gdalopts":"BIGTIFF=IF_SAFER,COMPRESS=DEFLATE",
		"data_type":"float"
	},{
		"type":"filters.plot_metrics",
		"plot_file":"${INPUT_PLOTS_FILE}",
		"metrics":"extra-allt",
		"nilsson_level":"1.5"
	},{
		"type":"filters.plot_points",
		"plot_file":"${INPUT_PLOTS_FILE}",
		"dest_plot_points":"${OUTPUT_PLOTS_POINTS_DIR}",
		"max_distance":"12",
		"id_column":"Descriptiontxt",
		"dest_format":".laz"
	} ] }'

# Then we can calculate raster metrics for ${INPUT_POINTS_FILE} and for all 
# plots in ${INPUT_PLOTS_FILE} we calculate metrics and save individual point 
# cloud files using the command:

#echo "${PDAL_METRICS_PIPELINE}"

pdal translate \
	--json="${PDAL_METRICS_PIPELINE}" \
	--input="${INPUT_POINTS_FILE}" \
	--output=null.bull \
	--writer=writers.null \
	--filters.raster_metrics.dest="${OUTPUT_RASTER_METRICS}" \
	--filters.plot_metrics.dest_plot_metrics="${OUTPUT_PLOTS_METRICS}" \
	--metadata=.json
