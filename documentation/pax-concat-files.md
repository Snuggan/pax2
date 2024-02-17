# pax-concat-tables

A tool to concatenate csv-like textual table files into one.

1. Ignore empty files.
2. Get Header from first non-empty file.
3. Check that headers in other non-empty files equal Header. 
	- If equal, append non-header rows. 
	- If not equal, terminates with an error message. 
4. When finished concatenating, sorts all rows except the header.
5. Save the result.


## Parameters

### `--source`
Source *directory* path. The files to concatenate.

### `--dest`
Destination *file* path. Path of the resulting file.

### `--meta`
Save metadata files with execution info.

### `--verbose`
Display execution progress.

### `--count`
Use progress report based on *count* items.


## See also

- [plot_metrics](pdal-plot_metrics.md) calculates metrics (statistics) for *z*-values of all points within each plot. 
