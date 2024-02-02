# encoding: utf-8



#' Get a vector of strings, with the names of the required variables used in the candidate selection.
#' @return 						A vector of strings, with the names of the required variables used in the candidate selection.
#' @author 						Peder Axensten
required_plot_cols.plot_sample_ruta <- function() {
	c(	"east", "north", "radius", "leaveson", "scanner_type", "Hgv", "count_all", "p95_all_ge150cm"	)
}


#' Given a table of all plots, it finds an optimal sub-sample of plots for the given file id.
#' @param ruta_id, .
#' @param plots					A path to a .csv file or a data.frame of the plots.
#' @param auxiliary_data_dir	A path to the auxiliary data directory. 
#' @param number_of_candidates	(unsigned integer) The number of plot kandidates to supply to the outlier removal.
#' @param k_min					Exclude plots with hgv/p95 < k_min.
#' @param k_max					Exclude plots with hgv/p95 > k_max.
#' @param metadata_dest_dir		If no is.na metadata is stored here.
#' @return 						A data.frame containing a subset of the lines of the original plots table.
#' @author						Peder Axensten
#' @seealso 					\code{\link{pax.regression.package}}
#' @export
plot_sample_ruta <- function(
	ruta_id, 
	plots, 
	ruta_metadata, 
	number_of_candidates, 
	metadata_dest_dir	= NA
) {
	log_begin()
	log_item( "Ruta metadata file:     ", ruta_metadata )
	if( !file.exists( ruta_metadata ) ) 
		log_stop( "Error in plot_sample_ruta: no such ruta metadata file: ", ruta_metadata, "\n" )

	if( !is.na( metadata_dest_dir ) ) {
		metadata_dest_dir	<- file.path( metadata_dest_dir, "plot_sample_ruta" )
		dir.create( metadata_dest_dir, showWarnings=FALSE, recursive=TRUE )
	}

	# Remove suffix from ruta_id.
	ruta_id			<- file.name( basename( ruta_id ) )
	
	# Read the input data (if necessary) and check that it contains reequired comumns.
	message			<- "In plot_sample_ruta: "
	plots			<- read_csv_file( path_or_df=plots, message=paste0( "plots file, " ), 
							required_cols=required_plot_cols.plot_sample_ruta() )


	# Get the metadata for the specific ruta_id.
	ruta_metadata	<- read_csv_file( path_or_df=ruta_metadata, message=paste0( "ruta_metadata file, " ), 
							required_cols=c( "east", "north", "id", "leaveson", "scanner_type" ) )
	ruta			<- ( ruta_metadata$id == ruta_id )	# Check that we have a unique id. 
	if( sum( ruta ) < 1 )	log_stop( message, "No such ruta id: '", ruta_id, "'\n" )
	if( sum( ruta ) > 1 )	log_stop( message, "More than one such ruta id: '", ruta_id, "'\n" )
	ruta			<- c( ruta_metadata[ ruta, ] )		# A list of scalar values for that row in the ruta_metadata data.frame.

	log_item( "STILL: plot_sample_ruta" )
	log_item( "Total plots:            ", nrow( plots ) )


	keep			<-	(	!is.na( plots$p95_all_ge150cm )
						&	!is.na( plots$Hgv )
						)

	# Require minimum number of points.
	keep			<- keep & ( plots$count_all >= 20 )
	log_item( "Points >= 20:           ", sum( plots$count_all >= 20 ) )
	log_item( "Aggregated:             ", sum( keep ) )

	# Require minimum Hgv, 3 m. 
	minimum_hgv <- 3.0
	log_item( "!is.na( Hgv ):          ", sum( !is.na( plots$Hgv ) ) )
	log_item( "Aggregated:             ", sum( keep ) )
	keep			<- keep & ( plots$Hgv > minimum_hgv )
	log_item( "Hgv > ", minimum_hgv, ":                ", sum( plots$Hgv > minimum_hgv ) )
	log_item( "Aggregated:             ", sum( keep ) )

	# Require "no overstorey", if that colmn is available.
	if( !is.null(plots$Volume_overstorey) ) {
		keep		<- keep & !is.na( plots$Volume_overstorey )
		log_item( "!is.na( Volume_overstorey ): ", sum( !is.na( plots$Volume_overstorey ) ) )
		log_item( "Aggregated:               ", sum( keep ) )
		keep		<- keep & ( plots$Volume_overstorey == 0 )
		log_item( "Volume_overstorey == 0:   ", sum( plots$Volume_overstorey == 0 ) )
		log_item( "Aggregated:               ", sum( keep ) )
	}
	
	# Require same status as the ruta metadata for leaves/no-leaves.
	keep			<- keep & ( plots$leaveson == ruta$leaveson )
	log_item( "equal leaveson:         ", sum( plots$leaveson == ruta$leaveson ) )
	log_item( "Aggregated:             ", sum( keep ) )

	# Require the same scanner type as the ruta metadata for leaves/no-leaves.
	keep			<- keep & ( plots$scanner_type == ruta$scanner_type )
	log_item( "equaln scanner_type:    ", sum( plots$scanner_type == ruta$scanner_type ) )
	log_item( "Aggregated:             ", sum( keep ) )
	
	# Require plots to have a factor p95_all_ge150cm/hgv to be between two lines.
	m_min			<- -0.7		# was: -1.0
	k_min			<-  0.6		# was:  0.8
	m_max			<-  2.6		# was:  1.5
	k_max			<-  1.1		# was:  1.05
	keep			<- ( keep & ( ( m_min + k_min*plots$p95_all_ge150cm ) < plots$Hgv )
							  & ( ( m_max + k_max*plots$p95_all_ge150cm ) > plots$Hgv ) );


	# Get the plots that fulfill all the above requirements.
	if( !is.na( metadata_dest_dir ) ) {
		grDevices::pdf( file=file.path( metadata_dest_dir, "diagram-all.pdf" ), paper="a4" )
		graphics::plot( Hgv ~ p95_all_ge150cm, data=plots, main="All available plots", 
			xlim=c( 0,25 ), ylim=c( 0,25 ), xlab="p95_all_ge150cm", ylab="HGV" )
		graphics::lines( x=c( 0, 25 ), y=c( m_min, m_min+k_min*25 ) )
		graphics::lines( x=c( 0, 20 ), y=c( m_max, m_max+k_max*20 ) )
	}
	plots			<- plots[ keep, ]
	log_item( "plots kept:             ", nrow( plots ) )
	keep			<- NULL
	if( !is.na( metadata_dest_dir ) ) {
		grDevices::pdf( file=file.path( metadata_dest_dir, "diagram-filtered.pdf" ), paper="a4" )
		graphics::plot( Hgv ~ p95_all_ge150cm, data=plots, main="All available plots, filtered", 
			xlim=c( 0,25 ), ylim=c( 0,25 ), xlab="p95_all_ge150cm", ylab="HGV" )
		graphics::lines( x=c( 0, 25 ), y=c( m_min, m_min+k_min*25 ) )
		graphics::lines( x=c( 0, 20 ), y=c( m_max, m_max+k_max*20 ) )
	}

	
	# Let the basic badness value be the geographic distance between the ruta and each plot.
	# Using Sweref99 TM coordinates means that the unit of badness is meter. 
	plots$badness	<- sqrt( ( plots$east - ruta$east )^2 + ( plots$north - ruta$north )^2 )


	# Extract the "best" plots, the plots with the smallest badness values.
	minimum_number_of_candidates <- 50
	if( nrow( plots ) > number_of_candidates ) {
		best_plots		<- plots[ order( plots$badness )[ 1:number_of_candidates ], ]
	} else if( nrow( plots ) >= minimum_number_of_candidates ) {
		best_plots		<- plots[ order( plots$badness ), ]
	} else {
		log_stop( "Area '", ruta_id, "' has ", nrow( plots ), 
			" candidate plots -- to few to build a model. Minimum required is ", minimum_number_of_candidates, ".\n" )
	}
	log_item( "best_plots:             ", nrow( best_plots ) )
	

	# Metadata output
	if( !is.na( metadata_dest_dir ) ) {
		write_csv_file( df=plots,      dest_path=file.path( metadata_dest_dir, "all_plots_with_badness.csv" ) )
		write_csv_file( df=best_plots, dest_path=file.path( metadata_dest_dir, "choosen_plots_with_badness.csv" ) )

		grDevices::pdf( file=file.path( metadata_dest_dir, "diagram-final.pdf" ), paper="a4" )
		graphics::plot( Hgv ~ p95_all_ge150cm, data=best_plots, main="Plot candidates for regression", 
			xlim=c( 0,25 ), ylim=c( 0,25 ), xlab="p95_all_ge150cm", ylab="HGV" )
		graphics::lines( x=c( 0, 20 ), y=c( m_min, m_min+k_min*20 ) )
		graphics::lines( x=c( 0, 20 ), y=c( m_max, m_max+k_max*20 ) )
	}
	
	log_end()
	return( best_plots )
}
