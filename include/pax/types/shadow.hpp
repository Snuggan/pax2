//	Copyright (c) 2014-2026, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

/// shadow: a string_view- and span-like class.
/// Similar to span, but...
/// 1. It removes a number of ub or exceptions by defining logical non-ub return values, i.e. see first.
/// 2. It accepts also char-types, thus mimicking string_view.
/// 3. Modular design where contiguous_shell can be used with different base classes. 
/// 
/// Comment on 1.: By *always* returning clearly defined values from first, last, not_first, not_last, mid, find,
/// split etc., it is cleaner to identify and handle also out of bound values retrurned.
/// For example, `auto sh2 = sh.not_first( sh.size() + 5 )`will return a `sh2 == shadow( sh.end(), 0u )`. 
/// And `sh2.find( "abs" )` will return a copy of `sh2`, as the string `abc` was not found in [the empty] `sh2`.
/// 
///	The `shadow` consists of a common base class for most of the functionality and small classes implementing 
/// either static or dynamic size -- and also a variant usable for template parameter arrays. The main idea is 
/// to keep member functions basic, unmutable, and with a minimum of ub. The referenced values are mutable unless 
/// specifically declared const. 

#pragma once
#include <pax/concepts.hpp>		// traits:: stuff.
#include <algorithm>			// std::ranges::equal, std::lexicographical_compare_three_way, etc.
#include <iterator>				// std::reverse_iterator.
#include <assert.h>				// The classic assert macro.
#include <stdexcept>			// std::out_of_range.

namespace pax {
	constexpr std::size_t dynamic_extent = traits::dynamic_extent;

	template< typename T >
	[[nodiscard]] constexpr auto no_nullchar_end( T && t_ )				{	using std::end; return end( t_ );		}

	template< traits::character Char, std::size_t N >	requires( N > 0 )
	[[nodiscard]] constexpr Char const * no_nullchar_end( Char const ( & str_ )[ N ] )	{
		return str_ + N - !str_[ N - 1 ];
	}


	/// The core for span-like utilities of static or dynamic size. A minimal std::ranges::contiguous_range.
	template< typename T, std::size_t N = dynamic_extent >
	struct shadow_core {
		using element_type							  = T;
		using value_type							  = std::remove_cv_t< element_type >;
		using pointer								  = element_type *;
		using const_pointer							  = element_type const *;
		static constexpr std::size_t 					extent = N;

		[[nodiscard]] constexpr shadow_core() noexcept = default;
		[[nodiscard]] constexpr shadow_core( const shadow_core & ) noexcept = default;
		[[nodiscard]] constexpr shadow_core( pointer src_ ) noexcept : m_source{ src_ } {}

		template< typename U >
		[[nodiscard]] constexpr shadow_core( const U ( & str_ )[ N+1 ] ) noexcept 
			// Somewhat constructed due to disambiguation:
			requires( std::is_same_v< U, value_type > && traits::character< value_type > )
			: m_source{ str_ }		{	assert( !str_[ N ] && "Removing a non-zero character suffix!" );			}

		template< std::ranges::contiguous_range U >		requires( N == traits::extent_v< U > )
		[[nodiscard]] constexpr shadow_core( U & src_ ) noexcept : m_source{ std::ranges::data( src_ ) } {}

		constexpr shadow_core & operator=( const shadow_core & ) noexcept = default;
		[[nodiscard]] constexpr pointer data()			const noexcept	{	return m_source;						}
		[[nodiscard]] static constexpr std::size_t size()	  noexcept	{	return extent;							}
		[[nodiscard]] constexpr pointer begin()			const noexcept	{	return data();							}
		[[nodiscard]] constexpr pointer end()			const noexcept	{	return data() + size();					}

	private:
		pointer											m_source{ nullptr };
	};

	template< typename T >
	struct shadow_core< T, dynamic_extent > {
		using element_type							  = T;
		using value_type							  = std::remove_cv_t< element_type >;
		using pointer								  = element_type *;
		using const_pointer							  = element_type const *;
		static constexpr std::size_t 					extent = dynamic_extent;

		[[nodiscard]] constexpr shadow_core() noexcept = default;
		[[nodiscard]] constexpr shadow_core( const shadow_core & ) noexcept = default;
		[[nodiscard]] constexpr shadow_core( pointer src_, std::size_t sz ) noexcept 
			: m_source{ src_ }, m_size{ src_ ? sz : 0u } {}
		[[nodiscard]] constexpr shadow_core( pointer begin_, pointer end_ ) noexcept 
			: shadow_core{ begin_, std::size_t( end_ - begin_ ) } {}

		template< std::ranges::contiguous_range U >
		[[nodiscard]] constexpr shadow_core( U & src_ )		  noexcept 
			: m_source( std::ranges::data( src_ ) ), m_size{ std::ranges::size( src_ ) } {}

		constexpr shadow_core & operator=( const shadow_core & ) noexcept = default;
		[[nodiscard]] constexpr pointer data()			const noexcept	{	return m_source;						}
		[[nodiscard]] constexpr std::size_t size()		const noexcept	{	return m_size;							}
		[[nodiscard]] constexpr pointer begin()			const noexcept	{	return data();							}
		[[nodiscard]] constexpr pointer end()			const noexcept	{	return data() + size();					}

	private:
		pointer											m_source{ nullptr };
		std::size_t										m_size{};
	};

	template< traits::character T, std::size_t N >
	shadow_core( T const ( & )[ N ] ) -> shadow_core< T const, N-(N>0) >;

	template< std::ranges::contiguous_range Cont >
	shadow_core( Cont & )			  -> shadow_core< traits::element_type_t< Cont >, traits::extent_v< Cont > >;



	/// Implements most of the functionality of a class referensing contiguous elements.
	/// The interface is similar to that of std::string_view and std::span.
	/// Core must implement data(), size(), and define element_type and extent (possibly = dynamic_extent).
	// template< typename Core >
	template< 
		template< typename, std::size_t > typename Core, 
		typename X, 
		std::size_t N = dynamic_extent 
	>
	struct contiguous_shell : public Core< X, N > {
		using Base									  = Core< X, N >;
		using element_type							  = X;
		using value_type							  = std::remove_cv_t< element_type >;
		using pointer								  = element_type *;
		using const_pointer							  = element_type const *;
		using reference								  = element_type &;
		using const_reference						  = const element_type &;
		using iterator								  = pointer;
		using const_iterator						  = const_pointer;
		using reverse_iterator						  = std::reverse_iterator< iterator >;
		using const_reverse_iterator				  = std::reverse_iterator< const_iterator >;
		using size_type								  = std::size_t;
		using difference_type						  = std::ptrdiff_t;

		template< size_type N0 >
		using shadowN								  = pax::contiguous_shell< shadow_core, const element_type, N0 >;
		using shadow0								  = shadowN< dynamic_extent >;
		using pair									  = std::pair< shadow0, shadow0 >;

		static constexpr size_type						extent{ N };
		static constexpr bool							is_static{ extent != dynamic_extent };
		static constexpr bool							is_string{ traits::character< value_type > };

		using Base::Base, Base::data, Base::size;

		/// True iff the pointer is not the null pointer. 
		[[nodiscard]] constexpr bool valid()			const noexcept	{	return  data() != nullptr;				}

		/// True iff the size is zero. 
		[[nodiscard]] constexpr bool empty()			const noexcept	{	return !size();							}

		/// True iff the size is not zero. 
		[[nodiscard]] constexpr explicit operator bool()const noexcept	{	return  size();							}

		/// Iterators.
		[[nodiscard]] constexpr const_iterator begin()	const noexcept	{	return data();							}
		[[nodiscard]] constexpr const_iterator end()	const noexcept	{	return begin() + size();				}
		[[nodiscard]] constexpr iterator begin()			  noexcept	{	return data();							}
		[[nodiscard]] constexpr iterator end()				  noexcept	{	return begin() + size();				}
		[[nodiscard]] constexpr const_iterator cbegin()	const noexcept	{	return begin();							}
		[[nodiscard]] constexpr const_iterator cend()	const noexcept	{	return end();							}

		/// Reverse iterators.
		[[nodiscard]] constexpr auto rbegin()			const noexcept	{	return std::reverse_iterator( end() );	}
		[[nodiscard]] constexpr auto rend()				const noexcept	{	return std::reverse_iterator( begin() );}
		[[nodiscard]] constexpr auto crbegin()			const noexcept	{	return std::reverse_iterator( cend() );	}
		[[nodiscard]] constexpr auto crend()			const noexcept	{	return std::reverse_iterator( cbegin() );}

		/// Return a reference to the first element. Does operator[]( 0u ).
		[[nodiscard]] constexpr value_type front()		const noexcept	{	return operator[]( 0u );				}
		[[nodiscard]] constexpr reference front()			  noexcept	{	return operator[]( 0u );				}

		/// Return a reference to the last element. Does operator[]( size() - 1 ).
		[[nodiscard]] constexpr value_type back()		const noexcept	{	return operator[]( size() - 1 );		}
		[[nodiscard]] constexpr reference back()			  noexcept	{	return operator[]( size() - 1 );		}

		/// Return the i_:th element. Does assert( i_ < size() ).
		[[nodiscard]] constexpr value_type operator[]( const size_type i_ ) const noexcept {
			assert( i_ < size() && "Index out of range." );
			return data()[ i_ ];
		}
		[[nodiscard]] constexpr reference operator[]( const size_type i_ ) noexcept {
			assert( i_ < size() && "Index out of range." );
			return data()[ i_ ];
		}

		/// Return the i_:th element. Throws a std::out_of_range if i_ >= size().
		[[nodiscard]] constexpr reference at( const size_type i_ ) const {
			if( i_ >= size() )		throw std::out_of_range( "Index out of range." );
			return data()[ i_ ];
		}

		/// In strings a terminating \0 is ignored.
 		template< std::ranges::contiguous_range U >
		[[nodiscard]] constexpr bool operator==( U && u_ )					const noexcept	{
			return std::equal(	begin(), end(), std::ranges::begin( u_ ), no_nullchar_end( u_ ) );
		}

		/// In strings a terminating \0 is ignored.
 		template< std::ranges::contiguous_range U >
		[[nodiscard]] constexpr auto operator<=>( U && u_ )			const noexcept	{
			return std::lexicographical_compare_three_way(
								begin(), end(), std::ranges::begin( u_ ), no_nullchar_end( u_ ) );
		}

		/// Return `true`, iff `ptr_` references an element in this..
		[[nodiscard]] constexpr bool within( const const_pointer ptr_ )		const noexcept	{
			return ( cbegin() <= ptr_ ) && ( ptr_ < cend() );
		}

		/// Return a dynamic shadow of the first min(n_, size()) elements.
		[[nodiscard]] constexpr auto first( const size_type n_ )			const noexcept	{
			return shadow0{ data(), std::min( n_, size() ) };
		}

		/// Return a static shadow of the first min(N, extent) elements.
		/// Does assert( N <= size() && !is_static ).
		template< std::size_t N0 >		requires( N0 != dynamic_extent )
		[[nodiscard]] constexpr auto first() 								const noexcept	{
			if constexpr( !is_static )	assert( N0 <= size() && "first< N >() requires N <= size()." );
			return shadowN< std::min( N0, extent ) >( data() );
		}

		/// Return a dynamic shadow of the last size() - min(n_, size()) elements.
		[[nodiscard]] constexpr auto not_first( size_type n_ = 1 )			const noexcept	{
			n_ = std::min( n_, size() );
			return shadow0{ data() + n_, size() - n_ };
		}

		/// Return a static shadow of the last size() - min(N, extent) elements.
		/// Does assert( N <= size() && !is_static ).
		template< std::size_t N0 >		requires( is_static )
		[[nodiscard]] constexpr auto not_first() 							const noexcept	{
			return last< extent - std::min( N0, extent ) >();
		}

		/// Return a dynamic shadow of the last min(n_, size()) elements.
		[[nodiscard]] constexpr auto last( size_type n_ )					const noexcept	{
			n_ = std::min( n_, size() );
			return shadow0{ data() + size() - n_, n_ };
		}

		/// Return a static shadow of the first min(N, extent) elements.
		/// Does assert( N <= size() && !is_static ).
		template< std::size_t N0 >		requires( N0 != dynamic_extent )
		[[nodiscard]] constexpr auto last() 								const noexcept	{
			if constexpr( !is_static )	assert( N0 <= size() && "last< N0 >() requires N0 <= size()." );
			return shadowN< std::min( N0, extent ) >{ data() + size() - std::min( N0, size() ) };
		}

		/// Return a dynamic shadow of the first size() - min(n_, size()) elements.
		[[nodiscard]] constexpr auto not_last( const size_type n_ = 1 )		const noexcept	{
			return shadow0{ data(), size() - std::min( n_, size() ) };
		}

		/// Return a static shadow of the first size() - min(N, extent) elements.
		/// Does assert( N <= size() && !is_static ).
		template< std::size_t N0 >		requires( traits::has_extent< contiguous_shell > )
		[[nodiscard]] constexpr auto not_last() 							const noexcept 	{
			return first< extent - std::min( N0, size() ) >();
		}

		/// Return a dynamic shadow of the n_ elements starting with offs_, but restricted to the bounds of this.
		/// A negative offs_ is counted from the end.
		[[nodiscard]] constexpr auto mid( difference_type offs_, const size_type n_ )	const noexcept	{
			offs_ =	( offs_ >= 0 )	?		   std::min( size_type(  offs_ ), size() )
									: size() - std::min( size_type( -offs_ ), size() );
			return shadow0{ data() + offs_, std::min( size() - offs_, n_ ) };
		}

		/// Return a static shadow of the N elements starting with offs_, but restricted to the bounds of this.
		/// A negative offs_ is counted from the back. Does assert( offs_ + N <= size() ).
		template< std::size_t N0 >		requires( N0 != dynamic_extent && N0 <= extent )
		[[nodiscard]] constexpr auto mid( difference_type offs_ )			const noexcept	{
			offs_ =	( offs_ >= 0 )	?		   std::min( size_type(  offs_ ), size() )
									: size() - std::min( size_type( -offs_ ), size() );
			assert( offs_ + N0 <= size() && "mid< N0 >() requires offs_ + N0 <= size()." );
			return shadowN< N0 >{ data() + offs_ };
		}

		/// Return true iff u_ equals the first elements of this.
 		template< std::ranges::contiguous_range U >
		[[nodiscard]] constexpr bool starts_with( U && u_ )					const noexcept	{
			return std::ranges::starts_with( *this, shadow_core( u_ ) );
		}

		/// Return true iff u_ equals the last elements of this.
 		template< std::ranges::contiguous_range U >
		[[nodiscard]] constexpr bool ends_with( U && u_ )					const noexcept	{
			return std::ranges::ends_with( *this, shadow_core( u_ ) );
		}

		/// Return a shadow of where t_ is -- or a zereo-sized shadow located at end().
		[[nodiscard]] constexpr shadow0 find( const value_type t_ )			const noexcept	{
			const auto result = std::find( begin(), end(), t_ );
			return { result, result != end() };
		}

		/// Return a shadow of where u_ is -- or a zereo-sized shadow located at end().
 		template< std::ranges::contiguous_range U >
		[[nodiscard]] constexpr shadow0 find( U && u_ )						const noexcept	{
			const shadow_core		vw( u_ );
			const auto result = std::search( begin(), end(), vw.begin(), vw.end() );
			return { result, ( result == end() ) ? 0u : vw.size() };
		}

		/// Return a shadow of the first contigous range where all Test( value ) are true.
		/// If none is found, { end(), 0u } is returned.
		template< typename Test >	requires std::is_invocable_r_v< bool, Test, value_type >
		[[nodiscard]] constexpr shadow0 find( Test && test_ )				const noexcept	{
			const auto	b = std::ranges::find_if( *this, test_ );
			auto 		e = b;
			while( ( e != end() ) && test_( *e ) ) 	++e;
			return { b, e };
		}

		/// Find any of "\n\r", "\n", "\r\n", or "\r" and return a shadow reference to it.
		/// If none is found, { end(), 0u } is returned.
		[[nodiscard]] constexpr shadow0 find_linebreak()					const noexcept requires( is_string ) {
			value_type		previous{ ' ' };
			for( element_type & c : *this )	{
				[[unlikely]] if( previous == '\n' )	return { &c - 1, 1u + ( c == '\r' ) };
				[[unlikely]] if( previous == '\r' )	return { &c - 1, 1u + ( c == '\n' ) };
				previous = c;
			}
			return { end() - ( ( previous == '\n' ) || ( previous == '\r' ) ), end() };
		};

		/// Split this in two at offset t_ so that first.end() == second.begin() and first.size() == t_.
		[[nodiscard]] constexpr pair split( size_type mid_ )				const noexcept	{
			mid_		  = std::min( mid_, size() );
			return { { begin(), mid_ }, { begin() + mid_, end() } };
		}

		/// Split this in two: before and after gap_, but everything clamped to [begin(), end()].
		template< size_type I >
		[[nodiscard]] constexpr pair split( const shadowN< I > gap_ )		const noexcept	{
			return { { begin(), std::clamp( gap_.begin(), begin(), end() ) },
							  { std::clamp( gap_.end(),   begin(), end() ), end() } };
		}

		template< std::size_t I >						requires( is_static && ( I < extent ) )
		[[nodiscard]] friend element_type & get( contiguous_shell & sh_ )	noexcept {
			return *( sh_.data() + I );
		}

		template< std::size_t I >						requires( is_static && ( I < extent ) )
		[[nodiscard]] friend const element_type & get( const contiguous_shell & sh_ )	noexcept {
			return *( sh_.data() + I );
		}

		/// Stream the elements to out_.
		template< typename Out >
		friend Out & operator<<( Out & out_, const contiguous_shell & sh_ )	{
			if constexpr( is_string )	out_.write( sh_.data(), sh_.size() );
			else if( sh_.empty() )		out_ << "[]";
			else {
				out_ << '[' << sh_.front();
				for( const auto & item : sh_.not_first() )	out_ << ", " << item;
				out_ << ']';
			}
			return out_;
		}
 	};


	template< typename T, std::size_t N = dynamic_extent >
	struct shadow : public contiguous_shell< shadow_core, T, N > {
		using contiguous_shell< shadow_core, T, N >::contiguous_shell;
	};

	template< typename T >
	shadow( T *, std::size_t )		 -> shadow< T, dynamic_extent >;

	template< typename T >
	shadow( T *, T * )				 -> shadow< T, dynamic_extent >;

	template< typename T, std::size_t N >
	shadow( T( & )[ N ] )			 -> shadow< T, N >;

	template< traits::character T, std::size_t N >
	shadow( T const( & )[ N ] )		 -> shadow< T const, N-(N>0) >;

	template< std::ranges::contiguous_range Cont >
	shadow( Cont & )				 -> shadow< traits::element_type_t< Cont >, traits::extent_v< Cont > >;



	/// The core for a string_view-like type that can be used as a template argument.
	/// The alias litteral defines the actual type and the function litt creates it. 
	template< typename T, std::size_t N >			requires( N != dynamic_extent )
	struct litteral_core {
		using element_type						  = const T;
		using value_type						  = std::remove_cv_t< element_type >;
		static constexpr std::size_t	extent	  = N;

		[[nodiscard]] consteval litteral_core( T * ptr_ ) noexcept requires( !traits::character< T > ) {
			std::copy_n( ptr_, N, value );
		}

		[[nodiscard]] consteval litteral_core( T ( & ptr_ )[  N  ] ) noexcept 
			: litteral_core( ( T* )( ptr_ ) ) {}

		template< typename U >
		[[nodiscard]] constexpr litteral_core( const U ( & str_ )[ N+1 ] ) noexcept 
			requires( std::is_same_v< U, value_type > && traits::character< value_type > )
		{
			assert( !str_[ N ] && "Removing a non-zero character suffix!" );
			std::copy_n( str_, N, value );
		}

		template< typename... Args >	requires( ( true && ... && std::is_nothrow_convertible_v< Args, T > ) )
		[[nodiscard]] consteval litteral_core( Args && ...args_ ) noexcept
			: value{ value_type( std::forward< Args >( args_ ) )... } {}

		[[nodiscard]] constexpr element_type * data()	const noexcept			{	return value;					}
		[[nodiscard]] static consteval std::size_t size()	  noexcept			{	return N;						}

		value_type						value[ N ];
	};


	template< typename T, std::size_t N = dynamic_extent >
	struct litteral : public contiguous_shell< litteral_core, T, N > {
		using contiguous_shell< litteral_core, T, N >::contiguous_shell;
	};

	template< typename T >
	litteral( T *, std::size_t )	 -> litteral< T, dynamic_extent >;

	template< typename T >
	litteral( T *, T * )			 -> litteral< T, dynamic_extent >;

	template< typename T, std::size_t N >
	litteral( T( & )[ N ] )			 -> litteral< T, N >;

	template< traits::character T, std::size_t N >
	litteral( T const( & )[ N ] )	 -> litteral< T const, N-(N>0) >;

	template< std::ranges::contiguous_range Cont >
	litteral( Cont & )				 -> litteral< traits::element_type_t< Cont >, traits::extent_v< Cont > >;

	template< typename... Args >		requires( ( ... && !std::is_pointer_v< Args > ) )
	litteral( Args && ...args_ ) 	 -> litteral< const std::common_type_t< Args... >, sizeof ...( Args ) >;



	template< typename Tag, typename T >	struct Tagged;

	template< traits::character Char, size_t N >
	[[nodiscard]] constexpr Tagged< struct general, litteral< Char, N > > tagged( Char ( & str_ )[ N ] ) {
		return { str_ };
	}

	template< traits::character Char, size_t N >	requires( N > 0 )
	[[nodiscard]] constexpr Tagged< struct general, litteral< Char const, N-(N>0) > > tagged( Char const ( & str_ )[ N ] ) {
		assert( !str_[ N-1 ] && "Removing a non-zero character suffix!" );
		return { str_ };
	}

	template< typename Tag, traits::character Char, size_t N >
	[[nodiscard]] constexpr Tagged< Tag, litteral< Char, N > > tagged( Char ( & str_ )[ N ] ) {
		return { str_ };
	}

	template< typename Tag, traits::character Char, size_t N >	requires( N > 0 )
	[[nodiscard]] constexpr Tagged< Tag, litteral< Char const, N-(N>0) > > tagged( Char const ( & str_ )[ N ] ) {
		assert( !str_[ N-1 ] && "Removing a non-zero character suffix!" );
		return { str_ };
	}

}	// namespace pax
