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

	/// Scans a string and determines if it could be interpreted as a numeric.
	/** Recognises the following forms:
		- undetermined 			Default value, no string scanned yet.
		- uinteger 				The string contains only digits.
		- integer 				The string contains '-' followed by only digits.
		- floating_point 		The string is in either form: "d.d", "d.", or ".d", possibly after a leading sign.
		- floating_point_xtra	Same as floating_point, but also includes nan, inf, etc.
		- non_numeric			Can not be interpreted as a numeric. 
	**/
	class Strtype {
		enum class Type {
			undetermined, 			// Default value, no string scanned yet.
			uinteger, 				// The string contains only digits.
			integer, 				// The string contains '-' followed by only digits.
			floating_point, 		// The string is in either form: "d.d", "d.", or ".d", possibly after a leading sign.
			floating_point_xtra,	// Same as floating_point, but also includes nan, inf, etc.
			non_numeric,			// Can not be interpreted as a numeric. 
		};
		Type	m_type{ Type::undetermined };

		// Scan through digits and return true if there was at least one. 
		template< typename Itr >
		static constexpr bool digits( Itr & itr_, const Itr end_ ) noexcept {
			if(    (   itr_ == end_ ) || ( *itr_ <  '0' ) || ( *itr_ >  '9' ) )	return false;	// No digit.
			while( ( ++itr_ != end_ ) && ( *itr_ >= '0' ) && ( *itr_ <= '9' ) );				// Scan all digits.
			return true;																		// At least one digit.
		}

		template< typename Char, typename Traits >
		static constexpr Type scan( const std::basic_string_view< Char, Traits > str_ ) noexcept {
			static constexpr Type	float_or_not[ 2 ]      = { Type::non_numeric, Type::floating_point };
			static constexpr Type	float_xtra_or_not[ 2 ] = { Type::non_numeric, Type::floating_point_xtra };

			if( str_.empty() )	return Type::non_numeric;
			auto				itr{ str_.begin() };
			const auto			end{ str_.end() };
			bool				negative{ false };
			bool 				predigit{ true };
			switch( *itr ) {
				case '.': 		predigit = false;
								break;	// Possibly a float. 

				case '-': 		negative = true;
								[[fallthrough]];
				case '+': 		if( ++itr == end )	return Type::non_numeric;
								{
									const auto		rest{ std::basic_string_view< Char, Traits >{ itr, end } };
									switch( *itr ) {
										case 'i': 	return float_xtra_or_not[ ( rest == "inf" ) || ( rest == "infinity" ) ];
										case 'I': 	return float_xtra_or_not[ ( rest == "INF" ) || ( rest == "INFINITY" ) ];
										case '.': 	predigit = false;						break;
										default:	if( ( *itr < '0' ) || ( *itr > '9' ) )	return Type::non_numeric;
									}
								}
								[[fallthrough]];
				case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': 
								if( digits( itr, end ) && ( itr == end ) )
									return negative ? Type::integer : Type::uinteger;
								break;

				case 'n': 		return float_xtra_or_not[   str_ == "nan"   ];
				case 'N': 		return float_xtra_or_not[ ( str_ == "NAN" ) || ( str_ == "NaN"      ) ];
				case 'i': 		return float_xtra_or_not[ ( str_ == "inf" ) || ( str_ == "infinity" ) ];
				case 'I': 		return float_xtra_or_not[ ( str_ == "INF" ) || ( str_ == "INFINITY" ) ];
				default: 		return Type::non_numeric;
			}

			// itr now points at '.' -- is str_ a float?
			++itr;
			return float_or_not[
					(  ( digits( itr, end ) || predigit        ) && (   itr == end ) )	// Float: either "d.d", "d.", or ".d" maybe...
				||	(	( ( ( *itr == 'e' ) || ( *itr == 'E' ) ) && ( ++itr != end ) )	// ...with an exponent: an 'e' followed by...
					&&	( ( ( *itr != '-' ) && ( *itr != '+' ) ) || ( ++itr != end ) )	// ...an optional sign...
					&&	( digits( itr, end )                     && (   itr == end ) )	// ...and digits and nothing else.
					)
			];
		}
	
	public:
		constexpr Strtype()									noexcept = default;
		constexpr Strtype( const Strtype & )				noexcept = default;
		constexpr Strtype & operator=( const Strtype & )	noexcept = default;
		constexpr Strtype( const std::string_view str_ )	noexcept : m_type{ scan( str_ ) } {};
	
		constexpr auto view()					const noexcept {
			static constexpr std::string_view ids[ int( Type::non_numeric ) + 1 ] = 
				{ "undetermined", "unsigned integer", "integer", "floating point", "floating point (inf/NaN)", "textual" };
			return ids[ unsigned( m_type ) ];
		}
		constexpr bool is_determined()			const noexcept {	return ( m_type != Type::undetermined );					}
		constexpr bool is_unsigned_integer()	const noexcept {	return ( m_type == Type::uinteger );						}
		constexpr bool is_integer()				const noexcept {	return is_determined() && ( m_type <= Type::integer );		}
		
		/// Floating point number, including inf and nan.
		constexpr bool is_floating_point()		const noexcept {	return ( m_type == Type::floating_point )
																		|| ( m_type == Type::floating_point_xtra );				}
		
		/// Floating point number, doeas contain inf and/or nan.
		constexpr bool is_floating_point_xtra()	const noexcept {	return ( m_type == Type::floating_point_xtra );				}

		constexpr bool is_numeric()				const noexcept {	return is_determined() && ( m_type < Type::non_numeric );	}
		constexpr bool is_nonnumeric()			const noexcept {	return ( m_type == Type::non_numeric );						}

		constexpr Strtype & operator+=( const Strtype other_ ) noexcept {
			m_type = ( m_type >= other_.m_type ) ? m_type : other_.m_type;
			return *this;
		}

		constexpr friend Strtype operator+( 
			Strtype			a_, 
			const Strtype	b_
		) noexcept {
			return a_ += b_;
		}

		template< typename Stream >
		constexpr friend Stream & operator<<( 
			Stream				  & stream_, 
			const Strtype	sn_
		) noexcept {
			return stream_ << sn_.view();
		}
	};



	/// Check a string for a number of mainly table related properties. 
	class String_meta {
		/// Counter that counts totel number and min/max in rows.
		class Count_by_row {
			using Size = std::size_t;
			Size 		m_row{}, m_total{}, 
						m_row_min{ std::numeric_limits< Size >::max() }, 
						m_row_max{ std::numeric_limits< Size >::lowest() };
	
		public:
			constexpr Count_by_row()				noexcept		{};

			/// The total count (all rows).
			[[nodiscard]] constexpr Size total()	const noexcept	{				return m_total;						}

			/// The minimum count in one row (all rows).
			[[nodiscard]] constexpr Size row_min()	const noexcept	{				return m_row_min;					}

			/// The maximum count in one row (all rows).
			[[nodiscard]] constexpr Size row_max()	const noexcept	{				return m_row_max;					}

			/// Increase count by one. 
			constexpr Count_by_row & operator++()	noexcept		{	++m_row;	return *this;						}

			/// Set all counters to zero. 
			constexpr void zero()					noexcept		{	m_row_min = m_row_max = m_total = m_row = {};	}

			/// Update row statistics. 
			constexpr void row_end()				noexcept		{
				( m_row < m_row_min ) ? ( m_row_min = m_row ) : m_row;
				( m_row > m_row_max ) ? ( m_row_max = m_row ) : m_row;
				m_total		 += m_row;
				m_row		  = {};
			}

			/// Output the counter. 
			template< typename Out >
			friend constexpr Out & operator<<( Out & out_, const Count_by_row counter_ ) {
				return out_ << "{" << counter_.total() << " (" << counter_.row_min() << '-' << counter_.row_max() << ")}";
			}

			/// Return metadata of result. 
			template< typename Json >
			[[nodiscard]] friend Json json( const Count_by_row counter_ ) {
				return Json{
					{	"total",	counter_.total()		},
					{	"row min",	counter_.row_min()		},
					{	"row max",	counter_.row_max()		}
				};
			}
		};

		using Count				  = std::size_t;
		static constexpr Count		Asciis = 128;
		static constexpr Count		Non_ascii = Asciis;

		Count_by_row				m_count_by_row[ Non_ascii + 1 ]={};
		Count		 				m_rows{}, m_non_empty_rows{}, m_glyphs{}, m_cols_in_first_row{};
		char						m_col_delimit = ';';
		
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
		
		template< typename Ch, typename Traits >
		constexpr String_meta( const std::basic_string_view< Ch, Traits > str_ ) noexcept;
		
		/// The total number of glyphs.
		constexpr auto size()						const noexcept	{	return m_glyphs;								}

		/// Count_by_row for glyph c. Returns an empty Count_by_row if c > 127.
		constexpr auto statistics( const Count c )	const noexcept	{
			return ( c < Asciis ) ? m_count_by_row[ c ] : Count_by_row{};
		}

		/// The total number of glyph c. Returns zero if c > 127.
		constexpr auto operator[]( const Count c )	const noexcept	{	return statistics( c ).total();						}

		/// The total number of non-ascii characters (code > 127). 
		constexpr auto non_ascii()					const noexcept	{	return m_count_by_row[ Non_ascii ].total();			}

		/// The total number of ascii characters (<128). 
		constexpr auto ascii()						const noexcept	{	return size() - non_ascii();						}
		
		/// The total number of rows.
		constexpr auto rows()						const noexcept	{	return m_rows;										}
		
		/// The total number of empty rows.
		constexpr auto non_empty_rows()				const noexcept	{	return m_non_empty_rows;							}

		/// The [probable] column delimiter (the most frequent of "\t!#$%@,;.:"). 
		constexpr auto col_delimiter()				const noexcept	{	return m_col_delimit;								}
		
		/// The number of columns in first row. Note: an empty row has one [empty] column!
		constexpr auto cols_in_first()				const noexcept	{	return m_cols_in_first_row;							}
		
		/// The minimum number of columns in a row.
		constexpr auto minimum_cols()				const noexcept	{	return 1 + statistics( col_delimiter() ).row_min();	}
		
		/// The maximum number of columns in a row.
		constexpr auto maximum_cols()				const noexcept	{	return 1 + statistics( col_delimiter() ).row_max();	}
	};


	template< typename Ch, typename Traits >
	constexpr String_meta::String_meta( const std::basic_string_view< Ch, Traits > str_ ) noexcept	{
		for( auto row : String_splitter( str_, Newline{} ) ) {	// Iterate str_ row by row.
			if( row.size() ) {									// Ignore empty rows.
				for( const unsigned c : row )			++m_count_by_row[ ( c < Asciis ) ? c : Non_ascii ];
				for( auto & cnt : m_count_by_row )		cnt.row_end();
				++m_non_empty_rows;
			}
			++m_rows;
		}
		if( m_non_empty_rows == 0 ) 
			for( auto & cnt : m_count_by_row )			cnt.zero();

		// Find the probable column delimiter.
		constexpr const std::string_view	candidates = "\t!#$%@,;.:";
		for( const short c : candidates )
			m_col_delimit = ( statistics( c ).total() > statistics( m_col_delimit ).total() ) ? c : m_col_delimit;

		m_glyphs						  = str_.size();
		m_cols_in_first_row				  = maximum_cols() ? count( until( str_, Newline{} ), col_delimiter() ) + 1 : 0;
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
