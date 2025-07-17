//	Copyright (c) Peder Axensten, all rights reserved.


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
		using value_type								  = std::remove_reference_t< T >;
		using tag_type									  = Tag;
		static constexpr bool is_strict					  = Strict;
		
		constexpr Tagged() 						   			noexcept : m_value{} {}
		constexpr Tagged( const Tagged & ) 				  = default;
		constexpr Tagged & operator=( const Tagged & )	  = default;
		constexpr Tagged( const value_type v_ )				noexcept : m_value{  v_ } {}
		constexpr Tagged & operator=( const value_type v_ )	noexcept { m_value = v_;			return *this;	 }

		/// Return m_value when specifically const-accessing. 
		constexpr value_type value()				const   noexcept						{	return m_value;	 }
		
		/// Return m_value when indirectly const-accessing. Set Strict to false to use it.
		constexpr operator value_type()				const   noexcept requires( !is_strict )	{ 	return m_value;	 }
	
	private:
		value_type m_value;
	};

	// Some shortcuts...
	template< typename Tag, bool Strict = true >	using Bool		= Tagged< bool,			 Tag, Strict >;
	template< typename Tag, bool Strict = true >	using Float		= Tagged< float,		 Tag, Strict >;
	template< typename Tag, bool Strict = true >	using Double	= Tagged< double,		 Tag, Strict >;
	template< typename Tag, bool Strict = true >	using Double2	= Tagged< long double,	 Tag, Strict >;
	template< typename Tag, bool Strict = true >	using Size		= Tagged< std::size_t,	 Tag, Strict >;
	template< typename Tag, bool Strict = true >	using Int64		= Tagged< std::int64_t,	 Tag, Strict >;
	template< typename Tag, bool Strict = true >	using Uint64	= Tagged< std::uint64_t, Tag, Strict >;


	namespace only_for_testing_purposes {
		static_assert(  Tagged< bool, struct test >().is_strict );	// Default Strict flag is "true".
		namespace strict {
			using Test	  = Tagged< bool, struct test, true >;
			static_assert(  Test( true  ).value() );
			// static_assert(  Test( true  ) );						// Unabled, as Strict == true.
			// static_assert( !Test( false ) );						// Unabled, as Strict == true.
			static_assert(  ( Test() = true  ).value() );
			// static_assert(  ( Test() = true  ) );				// Unabled, as Strict == true.
			// static_assert( !( Test() = false ) );				// Unabled, as Strict == true.
		}
		namespace not_strict {
			using Test	  = Tagged< bool, struct test, false >;
			static_assert(  Test( true  ).value() );
			static_assert(  Test( true  ) );
			static_assert( !Test( false ) );
			static_assert(  ( Test() = true  ).value() );
			static_assert(  ( Test() = true  ) );
			static_assert( !( Test() = false ) );
		}
	}	// namespace only_for_testing_purposes
}	// namespace pax
