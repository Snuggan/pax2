//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "table.hpp"				// stream_deligneated
#include "../reporting/error_message.hpp"

#include <vector>


namespace pax {
	
	/** Header reference: 
		Construct from std::span, ensure unique ids,
		id2idx, idx2id 
		add/remove cells, reference all headers (std::span). 
	**/
	template< typename Ch >
	class Header {
		using element_type		  = std::basic_string_view< Ch >;
		using value_type		  = std::remove_cv_t< element_type >;
		using span_type			  = std::span< value_type >;
		using cspan_type		  = std::span< const value_type >;

		std::vector< value_type >	m_cells;
		
	public:
		constexpr Header()									  = default;
		constexpr Header( const Header & )					  = default;
		constexpr Header( Header && )						  = default;
		constexpr Header & operator=( const Header & )		  = default;
		constexpr Header & operator=( Header && )			  = default;

		constexpr Header( const span_type cells_ ) : 
			m_cells{ cells_.begin(), cells_.end() }
		{}
		
		constexpr cspan_type span()								const noexcept	{	return m_cells;			}
		constexpr std::size_t size()							const noexcept	{	return m_cells.size();	}
		constexpr value_type operator[]( const std::size_t i_ )	const			{	return m_cells[ i_ ];	}
		constexpr value_type & operator[]( const std::size_t i_ )				{	return m_cells[ i_ ];	}


		/// Returns the id corresponding to index i_.
		/// Returns an empty item if i_ is out of range. 
		constexpr value_type id( const std::size_t i_ )			const noexcept	{
			return ( i_ < size() ) ? m_cells[ i_ ] : value_type{};
		}


		/// Returns the index corresponding to id_.
		/// Returns -1u, if there is no item == id_.
		constexpr std::size_t index( const value_type id_ )		const noexcept	{
			for( std::size_t i{}; i<m_cells.size(); ++i )	
				if( m_cells[ i ] == id_ )						return i;
			return -1;
		}


		/// Adds id_ to the header if it is not already there.
		/// - Throws if id_ is empty.
		/// - Returns the index of id_.
		constexpr std::size_t add( const value_type id_ )						{
			if( id_.empty() )	throw error_message( "Text_table: No header item is allowed to be empty." );
			for( std::size_t i{}; i<m_cells.size(); ++i )	
				if( id_ == m_cells[ i ] )		return i;
			m_cells.push_back( id_ );
			return m_cells.size() - 1;
		}


		/// Stream the header items to out_ using col_mark_ as column deligneater. 
		///
		template< typename Out >
		void stream(
			Out							  & out_,
			Ch								col_mark_
		) const {
			stream_deligneated( out_, span(), col_mark_ );
		}
	};


	template< typename U, std::size_t N >
	Header( std::span< U, N > )	-> Header< typename std::remove_cv_t< U >::value_type >;

}	// namespace pax
