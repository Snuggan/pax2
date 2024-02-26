# pdal module remove_overlap

Removes overlap created by multiple flights by – per pixel – only accepting points with the same source id as the point in the pixel with the smallest scan angle. The idea is to retain only the points from the flight that is most straight above the pixel. 

Execute `pdal --options filters.remove_overlap` for a list of available parameters and metrics.

Using this filter on photogrammetry files does not make much sense... 


## Parameters

**`overlap_resolution`**  
The pixel size used.


## Example

	pdal translate input.laz output.laz \
		-f filters.remove_overlap \
		--filters.remove_overlap.overlap_resolution="25"
