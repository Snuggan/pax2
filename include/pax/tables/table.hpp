//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "tools.hpp"
#include "strided-iterator.hpp"
#include "resize-reorder.hpp"

#include <vector>


namespace pax {

	/// Handle 2-dimensional data. 
	/// With time, use md_span?
	template< typename T >
	class Table {
		using element_type		  = T;
		using value_type		  = std::remove_cv_t< element_type >;
		using span_type			  = std::span< value_type >;
		using cspan_type		  = std::span< const value_type >;
		using Size				  = std::size_t;

		std::vector< value_type >	m_cells{};
		Size		 				m_rows{}, m_cols{};

	public:
		constexpr Table()								=	default;
		constexpr Table( const Table & )				=	default;
		constexpr Table( Table && )						=	default;
		constexpr Table & operator=( const Table & )	=	default;
		constexpr Table & operator=( Table && )			=	default;

		template< typename U, Size N >
			requires std::is_same_v< value_type, std::remove_cv_t< U > >
		constexpr Table(
			const std::span< U, N >	cells_, 
			const Size		 		cols_
		) : m_cells{ cells_.begin(), cells_.end() }, m_rows{ cells_.size() / cols_ }, m_cols{ cols_ } {
			assert( rows()*cols() == size() );
		}

		constexpr Size rows()						const noexcept	{	return m_rows;							}
		constexpr Size cols()						const noexcept	{	return m_cols;							}
		constexpr Size size()						const noexcept	{	return m_cells.size();					}
		constexpr const value_type * data()			const noexcept	{	return m_cells.data();					}
		constexpr value_type * data()					  noexcept	{	return m_cells.data();					}

		constexpr auto span()						const noexcept	{	return std::span( data(), size() );		}
		constexpr auto span()							  noexcept	{	return std::span( data(), size() );		}

		constexpr auto begin()						const noexcept	{	return m_cells.begin();					}
		constexpr auto begin()							  noexcept	{	return m_cells.begin();					}
		constexpr auto end  ()						const noexcept	{	return m_cells.end();					}
		constexpr auto end  ()							  noexcept	{	return m_cells.end();					}

		constexpr auto begin_row( const Size r_ )	const noexcept	{	return begin() +   r_      *cols();		}
		constexpr auto begin_row( const Size r_ )		  noexcept	{	return begin() +   r_      *cols();		}
		constexpr auto end_row  ( const Size r_ )	const noexcept	{	return begin() + ( r_ + 1 )*cols();		}
		constexpr auto end_row  ( const Size r_ )		  noexcept	{	return begin() + ( r_ + 1 )*cols();		}

		constexpr auto begin_col( const Size c_ )	const noexcept	{	return Strided_iterator( data()+c_, cols() );	}
		constexpr auto begin_col( const Size c_ )		  noexcept	{	return Strided_iterator( data()+c_, cols() );	}
		constexpr auto end_col  ( const Size c_ )	const noexcept	{	return begin_col( c_ ) + rows();		}
		constexpr auto end_col  ( const Size c_ )		  noexcept	{	return begin_col( c_ ) + rows();		}

		constexpr value_type operator[]( Size r_, Size c_ )  const	{	return *( begin_row( r_ ) + c_ );		}
		constexpr value_type operator[]( Size r_, Size c_ )			{	return *( begin_row( r_ ) + c_ );		}

		/// Return row r_ as a span.
		///
		constexpr cspan_type row( const Size r_ )	const noexcept	{
			return ( r_ < rows() ) ? cspan_type{ data() + cols()*r_, cols() } : cspan_type{};
		}

		/// Return row r_ as a span.
		///
		constexpr span_type row( const Size r_ )		  noexcept	{
			return ( r_ < rows() ) ? span_type{ data() + cols()*r_, cols() } : span_type{};
		}


		constexpr void resize( const Size rows_, const Size cols_ )	{
			if( cols_ >  cols() )	m_cells.resize( rows_*cols_ );
			resize_reorder_2d( std::span{ m_cells.data(), m_cells.size() }, rows(), cols(), rows_, cols_ );
			if( cols_ <= cols() )	m_cells.resize( rows_*cols_ );
			m_rows				  = rows_;
			m_cols				  = cols_;
		}


		/// Stream the rows for which predicate_[ i ] is true to out_ using col_mark_ as column deligneater. 
		/// Row deligneator is '\n'.
		template< typename Out, typename Predicate >
			requires( std::is_invocable_r_v< bool, Predicate, Size > )
		void stream(
			Out							  & out_,
			Predicate					 && predicate_, 
			const char						col_mark_ = ';'
		) const {
			Size		 					i{};
			auto							itr{ begin() };
			const auto						e  { end()   };
			while( itr < e ) {
				if( predicate_( i ) ) {
					stream_deligneated( out_, std::span{ itr, itr + cols() }, col_mark_ );
					out_ << '\n';
				}
				++i;
				itr+= cols();
			}
		}


		/// Stream the rows for which predicate_[ i ] is true to out_ using col_mark_ as column deligneater. 
		/// Row deligneator is '\n'.
		template< typename Out >
		void stream(
			Out							  & out_,
			const char 						col_mark_ = ';'
		) const {
			auto							itr{ begin() };
			const auto						e  { end()   };
			while( itr < e ) {
				stream_deligneated( out_, std::span{ itr, cols() }, col_mark_ );
				out_ << '\n';
				itr+= cols();
			}
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
