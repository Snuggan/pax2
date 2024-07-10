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

		static std::string process_header(
			const std::string_view				header_, 
			const std::span< String_numeric >	col_types_, 
			const char							col_delimiter_
		) noexcept {
			std::size_t						idx{};
			std::string						str;
			str.reserve( col_types_.size()*64 );
			pair							heads{ {}, header_ };
			while( heads.second.size() ) {
				heads					  = split_by( heads.second, col_delimiter_ );
				str						 += std20::format( td2, col_types_[ idx++ ].view(), heads.first );
			}
			return str;
		}

		static void process_row(
			const std::string_view			row_, 
			std::string					  & str_, 
			const char						col_delimiter_, 
			const std::size_t	 			num_col_, 
			std::vector< String_numeric > & numeric_
		) noexcept {
			std::size_t						idx{};
			pair							cols{ {}, row_ };
			str_ += tr;
			while( cols.second.size() ) {
				cols					  = split_by( cols.second, col_delimiter_ );
				const auto tooltips		  = split_by( cols.first, '|' );	// "<first/cell-contents>|<second/cell-tooltip>"
				str_					 += tooltips.second.empty()
												? std20::format( td1, tooltips.first )
												: std20::format( td2, tooltips.second, tooltips.first );
				if( idx < numeric_.size() )	numeric_[ idx ]+= String_numeric( tooltips.first );
				++idx;
			}
			// Give all rows at least as many columns as in the header.
			while( ++idx <= num_col_ ) 	str_ += td0;
			str_ += tr_;
		}
		
	public:
		static std::string table2html( 
			const std::string_view			table_,
			const std::string_view			title_
		) noexcept {
			const auto 						meta = String_meta( table_ );
			pair							rows = split_by( table_, Newline{} );
			
			// Save the header for later processing.
			const auto						header = rows.first;
		
			// Process the body. 
			std::string						body{};
			body.reserve( 
				+ table_.size() 
				+ meta.rows()*( tr.size() + tr_.size() + meta.cols_in_first()*td2.size() )
				- meta.counts()[ meta.col_delimiter() ]
			);
			std::vector< String_numeric >	col_types( meta.cols_in_first() );
			while( rows.second.size() ) {
				rows					  = split_by( rows.second, Newline{} );
				if( rows.first.size() ) 	// Skip empty rows
					process_row( rows.first, body, meta.col_delimiter(), meta.cols_in_first(), col_types );
			}

			// Get numerical columns and make them right aligned.
			std::string						css2;
			for( std::size_t c{}; c<col_types.size(); ++c )
				if( col_types[ c ].is_numeric() )
					css2+= std20::format( "\ttd:nth-of-type({}) {{ text-align:right; }}\n", c+1 );

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
				title_, css2, process_header( header, col_types, meta.col_delimiter() ), body 
			);
		}
	};
	
	
	/// Returns the modulo of x_ and y_ (floating point or integer).
	/** If y_ == 0, 0 is returned.																	**/
	inline std::string table2html( 
		const std::string_view			table_,
		const std::string_view			title_
	) noexcept {
		return html_table::table2html( table_, title_ );
	}

}	// namespace pax
