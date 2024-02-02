# encoding: utf-8


#' Get a vector of strings, with the names of the required variables used in the outlier reduction.
#' @author						Peder Axensten
required_plot_cols.outlier_reduction <- function() {
	c(	"count_1ret", "count_1ret_ge150cm", "p30_all_ge150cm", "p95_all_ge150cm"	)
}


#' Implements outlier reductiuon on a forest plots table.
#' @param plots					A path to a .csv file or a data.frame of the plots.
#' @param outlier_removal_perc	(float in [0,95]) The percentage of plot candidates to remove in thge outlier removal.
#' @param metadata_dest_dir		If no is.na metadata is stored here.
#' @return 						A subset of plots.
#' @author						Peder Axensten
#' @seealso 					\code{\link{pax.regression.package}}
#' @export
plot_sample_outlier_reduction <- function(
	plots,
	outlier_removal_perc,
	metadata_dest_dir	= NA
) {
	log_begin()

	##### Läs taxenfil

	plots				<- read_csv_file( path_or_df=plots, message=paste0( "plots file, " ) )
	plots_orig			<- plots

	# Ta bort punkter i variabelnamn
	vecOriginalRownames	<- row.names( plots )
	names( plots )		<- gsub( "\\.", "", names( plots ) )
	check_available_columns( df=plots, required_cols=required_plot_cols.outlier_reduction(), message=paste0( "plots file, " ) )

	# (Provytor med överståndare rensas möjligen bort i plot_sample_ruta(  ).)

	# Skapa nya variabler
	plots$vegprop		<- plots$"count_1ret_ge150cm" / plots$"count_1ret"
	plots$h95veg   		<- plots$"p95_all_ge150cm" * plots$vegprop
	plots$RootVOL		<- plots$Volume^0.5
	
	
	##### Korsvalidering

	### Residualer tas fram separat för höjd och volym	

  	# Ta undan den i:te provytan och beräkna modellparametrar.
  	# Skatta sedan responsvariabeln för den i:te provytan Lagra residual (skattade y - verkligt).
	plots$diffH			<- NA
	plots$diffV			<- NA
	for( i in 1:nrow( plots ) ) {
		# Korsvalidering - Höjd
	  	reg				<- MASS::rlm( Hgv ~ p95_all_ge150cm, data=plots[ -i, ] )
	  	y_est			<- stats::predict( reg, newdata= plots[ i, ] )
	  	plots$diffH[ i ] <- y_est - plots$Hgv[ i ]

		# Korsvalidering Volym
	  	reg				<- MASS::rlm( RootVOL ~ p30_all_ge150cm + p95_all_ge150cm + h95veg, data=plots[ -i, ] )
	  	y_est			<- stats::predict( reg, newdata= plots[ i, ] )
	  	plots$diffV[ i ] <- y_est^2 - plots$ Volume[ i ]
	}

	
	
	### Identifiera outliers mha Mahalanobis-avstånd

	# Robust skattning av kovariansmatrisen för att minska påverkan av outliers
	Sx					<- robustbase::covMcd( plots[ ,c( "diffH", "diffV" ) ],
								cor=FALSE, alpha=1/2, nsamp=500, seed=NULL,
  								trace=FALSE, use.correction=TRUE, control=robustbase::rrcov.control() )
	plots$mahDist		<- sqrt( Sx$mah )

	# Tröskelvärde för när en provyta klassas som outlier
	Percentil			<- ( 100 - outlier_removal_perc )/100
	Outlier_threshold	<- stats::quantile( plots$mahDist, Percentil )
	vecRemaining		<- which( plots$mahDist < Outlier_threshold )

	# Vektor med radnummer för kvarvarande RT-ytor. 
	vecPlotsToKeep		<- match( row.names( plots[ vecRemaining, ] ), vecOriginalRownames )
	plots_orig			<- plots_orig[ vecPlotsToKeep, ]



	##### Produce metadata
	if( !is.na( metadata_dest_dir ) ) {
		write_csv_file( df=plots_orig, dest_path=file.path( metadata_dest_dir, "plot_sample_without_outliers.csv" ) )
		metadata_dest_dir	<- file.path( metadata_dest_dir, "plot_sample_outlier_reduction" )
		dir.create( metadata_dest_dir, showWarnings=FALSE, recursive=TRUE )

		grDevices::pdf( file=file.path( metadata_dest_dir, "diagram.pdf" ), paper="a4" )
		graphics::par( mfrow=c( 3,2 ) )

		graphics::plot( Hgv ~ p95_all_ge150cm, data=plots, main="Before", 
			xlim=c( 0,25 ), ylim=c( 0,25 ), xlab="p95_all_ge150cm", ylab="HGV" )
		graphics::plot( Hgv ~ p95_all_ge150cm, data=plots[ vecRemaining, ], main="After", 
			xlim=c( 0,25 ), ylim=c( 0,25 ), xlab="p95_all_ge150cm", ylab="HGV" )
	
		graphics::plot( RootVOL ~ p95_all_ge150cm, data=plots, main="Before", 
			xlim=c( 0,25 ), ylim=c( 0,25 ), xlab="p95_all_ge150cm", ylab="Volym^0.5" )
		graphics::plot( RootVOL ~ p95_all_ge150cm, data=plots[ vecRemaining, ], main="After", 
			xlim=c( 0,25 ), ylim=c( 0,25 ), xlab="p95_all_ge150cm", ylab="Volym^0.5" )
	
		graphics::plot( RootVOL ~ h95veg, data=plots, main="Before", 
			xlim=c( 0,30 ), ylim=c( 0,25 ), xlab="p95_all_ge150cm * Veg", ylab="Volym^0.5" )
		graphics::plot( RootVOL ~ h95veg, data=plots[ vecRemaining, ], main="After", 
			xlim=c( 0,30 ), ylim=c( 0,25 ), xlab="p95_all_ge150cm * veg", ylab="Volym^0.5" )
	
		grDevices::dev.off()
	}

	log_end()
	return( plots_orig )
}
