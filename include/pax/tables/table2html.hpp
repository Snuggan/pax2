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
			"	thead {\n"
			"		background-color: #70AD47;\n"
			"		font-weight: bold;\n"
			"		position: sticky;\n"
			"		top: 0;\n"
			"	}\n"
			"	thead tr td {\n"
			"		background-color: #70AD47;\n"
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
		static constexpr std::size_t		td_sz = td.size() + td_.size();
		static constexpr std::string_view	tr	  = "\t<tr>";
		static constexpr std::string_view	tr_	  = "</tr>\n";
		static constexpr std::size_t		tr_sz = tr.size() + tr_.size();

		using pair						  = std::pair< std::string_view, std::string_view >;

		static std::string link( std::string_view link_ ) noexcept {
			link_ = trim( link_, []( char c ){ return c == ' '; } );
			return std::string( "<a href=\"mailto:" ) + link_ + "\">" + link_ + "</a>";
		}

		static void chew_row(
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
				str_ += cols.first.contains( '@' ) ? link( cols.first ) : cols.first;
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
			std::string_view				css_,			///< Css code without <style> tags.
			const bool						add_header_		///< Table if tag, if any.
		) noexcept {
			if( css_.empty() )				css_ = default_css;
			const auto 						meta	  = String_count2( table_ );
			std::string						header{}, body{};
			pair							rows{ {}, table_ };
			
			// Get the header, if requiered.
			if( add_header_ && rows.second.size() ) {
				rows					  = split_by( rows.second, Newline{} );
				header.reserve( rows.first.size() +	tr_sz + meta.cols_in_first()*( td_sz ) );
				chew_row( rows.first, header, meta.col_delimiter(), meta.cols_in_first() );
			}
		
			// Get the body. 
			body.reserve( rows.second.size() + meta.rows()*( tr_sz + meta.cols_in_first()*( td_sz ) ) );
			while( rows.second.size() ) {
				rows					  = split_by( rows.second, Newline{} );
				if( rows.first.size() ) 
					chew_row( rows.first, body, meta.col_delimiter(), meta.cols_in_first() );
			}
		
			// Put together the parts.
			return std::string{}
				+ base[ 0 ] + title_ 
				+ base[ 1 ] + css_
				+ base[ 2 ] + header
				+ base[ 3 ] + body
				+ base[ 4 ];
		}
	};
	
	
	/// Returns the modulo of x_ and y_ (floating point or integer).
	/** If y_ == 0, 0 is returned.																	**/
	inline std::string table2html( 
		const std::string_view			table_,
		const std::string_view			title_,
		const std::string_view			css_ = "",			///< Css code without <style> tags.
		const bool						add_header_ = true	///< Table if tag, if any.
	) noexcept {
		return html_table::table2html( table_, title_, css_, add_header_ );
	}

}	// namespace pax
