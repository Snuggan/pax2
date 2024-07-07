//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "../textual/string_meta.hpp"	// String_meta
#include "../std/algorithm.hpp"			// split


namespace pax { 
	
	class html_table {
		static constexpr std::string_view	td	  = "\t\t<td>";
		static constexpr std::string_view	td_	  = "</td>\n";
		static constexpr std::string_view	tr	  = "<tr>\n";
		static constexpr std::string_view	tr_	  = "\t</tr>";

		using pair						  = std::pair< std::string_view, std::string_view >;

		static void process_row(
			const std::string_view			row_, 
			std::string					  & str_, 
			const char						col_delimiter_, 
			const int			 			num_col_, 
			std::vector< String_numeric > & numeric_
		) noexcept {
			int								num_col{};
			str_ += tr;
			pair							cols{ {}, row_ };
			while( cols.second.size() ) {
				cols					  = split_by( cols.second, col_delimiter_ );
				str_ += td;
				str_ += cols.first;
				str_ += td_;
				if( num_col >= int( numeric_.size() ) )
					numeric_.resize( numeric_.size() + 1 );
				numeric_[ num_col ]+= String_numeric( cols.first );
				++num_col;
			}
			num_col-= num_col_;
			while( ++num_col <= 0 ) {
				str_ += td;
				str_ += td_;
			}
			str_ += tr_;
		}
		
	public:
		static std::string table2html( 
			const std::string_view			table_,
			const std::string_view			title_
		) noexcept {
			const auto 						meta = String_meta( table_ );
			std::vector< String_numeric >	col_types( meta.cols_in_first() );
			pair							rows{ {}, table_ };
			
			// Process the header.
			std::string						header{};
			header.reserve( tr_.size() + meta.cols_in_first()*( 32 + td.size() + td_.size() ) );
			if( rows.second.size() ) {
				rows					  = split_by( rows.second, Newline{} );
				process_row( rows.first, header, meta.col_delimiter(), meta.cols_in_first(), col_types );
			}
		
			// Process the body. 
			std::string						body{};
			body.reserve( 
				+ table_.size() 
				+ meta.rows()*( tr.size() + tr_.size() + meta.cols_in_first()*( td.size() + td_.size() ) )
				- meta.counts()[ meta.col_delimiter() ]
			);
			col_types.clear();
			col_types.resize( meta.cols_in_first() );
			while( rows.second.size() ) {
				rows					  = split_by( rows.second, Newline{} );
				if( rows.first.size() ) 	// Skip empty rows
					process_row( rows.first, body, meta.col_delimiter(), meta.cols_in_first(), col_types );
			}

			// Get numerical columns.
			std::string		css2;
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
				"	:root {{\n"
				"		--bcolor: #70AD47;\n"
				"	}}\n"
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
				"	{}\n"
				"</thead>\n"
				"<tbody>\n"
				"	{}\n"
				"</tbody>\n"
				"</table>\n"
				"</body>\n"
				"</html>\n",
				title_, css2, header, body 
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
