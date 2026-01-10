//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "header.hpp"
#include "table.hpp"

#include "../textual/string_meta.hpp"
#include "../textual/from_string.hpp"
#include "../std/file.hpp"
#include "../reporting/error_message.hpp"
#include "../external/type_name_rt.hpp"		// type_name_rt
#include "../meta/class-meta.hpp"

#include <span>
#include <sstream>


namespace pax {
	
	/// Always returns true.
	/// Useful in calls with required predicate but you don't need one: use always_true. 
	static constexpr auto always_true = []( std::size_t ){ return true; };



	/** Text_table principles
		- Read a text file into one text string.
		- Organises a table of string views into the string (and an vector of string views of the header). 
		– A cell is string view. 
		– The content strings will not change -- a vector of them is maintained. 
		- Table< T > object: 
			+ Constructed from a span< string_view >. 
			+ Return cell reference, row reference (std::span), col reference (striding iterator). 
		– Text_table< traits::character >:
			+ Based on Table_reference. 
			+ Stream contents, also with a predicate( index ). 
	**/
	template< traits::character Ch >
	class Text_table {
		using element_type				  = Ch;
		using value_type				  = std::remove_cv_t< element_type >;
		using view_type					  = std::basic_string_view< value_type >;
		using string_type				  = std::basic_string< value_type >;
		using Size						  = std::size_t;

		Table< view_type >					m_table;
		std::vector< string_type >			m_data;
		Header< value_type >				m_header;
		value_type							m_col_mark{ ';' };


		template< typename T, typename ...U, typename Predicate, Size ...I >
			requires( std::is_invocable_r_v< bool, Predicate, Size > )
		constexpr std::vector< T > export_values_impl( 
			const class_meta< T, U... >	  & meta_,
			Predicate					 && pred_,
			std::integer_sequence< Size, I... >
		) const {
			const auto						idxs = m_header.index( meta_.names() );
			Size		 					r{};	// The predicator needs the row number.
			std::vector< T >				result;
			result.reserve( m_table.rows() );
			try {									// Create and push a T value for each row.
				for( auto itr{ m_table.begin() }; r < m_table.rows(); ++r, itr+= m_table.cols() ) 
					if( pred_( r ) )		result.emplace_back( from_string< U...[ I ] >( itr[ idxs[ I ] ] )... );
			} catch( const std::exception & error_ ) {
				throw error_message( std20::format( 
					"{} (Text_table row {}: Creating type '{}' from columns {}.)", 
					error_.what(), r+1, type_name_rt< T >, meta_.names() ) );
			}
			return result;
		}


	public:
		constexpr Text_table()								=	default;
		constexpr Text_table( Text_table && )				=	default;
		constexpr Text_table & operator=( Text_table && )	=	default;

		/// Construct from a temporary string.
		constexpr Text_table( std::basic_string< Ch > && str_ ) {
			m_data.push_back( std::move( str_ ) );

			// We *really* want to avoid small string optimization (sso). 
			// A typical std::string object is 24 bytes: pointer, size, and capacity are 8 bytes each. 
			// With sso the glyphs are stored in the actual std::string, replacing pointer, size, and capacity. 
			// A consequence is that moving a small string invalidades iterators and string views. 
			// As this object requires "static" strings, we need to avoid moving glyphs from "inside" the string 
			// to the heap already from the start by reserving more than sizeof( std::string ).
			// A good traitise on sso: https://devblogs.microsoft.com/oldnewthing/20230803-00/?p=108532
			m_data.front().reserve( sizeof( std::string ) + 8 );

			const auto[ cells, cols, col_mark ] = parse2table( std::basic_string_view( m_data.front() ) );
			const std::span 				cells_ref{ cells };
			m_header					  = Header{ cells_ref.subspan( 0, cols ) };
			m_table						  = Table< view_type >{ cells_ref.subspan( cols ), cols };
			m_col_mark 					  = col_mark;
		}

		/// Construct from file contents.
		explicit Text_table( const std::filesystem::path & path_ ) : Text_table{ read_string( path_ ) } {}


		/// Dimensions.
		constexpr auto rows()							const noexcept	{	return m_table.rows();			}
		constexpr auto cols()							const noexcept	{	return m_table.cols();			}
		constexpr auto size()							const noexcept	{	return m_table.size();			}
		
		/// Access the cell at row r_, column c_.
		constexpr auto operator[]( Size r_, Size c_ )	const			{	return m_table[ r_, c_ ];		}
		
		/// Access the header row.
		constexpr const auto & header()					const noexcept	{	return m_header;				}
		
		/// Get row r_ as a span.
		constexpr auto row( const Size r_ )				const noexcept	{	return m_table.row( r_ );		}
		
		/// Get iterators to all cells. 
		constexpr auto begin()							const noexcept	{	return m_table.begin();			}
		constexpr auto end()							const noexcept	{	return m_table.end();			}
		
		/// Get iterators to all cells in row r_. 
		constexpr auto begin_row( const Size r_ )		const noexcept	{	return m_table.begin_row( r_ );	}
		constexpr auto end_row( const Size r_ )			const noexcept	{	return m_table.end_row( r_ );	}
		
		/// Get iterators to all cells in column c_. 
		constexpr auto begin_col( const Size c_ )		const noexcept	{	return m_table.begin_col( c_ );	}
		constexpr auto end_col( const Size c_ )			const noexcept	{	return m_table.end_col( c_ );	}


		/// Fuse other_ into this by adding the other_ as columns.
		/// - The tables must have the same number of rows. 
		///	– If there are columns with the same id, the column from other_ will replace the old column.
		constexpr void insert_cols( Text_table && other_ ) {
			if( m_table.rows() != other_.m_table.rows() )	throw error_message( std20::format( 
				"Text_table: The tables must have the equal number of rows, but {} (this) != {} (other).", 
				m_table.rows(), other_.m_table.rows() ) );
			
			// Copy columns with same existing id, save index of others.
			std::vector< Size > 			cols_to_add{};
			for( Size i{}; i<other_.m_table.cols(); ++i ) {
				const auto c			  = m_header.add( other_.m_header[ i ] );
				if( c < m_table.cols() )	std::copy_n( other_.m_table.begin_col( i ), m_table.rows(), m_table.begin_col( c ) );
				else						cols_to_add.push_back( i );
			}
			
			// Add the columns with new header. 
			Size		 					c{ m_table.cols() };
			m_table.resize( m_table.rows(), c + cols_to_add.size() );
			for( const Size j : cols_to_add ) {
				std::copy_n( other_.m_table.begin_col( j ), m_table.rows(), m_table.begin_col( c ) );
				++c;
			}

			// Move the the items (strings) from other_.m_data to push into *this.
			for( string_type & str : other_.m_data )
				m_data.push_back( std::move( str ) );

			// Set other_ to the regular empty state.
			other_						  = Text_table< value_type >{};
		}


		/// Return a vector with T, using meta_ for constructor types and column names.
		/// - All meta_.names() must be in the header. 
		template< typename Predicate = const decltype( always_true ) &, typename T, typename ...U >
			requires( std::is_invocable_r_v< bool, Predicate, Size > )
		constexpr std::vector< T > export_values( 
			const class_meta< T, U... >	  & meta_,
			Predicate					 && pred_ = always_true
		) const {
			return export_values_impl( meta_, pred_, std::make_index_sequence< sizeof ...( U ) >{} );
		}


		/// Return a vector with T, formed from the column col_id_.
		/// - col_id_ must be in the header. 
		template< typename T, traits::string Str, typename Predicate = const decltype( always_true ) & >
			requires( std::is_invocable_r_v< bool, Predicate, Size > )
		constexpr std::vector< T > export_values( 
			const Str					  & col_id_,
			Predicate					 && pred_ = always_true
		) const {
			return export_values_impl( class_meta< T, T >{ col_id_ }, pred_, std::make_index_sequence< 1 >{} );
		}


		/// Returns a compact copy of *this.
		/// - Compact means that it contains no unused space due to previos inserts etc.
		///	– *this is streamed to a string and re-parsed.
		///	- You can compact a table by table = table.make_copy()
		///	- If predicate_ is given, only rows with true predicate(r) are copied. 
		template< typename Predicate = const decltype( always_true ) & >
			requires( std::is_invocable_r_v< bool, Predicate, Size > )
		constexpr Text_table make_copy( Predicate && predicate_ = always_true ) const {
			std::stringstream				temp;
			stream( temp, m_col_mark, predicate_ );
			return Text_table( std::move( temp.str() ), m_col_mark );
		}


		/// Stream the table as text to out_.
		///
		template< typename Out >
		void stream(
			Out							  & out_,
			value_type						col_mark_ = value_type{}	///< If zero, the previous col mark is used.
		) const {
			col_mark_					  = ( col_mark_ == value_type{} ) ? m_col_mark : col_mark_;
			m_header.stream( out_, col_mark_ );
			out_ << '\n';
			m_table .stream( out_, col_mark_ );
		}

		/// Stream the table as text to out_.
		///
		template< typename Out, typename Predicate >
			requires( std::is_invocable_r_v< bool, Predicate, Size > )
		void stream(
			Out							  & out_,
			Predicate						predicate_,					///< Only rows with true predicate_(r) are srtreamed.
			value_type						col_mark_ = value_type{}	///< If zero, the previous col mark is used.
		) const {
			col_mark_					  = ( col_mark_ == value_type{} ) ? m_col_mark : col_mark_;
			m_header.stream( out_, col_mark_ );
			out_ << '\n';
			m_table .print( out_, predicate_, col_mark_ );
		}

		/// Stream the table as text to out_.
		///
		template< typename Out >
		friend Out & operator<<(
			Out							  & out_,
			const Text_table			  & table_
		) {
			table_.stream( out_ );
			return out_;
		}
		
		/// Save the table contents to a file at path_.
		template< typename Predicate = const decltype( always_true ) & >
			requires( std::is_invocable_r_v< bool, Predicate, Size > )
		void save( 
			const dest_path				  & path_, 						///< A path to a file in an existing directory.
			Predicate						predicate_ = always_true,	///< Only rows with true predicate_(r) are srtreamed.
			value_type						col_mark_  = value_type{}	///< If zero, the previous col mark is used.
		) const {
			Safe_ofstream< value_type >		out{ path_ };
			stream( out, predicate_, col_mark_ );
			out.close();
		}

		/// Return the table as a string.
		/// Mainly for testing and debugging purposes. 
		template< typename Predicate = const decltype( always_true ) & >
			requires( std::is_invocable_r_v< bool, Predicate, Size > )
		string_type as_str(
			Predicate						predicate_ = always_true,	///< Only rows with true predicate_(r) are srtreamed.
			value_type						col_mark_  = value_type{}	///< If zero, the previous col mark is used.
		) const {
			std::stringstream				temp;
			stream( temp, predicate_, col_mark_ );
			return temp.str();
		}
		
	};


	template< traits::character Ch >
	Text_table( std::basic_string< Ch > )	-> Text_table< std::remove_cv_t< Ch > >;

}	// namespace pax
