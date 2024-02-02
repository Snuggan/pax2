# encoding: utf-8


#' Report missing columns in a data.frame.
#' @param df			The data.frame.
#' @param required_cols	A character vector of the required column names.
#' @param message		A prefix for error messages.
#' @return 				A character vector of the missing column names.
#' @seealso 			read_csv_file, write_csv_file
#' @author 				Peder Axensten
#' @export
check_available_columns <- function( 
	df, 
	required_cols=NULL,
	message	= ""
) {
	log_begin()
	log_item( "Required columns:   ", paste( required_cols, collapse=", " ) )
	if( !is.null( required_cols ) ) {
		missing		<- required_cols[ !( required_cols %in% colnames( df ) ) ]
		if( length( missing ) > 0 ) {
			message2 <- paste0( message, "missing columns: [ ", paste( missing, collapse=", " ), " ]" )
			if( ncol( df ) == 1 )
				log_stop( message2, " --- Probably wrong separator, must be ';'" )
			else
				print( paste( colnames( df ), collapse=", " ) )
				log_stop( message2, "\n  ", message, "column names:    [ ", paste( colnames( df ), collapse=", " ), " ]" )
		}
	}
	log_end()
}


#' Read a csv-like file and return a data.frame. If the argument is a data.frame, just return it.
#'
#' The function does some checks:
#' - If \code{path_or_df} is a character string it is treated as a path. If there is no such file
#'   an error is reported.
#' - If \code{required_cols} is defined, any missing column names are reported as an error. 
#'
#' @param path_or_df	A string. The path or data.frame of the data.
#' @param required_cols	A character vector of required column names.
#' @param message		A prefix for error messages.
#' @return 				The data.frame.
#' @seealso 			write_csv_file, check_available_columns
#' @author 				Peder Axensten
#' @export
read_csv_file <- function( 
	path_or_df,
	required_cols=NULL,
	message	= ""
) {
	log_begin()
	if( is.character( path_or_df ) ) {
		log_item( "reading '", path_or_df, "'" )

		if( !file.exists( path_or_df ) )	log_stop( message, "No such file: '", path_or_df, "'\n" )
		path_or_df <- utils::read.table( 
			path_or_df, 
			header=TRUE, 
			sep=";", 
			dec=".", 
			stringsAsFactors=FALSE, 
			comment.char=""
		)
	} else if( !is.data.frame( path_or_df ) )
		log_stop( message, "Strange path_or_df argument class in call to read_csv_file: '", class( path_or_df ), "'\n" )

	# Check if there are missing columns.
	check_available_columns( df=path_or_df, required_cols=required_cols, message=message )
	log_end()
	return( path_or_df )
}


#' Write a data.frame or similar object as a csv-like file.
#' @param df			A data.frame or similar object.
#' @param dest_path		A string. The destina<tion path.
#' @return 				The data.frame.
#' @seealso 			read_csv_file, check_available_columns
#' @author 				Peder Axensten
#' @export
write_csv_file <- function( df, dest_path ) {
	log_begin()
	log_item( "writing '", dest_path, "'" )
	utils::write.table( x=df, file=dest_path, sep=";", dec=".", quote=FALSE, eol="\n", col.names=TRUE, row.names=FALSE )
	log_end()
}
