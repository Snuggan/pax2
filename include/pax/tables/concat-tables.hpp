//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "../std/format.hpp"
#include "../std/algorithm.hpp"
#include "../std/file.hpp"
#include "../reporting/progress.hpp"
#include "../textual/json.hpp"


namespace pax {

	/// Append table text files, e.g. csv files.
	/** 1. Ignore empty files.
		2. Get Header from first non-empty file.
		3. Check that headers in Other non-empty files equal Header.
			- Unequal: throw an exception.
			- Equal:   append all rows in Other except the header.
		4. (Sort all rows except header.)
		5. Save concatenated files.
	**/
	class Append_tables {
		std::string								m_table{};
		std::filesystem::path					m_main_file{};
		Progress								m_progress{};
		std::size_t 							m_header_size{}, m_empty_files{}, m_used_files{};

	public:
		Append_tables()						  = default;
		Append_tables(
			const bool			verbose_, 
			const std::size_t	count_ = 0
		) noexcept : 
			m_progress{ verbose_ ? textual_progress( "Concatenate files", count_ ) : Progress{} } 
		{}

		/// Return the table as a string. 
		const std::string & str()				const noexcept	{	return m_table;		}

		/// Process an individual table file. 
		void process( const file_path & file_ ) {
			if( !is_file_path::csv( file_ ) )	return;		// Only process csv files. 

			if( std::filesystem::file_size( file_ ) ) {		// Ignore empty files.
				const std::string table		  = read_string( file_ );
				auto [ hdr, rows ]			  = split_by( std::string_view( table ), Newline{} );

				if( !m_header_size ) {
					m_table					  = table;
					m_main_file				  = file_;
					m_header_size			  = hdr.size();
				} else if( ( m_header_size != hdr.size() ) 
						|| ( std::string_view( m_table.data(), m_header_size ) != hdr ) 
				) {
					throw error_message( std20::format(
						"Headers do not match when concatinating table files."
						"\n\tNew file:    '{}'"
						"\n\tNew header:  '{}'"
						"\n\tMain file:   '{}'"
						"\n\tMain header: '{}'",
						to_string( file_ ),       hdr,
						to_string( m_main_file ), std::string_view( m_table.data(), m_header_size )
					) );
				} else {
					m_table					 += rows;
				}

				if( !ends_with( std::string_view( m_table ), Newline{} ) )
					m_table += "\n";
				++m_used_files;
				
			} else {
				++m_empty_files;
			}
			++m_progress;
		}


		/// Process a directory of table files. 
		void process( const dir_path & directory_ ) {
			try {
				for( const std::filesystem::path & file : std::filesystem::recursive_directory_iterator( directory_ ) ) 
					if( is_file_path::is( file ) )		process( file_path( file ) );
			} catch( const std::exception & e_ ) {
				throw error_message( std20::format( "{}\n\tProcessing directory '{}'.", e_.what(), to_string( directory_ ) ) );
			}
		}


		/// Return metadata of result. 
		[[nodiscard]] Json_value json()		const	{
			return Json_value{
				{	"used-files",			m_used_files					},
				{	"empty-files",			m_empty_files					},
				{	"total-files",			m_used_files + m_empty_files	}
			};
		}


		/// Save the plots table to a new csv type of file, similar as the original but with new or updated metrics. 
		/** Returns the number of rows with calculated metrics.		**/
		void save( const dest_path & dest_ )		const	{
			try{
				// Sort? Remove repeated rows?
				stream( dest_, std::string_view( str() ) );
			} catch( const std::exception & e_ ) {
				throw error_message( std20::format( "{}\n\tTrying to save concatenated tables to '{}'.", e_.what(), to_string( dest_ ) ) );
			}
		}
	};
	
	
	Json_value concat_tables(
		const dir_path					  & source_,
		const std::filesystem::path		  & dest_, 
		const bool							verbose_ = false, 
		const std::size_t					count_ = 0
	) {
		Append_tables						concat{ verbose_, count_ };
		concat.process( source_ );
		concat.save( dest_ );
		return concat.json();
	}

}	// namespace pax
