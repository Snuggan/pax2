# pdal module slu_lm

Filtering according to *z*-values and point type, see the options below for details. 
If there is a dimension `HeightAboveGround`, its values are copied into dimension `Z`. 
Execute `pdal --options filters.slu_lm` for a list of available parameters. ,


## Parameters

### `--min_z`
Remove points with a *z*-value smaller than the argument `--min_z`.
Also,sets all remaining negative z-values to zero. You probably don't want to do use this option unless the points are normalized. If '--max_z' has a value smaller than '--min_z', no *z*-filtering will occur.,

### `--max_z`
Remove points with a *z*-value larger than than the argument `--max_z`.
You probably don't want to do use this option unless the points are normalized. 
If 'max_z' has a value smaller than 'min_z', no z-filtering will occur.,

### `--lm_filter`
If false, all point classes will pass.  
If true, only these point classes will pass: 
never classified (0), unspecified (1), and point on ground (2). 


## Comments
This is a rather specific filter that we use at slu to filter point cloud files from the Swedish Land Survey. 