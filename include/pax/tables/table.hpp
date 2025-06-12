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
		typename Layout			  = std::layout_right,
		typename Accessor		  = std::default_accessor< T >
	>	requires( Extents::rank() == 2 )
	class Table : public std::mdspan< T, Extents, Layout, Accessor > {
		using mdspan			  = std::mdspan< T, Extents, Layout, Accessor >;
		using value_type		  = std::remove_cv_t< T >;
		using span_type			  = std::span< value_type >;
		using cspan_type		  = std::span< const value_type >;
		using Index				  = std::make_unsigned_t< typename Extents::index_type >;
		static constexpr Index		Row = !is_layout_right_v< Layout >;
		static constexpr Index		Col =  is_layout_right_v< Layout >;

		std::vector< value_type >	m_data{};

		constexpr void m_copy( const mdspan & other_ )				{
			m_data.assign( other_.data_handle(), other_.data_handle() + other_.size() );
			mdspan::operator=( mdspan( m_data.data(), other_.extents() ) );
		}

		template< typename ...Sz >
		void m_adjust( Sz && ...sz_ )								{
			m_data.resize( ( 1 * ... *sz_ ) );
			mdspan::operator=( m_data.empty() ? mdspan{} : mdspan( m_data.data(), std::array{ sz_... } ) );
		}

	public:
		static constexpr bool		is_layout_right = std::is_same_v< Layout, std::layout_right >;
		static constexpr bool		is_layout_left  = std::is_same_v< Layout, std::layout_left  >;
		static_assert( is_layout_right || is_layout_left );

		constexpr Table()											=	default;
		constexpr Table( const Index rows_, const Index cols_ )		{	m_adjust( rows_, cols_ );					}
		constexpr Table( const Table  & other_ ) : mdspan{}			{	m_copy( other_ );							}
		constexpr Table( const mdspan & other_ )					{	m_copy( other_ );							}
		constexpr Table( Table && other_ )				noexcept	{	*this = std::move( other_ );				}
		constexpr Table & operator=( const Table & other_ )			{	m_copy( other_ );		return *this;		}
		constexpr Table & operator=( const mdspan & other_ )		{	m_copy( other_ );		return *this;		}

		constexpr Table & operator=( Table && other_ )	noexcept	{
			m_data				  = std::move( other_.m_data );
			mdspan::operator=( mdspan( m_data.data(), other_.extents() ) );
			return *this;
		}

		template< typename U, Index N >
			requires std::is_same_v< value_type, std::remove_cv_t< U > >
		constexpr Table(
			const std::span< U, N >	data_, 
			const Index		 		cols_
		) : m_data( data_.begin(), data_.end() )					{	m_adjust( data_.size()/cols_, cols_ );		}

		using mdspan::extent;
		using mdspan::size;
		constexpr Index rows()						const noexcept	{	return extent( Row );						}
		constexpr Index cols()						const noexcept	{	return extent( Col );						}
		constexpr const value_type * data()			const noexcept	{	return m_data.data();						}
		constexpr value_type * data()					  noexcept	{	return m_data.data();						}

		constexpr auto span()						const noexcept	{	return cspan_type( data(), size() );		}
		constexpr auto span()							  noexcept	{	return  span_type( data(), size() );		}

		constexpr auto begin()						const noexcept	{	return m_data.begin();						}
		constexpr auto begin()							  noexcept	{	return m_data.begin();						}
		constexpr auto end  ()						const noexcept	{	return m_data.end();						}
		constexpr auto end  ()							  noexcept	{	return m_data.end();						}

		constexpr auto begin_row( const Index r_ )	const noexcept	{	return pax::begin< Row >( *this, r_ );		}
		constexpr auto begin_row( const Index r_ )		  noexcept	{	return pax::begin< Row >( *this, r_ );		}
		constexpr auto end_row  ( const Index r_ )	const noexcept	{	return pax::end  < Row >( *this, r_ );		}
		constexpr auto end_row  ( const Index r_ )		  noexcept	{	return pax::end  < Row >( *this, r_ );		}

		constexpr auto begin_col( const Index c_ )	const noexcept	{	return pax::begin< Col >( *this, c_ );		}
		constexpr auto begin_col( const Index c_ )		  noexcept	{	return pax::begin< Col >( *this, c_ );		}
		constexpr auto end_col  ( const Index c_ )	const noexcept	{	return pax::end  < Col >( *this, c_ );		}
		constexpr auto end_col  ( const Index c_ )		  noexcept	{	return pax::end  < Col >( *this, c_ );		}

		/// Return row r_ as a Strider.
		constexpr auto row( const Index r_ )		const noexcept	{	return Strider( begin_row( r_ ), cols() );	}

		/// Return row r_ as a Strider.
		constexpr auto row( const Index r_ )			  noexcept	{	return Strider( begin_row( r_ ), cols() );	}

		/// Return column c_ as a Strider.
		constexpr auto col( const Index c_ )		const noexcept	{	return Strider{ begin_col( c_ ), rows() };	}

		/// Return column c_ as a Strider.
		constexpr auto col( const Index c_ )			  noexcept	{	return Strider{ begin_col( c_ ), rows() };	}

		/// Resize the table.
		/// If the table is enlarged, old values are retained and new items are set to T{}.
		/// If the table is decreased, the values within the new size are retained. 
		template< typename ...Sz >
		constexpr void resize( Sz && ...sz_ ) 		requires( sizeof...( Sz ) == Extents::rank() ) {
			const Index			new_size( ( 1 * ... *sz_ ) );
			if( m_data.size() < new_size )	m_data.resize( new_size );
			pax::resize( 
				mdspan{ m_data.data(), mdspan::extents() }, 
				mdspan{ m_data.data(), std::array{ sz_... } } 
			);
			m_adjust( sz_... );
		}

		// /// Remove rows.
		// constexpr void remove_rows( const Index r_, const Index qtity_ = 1 )					{
		// 	const auto			extents = remove< Row >( mdspan{ m_data.data(), mdspan::extents() }, r_, qtity_ );
		// 	m_data.resize( extents[ 0 ]*extents[ 1 ] );
		// 	mdspan::operator=( m_data.empty() ? mdspan{} : mdspan( m_data.data(), extents ) );
		// }

		/// Remove columns.
		constexpr void remove_cols( const Index c_, const Index qtity_ = 1 ) {
			const auto			exts = extents( remove< Col >( mdspan{ m_data.data(), mdspan::extents() }, c_, qtity_ ) );
			m_data.resize( exts[ 0 ]*exts[ 1 ] );
			mdspan::operator=( m_data.empty() ? mdspan{} : mdspan( m_data.data(), exts ) );
		}

		/// Stream the rows for which predicate_[ i ] is true to out_ using col_mark_ as column deligneater. 
		/// Row deligneator is '\n'.
		template< typename Out, typename Predicate >
			requires( std::is_invocable_r_v< bool, Predicate, Index > )
		void print(
			Out							  & out_,
			Predicate					 && row_predicate_, 
			const char 						separator_ = 0
		) const {
			for( Index r{}; r<rows(); ++r ) {
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
