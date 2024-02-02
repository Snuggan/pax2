//	Copyright (c) 2014-2021, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/pdal/utilities/bbox_indexer.hpp>
#include <pax/external/doctest/doctest-pax.hpp>


namespace pax {
	
	DOCTEST_TEST_CASE( "Bbox_indexer" ) {

		// BOX2D(double minx, double miny, double maxx, double maxy) :
		// 	minx(minx), maxx(maxx), miny(miny), maxy(maxy) {}

		// Check the constructor:
		const Bbox_indexer	   box{ Box2( 0.3, 4.01, 6.9, 15.0001 ), 1.0 };
		DOCTEST_FAST_CHECK_EQ( box.minx(),		 	    0 );
		DOCTEST_FAST_CHECK_EQ( box.maxx(),		 	    7 );
		DOCTEST_FAST_CHECK_EQ( box.miny(),		 	    4 );
		DOCTEST_FAST_CHECK_EQ( box.maxy(),		 	   16 );
		DOCTEST_FAST_CHECK_EQ( box.rows(),		 	   12u );
		DOCTEST_FAST_CHECK_EQ( box.cols(), 		 	    7u );
		DOCTEST_FAST_CHECK_EQ( box.pixels(), 		   84u );
		DOCTEST_FAST_CHECK_EQ( to_string( box ), 	    "{E[0, 7], N[4, 16], res 1}" );
		DOCTEST_CHECK_NOTHROW(   Bbox_indexer( Box2( 0.0, 4.0, 0.0, 16.0 ), 1.0 ) );
		DOCTEST_FAST_CHECK_EQ(   Bbox_indexer( Box2( 0.0, 4.0, 0.0, 16.0 ), 1.0 ).pixels(), 12u );
		DOCTEST_CHECK_NOTHROW(   Bbox_indexer( Box2( 0.0, 4.0, 7.0,  4.0 ), 1.0 ) );
		DOCTEST_FAST_CHECK_EQ(   Bbox_indexer( Box2( 0.0, 4.0, 7.0,  4.0 ), 1.0 ).pixels(),  7u );
		DOCTEST_CHECK_THROWS_AS( Bbox_indexer( Box2( 1.0, 4.0, 0.0, 16.0 ), 1.0 ), std::runtime_error );
		DOCTEST_CHECK_THROWS_AS( Bbox_indexer( Box2( 0.0, 5.0, 7.0,  4.0 ), 1.0 ), std::runtime_error );
		DOCTEST_CHECK_THROWS_AS( Bbox_indexer( Box2( 0.0, 4.0, 7.0, 16.0 ), 0.0 ), std::runtime_error );

		// Check affine vector:
		const auto			   aff{ box.affine_vector() };
		DOCTEST_FAST_CHECK_EQ( aff[ 0 ],			    box.minx() );
		DOCTEST_FAST_CHECK_EQ( aff[ 1 ],			    box.resolution() );
		DOCTEST_FAST_CHECK_EQ( aff[ 2 ],			    0.0 );
		DOCTEST_FAST_CHECK_EQ( aff[ 3 ],			    box.maxy() );
		DOCTEST_FAST_CHECK_EQ( aff[ 4 ],			    0.0 );
		DOCTEST_FAST_CHECK_EQ( aff[ 5 ],			   -box.resolution() );

		// Check belong_to:
		DOCTEST_FAST_CHECK_UNARY(  box.contains( 0.0,  4.0 ) );
		DOCTEST_FAST_CHECK_UNARY(  box.contains( 0.0, 16.0 ) );
		DOCTEST_FAST_CHECK_UNARY(  box.contains( 7.0, 16.0 ) );
		DOCTEST_FAST_CHECK_UNARY(  box.contains( 7.0,  4.0 ) );
		DOCTEST_FAST_CHECK_UNARY( !box.contains( nudge_down( 0.0 ),  5.0 ) );
		DOCTEST_FAST_CHECK_UNARY( !box.contains( nudge_up  ( 7.0 ),  5.0 ) );
		DOCTEST_FAST_CHECK_UNARY( !box.contains( 2.0,  nudge_down(  4.0 ) ) );
		DOCTEST_FAST_CHECK_UNARY( !box.contains( 2.0,  nudge_up  ( 16.0 ) ) );

		// Check the row calculation:
		DOCTEST_FAST_CHECK_EQ( box.row( box.miny() ),	box.rows() - 1 );
		DOCTEST_FAST_CHECK_EQ( box.row( 12.5 ),			3u );
		DOCTEST_FAST_CHECK_EQ( box.row( box.maxy() ),	0u );
		DOCTEST_CHECK_THROWS_AS( box.row( nudge_down( box.miny() ) ), std::runtime_error );
		DOCTEST_CHECK_THROWS_AS( box.row( nudge_up  ( box.maxy() ) ), std::runtime_error );

		// Check the col calculation:
		DOCTEST_FAST_CHECK_EQ( box.col( box.minx() ),	0u );
		DOCTEST_FAST_CHECK_EQ( box.col( 2.5 ),			2u );
		DOCTEST_FAST_CHECK_EQ( box.col( box.maxx() ),	box.cols() - 1 );
		DOCTEST_CHECK_THROWS_AS( box.col( nudge_down( box.minx() ) ), std::runtime_error );
		DOCTEST_CHECK_THROWS_AS( box.col( nudge_up  ( box.maxx() ) ), std::runtime_error );

		// Check the index calculation:
		DOCTEST_FAST_CHECK_EQ( box.index( box.minx(), box.miny() ), box.pixels() - box.cols() );
		DOCTEST_FAST_CHECK_EQ( box.index( box.minx(), box.maxy() ), 0 );
		DOCTEST_FAST_CHECK_EQ( box.index( box.maxx(), box.miny() ), box.pixels() - 1 );
		DOCTEST_FAST_CHECK_EQ( box.index( box.maxx(), box.maxy() ), box.cols() - 1 );

	}

}	// namespace pax