# pdal module remove_overlap

Removes overlap created by multiple flights by only accepting points with the same source id as the pixel's source id. 
The pixel's source id is the source id of the point with the smallest scan angle within the pixel (so different pixels might have different source id). 

Execute `pdal --options filters.remove_overlap` for a list of available parameters and metrics.


## Parameters

### `--overlap_resolution`
The pixel size. The filter removes overlap by only accepting points with the same source id as the pixel's source id. 
The pixel's source id is the source id of the point with the smallest scan angle in the pixel (so different pixels might have different source id). 
You probably do not want use this filter on photogrammetry files. 


## Example

	to be written
