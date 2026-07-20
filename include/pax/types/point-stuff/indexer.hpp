//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "point.hpp"


namespace pax {
	
	using std::get, std::array, std::span;


	/// Let the specialisation of unsigned always have zero as minimum. Useful for indeces.
	template< uinteger U, std::size_t N >						requires( N != std::dynamic_extent )
	class Indexer {
		Point< U, N >											m_max{}, m_offsets{};
		static constexpr Point< U, N >							m_min{};
		using 										Span	  = span< U, N >;
		
		static constexpr Point< U, N > do_offsets( Span pt_ )	noexcept	{
			U						product{};
			static constexpr auto	f = [ & product ]( U t_ )	{	return product *= t_;	};
			auto [ ... t, tn ] = pt_;
			return { 1u, f( t ) ... };
		}
		
	public:
		static constexpr std::size_t 				rank	  = N;
		using 										Point	  = array< U, N >;
		using Point::value_type;

		constexpr Indexer()									  = default;
		constexpr Indexer( const Indexer & )				  = default;
		constexpr Indexer( Indexer && )						  = default;
		constexpr Indexer & operator=( const Indexer & )	  = default;
		constexpr Indexer & operator=( Indexer && )			  = default;
		constexpr Indexer( Span size_ )  noexcept : m_max( size_ ), m_offsets{ do_offsets( size_ ) } {}

		constexpr Indexer grow( Span pt_ )						const noexcept	{	return { max( m_max, pt_ ) };			}

		constexpr const Point & min()							const noexcept	{	return m_min;							}
		constexpr const Point & max()							const noexcept	{	return m_max;							}
		constexpr const Point & offsets()						const noexcept	{	return m_offsets;						}

		constexpr bool strictly_inside( Span pt_ )				const noexcept	{	return all_lt( pt_, max() );			}
		constexpr bool inside_or_on( Span pt_ )					const noexcept	{	return all_le( pt_, max() );			}
		constexpr bool in_range( Span pt_ )						const noexcept	{	return strictly_inside( pt_ );			}
		
		/// Calculate an index into a vector for the index represented by pt_.
		template< uinteger ...U2 >								requires( sizeof...( U2 ) == N )
		constexpr U operator[]( U2 && ... u_ )					const noexcept	{	return operator[]( Point( u_ ... ) );	}
		
		/// Calculate an index into a vector for the index represented by pt_.
		constexpr U operator[]( Point pt_ )						const noexcept	{	return dot_product( pt_, m_offsets );	}
		
		/// Calculate an index into a vector for the index represented by pt_.
		template< uinteger ...U2 >								requires( sizeof...( U2 ) == N )
		constexpr U at( U2 && ...u_ )							const noexcept	{	return at( Point( u_ ... ) );			}
		
		/// Calculate an index into a vector for the index represented by pt_.
		constexpr U at( Point pt_ )								const noexcept	{
			assert( all_lt( pt_, max() ) );
			return dot_product( pt_, m_offsets );
		}
	};

}	// namespace pax
