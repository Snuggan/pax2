//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "strided-iterator.hpp"
#include "../types/point-stuff/box.hpp"
#include <pax/debug.hpp>

#include <vector>
#include <span>
#include <print>
#include <algorithm>	// std::fill, std::fill_n, std::copy_n


namespace pax {

	/// Rearranges the items of srce_ so that they correspond to the destination extent.
	template< std::size_t N, typename T, std::size_t M >
	constexpr void resize(
		const Indexer< N >					srce_, 	///< The source extents.
		const Indexer< N >					dest_, 	///< The destination extents. 
		const std::span< T, M >				data_
	);



	/// Handle 2-dimensional data. 
	/// With time, use mdarray?
	template< 
		typename T, 
		typename Indexer_			  = Indexer< 2 >
	>
	class Table : Indexer_ {
		using Indexer				  = Indexer_;
		using Idx					  = Indexer::Idx;
		using index_type			  = Idx::value_type;

		using element_type			  = T;
		using value_type			  = std::remove_cv_t< element_type >;
		using cspan_type			  = std::span< const value_type >;
		using span_type				  = std::span< value_type >;
		using CStrider				  = Strider< const element_type >;
		using MStrider				  = Strider< element_type >;

		std::vector< value_type >		m_data{};

		/// Returns a strided iterator to first item at [ col_, row_ ].
		constexpr Strided_iterator< const element_type > stride( 
			const std::size_t 			col_, 
			const std::size_t 			row_, 
			const std::ptrdiff_t 		stride_ 
		)	const noexcept	{
			return { m_data.data() + Indexer::scalar_index( col_, row_ ), stride_ };
		};

		constexpr Strided_iterator< element_type > stride( 
			const std::size_t 			col_, 
			const std::size_t 			row_, 
			const std::ptrdiff_t 		stride_ 
		)	noexcept		{
			return { m_data.data() + Indexer::scalar_index( col_, row_ ), stride_ };
		};

	public:
		constexpr Table()									  noexcept	=	default;
		constexpr Table( const Table  & )					  noexcept	=	default;
		constexpr Table( Table && )							  noexcept	=	default;
		constexpr Table & operator=( const Table & )		  noexcept	=	default;
		constexpr Table & operator=( Table && )				  noexcept	=	default;

		constexpr Table( const index_type cols_, const index_type rows_ )
			: Indexer( Idx{{ cols_, rows_ }} ), m_data( cols_*rows_ ) {}

		template< typename U, index_type N >			requires std::is_same_v< value_type, std::remove_cv_t< U > >
		constexpr Table(
			const std::span< U, N >	data_, 
			const index_type 		cols_
		) : Indexer( Idx{{ cols_, data_.size()/cols_ }} ), m_data( data_.begin(), data_.end() ) {}

		constexpr value_type operator[]( const index_type c_, const index_type r_ )		const noexcept	{
			return m_data[ Indexer::scalar_index( c_, r_ ) ];
		}

		constexpr element_type & operator[]( const index_type c_, const index_type r_ )		  noexcept	{
			return m_data[ Indexer::scalar_index( c_, r_ ) ];
		}

		constexpr value_type operator[]( const Idx i_ ) 								const noexcept	{
			return m_data[ Indexer::scalar_index( i_ ) ];
		}

		constexpr element_type & operator[]( const Idx i_ )									  noexcept	{
			return m_data[ Indexer::scalar_index( i_ ) ];
		}

		using Indexer::rows;
		using Indexer::cols;
		constexpr index_type size()						const noexcept	{	return Indexer::elements();					}
		constexpr const value_type * data()				const noexcept	{	return m_data.data();						}
		constexpr value_type * data()						  noexcept	{	return m_data.data();						}

		constexpr auto span()							const noexcept	{	return cspan_type( data(), size() );		}
		constexpr auto span()								  noexcept	{	return  span_type( data(), size() );		}

		/// Iterators for all elements. Row first.
		constexpr auto begin()							const noexcept	{	return m_data.begin();						}
		constexpr auto begin()								  noexcept	{	return m_data.begin();						}
		constexpr auto end  ()							const noexcept	{	return m_data.end();						}
		constexpr auto end  ()								  noexcept	{	return m_data.end();						}

		/// Iterators for column c_.
		constexpr auto begin_col( const index_type c_ )	const noexcept	{	return stride( c_, 0u, cols() );			}
		constexpr auto begin_col( const index_type c_ )		  noexcept	{	return stride( c_, 0u, cols() );			}
		constexpr auto end_col  ( const index_type c_ )	const noexcept	{	return stride( c_, rows(), cols() );		}
		constexpr auto end_col  ( const index_type c_ )		  noexcept	{	return stride( c_, rows(), cols() );		}

		/// Iterators for row r_.
		constexpr auto begin_row( const index_type r_ )	const noexcept	{	return stride( 0u, r_, 1u );				}
		constexpr auto begin_row( const index_type r_ )		  noexcept	{	return stride( 0u, r_, 1u );				}
		constexpr auto end_row  ( const index_type r_ )	const noexcept	{	return stride( 0u, r_ + 1u, 1u );			}
		constexpr auto end_row  ( const index_type r_ )		  noexcept	{	return stride( 0u, r_ + 1u, 1u );			}

		/// Return column c_ as a Strider.
		constexpr CStrider col( const index_type c_ )	const noexcept	{	return { begin_col( c_ ), rows() };			}
		constexpr MStrider col( const index_type c_ )		  noexcept	{	return { begin_col( c_ ), rows() };			}

		/// Return row r_ as a Strider.
		constexpr CStrider row( const index_type r_ )	const noexcept	{	return { begin_row( r_ ), cols() };			}
		constexpr MStrider row( const index_type r_ )		  noexcept	{	return { begin_row( r_ ), cols() };			}

		/// Resize the table.
		/// If the table is enlarged, old values are retained and new items are set to T{}.
		/// If the table is decreased, the values within the new size are retained. 
		constexpr void resize( const index_type cols_, const index_type rows_ ) {
			const index_type				new_size( cols_*rows_ );
			pax::Indexer< 2 > new_indexer = new_size ? pax::Indexer< 2 >{{ cols_,  rows_  }} : pax::Indexer< 2 >{};
			if( m_data.size()  < new_size )	m_data.resize( new_size );
			pax::resize( 
				pax::Indexer< 2 >{{ cols(), rows() }},
				new_indexer,
				std::span( m_data )
			);
			if( m_data.size() != new_size )	m_data.resize( new_size );
			static_cast< Indexer & >( *this )  = new_indexer;
		}

		/// Remove columns.
		constexpr void remove_cols( const index_type col_, const index_type qtity_ = 1 ) {
			assert( col_ + qtity_ <= cols() );

			auto	dest		  = m_data.data() + col_;
			auto	new_row		  = cols() - qtity_;
			auto	srce		  = dest + qtity_;
			auto	end			  = m_data.data() + size() - cols();
			while( srce <= end ) {
				std::copy_n( srce, new_row, dest );
				dest			 += new_row;
				srce			 += cols();
			}
			std::copy_n( srce, cols() - col_ - qtity_, dest );
			static_cast< Indexer & >( *this )  = ( cols() > qtity_ ) 
				? pax::Indexer< 2 >{{ cols() - qtity_,  rows() }} 
				: pax::Indexer< 2 >{};
		}

		/// Stream the rows for which predicate_[ i ] is true to out_ using col_mark_ as column deligneater. 
		/// Row deligneator is '\n'.
		template< typename Out, typename Predicate >
			requires( std::is_invocable_r_v< bool, Predicate, index_type > )
		void print(
			Out							  & out_,
			Predicate					 && row_predicate_, 
			const char 						separator_ = ';'
		) const {
			for( index_type r{}; r<rows(); ++r ) {
				if( row_predicate_( r ) ) {
					bool first			  = true;
					for( auto && e : row( r ) ) {
						first	? std::print( out_, "{}", e )
								: std::print( out_, "{}{}", separator_, e );
						first = false;
					}
					std::println( out_, "" );
				}
			}
		}

		/// Stream the rows for which predicate_[ i ] is true to out_ using col_mark_ as column deligneater. 
		/// Row deligneator is '\n'.
		template< typename Out >
		void print(
			Out							  & out_,
			const char 						separator_ = ';'
		) const {
			print( out_, []( std::size_t ){ return true; }, separator_ );
		}

		/// Stream the table as text to out_.
		///
		template< typename Out >
		friend Out & operator<<(
			Out							  & out_,
			const Table					  & table_
		) {
			table_.print( out_ );
			return out_;
		}
	};

	template< typename U, std::size_t N >
	Table( std::span< U, N >, std::size_t )	-> Table< std::remove_cv_t< U > >;
	
	
	
	/// Rearranges the items of srce_ so that they correspond to the destination extent.
	template< std::size_t N, typename T, std::size_t M >
	constexpr void resize(
		const Indexer< N >					srce_, 	///< The source extents.
		const Indexer< N >					dest_, 	///< The destination extents. 
		const std::span< T, M >				data_
	) {
		using Ptr						  = T*;
		using Size						  = std::size_t;

		assert( ( srce_.elements() <= data_.size() ) && ( dest_.elements() <= data_.size() ) );

		const Ptr			srce_data	  = data_.data();
		const Ptr			dest_data	  = data_.data();

		if constexpr( N == 0u ) {
			// Strange case, but do nothing.
		} else if constexpr( N == 1u ) {
			// No matter if we grow or shrink, we don't need to do anything.
		} else if constexpr( N == 2u ) {
			if( srce_.empty() || dest_.empty() ) {	// If either is empty, there is nothing to copy...
				std::fill( dest_data, dest_data + srce_.elements(), T{} );
			} else {
				const Size		srce_step	 = srce_.extents()[ 0u ];	// extent[ 0 ] is one.
				const Size		dest_step	 = dest_.extents()[ 0u ];	// extent[ 0 ] is one.
				const Size		iters		 = std::min( srce_.elements()/srce_step, dest_.elements()/dest_step );
				// Debug{} << srce_step << ", " << dest_step << ", " << iters;

				if( srce_step < dest_step ) {				// Expand: copy from last to first.
					Ptr			srce		 = srce_data + srce_step*iters;
					Ptr			dest		 = dest_data + dest_step*iters;
					const Size	remains		 = dest_step - srce_step;
					while( srce	!= srce_data ) {
						std::copy_n( srce	-= srce_step, srce_step, dest -= dest_step );
						std::fill_n( dest	+  srce_step, remains, T{} );
					}
				} else if( srce_step > dest_step ) {		// Shrink: copy from first to last.
					Ptr			srce		 = srce_data;
					Ptr			dest		 = dest_data;
					const Ptr	srce_end	 = srce + srce_step*iters;
					while( srce	!= srce_end ) {
						std::copy_n( srce, dest_step, dest );
						srce				+= srce_step;
						dest				+= dest_step;
					}
				}
				if( dest_step*iters < dest_.elements() )	// Zero-out trailing values, if any.
					std::fill_n( dest_data + dest_step*iters, dest_.elements() - dest_step*iters, T{} );
			}
		} else {
			// Probably higher dimensions can be implemented through recursion. 
			static_assert( false, "Not yet implemented for dimensionality > 2." );
		}
	}

}	// namespace pax
