//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "strided-iterator.hpp"

#include <cassert>
#include <print>
#include <vector>
#include <span>
#include <mdspan>
#include <algorithm>	// std::copy_n, std::fill_n, std::min


namespace pax {

	template< typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy >
	static void resize(
		const std::span< T >											data_, 		///< The actual data items.
		const std::mdspan< T, Extents, LayoutPolicy, AccessorPolicy >	srce_map_, 	///< The source extents.
		const std::mdspan< T, Extents, LayoutPolicy, AccessorPolicy >	dest_map_ 	///< The destination extents. 
	) {
		// If either source or destination is empty, there is nothing to copy.
		if( !srce_map_.size() || !dest_map_.size() )					return;

		static_assert( Extents::rank() );
		assert( data_.size() >= srce_map_.size() );
		assert( data_.size() >= dest_map_.size() );

		using Ptr					 	  = T*;
		static constexpr std::size_t D	  = Extents::rank() - 1;	// What dimension has contiguous elements? (Either 0 or rank-1.)

		//	From [ 5, 3, 2, 2 ] to [ 4, 4, 2, 2 ], which gives stride [ 12,  4, 2, 1 ] to [ 16,  4, 2, 1 ]
		//	From [ 2, 2, 3, 5 ] to [ 2, 2, 4, 4 ], which gives stride [ 30, 15, 5, 1 ] to [ 32, 16, 4, 1 ]
		//	|  0  1  2  3  4 | | 15 16 17 18 19 |		|  0  1  2  3 | | 15 16 17 18 |		|  0  1  2  3 | | 16 17 18 19 |
		//	|  5  6  7  8  9 | | 20 21 22 23 24 |		|  5  6  7  8 | | 20 21 22 23 |		|  4  5  6  7 | | 20 21 22 23 |
		//	| 10 11 12 13 14 | | 25 26 27 28 29 ]		| 10 11 12 13 | | 25 26 27 28 ]		|  8  9 10 11 | | 24 25 26 27 ]
		//								   				|  0  0  0  0 | |  0  0  0  0 |		| 12 13 14 15 | | 28 29 30 31 |
		//
		//	| 30 31 32 33 34 ] | 45 46 47 48 49 |		| 30 31 32 33 ] | 45 46 47 48 |		| 32 33 34 35 ] | 48 49 50 51 |
		//	| 35 36 37 38 39 | | 50 51 52 53 54 |		| 35 36 37 38 | | 50 51 52 53 |		| 36 37 38 39 | | 52 53 54 55 |
		//	| 40 41 42 43 44 | | 55 56 57 58 59 |		| 40 41 42 43 | | 55 56 57 58 |		| 40 41 42 43 | | 56 57 58 59 |
		//												|  0  0  0  0 | |  0  0  0  0 |		| 44 45 46 47 | | 60 61 62 63 |

		const auto		  & srce_step  = srce_map_.extent( D );
		const auto		  & dest_step  = dest_map_.extent( D );
		const std::size_t	iters	  = std::min( srce_map_.size()/srce_step, dest_map_.size()/dest_step );

		if( srce_step < dest_step ) {				// Expoand: copy from last to first.
			Ptr				srce	  = data_.data() + srce_step*iters;
			Ptr				dest	  = data_.data() + dest_step*iters;
			const Ptr		srce_end  = data_.data();
			const auto		remains	  = dest_step - srce_step;
			while( srce	!= srce_end ) {
				std::copy_n( srce -= srce_step, srce_step, dest -= dest_step );
				std::fill_n( dest +  srce_step, remains, T{} );
			}
		} else if( srce_step > dest_step ) {		// Shrink:  copy from first to last.
			Ptr				srce	  = data_.data();
			Ptr				dest	  = data_.data();
			const Ptr		srce_end  = data_.data() + srce_step*iters;
			while( srce	!= srce_end ) {
				std::copy_n( srce, dest_step, dest );
				srce += srce_step;
				dest += dest_step;
			}
		}
		if( dest_step*iters < dest_map_.size() )	// Zero-out trailing values, if any.
			std::fill_n( data_.data() + dest_step*iters, dest_map_.size() - dest_step*iters, T{} );
	}




	/// Stream the header items to out_ using col_mark_ as column deligneater. 
	///
	template< typename Out, typename Itr, typename Ch = char >
	Out & stream(
		Out							  & out_,
		Itr								itr_, 
		const Itr						end_, 
		const Ch						col_mark_ = ';'
	) {
		if( itr_ != end_ ) {
			out_ << std::format( "{}", *itr_ );
			while( ++itr_ != end_ )		out_ << std::format( "{}{}", col_mark_, *itr_ );
		}
		return out_;
	}

	template< typename Out, typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy, typename Ch = char >
	Out & stream( 
		Out													  & out_, 
		std::mdspan< T, Extents, LayoutPolicy, AccessorPolicy >	md_, 
		const Ch												col_mark_ = ';'
	) {
		out_	<< std::format( "-----------------------------------------------------\n" )
				<< std::format( "rank {}, size {}, ", md_.rank(), md_.size() )
				<< std::format( "extents [ {}, {}, {}, {} ], ", md_.extent( 0 ), md_.extent( 1 ), md_.extent( 2 ), md_.extent( 3 ) )
				<< std::format( "strides [ {}, {}, {}, {} ]\n", md_.stride( 0 ), md_.stride( 1 ), md_.stride( 2 ), md_.stride( 3 ) );
		for( std::size_t m{}; m<md_.extent( 0 ); ++m ) {
			for( std::size_t n{}; n<md_.extent( 1 ); ++n ) {
				out_ << std::format( "\n[ {}, {}, r, c ]; \n", m, n );
				for( std::size_t r{}; r<md_.extent( 2 ); ++r ) {
					stream( out_, &md_[ m, n, r, 0 ], &md_[ m, n, r, md_.extent( 3 ) ], col_mark_ );
					out_ << std::format( "\n" );
				}
			}
		}
		// std::println( "{}", std::span{ md_.data_handle(), md_.size() } );
		return out_ << "-----------------------------------------------------\n";
	}


	/// Handle 2-dimensional data. 
	/// With time, use md_span?
	template< 
		typename T, 
		typename Extents		  = std::dextents< std::size_t, 2 >,
		typename LayoutPolicy	  = std::layout_right,
		typename AccessorPolicy	  = std::default_accessor< T >
	>
	class Table : public std::mdspan< T, Extents, LayoutPolicy, AccessorPolicy > {
		using mdspan			  = std::mdspan< T, Extents, LayoutPolicy, AccessorPolicy >;
		using element_type		  = T;
		using value_type		  = std::remove_cv_t< element_type >;
		using span_type			  = std::span< value_type >;
		using cspan_type		  = std::span< const value_type >;
		using index_type		  = typename Extents::index_type;
		using Size				  = std::make_unsigned_t< index_type >;
		
		static constexpr Size		rowR = 0;
		static constexpr Size		colR = 1;

		std::vector< value_type >	m_cells{};

		template< typename ...Indeces >
		void adjust_size( Indeces && ...indeces_ )								  noexcept	{
			mdspan::operator=( ( true && ... && indeces_ ) ? mdspan{ m_cells.data(), std::array{ std::forward< Indeces >( indeces_ )... } } : mdspan{} );
			m_cells.resize( mdspan::size() );
		}
		
		constexpr auto strider( const Size offset_, const Size stride_ )	const noexcept	{
			return Strided_iterator( data()+offset_, stride_ );
		}
		constexpr auto strider( const Size offset_, const Size stride_ )		  noexcept	{
			return Strided_iterator( data()+offset_, stride_ );
		}

	public:
		constexpr Table()											=	default;
		constexpr Table( const Table & )							=	default;
		constexpr Table( Table && )									=	default;
		constexpr Table & operator=( const Table & )				=	default;
		constexpr Table & operator=( Table && )						=	default;

		template< typename U, Size N >
			requires std::is_same_v< value_type, std::remove_cv_t< U > >
		constexpr Table(
			const std::span< U, N >	cells_, 
			const Size		 		cols_
		) : m_cells{ cells_.begin(), cells_.end() }					{	adjust_size( cells_.size() / cols_, cols_ );			}

		using mdspan::extent;
		using mdspan::size;
		using mdspan::stride;
		constexpr Size rows()						const noexcept	{	return extent( rowR );									}
		constexpr Size cols()						const noexcept	{	return extent( colR );									}
		constexpr const value_type * data()			const noexcept	{	return mdspan::data_handle();							}
		constexpr value_type * data()					  noexcept	{	return m_cells.data();									}

		constexpr auto span()						const noexcept	{	return std::span( data(), size() );						}
		constexpr auto span()							  noexcept	{	return std::span( data(), size() );						}

		constexpr auto begin()						const noexcept	{	return m_cells.begin();									}
		constexpr auto begin()							  noexcept	{	return m_cells.begin();									}
		constexpr auto end  ()						const noexcept	{	return m_cells.end();									}
		constexpr auto end  ()							  noexcept	{	return m_cells.end();									}

		constexpr auto begin_row( const Size r_ )	const noexcept	{	return strider( r_*stride( rowR ), stride( colR ) );	}
		constexpr auto begin_row( const Size r_ )		  noexcept	{	return strider( r_*stride( rowR ), stride( colR ) );	}
		constexpr auto end_row  ( const Size r_ )	const noexcept	{	return begin_row( r_ + 1 );								}
		constexpr auto end_row  ( const Size r_ )		  noexcept	{	return begin_row( r_ + 1 );								}

		constexpr auto begin_col( const Size c_ )	const noexcept	{	return strider( c_*stride( colR ), stride( rowR ) );	}
		constexpr auto begin_col( const Size c_ )		  noexcept	{	return strider( c_*stride( colR ), stride( rowR ) );	}
		constexpr auto end_col  ( const Size c_ )	const noexcept	{	return begin_col( c_ ) + rows();						}
		constexpr auto end_col  ( const Size c_ )		  noexcept	{	return begin_col( c_ ) + rows();						}

		/// Return row r_ as a span.
		///
		constexpr cspan_type row( const Size r_ )	const noexcept	{
			return ( r_ < rows() ) ? cspan_type{ data() + cols()*r_, cols() } : cspan_type{};
		}

		/// Return row r_ as a span.
		///
		constexpr span_type row( const Size r_ )		  			{
			return ( r_ < rows() ) ? span_type{ data() + cols()*r_, cols() } : span_type{};
		}

		/// Resize the tablen.
		/// If the table is enlarged, old values are retained and new items are set to T{}.
		/// If the table is decreased, the values within the new size are retained. 
		constexpr void resize( const Size rows_, const Size cols_ ) {
			const mdspan		  dest{ m_cells.data(), std::array{ rows_, cols_ } };
			if( m_cells.size() <  dest.size() )	m_cells.resize( dest.size() );
			pax::resize( std::span( data(), m_cells.size() ), *this, dest );
			if( m_cells.size() != dest.size() )	m_cells.resize( dest.size() );
			adjust_size( rows_, cols_ );
		}

		/// Remove a column.
		/// 
		constexpr void remove_column( const Size col_ ) 			{
			if( ( col_ < extent( colR ) ) && size() ) {
				// We leave be the elements before the first column item to remove.
				const auto		new_cols = extent( colR ) - 1;
				T			  * dest = data() + col_;
				T			  * srce = dest + 1;
				const T		  * send = dest + extent( colR ) * ( extent( rowR ) - 1 );

				// Copy the rows from behind one column item to remove to just before the next. 
				while( srce < send ) {
					std::copy_n( srce, new_cols, dest );
					srce	 += extent( colR );
					dest	 += new_cols;
				}

				// Copy the elements behind the last column item to remove, the trailing elements of the last row. 
				std::copy_n( srce, new_cols - col_, dest );

				adjust_size( extent( rowR ), extent( colR ) - 1 );
			}
		}

		/// Stream the rows for which predicate_[ i ] is true to out_ using col_mark_ as column deligneater. 
		/// Row deligneator is '\n'.
		template< typename Out, typename Predicate >
			requires( std::is_invocable_r_v< bool, Predicate, Size > )
		Out & stream(
			Out							  & out_,
			Predicate					 && row_predicate_, 
			const char						col_mark_ = ';'
		) const {
			for( Size r{}; r<rows(); ++r ) 
				if( row_predicate_( r ) )	pax::stream( out_, begin_row( r ), end_row( r ), col_mark_ ) << '\n';
			return out_;
		}

		/// Stream the table to out_ using col_mark_ as column deligneater. 
		/// Row deligneator is '\n'.
		template< typename Out >
		Out & stream(
			Out							  & out_,
			const char 						col_mark_ = ';'
		) const {
			for( Size r = 1; r<=rows(); ++r ) 
				pax::stream( out_, begin_row( r ), end_row( r ), col_mark_ ) << '\n';
			return out_;
		}

		/// Stream the table as text to out_.
		///
		template< typename Out >
		friend Out & operator<<(
			Out							  & out_,
			const Table					  & table_
		) {
			table_.stream( out_ );
			return out_;
		}
	};

	template< typename U, std::size_t N >
	Table( std::span< U, N >, std::size_t )	-> Table< std::remove_cv_t< U > >;

}	// namespace pax
