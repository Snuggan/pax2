# encoding: utf-8


#' A family of functions that implements simple logging. 
#' @seealso 		\code{\link{pax.regression.package}}
#' @name 			logging
NULL
#> NULL

#' @describeIn 		logging Set (or reset) the logging sink (destination).
#' @param sink 		Any textual sink, such as a file name, \code{nullfile()}, \code{stdout()}, \code{stderr()}, etc. 
#'					It is saved and used in following calls to log functions. A value of \code{nullfile()} (the default value) will turn logging off. 
#' @export
log_set_sink <- function( sink=nullfile() ) {
	log_item_dest	<<- sink
	log_item_tabs	<<- 0
	cat( as.character( Sys.time() ), "\n\n", sep="", file=log_item_dest, append=FALSE )
}

#' @describeIn 		logging Log an item.
#' @param ... 		Text to log. It will be logged as \code{paste0(...)}.
#' @export
log_item <- function( ... ) {
	if( ...length() > 0 )
		cat( replicate( log_item_tabs, "\t" ), ..., "\n", sep="", file=log_item_dest, append=TRUE )
}

#' @describeIn 		logging An error occured, log the error message and stop.
#' @param ... 		Error message. It will be logged as \code{paste0(...)}.
#' @param call. 	Past on to stop: logical, indicating if the call should become part of the error message.
#' @param domain 	Past on to stop: see \code{gettext}. If \code{NA}, messages will not be translated.
#' @export
log_stop <- function( ..., call.=FALSE, domain=NULL ) {
	log_item( "Error in call from: ", deparse( sys.calls()[[ sys.nframe() - 1 ]][ 1 ] ) )
	log_item( ... )
	cat( "Error in call from: ", deparse( sys.calls()[[ sys.nframe() - 1 ]][ 1 ] ), "\n", sep="", file=stderr() )
	traceback()
	stop( ..., call.=FALSE, domain=domain )
}

#' @describeIn 		logging Log Returns the logging sink (destination).
#' @export
log_sink <- function() {
	return( log_item_tabs )
}

#' @describeIn 		logging Log the entry into a function.
#' @export
log_begin <- function() {
	log_item( "BEGIN: ", deparse( sys.calls()[[ sys.nframe() - 1 ]][ 1 ] ) )
	log_item_tabs <<- log_item_tabs + 1
}

#' @describeIn 		logging Log the exit from a function.
#' @export
log_end <- function() {
	log_item_tabs <<- max( log_item_tabs - 1, 0 )
	log_item( "END:   ", deparse( sys.calls()[[ sys.nframe() - 1 ]][ 1 ] ) )
}
