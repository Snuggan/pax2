#pragma once

#include <vector>
#include <string>


namespace pax { 
	
	using Vector = std::vector< int >;
	template< typename T > struct Serialize;


	struct Small1 {	int m_int;	};
	template<>
	struct Serialize< Small1 > {
		template< typename J >
		static void assign( J & j_, const Small1 & s_ ) noexcept	{	j_ = { { "small1", s_.m_int } };						}

		template< typename J >
		static auto as( const J & j_ )								{	return Small1{ j_.template as< Small1 >( "small1" ) };	}
	};

	struct Small2 {	int m_int;	};
	template<>
	struct Serialize< Small2 > {
		template< typename J >
		static void assign( J & j_, const Small2 & s_ ) noexcept	{	j_ = { { "small2", s_.m_int } };						}

		template< typename J >
		static auto as( const J & j_ )								{	return Small2{ j_.template as< Small2 >( "small2" ) };	}
	};

	struct Small3 {	int m_int;	};
	template<>
	struct Serialize< Small3 > {
		template< typename J >
		static void assign( J & j_, const Small3 & s_ ) noexcept	{	j_ = { { "small3", s_.m_int } };						}

		template< typename J >
		static auto as( const J & j_ )								{	return Small3{ j_.template as< Small3 >( "small3" ) };	}
	};

	using Smalls = std::tuple< Small1, Small2, Small3 >;



	struct Test {
		bool					m_bool;
		int						m_int;
		unsigned				m_unsigned;
		double					m_double;
		std::string				m_string;
		Vector					m_vector;
	};
	
	template<> struct Serialize< Test > {
		template< typename J >
		static void assign(
			J				  & j_, 
			const Test		  & test_ 
		) noexcept {
			j_ = {
				{ "bool",		test_.m_bool 		},
				{ "int",		test_.m_int 		},
				{ "unsigned",	test_.m_unsigned 	},
				{ "double",		test_.m_double 		},
				{ "string",		test_.m_string 		},
				{ "vector",		test_.m_vector 		}
			};
		}

		template< typename J >
		static auto as( const J & j_ ) {
			return Test {
				j_.at( "bool"		).template get< bool 		>(),
				j_.at( "int"		).template get< int 		>(),
				j_.at( "unsigned"	).template get< unsigned 	>(),
				j_.at( "double"		).template get< double 		>(),
				j_.at( "string"		).template get< std::string	>(),
				j_.at( "vector"		).template get< Vector 		>()
			};
		}
	};




	struct Test2 {	Test	m_test1, m_test2;	};

	template<>
	struct Serialize< Test2 > {
		template< typename J >
		static void assign( J & j_, const Test2 & test_ ) noexcept 
		{	j_ = { { "test1", test_.m_test1 }, { "test2", test_.m_test2 } };						}

		template< typename J >
		static auto as( const J & j_ ) 
		{	return Test2 { j_.template as< Test >( "test1" ), j_.template as< Test >( "test2" ) };	}
	};

}	// namespace pax
