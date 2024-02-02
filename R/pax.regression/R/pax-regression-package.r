#' Tools to use for regression in the forestry domain.
#'
#' This package implements various regression methods.
#'
#' \subsection{Regression methods}{
#' \enumerate{
#'	\item \code{\link{basic_linear}} basic linear regression using rlm.
#'	\item \code{\link{lidar_exponential}} non-linear regression using nls.
#' }}
#'
#' @author		Peder Axensten
#' @docType		package
#' @seealso		\code{\link{pax.regression}} To choose plots, remove outliers, build models, and predict.
#' @seealso		\code{\link{plot_sample_ruta}} To choose plots.
#' @seealso		\code{\link{plot_sample_outlier_reduction}} Remove outliers among ploits.
#' @seealso		\code{\link{variant_alts}} List what estimation methods that are available.
#' @name		pax.regression.package
NULL



#' Calls the 'variant' regression function.
#'
#' @param source				A path stub to the metrics files. Also defines the ruta id. 
#' @param plots					A path to a .csv file or a data.frame of the plots.
#' @param dest					A path + file prefix, used for all file output.
#' @param auxiliary_data_dir	A path to the auxiliary data directory. 
#' @param number_of_candidates	(unsigned integer) The number of plot kandidates to supply to the outlier removal.
#' @param outlier_removal_perc	(float in [0,95]) The percentage of plot candidates to remove in thge outlier removal.
#' @param metadata_dest_dir		If no is.na metadata is stored here.
#' @author						Peder Axensten
#' @seealso 					\code{\link{pax.regression.package}}
#' @export
pax.regression <- function( 
	source, 
	plots, 
	dest, 
	auxiliary_data_dir, 
	number_of_candidates	= 350, 
	outlier_removal_perc	=  10, 
	metadata_dest_dir_dir	= NULL
) {
	log_begin()

	if( ( 0 > outlier_removal_perc ) || ( outlier_removal_perc > 95 ) ) 
		log_stop( "Error in pax.regression: unreasonable proportion of outliers to remove: '", outlier_removal_perc, 
				  "' -- choose a value in [0,95]\n" )

	variant0				<- file.suffix( source )
	variant0				<- sub( "^.", "", variant0 )
	
	if( variant0 == "extra-allt" ) {
		variant 			<- "basic-linear"
	} else {
		variant 			<- variant0
	}
	class( variant )		<- sub( "-", "_", variant )

	if( variant %in% variant_alts() ) {

		source				<- paste0( file.name( opt$source ), ".tif" )
		ruta_id				<- get_ruta_id( source )
		dest				<- file.path( dest, ruta_id )

		# Get the plots best suited for this ruta.
		sample <- plot_sample_ruta( 
			ruta_id					= ruta_id, 
			plots					= plots, 
			ruta_metadata			= file.path( auxiliary_data_dir, "ruta_meta.csv" ), 
			number_of_candidates	= number_of_candidates, 
			metadata_dest_dir		= metadata_dest_dir 
		)
	
		# Remove outliers.
		sample <- plot_sample_outlier_reduction( 
			plots					= sample, 
			outlier_removal_perc	= outlier_removal_perc, 
			metadata_dest_dir		= metadata_dest_dir 
		)
	
		# Build a model and predict. Save the resulting variable rasters.
		model_and_predict(
			variant					= variant, 
			source_stub				= source,
			plots					= sample, 
			dest_dir				= dest, 
			auxiliary_data_dir		= auxiliary_data_dir, 
			metadata_dest_dir		= metadata_dest_dir
		)

		# Create suffix-independent empty file[s], as "done" flag. 
		file.create( paste0( file.name( dest ), ".", variant  ) )
		file.create( paste0( file.name( dest ), ".", variant0 ) )

	} else {
		log_stop( "*** No modelling implemented for variant '", variant, "': no dependent variables will be generated.\n" )
	}

	log_end()
}
