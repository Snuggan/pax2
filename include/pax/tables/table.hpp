//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "strided-iterator.hpp"
#include "../std/mdspan.hpp"

#include <vector>


namespace pax {

	/// Handle 2-dimensional data. 
	/// With time, use mdarray?
	template< 
		typename T, 
		typename Extents		  = std::dextents< std::size_t, 2 >,
		typename LayoutPolicy	  = std::layout_right,
		typename AccessorPolicy	  = std::default_accessor< T >
	>	requires( Extents::rank() == 2 )
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

		std::vector< value_type >	m_data{};

		template< typename ...Sz >
		void adjust_size( Sz && ...sz_ )							{
			m_data.resize( ( 1 * ... *sz_ ) );
			mdspan::operator=( m_data.empty() ? mdspan{}
				: mdspan{ m_data.data(), std::array{ std::forward< Sz >( sz_ )... } } );
		}

		constexpr void m_copy( const mdspan & other_ )				{
			m_data.assign( other_.data_handle(), other_.data_handle() + other_.size() );
			mdspan::operator=( mdspan( m_data.data(), other_.extents() ) );
		}

	public:
		constexpr Table()											=	default;
		constexpr Table( const Table  & other_ ) : mdspan{}			{	m_copy( other_ );								}
		constexpr Table( const mdspan & other_ )					{	m_copy( other_ );								}
		constexpr Table( Table && other_ )				noexcept	{	*this = std::move( other_ );					}
		constexpr Table & operator=( const Table & other_ )			{	m_copy( other_ );		return *this;			}
		constexpr Table & operator=( const mdspan & other_ )		{	m_copy( other_ );		return *this;			}

		constexpr Table & operator=( Table && other_ )	noexcept	{
			m_data				  = std::move( other_.m_data );
			mdspan::operator=( mdspan( m_data.data(), other_.extents() ) );
			return *this;
		}

		constexpr Table(
			const Size		 		rows_, 
			const Size		 		cols_
		)															{	adjust_size( rows_, cols_ );					}

		template< typename U, Size N >
			requires std::is_same_v< value_type, std::remove_cv_t< U > >
		constexpr Table(
			const std::span< U, N >	data_, 
			const Size		 		cols_
		) : m_data( data_.begin(), data_.end() )					{	adjust_size( data_.size()/cols_, cols_ );		}

		using mdspan::extent;
		using mdspan::size;
		constexpr Size rows()						const noexcept	{	return extent( rowR );							}
		constexpr Size cols()						const noexcept	{	return extent( colR );							}
		constexpr const value_type * data()			const noexcept	{	return m_data.data();							}
		constexpr value_type * data()					  noexcept	{	return m_data.data();							}

		constexpr auto span()						const noexcept	{	return std::span( data(), size() );				}
		constexpr auto span()							  noexcept	{	return std::span( data(), size() );				}

		constexpr auto begin()						const noexcept	{	return m_data.begin();							}
		constexpr auto begin()							  noexcept	{	return m_data.begin();							}
		constexpr auto end  ()						const noexcept	{	return m_data.end();							}
		constexpr auto end  ()							  noexcept	{	return m_data.end();							}

		constexpr auto begin_row( const Size r_ )	const noexcept	{	return pax::begin< rowR >( *this, r_ );			}
		constexpr auto begin_row( const Size r_ )		  noexcept	{	return pax::begin< rowR >( *this, r_ );			}
		constexpr auto end_row  ( const Size r_ )	const noexcept	{	return pax::end  < rowR >( *this, r_ );			}
		constexpr auto end_row  ( const Size r_ )		  noexcept	{	return pax::end  < rowR >( *this, r_ );			}

		constexpr auto begin_col( const Size c_ )	const noexcept	{	return pax::begin< colR >( *this, c_ );			}
		constexpr auto begin_col( const Size c_ )		  noexcept	{	return pax::begin< colR >( *this, c_ );			}
		constexpr auto end_col  ( const Size c_ )	const noexcept	{	return pax::end  < colR >( *this, c_ );			}
		constexpr auto end_col  ( const Size c_ )		  noexcept	{	return pax::end  < colR >( *this, c_ );			}

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
			const Size			new_size{ rows_ * cols_ };
			if( m_data.size() <  new_size )	m_data.resize( new_size );
			pax::resize( 
				std::span( m_data ), 
				mdspan{ m_data.data(), mdspan::extents() }, 
				mdspan{ m_data.data(), std::array{ rows_, cols_ } } 
			);
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
