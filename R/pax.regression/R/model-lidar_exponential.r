# encoding: utf-8


dependent_preparation.lidar_exponential <- function( variant, x ) {
	log_begin()
	x				<-	independent_preparation.lidar_exponential( x = x )
	x$RootVol 		<-	sqrt( x$Volume )
	x$RootBio 		<-	sqrt( x$Biomass )
	log_end()
	return( x )
}

independent_variables.lidar_exponential <- function( variant ) {
	c(	"prop_1ret_gel_of_1ret", "sample_std_dev_gel", "p30_gel", "p80_gel", "p95_gel", "species_type"	)
}

independent_preparation.lidar_exponential <- function( variant, x ) {
	log_begin()
	x$h80veg			<-	x$p80_gel * x$prop_1ret_gel_of_1ret
	log_end()
	return( x )
}


build_models.lidar_exponential <- function( 
	variant, 
	plots, 
	auxiliary_data_dir,
	metadata_dest_dir	=	NA 
) {
	log_begin()

	models 				<-	list(
		Basal_area		=	LM ( formula=Basal_area,	data=plots ),
		Biomass			=	LM ( formula=RootBio,		data=plots ),
		Dgv				=	LM ( formula=Dgv,			data=plots ),
		Hgv_dm			=	LM ( formula=Hgv,			data=plots ), 
		Volume_linear	=	LM ( formula=RootVol,		data=plots )

#		Volume_exp		=	NLS( formula=Volume_exp,	data=plots ),
#		Volume_pow		=	NLS( formula=Volume_pow,	data=plots ),
#		Volume_pow0		=	NLS( formula=Volume_pow0,	data=plots ),
#		Volume_sqA		=	NLS( formula=Volume_sqA,	data=plots ),
#		Volume_sqB		=	NLS( formula=Volume_sqB,	data=plots ),
#		Volume_sqC		=	NLS( formula=Volume_sqC,	data=plots )
	)

	class( models )		<-	"list_of_lidar_exponential_models"
	log_end()
	return( models )
}


predict.lidar_exponential <- function( 
	variant, 
	models, 
	metrics, 
	auxiliary_data_dir,
	field_plots 		=	NULL 
) {
	log_begin()

	# Add the (cropped) tree species raster to the metrics. 
	tree_species_file	<-	file.path( auxiliary_data_dir, "species_type.tif" )
	metrics$species_type <-	terra::crop( 
		x				=	read_one_raster( source = tree_species_file ), 
		y				=	metrics[[ 1 ]]
	)
	
	# Make predictions.
	x <- list()
	for( var in names( models ) ) {
		x[[ var ]]		<-	predict( metrics, models[[ var ]] )
	}

	# Post-processing of some values.
	x$Hgv_dm			<-	x$Hgv_dm * 10		# Multiply by 10 to make it decimeters. 
	x$Volume_linear		<-	fix_predicted_root( model=models$Volume_linear, values=x$Volume_linear, tag="Volume",  plots=field_plots )
	x$Biomass			<-	fix_predicted_root( model=models$Biomass,       values=x$Biomass,       tag="Biomass", plots=field_plots )

	# Which of the volumes should be
	x$Volume			<-	x$Volume_linear
	x$Volume_linear		<-	NULL

	log_end()
	return( x )
}

