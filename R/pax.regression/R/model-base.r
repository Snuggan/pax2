# encoding: utf-8


#' A bunch of functions that implements simple logging. 
#'
#' @param variant	The object used for S3 dispatch.
#' @param x			A data.frame of the data to process.
#' @seealso			\code{\link{pax.regression.package}}
#' @name			modelling_details
NULL
#> NULL

#' @describeIn		modelling_details	Returns a vector of strings, with the names of the dependent variables to calculate.
dependent_variables			<- function( variant )		{	UseMethod( "dependent_variables", variant )						}
dependent_variables.default	<- function( variant )		{	return( c( "Hgv", "Volume", "Biomass_above", "Biomass_below", "Basal_area", "Dgv" ) )	}

#' @describeIn		modelling_details	Returns the data.frame \code{x}, with whatever additional columns.
dependent_preparation		<- function( variant, x )	{	UseMethod( "dependent_preparation", variant )					}

#' @describeIn		modelling_details	Returns a vector of strings, with the names of the the independent variables.
independent_variables		<- function( variant )		{	UseMethod( "independent_variables", variant )					}

#' @describeIn		modelling_details	Calculate data needed by the modelling AND extra rasters before using the models.
#' 		 			Returns the data.frame \code{x}, with whatever additional columns.
independent_preparation		<- function( variant, x )	{	UseMethod( "independent_preparation", variant )					}

#' @describeIn		modelling_details	Calculate the models.
#' @param plots					A data.fram of the data to process.
#' @param auxiliary_data_dir	Where to find auxiliary data, if needed.
#' @param metadata_dest_dir		Store metadata here, if needed.
build_models <- function( 
	variant, 
	plots, 
	auxiliary_data_dir,
	metadata_dest_dir		= NA 
)	{	UseMethod( "build_models", variant )	}



model_metadata <- function( models, dest_dir = NULL ) {
	log_begin()
	params		<- ""

	grDevices::pdf( file=file.path( dest_dir, "residuals.pdf" ), paper="a4" )
	for( name in names( models ) ) {
		model	<- models[[ name ]]
		pred	<- fitted( model )
		res		<- residuals( model )
		lhs		<- pred + res

		graphics::plot( 
			x	= lhs,
			y	= res,
			main=name,
			xlab=all.vars( formula( model ) )[1], 
			ylab="residuals" 
		)
		
		cff		<- coef( model )
		params	<- paste0( params, name, ": ", paste( names( cff ), cff, sep="=", collapse=", " ), "\n" )
	}
	grDevices::dev.off()

	save( models, file=file.path( dest_dir, "models.Rdata" ) )
	cat ( params, file=file.path( dest_dir, "models-parameters.txt" ) )
	log_end()
}



#' Implements basic regression.
#'
#' \subsection{Want to implement more models or variants?}{
#' In this package, S3 dispatch is used. All you need to implement is the functions mention below, 
#' no changes in the base code such as this function should be necessary. 
#' If you want to implement other types of models, do it by following the 'basic_linear' case.
#' All relevant functions are implemented as S3 classes, so implement them as 
#' \enumerate{
#' - \code{independent_variables.my_model()},
#' - \code{dependent_preparation.my_model()}, 
#' - \code{build_models.my_model()}, and
#' - \code{predict.my_model()}.
#' }
#' After that you can call this function with the argument \code{variant="my_model"}.
#' If you want to implement an entirely different regression method, the you can reimplement this
#' function as \code{model_and_predict.my_model()}. But this default implementation should suffice for most cases.
#' }
#'
#' @param variant				(string) What kind of modelling to do. At the moment, only "basic_linear" is implemented.
#' @param source_stub			A path stub to the metrics files.
#' @param plots					A path or a data.fram of the data to process.
#' @param dest_dir				A path + file prefix, used for all file output.
#' @param auxiliary_data_dir	A path to the auxiliary data directory. 
#' @param number_of_candidates	(unsigned integer) The number of plot kandidates to supply to the outlier removal.
#' @param metadata_output		Produces process metadata files if true.
#' @name 						model_and_predict
#' @rdname 						model_and_predict
#' @author						Peder Axensten
model_and_predict <- function( 
	variant, 
	source_stub, 
	plots, 
	dest_dir,
	auxiliary_data_dir,
	number_of_candidates	= 350,
	metadata_dest_dir		= NA
) {
	log_begin()

	# General preparations.
	message					<- paste0( "In model_and_predict, using ", variant, ": " )
	metrics					<- independent_variables( variant=variant )
	variables				<- dependent_variables( variant=variant )

	# Build the models.
	plots					<- read_csv_file( path_or_df=plots, required_cols=c( metrics, variables ), message=message )
	plots					<- dependent_preparation( variant=variant, x=plots )
	models <- build_models( 
		variant				= variant, 
		plots				= plots, 
		auxiliary_data_dir	= auxiliary_data_dir, 
		metadata_dest_dir	= metadata_dest_dir 
	)

	# Export metradata.
	if( !is.na( metadata_dest_dir ) ) {
		metadata_dest_dir	<- file.path( metadata_dest_dir, "model_and_predict" )
		dir.create( metadata_dest_dir, showWarnings=FALSE, recursive=TRUE )

		write_csv_file( df=plots, dest_path=file.path( metadata_dest_dir, "plot_sample_with_extra_metrics.csv" ) )
		model_metadata( models, metadata_dest_dir )
	}

	# Prediction. 
	metrics					<-	read_rasters(
									source			= source_stub, 
									auxiliary_dir	= auxiliary_data_dir,
									names			= metrics,
									message			= message
								)
	metrics					<-	independent_preparation( variant=variant, x=metrics )
	predictions				<-	predict( 
									variant				=	variant, 
									models				=	models, 
									metrics				=	metrics, 
									auxiliary_data_dir	=	auxiliary_data_dir, 
									field_plots			=	plots 
								)
	
	# Export forest variable rasters.
	write_rasters( destination_stub=dest_dir, rasters=predictions, variant )

	log_end()
}
