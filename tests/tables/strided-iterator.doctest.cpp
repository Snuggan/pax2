//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/tables/strided-iterator.hpp>

#include <pax/external/doctest/doctest-pax.hpp>


namespace pax {

	// operator++() does "jump" values, so it is not a contiguous_iterator.
	// Difference might not be divisible by stride() becouse of different strides or different offsets, 
	// so the difference operator is deleted. 
	// static_assert( std::random_access_iterator< Strided_iterator< int > > );
	static_assert( std::bidirectional_iterator< Strided_iterator< int > > );

	DOCTEST_TEST_CASE( "" ) { 
		constexpr int arr[ 30 ] = {
			 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 
			10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 
			20, 21, 22, 23, 24, 25, 26, 27, 28, 29
		};

		{	// Positive stride
			Strided_iterator	itr{ arr, 3 };
			DOCTEST_FAST_CHECK_EQ( *itr, 			 0 );

			DOCTEST_FAST_CHECK_EQ( *( ++itr   ), 	 3 );
			DOCTEST_FAST_CHECK_EQ( itr[ 2 ], 		 9 );
			DOCTEST_FAST_CHECK_EQ( *itr, 			 3 );
			DOCTEST_FAST_CHECK_EQ( *( itr+= 4 ), 	15 );
			DOCTEST_FAST_CHECK_EQ( *( --itr   ), 	12 );
			DOCTEST_FAST_CHECK_EQ( *( itr-= 2 ), 	 6 );

			DOCTEST_FAST_CHECK_EQ( *( itr++ ), 		 6 );
			DOCTEST_FAST_CHECK_EQ( *itr, 			 9 );
			DOCTEST_FAST_CHECK_EQ( *( itr-- ), 		 9 );
			DOCTEST_FAST_CHECK_EQ( *itr, 			 6 );
			itr+= 4;
			
			{
				auto	end{ itr + 2 };
				DOCTEST_FAST_CHECK_EQ( *itr, 		18 );
				DOCTEST_FAST_CHECK_EQ( *end, 		24 );
			}
			{
				auto	end{ itr - 2 };
				DOCTEST_FAST_CHECK_EQ( *itr, 		18 );
				DOCTEST_FAST_CHECK_EQ( *end, 		12 );
			}
		}
		{	// Negative stride
			Strided_iterator	itr{ arr + 28, -3 };
			DOCTEST_FAST_CHECK_EQ( *itr, 			28 );

			DOCTEST_FAST_CHECK_EQ( *( ++itr   ), 	25 );
			DOCTEST_FAST_CHECK_EQ( itr[ 2 ], 		19 );
			DOCTEST_FAST_CHECK_EQ( *itr, 			25 );
			DOCTEST_FAST_CHECK_EQ( *( itr+= 4 ), 	13 );
			DOCTEST_FAST_CHECK_EQ( *( --itr   ), 	16 );
			DOCTEST_FAST_CHECK_EQ( *( itr-= 2 ), 	22 );

			DOCTEST_FAST_CHECK_EQ( *( itr++ ), 		22 );
			DOCTEST_FAST_CHECK_EQ( *itr, 			19 );
			DOCTEST_FAST_CHECK_EQ( *( itr-- ), 		19 );
			DOCTEST_FAST_CHECK_EQ( *itr, 			22 );
			
			{
				auto	end{ itr + 2 };
				DOCTEST_FAST_CHECK_EQ( *itr, 		22 );
				DOCTEST_FAST_CHECK_EQ( *end, 		16 );
			}
			{
				auto	end{ itr - 2 };
				DOCTEST_FAST_CHECK_EQ( *itr, 		22 );
				DOCTEST_FAST_CHECK_EQ( *end, 		28 );
			}
		}
	}



	namespace Strided_iterator_ns {
		using T = Strided_iterator< char >;
		
		static_assert(!std::has_virtual_destructor< T >{} );
		static_assert( std::is_nothrow_destructible< T >{} );
		static_assert( std::is_trivially_destructible< T >{} );

		static_assert( std::is_default_constructible< T >{} );
		static_assert( std::is_nothrow_default_constructible< T >{} );
		static_assert(!std::is_trivially_default_constructible< T >{} );

		static_assert( std::is_copy_constructible< T >{} );
		static_assert( std::is_nothrow_copy_constructible< T >{} );
		static_assert( std::is_trivially_copy_constructible< T >{} );

		static_assert( std::is_copy_assignable< T >{} );
		static_assert( std::is_nothrow_copy_assignable< T >{} );
		static_assert( std::is_trivially_copy_assignable< T >{} );

		static_assert( std::is_move_constructible< T >{} );
		static_assert( std::is_nothrow_move_constructible< T >{} );
		static_assert( std::is_trivially_move_constructible< T >{} );

		static_assert( std::is_move_assignable< T >{} );
		static_assert( std::is_nothrow_move_assignable< T >{} );
		static_assert( std::is_trivially_move_assignable< T >{} );

		static_assert( std::is_swappable< T >{} );
		static_assert( std::is_nothrow_swappable< T >{} );
	}

}	// namespace pax
