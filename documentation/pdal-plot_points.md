# pdal module plot_points

Save all points within plots (plus an optional buffer) to individual files, one for each plot. 


## Parameters

### `--plot_file`
File path to a csv-type file with at least the required columns `north`, `east`, `radius`, and `id`. 
Plots not entirely within the point cloud bounding box are ignored. 

### `--dest_plot_points`
Destination *directory* path for the plot point cloud files. 
Files will be named by the `id` column in the `--plot_file`.
.
### `--dest_format`
File format to use for resulting point clud files (e.g '.laz'). 

### `--id_column`
In what column to find the [unique] plot id, to use as destination file name. 

### `--max_distance`
How much to enlarge the plot diameters. A zero value (the default) will use the plot diameter as specified by the `id` column in `--plot_file`.
