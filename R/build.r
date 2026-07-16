#!/usr/bin/env -S Rscript --vanilla
# encoding: utf-8


################################################
#
# To prepare and install the package:
# 1. In R, run: roxygenize( package.dir, roclets = NULL, clean = TRUE )
# 2. I the terminal, run: R CMD check pax.regression
# 3. In R, install the package.
#
################################################


pax.dir			<- file.path( "/Users", "pax", "Documents", "develop", "pax2" )
if( !dir.exists( pax.dir ) ) {
	stop( "Update the path 'pax.dir': there is no such directory", pax.dir )
}


require( getopt,   quietly=TRUE )
require( roxygen2, quietly=TRUE )


args = matrix( c(
        'check',	'c',	0,	"logical",	"Run a check on the package",
        'help',		'h',	0,	"logical",	"this help"
), ncol=5, byrow=TRUE )


opt = getopt( args )
if( !is.null( opt$help ) ) {
    cat( getopt( args, usage=TRUE ), "\n" )
    q();
}

package.dir			<- file.path( pax.dir, "R", "pax.regression/" )

# Remove the invisible files that MacOS places everywhere and that turn up as warnings.
DS_Store 			<- list.files( package.dir, pattern="^\\.DS_Store$", all.files=TRUE, recursive=TRUE )
file.remove( file.path( package.dir, DS_Store ) )

if( dir.exists( package.dir ) ) {
	cat ( "In dir ", package.dir, "<\n" )
	roxygenise( package.dir=package.dir, roclets = NULL, clean = TRUE )
	if( !is.null( opt$check ) ) {
		cat( "Check\n" )
		system( paste( "RCMD check --as-cran", package.dir ) )
	}
	install.packages( pkgs=package.dir, dependencies=TRUE, repos=NULL, type="source" )
	
	# opy the shell script to run the regression to bin/:
	file.copy(	file.path( package.dir, "inst", "tools", "pax-regression/" ), 
				file.path( "/usr", "local", "bin" ) )
	cat( "Copy", file.path( package.dir, "inst", "tools", "pax-regression/" ), "to", file.path( "/usr", "local", "bin" ), "\n" )
} else {
	stop( "Update the path 'package.dir': there is no such directory", package.dir )
}
