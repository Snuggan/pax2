//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "point.hpp"


namespace pax {
	
	template< std::size_t N >
	using Index = std::array< std::size_t, N >;


	/// Let the specialisation of unsigned always have zero as minimum. Useful for indeces.
	template< std::size_t N >									requires( is_static< N > )
	class Indexer {
		using Idx					  = Index< N >;
		Idx								m_size{}, m_offsets{};
		static constexpr Idx			noll{};
		
		static constexpr Idx do_offs( const Idx & idx_ )		noexcept	{
			std::size_t					product{ 1u };
			auto [ ... t, tn ]		  = idx_;
			return { product, ( product *= t ) ... };
		}
		
	public:
		static constexpr std::size_t	rank  = N;
		using Idx::value_type;

		constexpr Indexer()									  = default;
		constexpr Indexer( const Indexer & )				  = default;
		constexpr Indexer( Indexer && )						  = default;
		constexpr Indexer & operator=( const Indexer & )	  = default;
		constexpr Indexer & operator=( Indexer && )			  = default;
		constexpr Indexer( const Idx & size_ )  				noexcept : m_size( size_ ), m_offsets{ do_offs( size_ ) } {}

		constexpr const Idx & size()							const noexcept	{	return m_size;							}
		constexpr const Idx & offsets()							const noexcept	{	return m_offsets;						}
		constexpr std::size_t elements()						const noexcept	{	return offsets().back();				}
		constexpr std::size_t cols()							const noexcept	{	return col( size() );					}
		constexpr std::size_t rows()							const noexcept	{	return row( size() );					}
		constexpr bool valid_index( const Idx & idx_ )			const noexcept	{	return all_lt( idx_, size() );			}
		constexpr Indexer grow( const Idx & idx_ )				const noexcept	{	return { max( idx_, size() ) };			}
		friend constexpr const Idx & min( const Indexer & i_ )	noexcept		{	return i_.noll;							}
		friend constexpr const Idx & max( const Indexer & i_ )	noexcept		{	return i_.m_size;						}
		
		/// Calculate an index into a vector for the index represented by pt_.
		template< uinteger ...U >								requires( sizeof...( U ) == N )
		constexpr std::size_t operator[]( U && ... u_ )			const noexcept	{	return operator[]( Idx( u_ ... ) );		}
		
		/// Calculate an index into a vector for the index represented by pt_.
		constexpr std::size_t operator[]( const Idx & idx_ )	const noexcept	{	return dot_product( idx_, offsets() );	}
		
		/// Calculate an index into a vector for the index represented by pt_.
		template< uinteger ...U >								requires( sizeof...( U ) == N )
		constexpr std::size_t at( U && ...u_ )					const noexcept	{	return at( Idx( u_ ... ) );				}
		
		/// Calculate an index into a vector for the index represented by pt_.
		constexpr std::size_t at( const Idx & idx_ )			const noexcept	{
			assert( all_lt( idx_, size() ) );
			return dot_product( idx_, offsets() );
		}
	};

}	// namespace pax
