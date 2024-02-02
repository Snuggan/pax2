//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "../std/algorithm.hpp"			// pax::split
#include "../reporting/error_message.hpp"

#include <span>
#include <vector>
#include <tuple>


namespace pax {

	class String_count2 {
		using Count				  = std::size_t;

		Count						m_count[ 128 ]={};
		Count		 				m_rows{}, m_glyphs{}, m_non_ascii{}, m_cols_in_first_row{};
		char						m_col_delimit = ';';
		bool						m_final_newline{};
		
		template< typename Ch >
		static constexpr Count count(
			const std::basic_string_view< Ch >	str_, 
			const Ch							c_
		) noexcept {
			Count		 			count{};
			for( const Ch c : str_ )
				if( c == c_ )		++count;
			return count;
		}
		
	public:
		constexpr String_count2()										= default;
		constexpr String_count2( const String_count2 & )				= default;
		constexpr String_count2 & operator=( const String_count2 & )	= default;
		
		template< typename Ch >
		constexpr String_count2( const std::basic_string_view< Ch > str_ ) noexcept;
		
		/// Returns a span to the total counts of all ascii characters (code<128).
		constexpr auto counts()			const noexcept	{
			return std::span< const Count, 128 >( m_count, 128 );
		}
		
		/// The total number of glyphs.
		constexpr auto size()			const noexcept	{	return m_glyphs;					}

		/// The total number of non-ascii characters (code > 127). 
		constexpr auto non_ascii()		const noexcept	{	return m_non_ascii;					}

		/// The total number of ascii characters (<128). 
		constexpr auto ascii()			const noexcept	{	return size() - non_ascii();		}

		/// Was the last character of the last string a newline character (LF or CR)?
		constexpr bool final_newline()	const noexcept	{	return m_final_newline;				}
		
		/// The total number of newlines plus !has_final_newline().
		constexpr auto rows()			const noexcept	{	return m_rows + !final_newline();	}

		/// The [probable] column delimiter (the most frequent of "\t!#$%@,;.:"). 
		constexpr auto col_delimiter()	const noexcept	{	return m_col_delimit;				}
		
		/// The number of columns in first row. Note: an empty row has one [empty] column!
		constexpr auto cols_in_first()	const noexcept	{	return m_cols_in_first_row;			}
	};


	template< typename Ch >
	constexpr String_count2::String_count2( const std::basic_string_view< Ch > str_ ) noexcept	{
		using View			  = std::basic_string_view< Ch >;
		
		for( const unsigned c : str_ )		( c < 128 )	? ++m_count[ c ] : ++m_non_ascii;

		// Find the number of lines.
		if     ( m_count[ unsigned( '\r' ) ] == 0 )		m_rows = m_count[ unsigned( '\n' ) ];
		else if( m_count[ unsigned( '\n' ) ] == 0 )		m_rows = m_count[ unsigned( '\r' ) ];
		else {
			View							row;
			View							remaining = str_;

			while( remaining.size() ) {
				++m_rows;
				std::tie( row, remaining )	  = split_by( remaining, Newline{} );
			}
		}

		// Find the probable column delimiter.
		constexpr const std::string_view	candidates = "\t!#$%@,;.:";
		for( const short c : candidates )
			m_col_delimit = ( m_count[ c ] > m_count[ unsigned( m_col_delimit ) ] ) ? c : m_col_delimit;

		m_glyphs						  = str_.size();
		m_final_newline					  = str_.size() && ends_with( str_, Newline{} );
		m_cols_in_first_row				  = count( until( str_, Newline{} ), col_delimiter() ) + 1;
	}




	/// Parse a string into a rectangular table (same number of columnbs in all rows). 
	/// To use when you know the number of rows and columns as well as the column mark.
	template< typename Ch >
	std::vector< std::basic_string_view< Ch > > parse2table(
		const std::basic_string_view< Ch >	str_,			///< String to parse.
		const std::size_t 					rows_, 
		const std::size_t 					cols_, 
		const Ch 							column_delimiter_
	) {
		constexpr auto invisible		  = []( const unsigned c_ ){	return c_ <= ' ';	};
		using View						  = std::basic_string_view< Ch >;
		std::vector< View >					result;
		result.reserve( rows_*cols_ );

		View								cell;
		View								row;
		View								remaining = trim_last( str_, invisible );


		while( remaining.size() ) {
			std::tie( row, remaining )	  = split_by( remaining, Newline{} );
			row							  = trim_first( row, invisible );

			const std::size_t				cell_count{ result.size() };
			while( row.size() ) {
				std::tie( cell, row )	  = split_by( row, column_delimiter_ );
				result.push_back( trim( cell, invisible ) );
			}
			if(	( result.size() - cell_count != cols_ ) 	// Check that all rows have the same number of columns, ...
			&&	( result.size() != cell_count ) ) 			// ... empty rows excepted.
				throw user_error_message( std20::format( 
					"parse2table: Varying number of columns as row {} has {} column[s] and not as expected {}.", 
					result.size() / cols_, 
					result.size() - cell_count, 
					cols_
				) );
		}
		return result;
	}


	/// Parse a string into a rectangular table (same number of columnbs in all rows). 
	/// Throws, if rows have not equal number of columns.
	template< typename Ch >
	std::tuple<
		std::vector< std::basic_string_view< Ch > >,			///< All cells.
		std::size_t, 											///< Number of columns.
		Ch														///< Column mark useg, e.g. ';'.
	> parse2table( const std::basic_string_view< Ch > str_ ) {
		const String_count2					count( str_ );
		const auto rows					  = count.rows();
		const auto cols					  = count.cols_in_first();
		const Ch   col_mark				  = count.col_delimiter();
		return { 
			parse2table( str_, rows, cols, col_mark ), 
			cols, 
			col_mark
		};
	};

}	// namespace pax
