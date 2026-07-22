//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#include <pax/types/point-stuff/box.hpp>
#include <pax/doctest.hpp>


namespace pax {

	constexpr auto bx			  = Box3d{ { 1., 2.5, 3. }, { 3., 2., 1. } };
	constexpr auto aligned_bx	  = Box3d{ { 0., 2.0, 0. }, { 4., 4., 4. } };
	constexpr auto idx			  = index( 3u, 5u, 7u );
	constexpr auto idx2			  = index( 2u, 4u, 6u );

	DOCTEST_TEST_CASE( "Box object" ) {
		DOCTEST_FAST_CHECK_EQ( bx,  				bx );
		DOCTEST_FAST_CHECK_NE( bx,  				aligned_bx );
		DOCTEST_FAST_CHECK_EQ( min( bx ),  			Point{ 1., 2.0, 1. } );
		DOCTEST_FAST_CHECK_EQ( max( bx ),  			Point{ 3., 2.5, 3. } );
		DOCTEST_FAST_CHECK_EQ( bx.sides(), 			Point{ 2., 0.5, 2. } );
		DOCTEST_FAST_CHECK_EQ( bx.aligned( 2 ), 	aligned_bx );
		
		DOCTEST_FAST_CHECK_EQ( 	bx.grow( Point{ 5., 2., 0. } ),
			 					Box3d{ { 1., 2., 0. }, { 5., 2.5, 3. } } );

		DOCTEST_FAST_CHECK_UNARY( !bx.strictly_inside( bx.min() ) );
		DOCTEST_FAST_CHECK_UNARY( !bx.strictly_inside( bx.max() ) );
		DOCTEST_FAST_CHECK_UNARY( !bx.strictly_inside( Point{ 2., 2.0, 2.  } ) );
		DOCTEST_FAST_CHECK_UNARY(  bx.strictly_inside( Point{ 2., 2.2, 2.  } ) );
		DOCTEST_FAST_CHECK_UNARY(  bx.inside_or_on   ( bx.min() ) );
		DOCTEST_FAST_CHECK_UNARY(  bx.inside_or_on   ( bx.max() ) );
		DOCTEST_FAST_CHECK_UNARY(  bx.in_range       ( bx.min() ) );
		DOCTEST_FAST_CHECK_UNARY( !bx.in_range       ( bx.max() ) );
	}
	DOCTEST_TEST_CASE( "Indexer object" ) {
		constexpr auto 		indexer = Indexer{ idx };
		DOCTEST_FAST_CHECK_EQ( idx,  						idx );
		DOCTEST_FAST_CHECK_EQ( indexer.elements(),  		105u );
		DOCTEST_FAST_CHECK_EQ( indexer.size(),  			idx );
		DOCTEST_FAST_CHECK_EQ( indexer.offsets(),  			index( 1u, 3u, 15u ) );
		DOCTEST_FAST_CHECK_UNARY(  indexer.valid_index( idx2 ) );
		DOCTEST_FAST_CHECK_UNARY( !indexer.valid_index( idx  ) );
		DOCTEST_FAST_CHECK_EQ( cols( indexer ),  			col( idx ) );
		DOCTEST_FAST_CHECK_EQ( rows( indexer ),  			row( idx ) );
		DOCTEST_FAST_CHECK_EQ( indexer[ idx2 ],  			104u );
		DOCTEST_FAST_CHECK_EQ( indexer[ 2u, 4u, 6u ],  		104u );
		DOCTEST_FAST_CHECK_EQ( indexer.at( idx2 ),  		104u );
		DOCTEST_FAST_CHECK_EQ( indexer.at( 2u, 4u, 6u ),  	104u );
	}
	DOCTEST_TEST_CASE( "Box_indexer object" ) {
		{	// 2d
			const Box_indexer								boxi{ Box2d{ { 1., 2.5 }, { 7., 5. } }, 2 };
			DOCTEST_FAST_CHECK_EQ( boxi,					Box2d { { 0., 2.0 }, { 8., 6. } } );
			DOCTEST_FAST_CHECK_EQ( boxi.resolution(), 		2 );
			DOCTEST_FAST_CHECK_EQ( boxi.size(), 			Index2d{ 4u, 2u } );
			DOCTEST_FAST_CHECK_EQ( boxi.offsets(), 			Index2d{ 1u, 4u } );
			DOCTEST_FAST_CHECK_EQ( boxi.elements(), 		8 );
			DOCTEST_FAST_CHECK_EQ( boxi.index( Point{ 0., 6. } ), 	0u );
			DOCTEST_FAST_CHECK_EQ( boxi.index( Point{ 8., 6. } ), 	3u );
			DOCTEST_FAST_CHECK_EQ( boxi.index( Point{ 0., 2. } ), 	4u );
			DOCTEST_FAST_CHECK_EQ( boxi.index( Point{ 8., 2. } ), 	7u );
			DOCTEST_FAST_CHECK_EQ( boxi.affine_vector(), 	std::array< double, 6 >{ 0., 2., 0., 6., 0., -2. } );
		}
		{	// 3d
			const Box_indexer								boxi{ Box3d{ { 1., 2.5, 0. }, { 7., 5., 3. } }, 2 };
			DOCTEST_FAST_CHECK_EQ( boxi,						  Box3d{ { 0., 2.0, 0. }, { 8., 6., 4. } } );
			DOCTEST_FAST_CHECK_EQ( boxi.resolution(), 		2 );
			DOCTEST_FAST_CHECK_EQ( boxi.size(), 			Index3d{ 4u, 2u, 2u } );
			DOCTEST_FAST_CHECK_EQ( boxi.offsets(), 			Index3d{ 1u, 4u, 8u } );
			DOCTEST_FAST_CHECK_EQ( boxi.elements(), 		16 );
			DOCTEST_FAST_CHECK_EQ( boxi.index( Point{ 0., 2.0, 0. } ), 	0u );
			DOCTEST_FAST_CHECK_EQ( boxi.index( Point{ 8., 2.0, 0. } ), 	3u );
			DOCTEST_FAST_CHECK_EQ( boxi.index( Point{ 0., 6.0, 0. } ), 	4u );
			DOCTEST_FAST_CHECK_EQ( boxi.index( Point{ 8., 6.0, 0. } ), 	7u );
			DOCTEST_FAST_CHECK_EQ( boxi.index( Point{ 0., 2.0, 4. } ), 	0u + 8u );
			DOCTEST_FAST_CHECK_EQ( boxi.index( Point{ 8., 2.0, 4. } ), 	3u + 8u );
			DOCTEST_FAST_CHECK_EQ( boxi.index( Point{ 0., 6.0, 4. } ), 	4u + 8u );
			DOCTEST_FAST_CHECK_EQ( boxi.index( Point{ 8., 6.0, 4. } ), 	7u + 8u );
		}
	}
	
}	// namespace pax
