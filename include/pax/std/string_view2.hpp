//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "../concepts.hpp"
#include <string_view>


namespace pax {

	/// A more "strict" variant of basic_string_view.
	/// - All member functions are constexpr, const, and noexcept. 
	/// - Only operator[](), front(), and back() have UB, Undefined Behaviour, in certain cases.
	/// - More member functions to return a substring: first(), last(), not_first(), and not_last().
	/// - Many basic_string_view member functions stay the same. 
	/// - Some member functions have been generalized to functions that work with all contiguous data.
	template< typename Ch, typename Traits = std::char_traits< std::remove_const_t< Ch > > >
	class basic_string_view2 : public std::basic_string_view< Ch, Traits > {
		using base = std::basic_string_view< Ch, Traits >;
		
		using base::at;										// Might throw an exception.
		using base::remove_prefix, base::remove_suffix;		// Changes state.
		using base::compare;								// More readable to use operator== etc.

		// Use stuff from "algorithms.hpp" (or <algorithm> instead).
		using base::starts_with, base::ends_with, base::find, base::rfind, base::contains, base::npos;
		using base::find_first_of, base::find_last_of, base::find_first_not_of, base::find_last_not_of;

	public:
		/// All basic_string_view have dynamic extent. (From basic_string_view.)
		static constexpr std::size_t 	extent = -1;		// NOT -1u!!		

		using element_type			  = const base::value_type;

		/// Creates a basic_string_view2. (From basic_string_view.)
		using base::basic_string_view;
		
		constexpr basic_string_view2()											noexcept = default;
		constexpr basic_string_view2( const basic_string_view2 & )				noexcept = default;
		constexpr basic_string_view2( basic_string_view2 && )					noexcept = default;
		constexpr basic_string_view2 & operator=( const basic_string_view2 & )	noexcept = default;
		constexpr basic_string_view2 & operator=( basic_string_view2 && )		noexcept = default;

		/// Set size to 0 if `first_` is `nullptr.
		template< typename Itr >
		constexpr basic_string_view2(
			Itr				first_,
			std::size_t		last_
		) noexcept : base{ first_, ( first_ == nullptr ) ? 0u : last_ } {}

		/// Turns a Contiguous_elements into a span.
		template< Contiguous_elements A >
		constexpr basic_string_view2( A && a_ )			noexcept : base( std::data( a_ ), std::size( a_ ) ) {}

		/// Turns an array of characters into a dynamic basic_string_view2. Does not include a final '\0' character.
		constexpr basic_string_view2( Ch * const & c_ )	noexcept : base( c_, Traits::length( c_ ) ) {}

		/// Turns an array into a static basic_string_view2.
		template< std::size_t N >
		constexpr basic_string_view2( Ch( & c_ )[ N ] )	noexcept : base( c_, N ? N - !( *( c_ + N - 1 ) ) : 0 ) {}


		/// Returns`data() != nullptr. Zero size is ok as long as pointer is not nullpointer.
		[[nodiscard]] constexpr bool valid() 									const noexcept {
			return base::data() != nullptr;
		}

		/// Returns a view to the first `i_` characters.
		/// If `i_ >= size()` `*this` is returned. 
		[[nodiscard]] constexpr auto first( const std::size_t i_ = 1 )			const noexcept {
			return basic_string_view2( base::data(), std::min( i_, base::size() ) );
		}

		/// Returns a view to the last `i_` characters.
		/// If `i_ >= size()` `*this` is returned. 
		[[nodiscard]] constexpr auto last( const std::size_t i_ = 1 )			const noexcept {
			return ( i_ < base::size() ) ? basic_string_view2( base::end() - i_, i_ ) : *this;
		}

		/// Returns a view to the last `size() - i_` characters.
		/// If `i_ >= size()` an empty view is returned. 
		[[nodiscard]] constexpr auto not_first( const std::size_t i_ = 1 )		const noexcept {
			return ( i_ < base::size() )	? basic_string_view2( base::data() + i_, base::size() - i_ )
											: basic_string_view2( base::end(), 0 );
		}

		/// Returns a view to the first `size() - i_` characters.
		/// If `i_ >= size()` an empty view is returned. 
		[[nodiscard]] constexpr auto not_last( const std::size_t i_ = 1 )		const noexcept {
			return basic_string_view2( base::data(), ( i_ < base::size() ) ? base::size() - i_ : 0u );
		}

		/// Returns a view to `length` characters starting at position `offset`.
		///	- If `offset < 0`, then `offset + size()` is used (the offset is seen from the end), 
		/// - If `offset >= size()` an empty view is returned. 
		/// - If `offset + length >= size()` a truncated view is returned. 
		template< std::integral Int >
		[[nodiscard]] constexpr auto substr( 
			Int			 						offset, 
			const std::size_t 					length 
		) 	const noexcept {
			if constexpr( std::is_unsigned_v< Int > ) {
				offset	= std::min( offset, base::size() );
			} else {
				offset	= ( offset >= 0 )							? std::min( std::size_t( offset ), base::size() ) 
						: ( std::size_t( -offset ) < base::size() )	? base::size() - std::size_t( -offset )
						:											  std::size_t{};
			}
			return basic_string_view2( base::data() + offset, std::min( base::size() - offset, length ) );
		}

		/// Copy the string referenced to by `*this` to dest_. Returns `size().
		constexpr std::size_t copy( Ch *dest_ )									const noexcept {
			return base::copy( dest_, base::size() );
		}
	};

	using string_view2 = basic_string_view2< char >;

	template< typename It, typename EndOrSize >
	basic_string_view2( It, EndOrSize )
		-> basic_string_view2< std::remove_reference_t< std::iter_reference_t< It > > >;

	template< String S >
	basic_string_view2( S && )
		-> basic_string_view2< Value_type_t< S >, typename std::remove_cvref_t< S >::traits_type >;

	template< Character Ch, std::size_t N >
	basic_string_view2( Ch( & c_ )[ N ] )	-> basic_string_view2< std::remove_reference_t< Ch > >;

	template< Character Ch >
	basic_string_view2( Ch * const & c_ )	-> basic_string_view2< std::remove_reference_t< Ch > >;

	/// Stream the contents of sp_ to dest_ in the form "[i0, i1, ...]".
	template< typename Dest, typename Ch, typename Traits >
	constexpr Dest & operator<<(
		Dest								  & dest_,
		const basic_string_view2< Ch, Traits >	v_
	) {
		if( v_.size() )		dest_.write( v_.data(), v_.size() - !v_.back() );
		return dest_;
	}

}	// namespace pax

namespace std {
	/// Concatenate a string and a string view.
	template< typename Ch, typename Traits, typename Allocator >
	[[nodiscard]] constexpr auto operator+(
		const basic_string< remove_cv_t< Ch >, Traits, Allocator >	  & str_,
		const basic_string_view< Ch, Traits >							view_
	) {
		return basic_string< remove_cv_t< Ch >, Traits, Allocator >( str_ )+= view_;
	}

	/// Concatenate a string view and a string.
	template< typename Ch, typename Traits, typename Allocator >
	[[nodiscard]] constexpr auto operator+(
		const basic_string_view< Ch, Traits >							view_, 
		const basic_string< remove_cv_t< Ch >, Traits, Allocator >	  & str_
	) {
		return basic_string< remove_cv_t< Ch >, Traits, Allocator >( view_ )+= str_ ;
	}
}	// namespace std
