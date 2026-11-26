//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

#include <pax/math/polynomial.hpp>
#include <pax/doctest.hpp>


namespace pax::test {
	
	template< typename Algo, pax::poly::floating F, std::size_t N >
	bool check_output(
		const Polynomial< Algo, F, N >	  & p_,
		const auto							str_
	) {
		if( std::format( "{}", p_ ) != str_ ) {
			std::cerr 	<< "Error with polynomial output: \"" << std::format( "{}", p_ ) << "\" shoud be \"" << str_ << "\"\n";
			return false;
		}
		return true;
	}

	template< typename Algo, poly::floating F, std::size_t N >
	void out( const Polynomial< Algo, F, N > & p_ ) {
		std::cerr << p_ << ( Algo::odd_coeffs_only ? ", odd" : Algo::even_coeffs_only ? ", even" : ", odd+even" ) << '\n';
	}




	DOCTEST_TEST_CASE( "polynomial output" ) {
		DOCTEST_FAST_CHECK_EQ( 0, 0
			+	!check_output( pax::polynull<>,							"0" )
			+	!check_output( polynomial( 0. ),						"0" )
			+	!check_output( polynomial( 1. ),						"1" )
			+	!check_output( polynomial( 0., 1 ),						"x" )
			+	!check_output( polynomial( 0., 1, 0 ),					"x" )
			+	!check_output( polynomial( 0., 1, 0, 3 ),				"x + 3x^3" )
			+	!check_output( polynomial( 1., 0 ),						"1" )
			+	!check_output( polynomial(-1., 2, 3, -4, 5, 1, -1 ),	"-1 + 2x + 3x^2 - 4x^3 + 5x^4 + x^5 - x^6" )
		);
	}

}	// namespace pax
