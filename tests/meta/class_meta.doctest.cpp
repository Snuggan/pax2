//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#include <pax/meta/class-meta.hpp>
#include <pax/doctest.hpp>


namespace pax {
	
	struct A {};
	constexpr class_meta< A, int, unsigned, std::string > meta_A0( "A", "B", "C" );
	static_assert( meta_A0.Rank == 3 );
	static_assert( meta_A0.names()[ 1 ] == "B" );

}	// namespace pax
