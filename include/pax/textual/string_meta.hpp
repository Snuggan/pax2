//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "html-table-template.hpp"	// pax::html_table_template
#include "../std/string_view.hpp"	// pax::split
#include "../reporting/error_message.hpp"

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
			static constexpr Type	uint_or_not[ 2 ]		= { Type::integer,     Type::uinteger };
			static constexpr Type	float_or_not[ 2 ]		= { Type::non_numeric, Type::floating_point };
			static constexpr Type	float_xtra_or_not[ 2 ]	= { Type::non_numeric, Type::floating_point_xtra };

			if( str_.empty() )	return Type::non_numeric;
			auto				itr{ str_.begin() };
			const auto			end{ str_.end() };
			bool				positive{ true  };	// Will be false if the first character is '-'.
			bool				predigit{ true  };	// Will be false if a '.' does nog have a preceding digit.
			switch( *itr ) {
				case '-': 		positive = false;
				[[fallthrough]];
				case '+': 		if( ++itr == end )	return Type::non_numeric;
								{
									const auto		rest{ std::basic_string_view< Char, Traits >{ itr, end } };
									switch( *itr ) {
										case 'i': 	return float_xtra_or_not[ ( rest == "inf" ) || ( rest == "infinity" ) ];
										case 'I': 	return float_xtra_or_not[ ( rest == "INF" ) || ( rest == "INFINITY" ) ];
										case '.': 	predigit = false;			break;	// Possibly the beginning of a float.
										default:	if( ( *itr < '0' ) || ( *itr > '9' ) )	return Type::non_numeric;
									}
								}
				[[fallthrough]];
				case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': 
								if( digits( itr, end ) && ( itr == end ) )		return uint_or_not[ positive ];	// An integer.
								break;

				case 'n': 		return float_xtra_or_not[   str_ == "nan"   ];
				case 'N': 		return float_xtra_or_not[ ( str_ == "NAN" ) || ( str_ == "NaN"      ) ];
				case 'i': 		return float_xtra_or_not[ ( str_ == "inf" ) || ( str_ == "infinity" ) ];
				case 'I': 		return float_xtra_or_not[ ( str_ == "INF" ) || ( str_ == "INFINITY" ) ];
				case '.': 		predigit = false;		break;	// Possibly the beginning of a float. 
				default: 		return Type::non_numeric;
			}

			// Is str_ a float?
			if( *itr != '.' )	return Type::non_numeric;
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
			using			 Size = std::size_t;
			static constexpr Size	maxsz = std::numeric_limits< Size >::max();
			Size 					m_row{}, m_total{}, m_min{ maxsz }, m_max{};
	
		public:
			constexpr Count_by_row()									noexcept {};
			constexpr Count_by_row( const Count_by_row & )				noexcept = default;
			constexpr Count_by_row & operator=( const Count_by_row & )	noexcept = default;

			/// Has the instance recieved at least one value?
			[[nodiscard]] constexpr bool valid()	const noexcept	{	return m_min != maxsz;					}

			/// The total count (all rows).
			[[nodiscard]] constexpr Size total()	const noexcept	{	return m_total;							}

			/// The minimum count in one row (all rows).
			[[nodiscard]] constexpr Size min()		const noexcept	{	return valid() ? m_min : Size{};		}

			/// The maximum count in one row (all rows).
			[[nodiscard]] constexpr Size max()		const noexcept	{	return m_max;							}

			/// Increase count by one. 
			constexpr Count_by_row & operator++()	noexcept		{	++m_row;	return *this;				}

			/// Update row statistics. 
			constexpr void row_end()				noexcept		{
				( m_row < m_min ) ? ( m_min = m_row ) : m_row;
				( m_row > m_max ) ? ( m_max = m_row ) : m_row;
				m_total		 += m_row;
				m_row		  = {};
			}

			/// Output the counter. 
			template< typename Out >
			friend constexpr Out & operator<<( Out & out_, const Count_by_row counter_ ) {
				return out_ << "{" << counter_.total() << " (" << counter_.min() << '-' << counter_.max() << ")}";
			}

			/// Return metadata of result. 
			template< typename Json >
			[[nodiscard]] friend Json json( const Count_by_row counter_ ) {
				return Json{
					{	"total",	counter_.total()	},
					{	"row min",	counter_.min()		},
					{	"row max",	counter_.max()		}
				};
			}
		};

		using Count				  = std::size_t;
		static constexpr Count		Asciis = 128;
		static constexpr Count		Non_ascii = Asciis;

		Count_by_row				m_count_by_row[ Non_ascii + 1 ] = {};
		Count		 				m_rows{}, m_non_empty_rows{}, m_glyphs{}, m_cols_in_first_row{};
		char						m_col_delimit = ';';
		
	public:
		constexpr String_meta()													= default;
		constexpr String_meta( const String_meta & )							= default;
		constexpr String_meta & operator=( const String_meta & )				= default;
		constexpr String_meta( String_meta && )						noexcept	= default;
		constexpr String_meta & operator=( String_meta && )			noexcept	= default;
		
		template< typename Ch, typename Traits >
		constexpr String_meta( const std::basic_string_view< Ch, Traits > str_ ) noexcept
			: m_glyphs{ str_.size() }
		{
			for( auto row : String_view_splitter( str_, Newline{} ) ) {	// Iterate str_ row by row.
				if( row.size() ) {										// Ignore empty rows.
					for( const unsigned c : row )			++m_count_by_row[ ( c < Asciis ) ? c : Non_ascii ];
					for( auto & cnt : m_count_by_row )		cnt.row_end();
					++m_non_empty_rows;
				}
				++m_rows;
			}

			// Find the probable column delimiter among the candidates.
			for( const short c : "\t!#$%@,;.:" )
				m_col_delimit = ( m_count_by_row[ c ].total() > m_count_by_row[ unsigned( m_col_delimit ) ].total() ) 
					? c : m_col_delimit;

			// Count columns in first row. (There is at least one column.) 
			++m_cols_in_first_row;
			const auto row = until( str_, Newline{} );
			for( const Ch c : row )
				if( c == m_col_delimit )			++m_cols_in_first_row;
		}
		
		/// The total number of glyphs.
		constexpr auto size()						const noexcept	{	return m_glyphs;								}

		/// Count_by_row for glyph c. Returns an empty Count_by_row if c > 127.
		constexpr auto statistics( const Count c )	const noexcept	{
													return ( c < Asciis ) ? m_count_by_row[ c ] : Count_by_row{};		}

		/// The total number of glyph c. Returns zero if c > 127.
		constexpr auto operator[]( const Count c )	const noexcept	{	return statistics( c ).total();					}

		/// The total number of non-ascii characters (code > 127). 
		constexpr auto non_ascii()					const noexcept	{	return m_count_by_row[ Non_ascii ].total();		}

		/// The total number of ascii characters (<128). 
		constexpr auto ascii()						const noexcept	{	return size() - non_ascii();					}
		
		/// The total number of rows.
		constexpr auto rows()						const noexcept	{	return m_rows;									}
		
		/// The total number of empty rows.
		constexpr auto non_empty_rows()				const noexcept	{	return m_non_empty_rows;						}

		/// The [probable] column delimiter (the most frequent of "\t!#$%@,;.:"). 
		constexpr auto col_delimiter()				const noexcept	{	return m_col_delimit;							}
		
		/// The number of columns in first row. Note: an empty row has one [empty] column!
		constexpr auto cols_in_first()				const noexcept	{	return m_cols_in_first_row;						}
		
		/// The minimum number of columns in a row.
		constexpr auto minimum_cols()				const noexcept	{	return 1 + statistics( col_delimiter() ).min();	}
		
		/// The maximum number of columns in a row.
		constexpr auto maximum_cols()				const noexcept	{	return 1 + statistics( col_delimiter() ).max();	}
	};



	/// Parse a string into a table. Throws, if not all rows have the same number of columns.
	template< typename Ch, typename Traits >
	std::tuple<
		std::vector< std::basic_string_view< Ch, Traits > >,	///< All cells.
		std::size_t, 											///< Number of columns per row.
		Ch														///< Column separator mark.
	> parse2table( const std::basic_string_view< Ch, Traits > str_ ) {
		const String_meta					count( str_ );		// Get the string metadata. 
		
		// Check if it is ok as a table.
		if(	count.minimum_cols() != count.maximum_cols() )		// We have rows with varying number of cols...
			throw user_error_message( std20::format( 
				"parse2table: Varying number of columns (smallest {}, largest {}).", 
				count.minimum_cols(), 
				count.maximum_cols()
			) );

		// Read the columns, row by row.
		std::vector< std::basic_string_view< Ch, Traits  > >	result;
		result.reserve( count.non_empty_rows() * count.cols_in_first() );
		for( const auto row : String_view_splitter( str_, Newline{} ) )  					// Iterate row by row.
			if( row.size() ) 				 												// Skip empty rows
				for( const auto cell : String_view_splitter( row, count.col_delimiter() ) )	// Iterate row cell by cell.
					result.push_back( cell );

		return { 
			result, 
			count.cols_in_first(), 
			count.col_delimiter()
		};
	}



	/// Tool to convert a text table to a html table.
	class html_table {
		static constexpr std::string_view	td0		= "\t\t<td></td>";
		static constexpr std::string_view	td1		= "\t\t<td>{} </td>\n";
		static constexpr std::string_view	td2		= "\t\t<td title=\"{}\">{} </td>\n";
		static constexpr std::string_view	tr	  	= "<tr>\n";
		static constexpr std::string_view	tr_	  	= "\t</tr>";

		using pair						  = std::pair< std::string_view, std::string_view >;
		using splitter					  = String_view_splitter< const char, char >;

		static std::string process_header(
			const splitter					header_splitter_, 
			const std::span< Strtype >		col_types_
		) noexcept {
			std::size_t						idx{};
			std::string						str;
			str.reserve( col_types_.size()*64 );
			for( const auto cell : header_splitter_ ) 	// Iterate cell by cell along the header row.
				str						 += std20::format( td2, col_types_[ idx++ ].view(), cell );
			return str;
		}

		// Inner part of main loop.
		static void process_row(
			const splitter					row_splitter_, 
			std::string					  & str_, 
			const std::size_t	 			num_col_, 
			std::vector< Strtype >		  & numeric_
		) noexcept {
			std::size_t						idx{};
			str_ += tr;
			for( const auto cell : row_splitter_ ) {	// Iterate cell by cell along one row.
				const auto tooltips		  = split_by( cell, '|' );	// "<first/cell-contents>|<second/cell-tooltip>"
				str_					 += tooltips.second.empty()
												? std20::format( td1, tooltips.first )
												: std20::format( td2, tooltips.second, tooltips.first );
				if( ( idx < numeric_.size() ) && !numeric_[ idx ].is_nonnumeric() )
					numeric_[ idx ]+= Strtype( tooltips.first );
				++idx;
			}
			while( ++idx <= num_col_ )		str_ += td0;
			str_ += tr_;
		}
		
	public:
		static std::string table2html( 
			const std::string_view			table_,
			const std::string_view			title_,
			const bool						metadata2cout_
		) noexcept {
			const auto 						meta = String_meta( table_ );
			const auto 						[ header, rows ] = split_by( table_, Newline{} );
			
			std::string						body{};
			body.reserve( 
				+ table_.size() 
				+ meta.rows()*( tr.size() + tr_.size() + meta.cols_in_first()*td2.size() )
				- meta[ meta.col_delimiter() ]
			);
			std::vector< Strtype >			col_types( meta.cols_in_first() );

			// Process the body. 
			for( const auto row : String_view_splitter( rows, Newline{} ) ) // Iterate row by row.
				if( row.size() ) 	// Skip empty rows
					process_row( String_view_splitter( row, meta.col_delimiter() ), body, meta.cols_in_first(), col_types );

			// Make numerical columns right aligned.
			std::string						css2;
			for( std::size_t c{}; c<col_types.size(); ++c ) {
				if( col_types[ c ].is_numeric() )
					css2+= std20::format( "\ttd:nth-of-type({}) {{ text-align:right; }}\n", c+1 );
			}

			// Output metadata for the table, if required.
			if( metadata2cout_ ) {
				std::cerr << std20::format( 
					"\nTable: \"{}\"\n"
					"  Column separator: {:?}\n"
					"  Rows:             {}{}\n"
					"  Columns:          {}\n",
					title_, 
					meta.col_delimiter(), 
					meta.rows(), ( meta.rows() == meta.non_empty_rows() ) 
						? std::string{} : std20::format( " ({} non-empty)", meta.non_empty_rows() ), 
					( ( meta.minimum_cols() == meta.maximum_cols() )
						? std20::format( "{}", meta.minimum_cols() )
						: std20::format( "min {}, max {} ({} in header)", 
							meta.minimum_cols(), meta.maximum_cols(), meta.cols_in_first() ) )
				);
				std::size_t 	i{};
				for( const auto col : String_view_splitter( header, meta.col_delimiter() ) ) 
					std::cerr << std20::format( "    {:15?} {}\n", col, col_types[ i++ ].view() );
			}

			// Collect the result.
			return std20::format( html_table_template,
				title_, css2, process_header( String_view_splitter( header, meta.col_delimiter() ), col_types ), body 
			);
		}
	};

	/// Returns the modulo of x_ and y_ (floating point or integer).
	/** If y_ == 0, 0 is returned.																	**/
	inline std::string table2html( 
		const std::string_view			table_,
		const std::string_view			title_,
		const bool						metadata2cout_ = true
	) noexcept {
		return html_table::table2html( table_, title_, metadata2cout_ );
	}

}	// namespace pax
