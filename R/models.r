# encoding: utf-8


#' Create a modelling 'package', that includes all parts that might be needed at modelling.
#
#' This means that all relevant modelling paramewters may be defined at one place. 
as.Model	<-	function(
	formula,
	start	=	NULL, 
	postpr	=	NULL
) {
	result	<-	list(
		formula	=	formula,
		start	=	start,
		postpr	=	postpr
	)
	class( result )	<- "Model"
	return( result )
}


# Linear modelling.
LM				<-	function( formula, ... )	{	UseMethod( "LM", formula )											}
LM.default		<-	function( formula, ... )	{	return( rlm( formula, ... ) )										}
LM.Model		<-	function( formula, ... )	{	return( LM( formula=formula$formula, ... ) )						}


# Non-linear modelling.
NLS				<-	function( formula, ... )	{	UseMethod( "NLS", formula )											}
NLS.default		<-	function( formula, ... )	{	return( nls( formula, ... ) )										}
NLS.Model		<-	function( formula, ... )	{	return( NLS( formula=formula$formula, start=formula$start, ... ) )	}



# The "old" linear models. 
Basal_area		<-	as.Model( formula=as.formula( Basal_area	~           h80veg + sample_std_dev_gel ) )
Dgv				<-	as.Model( formula=as.formula( Dgv			~ p95_gel + h80veg ) )
Hgv				<-	as.Model( formula=as.formula( Hgv		   	~ p95_gel ) )
RootVol			<-	as.Model( formula=as.formula( RootVol		~ p95_gel + h80veg + sample_std_dev_gel ) )
RootBio			<-	as.Model( formula=as.formula( RootBio		~ p95_gel + h80veg ) )

# The "old" linear model, but for volume directly and not sqrt( volume ). 
Volume_sqA		<-	as.Model( 
	formula		=	as.formula( Volume		~ ( b0 + b1*p95_gel + b2*h80veg + b3*sample_std_dev_gel )^2 ),
	start		=	c( b0=2.74, b1=0.276, b2=0.751, b3=-0.423 )
)

# # Same as Volume_sqA, but also with a parameter for tree species.
# Volume_sqB		<-	as.Model(
# 	formula		=	as.formula( Volume		~ ( b0 + b1*p95_gel + b2*h80veg + b3*sample_std_dev_gel + b4*species_type )^2 ),
# 	start		=	c( b0=1.24, b1=0.190, b2=0.833, b3=-0.359, b4=0.0207 )
# )

# # Same as Volume_sqA, but also with a parameter for tree species.
# Volume_sqC		<-	as.Model(
# 	formula		=	as.formula( Volume		~ ( b0 + b1*p95_gel + b2*h80veg + b3*sample_std_dev_gel + b4*species_type )^b5 ),
# 	start		=	c( b0=1.11, b1=0.200, b2=0.884, b3=-0.378, b4=0.0219, b5=1.97 )
# )

# Volume_exp		<-	as.Model(
# 	formula		=	as.formula( Volume 		~ b0 + b1*exp( b2*( p95_gel * prop_1ret_gel_of_1ret ) + b3*species_type ) ),
# 	start		=	c( b0=-100.0, b1=100.0, b2=0.1, b3=0.0 )
# )

# Volume_pow		<-	as.Model(
# 	formula		=	as.formula( Volume		~ b0 * ( p95_gel * prop_1ret_gel_of_1ret )^( b1 + b2 * species_type ) ),
# 	start		=	c( b0=0.013, b1=1.35, b2=0.0 )
# )

Volume_pow0		<-	as.Model( 
	formula		=	as.formula( Volume		~ b0 * ( p95_gel * prop_1ret_gel_of_1ret )^( b1 ) ),
	start		=	c( b0=0.011680, b1=1.373750 )
)
