//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "parse2table.hpp"			// String_count2
#include "../std/algorithm.hpp"		// split


namespace pax { 
	
	class html_table {
		static constexpr std::string_view base[ 5 ] = {
			"<!doctype html>\n"
			"<html lang=\"se\">\n"
			"<head>\n"
			"<title>",
			"</title>\n"
			"<meta http-equiv=Content-Type content=\"text/html; charset=utf-8\">\n"
			"<style>\n",
			"</style>\n"
			"</head>\n"
			"<body>\n"
			"<table>\n"
			"<thead>\n",
			"</thead>\n"
			"<tbody>\n",
			"</tbody>\n"
			"</table>\n"
			"</body>\n"
			"</html>\n"
		};
		static constexpr std::size_t	base_sz = base[ 0 ].size() + base[ 1 ].size() + base[ 2 ].size() + base[ 3 ].size() + base[ 4 ].size();

		static constexpr std::string_view default_css =
			"	table {\n"
			"		position: relative;\n"
			"		margin: 1em 0;\n"
			"		overflow-x: auto;\n"
			"		border-spacing: 0;\n"
			"		white-space: nowrap;\n"
			"		text-align: left;\n"
			"		font-family: ArialUnicodeMS, arial, sans-serif;\n"
			"		font-variant-numeric: lining-nums tabular-nums;\n"
			"	}\n"
			"	thead tr td {\n"
			"		background-color: #70AD47;\n"
			"		font-weight: bold;\n"
			"		position: sticky;\n"
			"		top: 0;\n"
			"	}\n"
			"	tr {\n"
			"		background-color: #D2DFCA;\n"
			"	}\n"
			"	tr:nth-of-type(odd) {\n"
			"		background-color: #E2EFDA;\n"
			"	}\n"
			"	td {\n"
			"		padding: 8px;\n"
			"		font-variant-numeric: tabular-nums;\n"
			"	}\n"
			;

		static constexpr std::string_view	td	  = "<td>";
		static constexpr std::string_view	td_	  = "</td>";
		static constexpr std::string_view	tr	  = "\t<tr>";
		static constexpr std::string_view	tr_	  = "</tr>\n";

		using pair						  = std::pair< std::string_view, std::string_view >;

		static void process_row(
			const std::string_view			row_, 
			std::string					  & str_, 
			const char						col_delimiter_, 
			int			 					num_col_
		) noexcept {
			str_ += tr;
			pair							cols{ {}, row_ };
			while( cols.second.size() ) {
				cols					  = split_by( cols.second, col_delimiter_ );
				str_ += td;
				str_ += cols.first;
				str_ += td_;
				--num_col_;
			}
			while( --num_col_ >= 0 ) {
				str_ += td;
				str_ += td_;
			}
			str_ += tr_;
		}
		
	public:
		static std::string table2html( 
			const std::string_view			table_,
			const std::string_view			title_,
			std::string_view				css_		///< Css code without <style> tags.
		) noexcept {
			if( css_.empty() )				css_	  = default_css;
			const auto 						meta	  = String_count2( table_ );
			pair							rows{ {}, table_ };
			std::string						body{};
			body.reserve( 
				  base_sz + title_.size() + css_.size() 
				+ table_.size() 
					+ meta.rows()*( tr.size() + tr_.size() + meta.cols_in_first()*( td.size() + td_.size() ) )
					- meta.counts()[ meta.col_delimiter() ]
			);
			body +=	base[ 0 ];
			body +=	title_;
			body +=	base[ 1 ];
			body +=	css_;
			body +=	base[ 2 ];
			
			// Process the header.
			if( rows.second.size() ) {
				rows					  = split_by( rows.second, Newline{} );
				process_row( rows.first, body, meta.col_delimiter(), meta.cols_in_first() );
			}
			body +=	base[ 3 ];
		
			// Process the body. 
			while( rows.second.size() ) {
				rows					  = split_by( rows.second, Newline{} );
				if( rows.first.size() ) 	// Skip empty rows
					process_row( rows.first, body, meta.col_delimiter(), meta.cols_in_first() );
			}
			body += base[ 4 ];
		
			return body;
		}
	};
	
	
	/// Returns the modulo of x_ and y_ (floating point or integer).
	/** If y_ == 0, 0 is returned.																	**/
	inline std::string table2html( 
		const std::string_view			table_,
		const std::string_view			title_,
		const std::string_view			css_ = ""		///< Css code without <style> tags.
	) noexcept {
		return html_table::table2html( table_, title_, css_ );
	}

}	// namespace pax
