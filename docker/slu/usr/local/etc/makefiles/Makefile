#############################################################################################################
#	Suggested use: make -k -j12
#
#	-k			--	Continue as much as possible after an error. While the target that failed, and those 
#					that depend on it, cannot be remade, the other prerequisites of these targets can be 
#					processed all the same.
#	-i			--	Ignore all errors in recipes executed to remake files.
#	-j [jobs]	--	Specifies the number of recipes (jobs) to run simultaneously. 
#					With no argument, make runs as many recipes simultaneously as possible.
#
#
#	To backup files, for example:
#	rsync -a --info=progress2 --exclude=1-rar-source* /Volumes/Pegasus/sverige/ /Volumes/LaCie/sverige
#
#############################################################################################################


# This specifies what metrics to calculate.
# Lots and lots of extra metrics:
metrics_set								:= extra-allt

# Necessary metrics only:
#metrics_set							:= basic-linear


# This specifies the nilsson height filter value.
nilsson_level							:= 1.5

# Resolution of raster metrics.
metric_resolution						:= 12.5

# Resolution of raster metrics.
plots_individual_points_dir_max_dist	:= 12

# In the regression part, how many plot candidates should be given to the outlier reduction.
number_of_candidates					:= 350

# In the regression part, the percentage of candidate plots to remove in the outlier reduction.
outlier_removal_percentage				:= 7.5

# Resolution of overlap filter.
# The overlap filter should be on for Lidar generated files.
FILTER_OVERLAP_RESOLUTION				:= $(metric_resolution)

# OVERLAP OR NO OVERLAP FILTER?
# Comment the next line IF YOU WANT the overlap filter
# The overlap filter should be off for photogrammetric files.
#FILTER_OVERLAP_RESOLUTION				:= 0


#############################################################################################################
#
#	This makefile is for use in point cloud processing.
#	You probably want to run 'make -k -jN'
#	-k:		Continue to process files, even if one file failed.
#	-jN	Process N files simutaneously using N threads (N is typically 4–7).
#
#	Put it in a directory, where all the point cloud source files are place in a directory called 
#	"pc-source". The files may be deeper into this directory, they need not be directly within "pc-source".
#
#	The resulting (filtered) point cloud files are put in a directory called "pc_dest" with the same 
#	directory structure in it, as in "pc-source". But onlu the point cloud files will be copied.
#
#	To run this file, cd into the base directory where the makefile is located and run the commando
#	"make". If you want multithreading, press "make -jn" where n is the number of threads, e.g. "make -j4".
#
#	Temporary files will be stored in a directory "pc_dest/_temp". You can delete this directory at any time
#	**when you have no active processes run by this makefile**.
#
#	Some options may be used when issuing the make command.
#	- source=... will set the directory where source files are.
#	  (default is "pc-source")
#	- dest=... will set the directory where filtered point cloud files will be stored.
#	  (default is "pc_dest")
#	- dem=... will set the ground model raster.
#	  (default is "pc-aux/ground-model-raster.tif")
#	- mask=... will set the mask raster.
#	  (default is "")
#	- exclude=... will set the flights exclusion list file.
#	  (default is "")
#	- FILTER_OVERLAP_RESOLUTION=... will give a overlap pixel size. 
#	  (default is 12.5)
#	- min_z=... will remove points with lower z-vaues.
#	  (default is -2)
#	- max_z=... will remove points with larger z-vaues.
#	  (default is 50)
#
#############################################################################################################
#
#	Copyright (c) 2014, Peder Axensten
#	All rights reserved.
#
#	Redistribution and use in source and binary forms, with or without
#	modification, are permitted provided that the following conditions are met:
#	    * Redistributions of source code must retain the above copyright
#	      notice, this list of conditions and the following disclaimer.
#	    * Redistributions in binary form must reproduce the above copyright
#	      notice, this list of conditions and the following disclaimer in the
#	      documentation and/or other materials provided with the distribution.
#	    * Neither the name of the the organization nor the
#	      names of its contributors may be used to endorse or promote products
#	      derived from this software without specific prior written permission.
#
#	This software is provided by the copyright holders and contributors "as is" and
#	any express or implied warranties, including, but not limited to, the implied
#	warranties of merchantability and fitness for a particular purpose are
#	disclaimed. In no event shall Peder Axensten be liable for any
#	direct, indirect, incidental, special, exemplary, or consequential damages
#	(including, but not limited to, procurement of substitute goods or services;
#	loss of use, data, or profits; or business interruption) however caused and
#	on any theory of liability, whether in contract, strict liability, or tort
#	(including negligence or otherwise) arising in any way out of the use of this
#	software, even if advised of the possibility of such damage.
#
#############################################################################################################

# Tool path:
TOOL_PREFIX					:=	pax-

# Tool arguments
base_data_dir				 =	./0-project/
base_project_dir			 =	./

pc_source_dir				 =	$(base_project_dir)2-pc-source/
pc_filtered_dir				 =	$(base_project_dir)3-pc-filtered/
raster_metrics_dir			 =	$(base_project_dir)4-raster-metrics/
plots_dest_dir				 =	$(base_project_dir)4-plots-metrics/
estimates_dir				 =	$(base_project_dir)5-estimates/
mosaic_dir					 =	$(base_project_dir)6-mosaic/
temporary_dir				 =	$(base_project_dir)__temporary/

plots_aggregated_metrics	 =	$(plots_dest_dir)plots-metrics.csv
plots_individual_metrics_dir =	$(plots_dest_dir)plots-metrics-files/
plots_individual_points_dir	 =	$(plots_dest_dir)plots-point-files/

ALL_SOURCE_DIRS				 =	$(base_data_dir) $(pc_source_dir) $(pc_filtered_dir)		\
								$(raster_metrics_dir) $(plots_dest_dir)						\
								$(estimates_dir) $(mosaic_dir) $(temporary_dir) #$(rar_source_dir) 

PDAL_PLUGIN_raster_metrics	 =	"filters.raster_metrics"
PDAL_PLUGIN_plot_metrics	 =	"filters.plot_metrics"
PDAL_PLUGIN_plot_points		 =	"filters.plot_points"
TOOL_concat_plot_metrics	 =	$(TOOL_PREFIX)concat-tables
TOOL_metrics				 =	$(TOOL_PREFIX)metrics


# Aliases to tools.
MKDIR						 = mkdir -p
RM							 = rm
MV							 = mv -f

GENERAL_ARGUMENTS			:= --verbose --meta




########################################################################################
###	Default make

.PHONY: all
all: create_dirs filter metrics estimate mosaic

.PHONY: create_dirs
create_dirs: $(ALL_SOURCE_DIRS) $(plots_individual_metrics_dir) $(plots_individual_points_dir)
$(ALL_SOURCE_DIRS) $(plots_individual_metrics_dir) $(plots_individual_points_dir): 
	@$(MKDIR) "$@"




####	FILTER					######################################

FILTER_DEM						:= $(base_data_dir)dem.tif
FILTER_MIN_Z					:= -2
FILTER_MAX_Z					:= 50
FILTER_LM						:= true

PAX_FILTER0						:= $(TOOL_PREFIX)pc $(GENERAL_ARGUMENTS)									\
										--dem="$(strip $(FILTER_DEM))"										\
										--overlap_resolution="$(strip $(FILTER_OVERLAP_RESOLUTION))"		\
										--max_z=$(strip $(FILTER_MAX_Z)) --min_z=$(strip $(FILTER_MIN_Z))

PAX_FILTER						 = $(PAX_FILTER0) --source=\"$(strip $<)\" --pc_dest=\"$(strip $@)\" 

PDAL_FILTER_PIPELINE			:=	{	"pipeline":[ 														\
										{																	\
											"type":"filters.hag_dem",										\
											"raster":"$(strip $(FILTER_DEM))",								\
											"band":"1",														\
											"zero_ground":"false"											\
										},{																	\
											"type":"filters.slu_lm",										\
											"min_z":"$(strip $(FILTER_MIN_Z))",								\
											"max_z":"$(strip $(FILTER_MAX_Z))",								\
											"lm_filter":"$(strip $(FILTER_LM))"								\
										},{																	\
											"type":"filters.remove_overlap",								\
											"overlap_resolution":"$(strip $(FILTER_OVERLAP_RESOLUTION))"	\
										}																	\
									] }

PDAL_FILTER0					:= pdal translate --json="$(strip $(subst ",\",$(PDAL_FILTER_PIPELINE)))"
PDAL_FILTER						 = $(PDAL_FILTER0)															\
											--input="$(strip $<)" 											\
											--output="$(strip $(temporary_dir)$@)" 							\
											--metadata="$(strip $(temporary_dir)$@).json"

PDAL_FILTER_PROGRESS			:= pdal filtered: overlap_res=$(FILTER_OVERLAP_RESOLUTION), keep=[$(FILTER_MIN_Z),$(FILTER_MAX_Z)], 

POINT_CLOUD_SUFFICES			 = .las .laz .LAS .LAZ
POINT_CLOUD_TARGET_SUFFIX		 = .laz

SOURCE_FILTER_FILES				:= $(foreach suffix,$(POINT_CLOUD_SUFFICES),\
										$(shell find $(pc_source_dir) -type f -name "*$(suffix)"))
SOURCE_FILTER_FILES				+= $(DEST_RAR_FILES)
SOURCE_FILTER_FILES				:= $(sort $(SOURCE_FILTER_FILES))

DEST_FILTER_FILES				 = $(SOURCE_FILTER_FILES)
DEST_FILTER_FILES				:= $(patsubst $(pc_source_dir)%,$(pc_filtered_dir)%,$(DEST_FILTER_FILES))		# Change folder


# The following line was an intent to produce compressed .laz files also from uncompressed .las files.
# To actually do this would require two distinct rules, one for .las and one for .laz.
# This would result in a more complex makefile, so instead either is just passed through.
# Thus, .laz files will result in .laz files and .las files will result in .las files -- but either is processed. 

.PHONY: filter
filter: create_dirs $(DEST_FILTER_FILES)
$(DEST_FILTER_FILES): $(pc_filtered_dir)%: $(pc_source_dir)%
	@$(MKDIR)   "$(dir $@)"
	@$(MKDIR)   "$(dir $(temporary_dir)$@)"
	@$(PDAL_FILTER)
	@$(MV)		"$(strip $(temporary_dir)$@)" "$(strip $(temporary_dir)$@).json" "$(dir $@)"
	@echo $(PDAL_FILTER_PROGRESS) input="$(strip $<)"





####	METRICS0 (calculation)		######################################
#	'raster_metrics' calculates raster and plot rasters for all point cloud files in $(pc_filtered_dir).

PDAL_METRICS_PIPELINE	:=	{	"pipeline":[						\
	{																\
		"type":$(PDAL_PLUGIN_raster_metrics),						\
		"resolution":"$(strip $(metric_resolution))",				\
		"metrics":"$(strip $(metrics_set))",						\
		"nilsson_level":"$(nilsson_level)", 						\
		"gdalopts":"BIGTIFF=IF_SAFER,COMPRESS=DEFLATE",				\
		"data_type":"float"											\
	},{																\
		"type":$(PDAL_PLUGIN_plot_metrics),							\
		"plot_file":"$(base_data_dir)plots.csv",					\
		"metrics":"$(strip $(metrics_set))",						\
		"nilsson_level":"$(nilsson_level)" 							\
	},{																\
		"type":$(PDAL_PLUGIN_plot_points),							\
		"plot_file":"$(base_data_dir)plots.csv",					\
		"dest_plot_points":"$(plots_individual_points_dir)",		\
		"max_distance":"$(plots_individual_points_dir_max_dist)",	\
		"id_column":"id",											\
		"dest_format":".laz"										\
	}																\
] }

PDAL_METRICS0			:= pdal translate --json="$(strip $(subst ",\",$(PDAL_METRICS_PIPELINE)))" 
PDAL_METRICS			 = $(PDAL_METRICS0)																	\
							--input="$(strip $<)" 															\
							--output="$(temporary_dir)/null.bull" 											\
							--writer="writers.null" 														\
							--$(PDAL_PLUGIN_raster_metrics).dest="$(strip $(_raster_metric_dest))" 			\
							--$(PDAL_PLUGIN_plot_metrics).dest_plot_metrics="$(strip $(_plot_metric_dest))" \
							--metadata="$(strip $($@_raster_metric_dest)).json"

PDAL_METRICS_PROGRESS	:= "pdal metrics using '$(strip $(metrics_set))': "

RASTER_TARGET_SUFFIX	 = .tif
RASTER_DEST_SUFFIX		 = .$(metrics_set)

SOURCE_METRIC_FILES		:= $(foreach suffix,$(POINT_CLOUD_SUFFICES),\
								$(shell find $(pc_filtered_dir) -type f -name "*$(suffix)"))
SOURCE_METRIC_FILES		+= $(DEST_FILTER_FILES)
SOURCE_METRIC_FILES		:= $(sort $(SOURCE_METRIC_FILES))

DEST_METRIC_FILES		 = $(SOURCE_METRIC_FILES)
DEST_METRIC_FILES		:= $(patsubst $(pc_filtered_dir)%,$(raster_metrics_dir)%,$(DEST_METRIC_FILES))	# Change folder
DEST_METRIC_FILES		:= $(addsuffix $(RASTER_DEST_SUFFIX),$(basename $(DEST_METRIC_FILES)))			# Change suffix


.PHONY: metrics
metrics: filter $(DEST_METRIC_FILES)
$(DEST_METRIC_FILES): $(raster_metrics_dir)%$(RASTER_DEST_SUFFIX): $(pc_filtered_dir)%$(POINT_CLOUD_TARGET_SUFFIX)
	$(eval _raster_metric_dest := $(patsubst %$(strip $(RASTER_DEST_SUFFIX)),%$(strip $(RASTER_TARGET_SUFFIX)),$@))
	$(eval _plot_metric_dest   := $(plots_individual_metrics_dir)$(notdir $(basename $@)).csv)
	@$(MKDIR)   "$(dir $@)"
	@$(PDAL_METRICS)
	@touch $@
	@echo $(PDAL_METRICS_PROGRESS) "$(strip $@)"




####	METRICS (aggregate individual metric files)	######################################

APPEND_PLOT_METRICS				 = $(TOOL_concat_plot_metrics) $(GENERAL_ARGUMENTS)
APPEND_PLOT_METRICS				+= --source="$(strip $(plots_individual_metrics_dir))"
APPEND_PLOT_METRICS				+= --dest="$(strip $(plots_aggregated_metrics))"

APPEND_INDIVIDUAL_FILES			 = $(shell find $(plots_individual_metrics_dir) -type f -name *.csv)

.PHONY: pre_estimate
pre_estimate: metrics $(plots_aggregated_metrics)
$(plots_aggregated_metrics): $(DEST_METRIC_FILES) $(APPEND_INDIVIDUAL_FILES) 
	@$(APPEND_PLOT_METRICS) --count=$(words $(APPEND_PLOT_FILES))
	@echo $(plots_aggregated_metrics)




####	ESTIMATION				######################################

ESTIMATE						 = $(TOOL_PREFIX)regression $(GENERAL_ARGUMENTS)
ESTIMATE						+= --plots_with_metrics=$(plots_aggregated_metrics)
ESTIMATE						+= --number_of_candidates=$(number_of_candidates)
ESTIMATE						+= --outlier_removal_perc=$(outlier_removal_percentage)
ESTIMATE						+= --aux_dir=$(strip $(base_data_dir))
#ESTIMATE						+= --debug

RUTA_METADATA_FILE				:= $(base_data_dir)ruta_meta.csv

# These files will also ber produced.
METRIC_FILTER_FILES				 = $(SOURCE_FILTER_FILES)
METRIC_FILTER_FILES				:= $(patsubst $(pc_source_dir)%,$(raster_metrics_dir)%,$(METRIC_FILTER_FILES))
METRIC_FILTER_FILES				:= $(addsuffix $(RASTER_DEST_SUFFIX),$(basename $(METRIC_FILTER_FILES)))


# There are three sources of source files for the estimation rule.
# The first source is files that are in raster_metrics_dir from the beginning:
ESTIMATION_FILES				 = $(shell find $(raster_metrics_dir) -type f -name $(RASTER_DEST_SUFFIX))

# The second source is from the rule 'metrics':
ESTIMATION_FILES				+= $(DEST_METRIC_FILES)

# The third source is from the rule 'filter':
ESTIMATION_FILES				+= $(METRIC_FILTER_FILES)

# Change folder from raster_metrics_dir to estimates_dir.
ESTIMATION_FILES				:= $(patsubst $(raster_metrics_dir)%,$(estimates_dir)%,$(ESTIMATION_FILES))

# Sorting also removes doubles.
ESTIMATION_FILES				:= $(sort $(ESTIMATION_FILES))

ESTIMATION_FILES				:= $(addsuffix $(RASTER_DEST_SUFFIX),$(basename $(ESTIMATION_FILES)))

# Sorting removes doubles.
ESTIMATION_FILES				:= $(sort $(ESTIMATION_FILES))


.PHONY: estimate 
estimate: pre_estimate $(ESTIMATION_FILES)
$(ESTIMATION_FILES): $(estimates_dir)%: $(raster_metrics_dir)% $(plots_aggregated_metrics) $(RUTA_METADATA_FILE)
	@$(MKDIR)   "$(dir $@)"
	@$(ESTIMATE) --source=$(strip $<) --dest=$(strip $(dir $@)) #--debug
	@touch $@





####	MOSAIC, variables		######################################

MOSAIC_METRICS					:= $(shell $(TOOL_metrics) --metrics $(metrics_set) --nilsson_level $(nilsson_level))
MOSAIC_VARIABLES				:= Basal_area Biomass_above Biomass_below Dgv Hgv_dm Volume
MOSAIC_SUFFICES					:= $(sort $(MOSAIC_VARIABLES) $(MOSAIC_METRICS))
MOSAIC_DEST						:= $(addprefix $(mosaic_dir),$(MOSAIC_SUFFICES))
MOSAIC_DEST						:= $(addsuffix .tif,$(MOSAIC_DEST))

NOOP							:=
SPACE							:= $(NOOP) $(NOOP)

GDALBUILDVRT					 = gdalbuildvrt -q -overwrite 				\
										"$(strip $(temporary_dir)$@).vrt" 	\
										-input_file_list "$(strip $(temporary_dir)$@).txt"

# Cloud Optimized GeoTIFF: https://geoexamples.com/other/2019/02/08/cog-tutorial.html
# COMPRESS=DEFLATE: DEFLATE or LZW -- the first compresses better, but is less compatible.
# TILED=YES: The image is divided in small tiles, for easy partial download.
# INTERLEAVE=BAND: You may want to download only one band at once. 
GDAL_TRANSLATE					 = gdal_translate -q -of GTiff				\
										-co BIGTIFF=IF_SAFER 				\
										-co COMPRESS=DEFLATE				\
										-co TILED=YES						\
										-co INTERLEAVE=BAND					\
										"$(strip $(temporary_dir)$@).vrt" 	\
										"$(strip $(temporary_dir)$@)"

# -ro: Create an external overview file (.ovr).
# COMPRESS_OVERVIEW DEFLATE: DEFLATE or LZW -- the first compresses better, but is less compatible.
GDALADDO						 = gdaladdo -q -r average 					\
										-ro 								\
										--config BIGTIFF_OVERVIEW IF_SAFER 	\
										--config COMPRESS_OVERVIEW DEFLATE 	\
										"$(strip $(temporary_dir)$@)"

.PHONY: mosaic 
mosaic: estimate $(MOSAIC_DEST)
$(MOSAIC_DEST): $(ESTIMATION_FILES)
	@$(MKDIR)   "$(dir $(temporary_dir)$@)"
	@echo $(subst $(SPACE),"\n",$(wildcard $(estimates_dir)*/*.$(notdir $@))$(wildcard $(raster_metrics_dir)*/*.$(notdir $@))) > "$(strip $(temporary_dir)$@).txt"
	@$(GDALBUILDVRT) 
	@$(GDAL_TRANSLATE)
	@$(GDALADDO) 
	@$(RM) "$(strip $(temporary_dir)$@).vrt" "$(strip $(temporary_dir)$@).txt"
	@$(MV) "$(strip $(temporary_dir)$@)"     "$(strip $(temporary_dir)$@).ovr" "$(dir $@)"
	@echo "    Mosaic done:    $@"





####	DEBUG				######################################

arg = svejs

.PHONY: debug
debug:
	@echo "Debuginfo -------------------------------------------------------- $(NAME)"
	@echo "APPEND_PLOT_METRICS       $(APPEND_PLOT_METRICS)"
