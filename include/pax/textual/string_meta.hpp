//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "../std/algorithm.hpp"			// pax::split
#include "../reporting/error_message.hpp"
#include <pax/reporting/debug.hpp>

#include <span>
#include <vector>
#include <tuple>


namespace pax {

	class String_numeric {
		enum class Contents {
			undetermined, 		// Default value, not yet checked.
			uinteger, 
			integer, 
			floating_point, 
			floating_point_xtra,	// Includes nan, inf, etc.
			non_numeric
		};
		Contents	m_contents;
		
		static constexpr std::string_view ids[ int( Contents::non_numeric ) + 1 ] = 
			{ "undetermined", "unsigned integer", "integer", "floating point", "floating point (inf/NaN)", "textual" };

		template< typename Itr >
		static constexpr bool is_negative( 
			Itr					  & itr_, 
			const Itr				end_
		) noexcept {
			if( itr_ != end_ ) {
				const bool			negative = ( *itr_ == '-' );
				if( negative || ( *itr_ == '+' ) )	++itr_;
				return negative;
			}
			return false;
		}

		template< typename Itr >
		static constexpr bool is_digits( 
			Itr					  & itr_, 
			const Itr				end_
		) noexcept {
			const auto				start = itr_;
			while( ( itr_ != end_ ) && ( *itr_ >= '0' ) && ( *itr_ <= '9' ) )	++itr_;
			return itr_ != start;	// At least one digit.
		}

		template< typename Char, typename Traits >
		static constexpr Contents contents( const std::basic_string_view< Char, Traits > str_ ) noexcept {
			
			// Most strings are not numeric, so start with a quick check...
			// Retyrn, unless first character is any of +,-./0123456789.
			if( str_.empty() || ( str_.front() < '+' ) || ( str_.front() > '9' ) ) {
				if( str_.empty() )	return Contents::non_numeric;
				switch( str_.front() ) {
					case 'n': 		return ( str_ == "nan" )
										? Contents::floating_point_xtra : Contents::non_numeric;
					case 'N': 		return ( ( str_ == "NAN" ) || ( str_ == "NaN" ) )
										? Contents::floating_point_xtra : Contents::non_numeric;
					case 'i': 		return ( ( str_ == "inf" ) || ( str_ == "infinity" ) )
										? Contents::floating_point_xtra : Contents::non_numeric;
					case 'I': 		return ( ( str_ == "INF" ) || ( str_ == "INFINITY" ) )
										? Contents::floating_point_xtra : Contents::non_numeric;
					default: 		return Contents::non_numeric;
				}
			}

			auto					itr  = str_.begin();
			const auto				end  = str_.end();

			// Is it an integer?
			const bool				negative = is_negative( itr, end );
			if( itr == end )		return Contents::non_numeric;
			const bool 				pre_integer = is_digits( itr, end );
			if( pre_integer && ( itr == end ) )
									return negative ? Contents::integer : Contents::uinteger;
	
			// Is it a floating point?
			if( *itr != '.' ) {
				const auto			rest = std::basic_string_view< Char, Traits >{ itr, end };
				switch( *itr ) {
					case 'i': 		return ( ( rest == "inf" ) || ( rest == "infinity" ) )
										? Contents::floating_point_xtra : Contents::non_numeric;
					case 'I': 		return ( ( rest == "INF" ) || ( rest == "INFINITY" ) )
										? Contents::floating_point_xtra : Contents::non_numeric;
					default: 		return Contents::non_numeric;
				}
			}
			++itr;
			if( !is_digits( itr, end ) && !pre_integer ) 				// A single '.' is not a floating point.
									return Contents::non_numeric;
			if( itr == end )		return Contents::floating_point;	// Either "d.d", "d.", or ".d".
	
			// Is it a floating point with exponent?
			if( ( *itr != 'e' ) && ( *itr != 'E' ) )					// An ending 'e' is not a floating point.
									return Contents::non_numeric;
			++itr;
			is_negative( itr, end );
			if( itr == end )		return Contents::non_numeric;		// An ending 'e' and a sign is not a floating point.

			if( !is_digits( itr, end ) )								// ... and some garbage.
									return Contents::non_numeric;
			if( itr == end )		return Contents::floating_point;
			return Contents::non_numeric;								// A floating point + garbage.
		}
	
	public:
		constexpr String_numeric() noexcept : m_contents{ Contents::undetermined } {};
		constexpr String_numeric( const std::string_view str_ ) noexcept : m_contents{ contents( str_ ) } {};
	
		constexpr auto view()					const noexcept {	return ids[ unsigned( m_contents ) ];								}
		constexpr bool is_determined()			const noexcept {	return ( m_contents != Contents::undetermined );					}
		constexpr bool is_unsigned_integer()	const noexcept {	return ( m_contents == Contents::uinteger );						}
		constexpr bool is_integer()				const noexcept {	return is_determined() && ( m_contents <= Contents::integer );		}
		
		/// Floating point number, including inf and nan.
		constexpr bool is_floating_point()		const noexcept {	return ( m_contents == Contents::floating_point )
																	|| ( m_contents == Contents::floating_point_xtra );					}
		
		/// Floating point number, doeas contain inf and/or nan.
		constexpr bool is_floating_point_xtra()	const noexcept {	return ( m_contents == Contents::floating_point_xtra );				}

		constexpr bool is_numeric()				const noexcept {	return is_determined() && ( m_contents < Contents::non_numeric );	}
		constexpr bool is_nonnumeric()			const noexcept {	return ( m_contents == Contents::non_numeric );						}

		constexpr String_numeric & operator+=( const String_numeric other_ ) noexcept {
			m_contents = ( m_contents >= other_.m_contents ) ? m_contents : other_.m_contents;
			return *this;
		}

		constexpr friend String_numeric operator+( 
			String_numeric			a_, 
			const String_numeric	b_
		) noexcept {
			return a_ += b_;
		}

		template< typename Stream >
		constexpr friend Stream & operator<<( 
			Stream				  & stream_, 
			const String_numeric	sn_
		) noexcept {
			return stream_ << sn_.view();
		}
	};



	/// Check a string for a number of mainly table related properties. 
	class String_meta {
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
		constexpr String_meta()										= default;
		constexpr String_meta( const String_meta & )				= default;
		constexpr String_meta & operator=( const String_meta & )	= default;
		
		template< typename Ch >
		constexpr String_meta( const std::basic_string_view< Ch > str_ ) noexcept;
		
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
	constexpr String_meta::String_meta( const std::basic_string_view< Ch > str_ ) noexcept	{
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
		const String_meta					count( str_ );
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
