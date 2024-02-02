# encoding: utf-8


#' Report a vector of modelling variants.
#'
#' @seealso \code{\link{pax.regression.package}}
#' @export
variant_alts	<- function()						{	return( c( 'basic-linear', 'lidar-exponential' ) )		}


#' Get the ruta id.
#'
#' @param metrics_path_stub, a path stub to the metrics files. Also defines the ruta id. 
#' @return The corresponding ruta's id.
#' @export
get_ruta_id		<- function( metrics_path_stub )	{	return( basename( metrics_path_stub ) )					}


#' Returns the file name without any suffix.
#'
#' @param path, a string. The path (or similar) to the data.
#' @return The file name in \code{path} without any suffix.
#' @author Peder Axensten
#' @seealso file.suffix
#' @export
file.name		<- function( path )					{	return( sub( "^(.*)\\..*$", "\\1", path ) )				}


#' Returns the file name suffix, including the leading ".".
#'
#' @param path, a string. The path (or similar) to the data.
#' @return The file name suffix of \code{path}, including the leading ".".
#' @author Peder Axensten
#' @seealso file.name
file.suffix		<- function( path )					{	return( sub( "^.*(\\..*)$", "\\1", path ) )				}


#' Bias correction for predicted squareroot values.
#
#' The correction factor is mean( true variable values )/mean( predicted variable values ).
fix_predicted_root	<-	function( 
	model,
	values,
	tag, 
	plots 			=	NULL 
) {
	log_begin()
	if( is.null( plots ) ) {
		log_end()
		return( values^2 )
	} else {
		# Correct for biasness.
		log_end()
		return( values^2 * ( mean( plots[ , tag ] ) / mean( model$fitted.values^2 ) ) )
	}
}
