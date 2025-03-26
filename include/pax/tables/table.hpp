//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "strided-iterator.hpp"
#include "../std/mdspan.hpp"

#include <vector>


namespace pax {

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
		static_assert( mdspan::rank() == 2 );
		
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
		) : m_cells{ cells_.begin(), cells_.end() }					{	adjust_size( cells_.size() / cols_, cols_ );		}

		using mdspan::extent;
		using mdspan::size;
		using mdspan::stride;
		constexpr Size rows()						const noexcept	{	return extent( rowR );								}
		constexpr Size cols()						const noexcept	{	return extent( colR );								}
		constexpr const value_type * data()			const noexcept	{	return mdspan::data_handle();						}
		constexpr value_type * data()					  noexcept	{	return m_cells.data();								}

		constexpr auto span()						const noexcept	{	return std::span( data(), size() );					}
		constexpr auto span()							  noexcept	{	return std::span( data(), size() );					}

		constexpr auto begin()						const noexcept	{	return m_cells.begin();								}
		constexpr auto begin()							  noexcept	{	return m_cells.begin();								}
		constexpr auto end  ()						const noexcept	{	return m_cells.end();								}
		constexpr auto end  ()							  noexcept	{	return m_cells.end();								}

		constexpr auto begin_row( const Size r_ )	const noexcept	{	return strider( r_*stride( rowR ), stride( colR ) );}
		constexpr auto begin_row( const Size r_ )		  noexcept	{	return strider( r_*stride( rowR ), stride( colR ) );}
		constexpr auto end_row  ( const Size r_ )	const noexcept	{	return begin_row( r_ + 1 );							}
		constexpr auto end_row  ( const Size r_ )		  noexcept	{	return begin_row( r_ + 1 );							}

		constexpr auto begin_col( const Size c_ )	const noexcept	{	return strider( c_*stride( colR ), stride( rowR ) );}
		constexpr auto begin_col( const Size c_ )		  noexcept	{	return strider( c_*stride( colR ), stride( rowR ) );}
		constexpr auto end_col  ( const Size c_ )	const noexcept	{	return begin_col( c_ ) + rows();					}
		constexpr auto end_col  ( const Size c_ )		  noexcept	{	return begin_col( c_ ) + rows();					}

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
		void print(
			Out							  & out_,
			Predicate					 && row_predicate_, 
			const char 						separator_ = 0
		) const {
			for( Size r{}; r<rows(); ++r ) {
				if( row_predicate_( r ) ) {
					pax::print( out_, begin_row( r ), end_row( r ), separator_ );
					std::println( out_, "" );
				}
			}
		}

		/// Stream the table as text to out_.
		///
		template< typename Out >
		friend Out & operator<<(
			Out							  & out_,
			const Table					  & table_
		) {
			pax::print( out_, table_, ';' );
			return out_;
		}
	};

	template< typename U, std::size_t N >
	Table( std::span< U, N >, std::size_t )	-> Table< std::remove_cv_t< U > >;

}	// namespace pax
