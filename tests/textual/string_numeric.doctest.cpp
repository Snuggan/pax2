//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/textual/string_meta.hpp>


namespace pax {
	
	static_assert(!String_numeric()					.is_determined() );
	static_assert( String_numeric( "" )				.is_determined() );
	static_assert( String_numeric( "" )				.is_nonnumeric() );
	static_assert( String_numeric( "+" )			.is_nonnumeric() );
	static_assert( String_numeric( "-" )			.is_nonnumeric() );
	static_assert( String_numeric( "123" )			.is_unsigned_integer() );
	static_assert( String_numeric( "+123" )			.is_unsigned_integer() );
	static_assert( String_numeric( "-123" )			.is_integer() );
	static_assert( String_numeric( "." )			.is_nonnumeric() );
	static_assert( String_numeric( "+." )			.is_nonnumeric() );
	static_assert( String_numeric( "-." )			.is_nonnumeric() );
	static_assert( String_numeric( "0." )			.is_floating_point() );
	static_assert( String_numeric( "+0." )			.is_floating_point() );
	static_assert( String_numeric( "-0." )			.is_floating_point() );
	static_assert( String_numeric( ".9" )			.is_floating_point() );
	static_assert( String_numeric( "+.9" )			.is_floating_point() );
	static_assert( String_numeric( "-.9" )			.is_floating_point() );
	static_assert( String_numeric( "123.9" )		.is_floating_point() );
	static_assert( String_numeric( "+123.9" )		.is_floating_point() );
	static_assert( String_numeric( "-123.9" )		.is_floating_point() );
	static_assert( String_numeric( "123.9e" )		.is_nonnumeric() );
	static_assert( String_numeric( "+123.9e+" )		.is_nonnumeric() );
	static_assert( String_numeric( "-123.9e-" )		.is_nonnumeric() );
	static_assert( String_numeric( "123.9e8" )		.is_floating_point() );
	static_assert( String_numeric( "+123.9e+8" )	.is_floating_point() );
	static_assert( String_numeric( "-123.9e-8" )	.is_floating_point() );
	static_assert(!String_numeric( "-123.9e-8" )	.is_floating_point_xtra() );

	static_assert( String_numeric( "inf" )			.is_floating_point() );
	static_assert( String_numeric( "+inf" )			.is_floating_point() );
	static_assert( String_numeric( "-inf" )			.is_floating_point() );
	static_assert( String_numeric( "INF" )			.is_floating_point() );
	static_assert( String_numeric( "+INF" )			.is_floating_point() );
	static_assert( String_numeric( "-INF" )			.is_floating_point() );
	static_assert( String_numeric( "INFINITY" )		.is_floating_point() );
	static_assert( String_numeric( "+INFINITY" )	.is_floating_point() );
	static_assert( String_numeric( "-INFINITY" )	.is_floating_point() );
	static_assert( String_numeric( "inf" )			.is_floating_point_xtra() );
	static_assert( String_numeric( "+inf" )			.is_floating_point_xtra() );
	static_assert( String_numeric( "-inf" )			.is_floating_point_xtra() );

	static_assert( String_numeric( "nan" )			.is_floating_point() );
	static_assert( String_numeric( "NaN" )			.is_floating_point() );
	static_assert( String_numeric( "NAN" )			.is_floating_point() );
	static_assert(!String_numeric( "nan " )			.is_floating_point() );
	static_assert(!String_numeric( "+nan" )			.is_floating_point() );
	static_assert(!String_numeric( "-nan" )			.is_floating_point() );
	static_assert( String_numeric( "nan" )			.is_floating_point_xtra() );
	static_assert(!String_numeric( "+nan" )			.is_floating_point_xtra() );
	static_assert(!String_numeric( "-nan" )			.is_floating_point_xtra() );

	static_assert( String_numeric( "123.9e8x" )		.is_nonnumeric() );
	static_assert( String_numeric( "+123.9e+8x" )	.is_nonnumeric() );
	static_assert( String_numeric( "-123.9e-8x" )	.is_nonnumeric() );

	static_assert( String_numeric( "-123.9e-8" )	.view() == "floating point" );
	static_assert( String_numeric( "-123.9e-8x" )	.view() == "textual" );

	static_assert( ( String_numeric() + String_numeric( "1" ) )			.is_integer() );
	static_assert( ( String_numeric( "1" ) + String_numeric( ".9" ) )	.is_numeric() );

}	// namespace pax
