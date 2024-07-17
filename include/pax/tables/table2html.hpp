//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "../textual/string_meta.hpp"	// String_meta
#include "../std/algorithm.hpp"			// split


namespace pax { 
	
	class html_table {
		static constexpr std::string_view	td0		= "\t\t<td></td>";
		static constexpr std::string_view	td1		= "\t\t<td>{} </td>\n";
		static constexpr std::string_view	td2		= "\t\t<td title=\"{}\">{} </td>\n";
		static constexpr std::string_view	tr	  	= "<tr>\n";
		static constexpr std::string_view	tr_	  	= "\t</tr>";

		using pair						  = std::pair< std::string_view, std::string_view >;
		using splitter					  = String_splitter< const char, char >;

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
			
			// Process the body. 
			std::string						body{};
			body.reserve( 
				+ table_.size() 
				+ meta.rows()*( tr.size() + tr_.size() + meta.cols_in_first()*td2.size() )
				- meta[ meta.col_delimiter() ]
			);
			std::vector< Strtype >			col_types( meta.cols_in_first() );

			for( const auto row : String_splitter( rows, Newline{} ) ) // Iterate row by row.
				if( row.size() ) 	// Skip empty rows
					process_row( String_splitter( row, meta.col_delimiter() ), body, meta.cols_in_first(), col_types );

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
					"  Rows:             {}\n"
					"  Columns:          {}\n",
					title_, 
					meta.col_delimiter(), 
					( meta.rows() == meta.non_empty_rows() )
						? std20::format( "{}", meta.rows() )
						: std20::format( "{} ({} non-empty)", meta.rows(), meta.non_empty_rows() ), 
					( ( meta.minimum_cols() == meta.maximum_cols() )
						? std20::format( "{}", meta.minimum_cols() )
						: std20::format( "min {}, max {}", meta.minimum_cols(), meta.maximum_cols() )
					) + ( ( ( meta.cols_in_first() == meta.maximum_cols() ) && ( meta.cols_in_first() == meta.maximum_cols() ) )
						? std::string{}
						: std20::format( " ({} in header)", meta.cols_in_first() )
					)
				);
				std::size_t 	i{};
				for( const auto col : String_splitter( header, meta.col_delimiter() ) ) 
					std::cerr << std20::format( "    {:15?} {}\n", col, col_types[ i++ ].view() );
			}

			// Collect the result.
			return std20::format( 
				"<!doctype html>\n"
				"<html lang=\"se\">\n"
				"<head>\n"
				"<title>{}</title>\n"
				"<meta http-equiv=Content-Type content=\"text/html; charset=utf-8\">\n"
				"<style>\n"
				"	body {{\n"
				"		margin: 0;\n"
				"	}}\n"
				"	table {{\n"
				"		position: relative;\n"
				"		overflow-x: auto;\n"
				"		border-spacing: 0;\n"
				"		white-space: nowrap;\n"
				"		text-align: left;\n"
				"		font-family: ArialUnicodeMS, arial, sans-serif;\n"
				"		font-variant-numeric: lining-nums tabular-nums;\n"
				"	}}\n"
				"	thead tr td {{\n"
				"		background-color: #70AD47;\n"
				"		font-weight: bold;\n"
				"		position: sticky;\n"
				"		top: 0;\n"
				"	}}\n"
				"	tr {{\n"
				"		background-color: #D2DFCA;\n"
				"	}}\n"
				"	tr:nth-of-type(odd) {{\n"
				"		background-color: #E2EFDA;\n"
				"	}}\n"
				"	td {{\n"
				"		padding: 6px;\n"
				"		font-variant-numeric: tabular-nums;\n"
				"	}}\n"
				"{}"
				"</style>\n"
				"<script src=\"https://www.kryogenix.org/code/browser/sorttable/sorttable.js\"></script>\n"
				"</head>\n"
				"<body>\n"
				"<table class=\"sortable\">\n"
				"<thead>\n"
				"	<tr>\n"
				"{}"
				"	</tr>\n"
				"</thead>\n"
				"<tbody>\n"
				"	{}\n"
				"</tbody>\n"
				"</table>\n"
				"</body>\n"
				"</html>\n",
				title_, css2, process_header( String_splitter( header, meta.col_delimiter() ), col_types ), body 
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
