//	Copyright (c) Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <type_traits>


namespace pax {

	/// Class to obfuscate type T. Use at function calls to differentiate arguments of the same type T.
	/// Tagged< T, ... > is no slower than simply using T. All member functions are constexpr. 
	/// Usage example: Tagged< bool, struct mybool, false >. 
	template< 
		typename T, 		///< Type to obfuscate.
		typename Tag, 		///< Tag that makes the class implementation unique.
		bool Strict = true	///< Member function operator T() only activated when Strict is false. 
	>
	struct Tagged {
		using value_type				  = std::remove_reference_t< T >;
		using tag_type					  = Tag;
		static constexpr bool is_strict	  = Strict;
		
		constexpr Tagged() 						   				noexcept : m_value{} 			{}
		constexpr Tagged( const Tagged & ) 					  = default;
		constexpr Tagged & operator=( const Tagged & )		  = default;
		constexpr Tagged( const value_type value_ )				noexcept : m_value{  value_ } 	{}
		constexpr Tagged & operator=( const value_type value_ )	noexcept { m_value = value_;		return *this;	 }

		/// Return m_value when specifically const-accessing a tagged value. 
		constexpr value_type value()					const   noexcept						{	return m_value;	 }
		
		/// Return m_value when indirectly const-accessing a tagged value. Set Strict to false to use it.
		constexpr operator value_type()					const   noexcept requires( !is_strict )	{ 	return m_value;	 }
	
	private:
		value_type m_value;
	};

	// Some shortcuts...
	template< typename Tag, bool Strict = true >	using Bool		= Tagged< bool,				Tag, Strict >;
	template< typename Tag, bool Strict = true >	using Float		= Tagged< float,			Tag, Strict >;
	template< typename Tag, bool Strict = true >	using Double	= Tagged< double,			Tag, Strict >;
	template< typename Tag, bool Strict = true >	using Double2	= Tagged< long double,		Tag, Strict >;
	template< typename Tag, bool Strict = true >	using Size		= Tagged< std::size_t,		Tag, Strict >;
	template< typename Tag, bool Strict = true >	using Int64		= Tagged< std::int64_t,		Tag, Strict >;
	template< typename Tag, bool Strict = true >	using Uint64	= Tagged< std::uint64_t,	Tag, Strict >;


	// Floating point types introduced by C++23: https://en.cppreference.com/w/cpp/types/floating-point.html
#if __STDCPP_FLOAT16_T__
	template< typename Tag, bool Strict = true >	using Float16	= Tagged< std::float16_t,	Tag, Strict >;
#endif
#if __STDCPP_BFLOAT16_T__
	template< typename Tag, bool Strict = true >	using Bfloat16	= Tagged< std::bfloat16_t,	Tag, Strict >;
#endif
#if __STDCPP_FLOAT32_T__
	template< typename Tag, bool Strict = true >	using Float32	= Tagged< std::float32_t,	Tag, Strict >;
#endif
#if __STDCPP_FLOAT64_T__
	template< typename Tag, bool Strict = true >	using Float64	= Tagged< std::float64_t,	Tag, Strict >;
#endif
#if __STDCPP_FLOAT128_T__
	template< typename Tag, bool Strict = true >	using Float128	= Tagged< std::float128_t,	Tag, Strict >;
#endif



	namespace only_for_testing_purposes {
		static_assert(  Tagged< bool, struct test >().is_strict );	// Default Strict flag is "true".
		namespace strict {
			using Test	  = Tagged< bool, struct test, true >;
			static_assert(  Test( true  ).value() == true );
			// static_assert(  Test( true  ) );						// Unabled by Strict == true.
			// static_assert( !Test( false ) );						// Unabled by Strict == true.
			// static_assert(  ( Test() = true  ) );				// Unabled by Strict == true.
			// static_assert( !( Test() = false ) );				// Unabled by Strict == true.
		}
		namespace not_strict {
			using Test	  = Tagged< bool, struct test, false >;
			static_assert(  Test( true  ).value() == true );
			static_assert(  Test( true  ) );
			static_assert( !Test( false ) );
			static_assert(  ( Test() = true  ) );
			static_assert( !( Test() = false ) );
		}
	}	// namespace only_for_testing_purposes
}	// namespace pax
