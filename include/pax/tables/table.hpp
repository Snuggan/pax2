//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "strided-iterator.hpp"

#if( 0 )
#	include <print>
#	define PRINT( ... )		std::print( __VA_ARGS__ )
#else
#	define PRINT( ... )	
#endif

#include <cassert>
#include <vector>
#include <span>
#include <mdspan>
#include <algorithm>	// std::copy_n, std::fill_n, std::min


namespace pax {

	template< typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy >
	static void resize(
		const std::span< T >											data_, 
		const std::mdspan< T, Extents, LayoutPolicy, AccessorPolicy >	srce_map_, 
		const std::mdspan< T, Extents, LayoutPolicy, AccessorPolicy >	dest_map_ 
	) {

		//	From [ 5, 3, 2, 2 ] to [ 4, 4, 2, 2 ], which gives stride [ 12,  4, 2, 1 ] to [ 16,  4, 2, 1 ]
		//	From [ 2, 2, 3, 5 ] to [ 2, 2, 4, 4 ], which gives stride [ 30, 15, 5, 1 ] to [ 32, 16, 4, 1 ]

		//	|  0  1  2  3  4 |	| 15 16 17 18 19 |		|  0  1  2  3 |	| 15 16 17 18 |		|  0  1  2  3 |	| 16 17 18 19 |
		//	|  5  6  7  8  9 |	| 20 21 22 23 24 |		|  5  6  7  8 |	| 20 21 22 23 |		|  4  5  6  7 |	| 20 21 22 23 |
		//	| 10 11 12 13 14 |	| 25 26 27 28 29 ]		| 10 11 12 13 |	| 25 26 27 28 ]		|  8  9 10 11 |	| 24 25 26 27 ]
		//								   				|  0  0  0  0 |	|  0  0  0  0 |		| 12 13 14 15 |	| 28 29 30 31 |
		//
		//	| 30 31 32 33 34 ]	| 45 46 47 48 49 |		| 30 31 32 33 ]	| 45 46 47 48 |		| 32 33 34 35 ]	| 48 49 50 51 |
		//	| 35 36 37 38 39 |	| 50 51 52 53 54 |		| 35 36 37 38 |	| 50 51 52 53 |		| 36 37 38 39 |	| 52 53 54 55 |
		//	| 40 41 42 43 44 |	| 55 56 57 58 59 |		| 40 41 42 43 |	| 55 56 57 58 |		| 40 41 42 43 |	| 56 57 58 59 |
		//												|  0  0  0  0 |	|  0  0  0  0 |		| 44 45 46 47 |	| 60 61 62 63 |

		PRINT( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n" );
		PRINT( "Resize from [ {}, {} ] to [ {}, {} ]\n",	srce_map_.extent( 0 ), srce_map_.extent( 1 ),
		 													dest_map_.extent( 0 ), dest_map_.extent( 1 ) );

		// If either source or destination is empty, there is nothing to copy.
		if( !srce_map_.size() || !dest_map_.size() )					return;

		static_assert( Extents::rank() );
		assert( data_.size() >= srce_map_.size() );
		assert( data_.size() >= dest_map_.size() );

		using Size					  = std::size_t;
		using Step					  = std::ptrdiff_t;
		static constexpr Size	D	  = Extents::rank() - 1;	// What dimension has contiguous elements? (Either 0 or rank-1.)

		// If srce_map_.extent( D ) > dest_map_.extent( D ) shrink:  copy from first to last.
		// If srce_map_.extent( D ) < dest_map_.extent( D ) expoand: copy from last to first.
		const Size		chunksize	  = std::min( srce_map_.extent( D ), dest_map_.extent( D ) );
		const Size		iters		  = std::min( srce_map_.size()/srce_map_.extent( D ), dest_map_.size()/dest_map_.extent( D ) );
		const bool		expand		  = srce_map_.extent( D ) < dest_map_.extent( D );
		
		const Step		srce_step	  = ( expand ? -1 : 1 )*static_cast< Step >( srce_map_.extent( D ) );
		const Step		dest_step	  = ( expand ? -1 : 1 )*static_cast< Step >( dest_map_.extent( D ) );
		T			  * srce		  = data_.data() + ( expand ? -srce_step*( iters - 1 ) : 0 );
		T			  * dest		  = data_.data() + ( expand ? -dest_step*( iters - 1 ) : 0 );
		T const		  * srce_end	  = srce + srce_step*iters;

		PRINT( "expand:    {}\n", expand );
		PRINT( "chunksize: {}\n", chunksize );
		PRINT( "iters:     {}\n", iters );
		PRINT( "srce:      {} + {} ({})\n", srce - data_.data(), srce_step, srce_end - data_.data() );
		PRINT( "dest:      {} + {}\n", dest - data_.data(), dest_step );

		if( expand ) {
			while( srce	!= srce_end ) {
				PRINT( "copy_n( {}, {}, {} )\n", srce-data_.data(), chunksize, dest-data_.data() );
				std::copy_n( srce, chunksize, dest );
				PRINT( "fill_n( {}, {}, 0 )\n", dest + chunksize - data_.data(), dest_map_.extent( D ) - chunksize );
				std::fill_n( dest + chunksize, dest_map_.extent( D ) - chunksize, T{} );
				srce += srce_step;
				dest += dest_step;
			}
		} else {
			while( srce	!= srce_end ) {
				PRINT( "copy_n( {}, {}, {} )\n", srce-data_.data(), chunksize, dest-data_.data() );
				std::copy_n( srce, chunksize, dest );
				srce += srce_step;
				dest += dest_step;
			}
		}

		// Zero trailing values, if any. 
		if( dest_step*iters < dest_map_.size() ) {	//true || expand ) {
			PRINT( "fill_n( {}, {}, {} )\n", dest_step*iters, dest_map_.size() - dest_step*iters, T{} );
			std::fill_n( data_.data() + dest_step*iters, dest_map_.size() - dest_step*iters, T{} );
		}
	}


	// template< typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy >
	template< typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy >
	static void resize( 
		std::vector< T >											  & data_, 
		const std::mdspan< T, Extents, LayoutPolicy, AccessorPolicy >	srce_, 
		const std::mdspan< T, Extents, LayoutPolicy, AccessorPolicy >	dest_ 
	) {
		if( data_.size() <  dest_.size() )	data_.resize  ( dest_.size() );
		resize( std::span{ data_ }, srce_, dest_ );
		if( data_.size() != dest_.size() )	data_.resize  ( dest_.size() );
	}




	/// Stream the header items to out_ using col_mark_ as column deligneater. 
	///
	template< typename Out, typename Itr, typename Ch >
	Out & stream(
		Out						  & out_,
		Itr							itr_, 
		const Itr					end_, 
		const Ch					col_mark_
	) {
		if( itr_ != end_ ) {
			out_ << *itr_;
			while( ++itr_ != end_ )		out_ << col_mark_ << *itr_;
		}
		return out_;
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
			pax::resize( m_cells, *this, mdspan{ m_cells.data(), std::array{ rows_, cols_ } } );
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
			Predicate					 && predicate_, 
			const char						col_mark_ = ';'
		) const {
			for( Size r{}; r<rows(); ++r ) 
				if( predicate_( r ) )		pax::stream( out_, begin_row( r ), end_row( r ), col_mark_ ) << '\n';
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
#undef PRINT
