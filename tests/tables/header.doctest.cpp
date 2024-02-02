//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/tables/header.hpp>

#include <pax/external/doctest/doctest-pax.hpp>


namespace pax {

	DOCTEST_TEST_CASE( "" ) { 
		DOCTEST_FAST_CHECK_EQ( 42, 42 );
	}

	namespace Header_ns {
		using T = Header< char >;
		
		static_assert(!std::has_virtual_destructor< T >{} );
		static_assert( std::is_nothrow_destructible< T >{} );
		static_assert(!std::is_trivially_destructible< T >{} );

		static_assert( std::is_default_constructible< T >{} );
		static_assert( std::is_nothrow_default_constructible< T >{} );
		static_assert(!std::is_trivially_default_constructible< T >{} );

		static_assert( std::is_copy_constructible< T >{} );
		static_assert(!std::is_nothrow_copy_constructible< T >{} );
		static_assert(!std::is_trivially_copy_constructible< T >{} );

		static_assert( std::is_copy_assignable< T >{} );
		static_assert(!std::is_nothrow_copy_assignable< T >{} );
		static_assert(!std::is_trivially_copy_assignable< T >{} );

		static_assert( std::is_move_constructible< T >{} );
		static_assert( std::is_nothrow_move_constructible< T >{} );
		static_assert(!std::is_trivially_move_constructible< T >{} );

		static_assert( std::is_move_assignable< T >{} );
		static_assert( std::is_nothrow_move_assignable< T >{} );
		static_assert(!std::is_trivially_move_assignable< T >{} );

		static_assert( std::is_swappable< T >{} );
		static_assert( std::is_nothrow_swappable< T >{} );
	}

}	// namespace pax
