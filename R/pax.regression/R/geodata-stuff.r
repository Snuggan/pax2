# encoding: utf-8


#' A family of functions that implements simple logging. 
#' @seealso 		\code{\link{pax.regression.package}}
#' @name 			geodata.stuff
NULL
#> NULL


#' @describeIn 		geodata.stuff Construct a crs string from an epsg number. Returns a WKT2 string of the CRS for recent versions of PROJ, rgdal & co., 
#' 					otherwise a "+init=epsg:x" proj-string (proj-strings are being deprecated in PROJ, replaced by WKT2). 
#' @references 		Kristian Evers and Thomas Knudsen, \emph{Transformation pipelines for PROJ.4,} 
#'     				\url{https://www.fig.net/resources/proceedings/fig_proceedings/fig2017/papers/iss6b/ISS6B_evers_knudsen_9156.pdf} (2017)
#' @references 		\emph{SpatialLite and PROJ.6,} 
#'     				\url{https://www.gaia-gis.it/fossil/libspatialite/wiki?name=PROJ.6}
#' @references 		Roger Bivand, \emph{ECS530: (III) Coordinate reference systems,} 
#'    				\url{https://rsbivand.github.io/ECS530_h19/ECS530_III.html} (2019-12-03)
#' @references 		Edzer Pebesma and Roger Bivand, \emph{R spatial follows GDAL and PROJ development,} 
#'					\url{https://www.r-spatial.org/r/2020/03/17/wkt.html#crs-objects-in-sf} (2020-03-17)
#' @param epsg 		The epsg id of the crs. 
#' @export
epsg_string <- function( epsg ) {
	log_begin()
	log_item( "epsg: ", epsg )
    if( nzchar( find.package( 'sf', quiet=TRUE ) ) && ( packageVersion( "sf" ) >= "0.9" ) ) 
        result <- sf::st_crs( epsg )$wkt
    else if( nzchar( find.package( 'rgdal', quiet=TRUE ) ) && ( packageVersion( "rgdal" ) >= "1.5.1" ) ) 
        result <- rgdal::showSRID( paste0( "EPSG:", epsg ), format="WKT2_2018" )
    else 
        result <- paste0( "+init=epsg:", epsg )
	log_end()
    return( result ) # Or return( gsub( "\\s\\s+", " ", result ) )
}


#' @describeIn 		geodata.stuff Read one raster file. Returns a [terra] raster.
#' @param source	The file path of the raster. 
#' @param extent	The part (bounding box, object of class SpatExtent) of the raster to read. 
#' @param message	A prefix for error messages.
#' @export
read_one_raster <- function(
	source,
	extent			= NULL,	
	message			= ""
) {
	log_begin()
	log_item( "Reading raster:  '", source, "'" )
	if( file.exists( source ) ) {
		r <- terra::rast( x=source )
		if( !is.null( extent ) )				r <- crop( r, extent )
		if( nchar( terra::crs( r ) ) == 0 )		terra::crs( r ) <- epsg_string( 3006 )

		log_item( "    Layers:      ", toString( terra::nlyr( r ) ) )
		log_item( "    Rows:        ", toString( terra::nrow( r ) ) )
		log_item( "    Columns:     ", toString( terra::ncol( r ) ) )
		log_item( "    Resolution:  ", toString( terra::res ( r ) ) )
		log_item( "    Extent:      ", toString( terra::ext ( r ) ) )
		log_item( "    Has crs:     ", nchar( toString( terra::crs( r ) ) ) )
	} else {
		log_stop( message, "No such metric raster file available: ", source, "'\n" )
	}
	log_end()
	return( r )
}


#' @describeIn 		geodata.stuff Returns the extension (bounding box) of a raster. Object of class SpatExtent.
#' @param source	The file path of the raster. 
#' @param message	A prefix for error messages.
read_raster_extent <- function(
	source,
	message			= ""
) {
	log_begin()
	log_item( "reading the bbox of '", source, "'" )
	r <- read_one_raster( source, message=message )
	log_end()
	return( terra::ext( r ) )
}


#' @describeIn 		geodata.stuff Read one or more raster files into a list. Returns a list of metric rasters.
#' @param dirs		One or more directory paths, where to look for the file. 
#' @param file		The file name. 
#' @param extent	The part (bounding box, object of class SpatExtent) of the raster to read. 
#' @param suffices	One or more optional file name suffices, an alias for names[ i ] will be paste0( names[ i ], suffix ).
#' @param message	A prefix for error messages.
read_rasters <- function(
	source, 
	auxiliary_dir, 
	names,
	extent			= NULL, 
	message			= ""
) {
	log_begin()
	rasters					<- list()
	if( length( names ) ) {
		names				<-	unique( names )
		suffix				<-	file.suffix( source )	# E.g. ".tif"
		source				<-	file.name( source )		# Path without suffix (e.g. ".tif").
		for( name in names ) {
			source_file1	<-	paste0( source, ".", name, suffix )
			source_file2	<-	file.path( auxiliary_dir, paste0( name, ".tif" ) )
			log_item( "extent: ", toString( extent ) )
			if( file.exists( source_file1 ) ) {
				log_item( "trying: '", source_file1, "'" )
				rasters[[ name ]]	<-	read_one_raster( source=source_file1, extent=extent, message=message )
			} else if( file.exists( source_file2 ) ) {
				log_item( "trying: '", source_file2, "'" )
				rasters[[ name ]]	<-	read_one_raster( source=source_file2, extent=extent, message=message )
			} else {
				log_stop( message, "Did not find file named '", name, "'" )
			}
			if( is.null( extent ) )
				extent <- terra::ext( rasters[[ name ]] )
		}
	}	
	log_end()
	return( terra::rast( rasters ) )
}


#' @describeIn 		geodata.stuff Write a list of named rasters.
#' @param destination_stub	A path stub that the name of each raster in the list is added to. 
#' @param rasters	A list of named rasters.
#' @export
write_rasters <- function( destination_stub, rasters, variant ) {
	log_begin()
	suffix				<- file.suffix( destination_stub )	# E.g. ".tif"
	destination_stub	<- file.name( destination_stub )	# Path without suffix (e.g. ".tif").
	for( v in names( rasters ) ) {
		log_begin()
		dest			<- paste0( destination_stub, ".", v, suffix )
		log_item( "writing: '", dest, "'" )
		terra::writeRaster(
			x			= rasters[[ v ]],
			filename	= dest,
			overwrite	= TRUE,
			datatype	= "INT2S",
			NAflag		= -1
		)
		log_end()
	}
	log_end()
}
