//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/textual/string_meta.hpp>


namespace pax {
	
	static_assert(!Strtype()				.is_determined() );
	static_assert( Strtype( "" )			.is_determined() );
	static_assert( Strtype( "" )			.is_nonnumeric() );
	static_assert( Strtype( "+" )			.is_nonnumeric() );
	static_assert( Strtype( "-" )			.is_nonnumeric() );
	static_assert( Strtype( "123" )			.is_unsigned_integer() );
	static_assert( Strtype( "+123" )		.is_unsigned_integer() );
	static_assert( Strtype( "-123" )		.is_integer() );
	static_assert( Strtype( "." )			.is_nonnumeric() );
	static_assert( Strtype( "+." )			.is_nonnumeric() );
	static_assert( Strtype( "-." )			.is_nonnumeric() );
	static_assert( Strtype( "0." )			.is_floating_point() );
	static_assert( Strtype( "+0." )			.is_floating_point() );
	static_assert( Strtype( "-0." )			.is_floating_point() );
	static_assert( Strtype( "0.a" )			.is_nonnumeric() );
	static_assert( Strtype( "+0.a" )		.is_nonnumeric() );
	static_assert( Strtype( "-0.a" )		.is_nonnumeric() );
	static_assert( Strtype( ".9" )			.is_floating_point() );
	static_assert( Strtype( "+.9" )			.is_floating_point() );
	static_assert( Strtype( "-.9" )			.is_floating_point() );
	static_assert( Strtype( ".a" )			.is_nonnumeric() );
	static_assert( Strtype( "+.a" )			.is_nonnumeric() );
	static_assert( Strtype( "-.a" )			.is_nonnumeric() );
	static_assert( Strtype( "123.9" )		.is_floating_point() );
	static_assert( Strtype( "+123.9" )		.is_floating_point() );
	static_assert( Strtype( "-123.9" )		.is_floating_point() );
	static_assert( Strtype( "123.9e" )		.is_nonnumeric() );
	static_assert( Strtype( "+123.9e+" )	.is_nonnumeric() );
	static_assert( Strtype( "-123.9e-" )	.is_nonnumeric() );
	static_assert( Strtype( "123.9e8" )		.is_floating_point() );
	static_assert( Strtype( "+123.9e+8" )	.is_floating_point() );
	static_assert( Strtype( "-123.9e-8" )	.is_floating_point() );
	static_assert(!Strtype( "-123.9e-8" )	.is_floating_point_xtra() );

	static_assert( Strtype( "inf" )			.is_floating_point() );
	static_assert( Strtype( "+inf" )		.is_floating_point() );
	static_assert( Strtype( "-inf" )		.is_floating_point() );
	static_assert( Strtype( "INF" )			.is_floating_point() );
	static_assert( Strtype( "+INF" )		.is_floating_point() );
	static_assert( Strtype( "-INF" )		.is_floating_point() );
	static_assert( Strtype( "INFINITY" )	.is_floating_point() );
	static_assert( Strtype( "+INFINITY" )	.is_floating_point() );
	static_assert( Strtype( "-INFINITY" )	.is_floating_point() );
	static_assert( Strtype( "inf" )			.is_floating_point_xtra() );
	static_assert( Strtype( "+inf" )		.is_floating_point_xtra() );
	static_assert( Strtype( "-inf" )		.is_floating_point_xtra() );

	static_assert( Strtype( "nan" )			.is_floating_point() );
	static_assert( Strtype( "NaN" )			.is_floating_point() );
	static_assert( Strtype( "NAN" )			.is_floating_point() );
	static_assert(!Strtype( "nan " )		.is_floating_point() );
	static_assert(!Strtype( "+nan" )		.is_floating_point() );
	static_assert(!Strtype( "-nan" )		.is_floating_point() );
	static_assert( Strtype( "nan" )			.is_floating_point_xtra() );
	static_assert(!Strtype( "+nan" )		.is_floating_point_xtra() );
	static_assert(!Strtype( "-nan" )		.is_floating_point_xtra() );

	static_assert( Strtype( "123.9e8x" )	.is_nonnumeric() );
	static_assert( Strtype( "+123.9e+8x" )	.is_nonnumeric() );
	static_assert( Strtype( "-123.9e-8x" )	.is_nonnumeric() );

	static_assert( Strtype( "-123.9e-8" )	.view() == "floating point" );
	static_assert( Strtype( "-123.9e-8x" )	.view() == "textual" );

	static_assert( ( Strtype() + Strtype( "1" ) )		.is_integer() );
	static_assert( ( Strtype( "1" ) + Strtype( ".9" ) )	.is_numeric() );

}	// namespace pax
