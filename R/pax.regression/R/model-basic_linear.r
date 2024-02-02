# encoding: utf-8


independent_variables.basic_linear <- function( variant ) {
	c( "count_1ret", "count_1ret_ge150cm", "variance_all_ge150cm", "p80_all_ge150cm", "p95_all_ge150cm", "species_type" )
}

independent_preparation.basic_linear <- function( variant, x ) {
	log_begin()
	x$stddev			= sqrt( x$variance_all_ge150cm )
	x$vegprop			= x$count_1ret_ge150cm / x$count_1ret
	x$h80veg	    	= x$p80_all_ge150cm * x$vegprop
	log_end()
	return( x )
}

dependent_preparation.basic_linear <- function( variant, x ) {
	log_begin()
	x					= independent_preparation.basic_linear( x=x )
	x$RootVol 			= sqrt( x$Volume )
	x$RootBioAbove 		= sqrt( x$Biomass_above )
	x$RootBioBelow 		= sqrt( x$Biomass_below )
	log_end()
	return( x )
}

build_models.basic_linear <- function( 
	variant, 
	plots, 
	auxiliary_data_dir,
	metadata_dest_dir	=	NA 
) {
	log_begin()

	Hgv					<-	as.formula( Hgv		   	  ~ species_type + p95_all_ge150cm )
	RootVol				<-	as.formula( RootVol 	  ~ species_type + p95_all_ge150cm + h80veg + stddev  )
	RootBioAbove		<-	as.formula( RootBioAbove  ~ species_type + p95_all_ge150cm + h80veg + vegprop )
	RootBioBelow		<-	as.formula( RootBioBelow  ~ species_type + p95_all_ge150cm + h80veg + vegprop )
	Basal_area			<-	as.formula( Basal_area	  ~ species_type +                   h80veg + stddev  )
	Dgv					<-	as.formula( Dgv			  ~ species_type + p95_all_ge150cm + h80veg )

	models <- list(
		Hgv				=	rlm( formula=Hgv,			data=plots ), 
		RootVol			=	rlm( formula=RootVol,		data=plots ),
		RootBioAbove	=	rlm( formula=RootBioAbove,	data=plots ), 
		RootBioBelow	=	rlm( formula=RootBioBelow,	data=plots ), 
		Basal_area		=	rlm( formula=Basal_area,	data=plots ), 
		Dgv				=	rlm( formula=Dgv,			data=plots )
	)
	
	class( models )		<-	"list_of_linear_models"
	log_end()
	return( models )
}


predict.basic_linear	<-	function( 
	variant, 
	models, 
	metrics, 
	auxiliary_data_dir,
	field_plots 		=	NULL 
) {
	log_begin()

	# Make predictions.
	x <- list(
		Hgv_dm			=	predict( metrics, models$Hgv 			) * 10,		# Multiply by 10 to make it decimeters. 
		Volume			=	predict( metrics, models$RootVol		),
		Biomass_above	=	predict( metrics, models$RootBioAbove	),
		Biomass_below	=	predict( metrics, models$RootBioBelow	),
		Basal_area		=	predict( metrics, models$Basal_area		),
		Dgv				=	predict( metrics, models$Dgv			)
	)

	# Invert linarisation and the bias it creates.
	x$Volume			<-	fix_predicted_root( model=models$RootVol,		values=x$Volume,		tag="Volume",			plots=field_plots )
	x$Biomass_above		<-	fix_predicted_root( model=models$RootBioAbove,	values=x$Biomass_above,	tag="Biomass_above",	plots=field_plots )
	x$Biomass_below		<-	fix_predicted_root( model=models$RootBioBelow,	values=x$Biomass_below,	tag="Biomass_below",	plots=field_plots )

	log_end()
	return( x )
}
