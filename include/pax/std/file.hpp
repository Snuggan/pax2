//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "../concepts.hpp"
#include "../reporting/error_message.hpp"

#include <filesystem>
#include <random>
#include <fstream>		// std::ofstream::trunc


namespace pax {

	/// Helper: return an absolut path if possible and path_ otherwise.
	inline std::filesystem::path normalize( const std::filesystem::path & path_ ) {
		std::error_code					ec;
		const auto						p{ std::filesystem::canonical( path_, ec ) };
		return ec ? path_ : p;
	}
	
	/// Helper: nice std::filesystem_path string.
	inline std::string to_string( const std::filesystem::path & path_ ) noexcept	{	return normalize( path_ ).native();	}

	/// Report a path related error.
	inline auto error_message( 
		const std::string_view			message_, 
		const std::filesystem::path	  & path_,
		const std::error_code			ec_ = std::error_code{},
		const std::source_location	  & sl_ = std::source_location::current()
	) {
		return error_message( message_, ec_, sl_ )
			<< std20::format( "Path\t'{}'", to_string( path_ ) );
	}




	/// Superclass to path that runs <Check> on whatever the constructor gets.
	template< typename Check >
	struct checked_path : std::filesystem::path {
		using path												=	std::filesystem::path;

		checked_path()											=	delete;
		checked_path( const checked_path & )					=	default;
		checked_path( checked_path && )							=	default;
		checked_path & operator=( const checked_path & )		=	default;
		checked_path & operator=( checked_path && )				=	default;

		checked_path( 
			const path					  & path_,
			const std::source_location	  & sl_ = std::source_location::current()
		)														:	path{ path_ }				{	Check::check( *this, sl_ );	}
		checked_path(
			path						 && path_,
			const std::source_location	  & sl_ = std::source_location::current()
		)														:	path{ std::move( path_ ) }	{	Check::check( *this, sl_ );	}

		template< typename T >
		checked_path(
			const T						  & t_,
			const std::source_location	  & sl_ = std::source_location::current()
		)														:	checked_path( path( t_ ), sl_ ) {}
	};

	/* This function is strictly not necessary, but required as long as there is a to_string( T ). */
	template< typename Check >
	std::string to_string( const checked_path< Check > & path_ ) noexcept {
		return normalize( path_ ).native();
	}


	struct is_dir_path {
		static bool is( const std::filesystem::path & path_ ) {
			std::error_code		ec;
			return std::filesystem::is_directory( 
				( path_.empty() ? std::filesystem::path( "." ) : path_ ),
				ec 
			) && !ec;
		}
		static void check( 
			const std::filesystem::path	  & path_,
			const std::string_view			message_,
			const std::source_location	  & sl_
		) {
			if( !is( path_ ) )				throw error_message( message_, path_, std::error_code{}, sl_ );
		}
		static void check( 
			const std::filesystem::path	  & path_,
			const std::source_location	  & sl_
		) {
			check( path_, "No such directory", sl_ );
		}
	};
	/// Throws, unless is_directory( path_ ).
	using dir_path = checked_path< is_dir_path >;


	struct is_file_path {
		static bool is( const std::filesystem::path & path_ ) {
			std::error_code		ec;
			return std::filesystem::is_regular_file( path_, ec ) && !ec; 
		}
		static bool is(
			const std::filesystem::path	  & path_, 
			const char					  * extension_
		) {
			return is( path_ ) && ( path_.extension() == extension_ );
		}
		static bool txt( const std::filesystem::path & path_ ) {
			return is( path_ ) && (
					( path_.extension() == ".txt" )
				||	( path_.extension() == ".text" )
			);
		}
		static bool csv( const std::filesystem::path & path_ ) {
			return is( path_ ) && (
					( path_.extension() == ".csv" )
				||	( path_.extension() == ".tsv" )
				||	( path_.extension() == ".tab" )
			);
		}

		static void check( 
			const std::filesystem::path	  & path_,
			const std::source_location	  & sl_
		) {
			if( !is( path_ ) )				throw error_message( "Not a regular file", path_, std::error_code{}, sl_ );
		}
	};
	/// Throws, unless is_regular_file( path_ ).
	using file_path = checked_path< is_file_path >;


	struct is_source_path {
		static void check( 
			const std::filesystem::path	  & path_,
			const std::source_location	  & sl_
		) {
			std::error_code		ec;
			if( !( std::filesystem::is_regular_file( path_, ec ) && !ec ) && !is_dir_path::is( path_ ) )
				throw error_message( "No such file or directory", path_, std::error_code{}, sl_ );
		}
	};
	/// Throws, unless is_regular_file( path_ ) or is_directory( path_ ). 
	using source_path = checked_path< is_source_path >;


	struct is_dest_path {
		static void check( 
			const std::filesystem::path	  & path_,
			const std::source_location	  & sl_
		) {
			if( normalize( path_ ).empty() )
				throw error_message( "Destination path is empty", path_, std::error_code{}, sl_ );
			is_dir_path::check( path_.parent_path(), "Can't save file here as there is no parent directory", sl_ );
		}
	};
	/// Throws, unless is_directory( path_.parent_path() ).
	using dest_path = checked_path< is_dest_path >;



	/// Open the file for writing in the safest way possible.
	/// Then stream conts_ to this stream and close it.
	template< typename Contents, traits::character Ch = char, typename Traits = std::char_traits< Ch > >
	void stream( 
		const std::filesystem::path		  & dest_, 
		const Contents					  & conts_ 
	);

	/// Stream conts_ to dest_. 
	/// Specialisation for character stuff. 
	template< traits::character Ch, typename Traits >
	void stream( 
		std::basic_ostream< Ch, Traits >  & dest_, 
		const std::basic_string_view< Ch > 	conts_ 
	) {
		dest_.write( conts_.data(), conts_.size() );
	}



	/// Read a complete text file and return it as a file.
	/// Emphasis on efficiency and, in case of a problem, reporting. 
	template< 
	    traits::character Char	  = char,		///< The character type.
	    typename Traits			  = std::char_traits< Char >, 
	    typename Allocator		  = std::allocator< Char >
	> 
	auto read_string( const file_path & path_ ) {
		using Stream			  = std::basic_ifstream< Char, Traits >;
		using String			  = std::basic_string< Char, Traits, Allocator >;

		// Open the file.
		errno					  = 0;
		Stream						in{ path_.native(), std::ios::in | std::ios::binary };
		if( !in ) throw error_message( 
			"Could not open file", 
			path_, 
			make_error_code( errno )
		);

		// Get the file size.
		in.seekg( 0, std::ios::end );
		const std::size_t			size( in.tellg() );
		in.seekg( 0, std::ios::beg );

		// Allocate a string with the right size and fill it with the file's contents..
		String						result{};
		errno					  = 0;
		try {
			// Resize the string without setting all chars to 0 and fill it with the contents of the file.
			result.resize_and_overwrite( size, 
				[ &in, &size ]( Char* data_, std::size_t ) {
					in.read( data_, size );
					return size;
				}
			);
		} catch( ... ) {
			result.resize( 0 );		// Error report just below...
		}		
		if( result.size() != size ) throw error_message( std20::format( 
			"Reading text file '{}', could not allocate a string of size {} ({}).", 
			to_string( path_ ), 
			size, 
			to_string( make_error_code( errno ) )
		) );

		// Check the file size.
		// https://stackoverflow.com/questions/22984956/tellg-function-give-wrong-size-of-file/22986486#22986486
		// https://en.cppreference.com/w/cpp/io/basic_istream/gcount
		const std::size_t 			size2( in.gcount() );
		if( size2 != result.size() ) {
			if( size2 > result.size() ) throw error_message( "Could not read all of the file contents", path_ );
			result.resize( size2 );
			result.shrink_to_fit();
		}
		
		in.close();
		return result;
	}




	/// Generate and manage a temporary path name, possibly removing it at destruction.
	/** At destruction, the tempfile is removed. Using .done(), tempfile() is moved to destination().	**/
	class Temppath {
		std::filesystem::path						m_temp, m_destination;
		static constexpr std::string_view			c_suffix{ ".%%%%%%%%%%%%%%%%-temp" };

		// Create random string.
		static std::string random_suffix() 							{
			using  Integer						  = short;	// Actually char, but that does not work in clang-15...
			using  Distribution					  = std::uniform_int_distribution< Integer >;
			static std::mt19937						rg{ std::random_device{}() };
			static Distribution 					picker{ Integer( 'a' ), Integer( 'z' ) };
			std::string								result{ c_suffix };
			for( auto & c : result )				if( c == '%' )	 c = picker( rg );
			return result;
		}
		
	public:
		Temppath()								  =	delete;
		Temppath( const Temppath & )			  =	delete;
		Temppath & operator=( const Temppath & )  =	delete;
		Temppath( Temppath && )					  =	default;
		Temppath & operator=( Temppath && )		  =	default;

		static constexpr std::string_view			suffix{ c_suffix };

		/// Creates a temporary path named the same as dest_, but with a randomized suffix.
		explicit Temppath( const std::filesystem::path & dest_ ) : Temppath{ dest_, dest_.native() + random_suffix() } {}

		/// Creates a temporary path temp_ and a final destination path dest_.
		explicit Temppath( const std::filesystem::path & dest_, const std::filesystem::path & temp_ ) 
			: m_temp( temp_ ), m_destination{ dest_ } {}

		/// Remove the tempfile() file, if it is still around. 
		virtual ~Temppath()											{
			std::error_code							ec;
			std::filesystem::remove( temporary(), ec );
		}

		/// The temporary path.
		const std::filesystem::path & temporary()	const noexcept	{	return m_temp;									}
		
		/// The final destination path.
		const std::filesystem::path & destination()	const noexcept	{	return m_destination;							}

		/// Renames the temporary() file to destination().
		/** Returns the error code reported by std::filesystem::rename( ... ).
			The probable cause for error is that no temporary file exists (std::errc::no_such_file_or_directory). 
	
			After a soccessful call no file will be removed at the destruction of this Temppath object. 
		**/
		std::error_code done() 						const noexcept	{
			std::error_code							ec;
			std::filesystem::rename( temporary(), destination(), ec );
			return ec;
		}

		/// Same as done(), but throws if there is an error.
		std::error_code done_with_assert()			const			{
			const std::error_code					ec{ done() };
			if( ec ) throw error_message( std20::format(
				"Safe file save: could not rename the temporary file '{}' to the destination path '{}', due to error code {} ({}).", 
				to_string( temporary() ), 
				to_string( destination() ), 
				ec.value(), ec.message()
			) );
			return ec;
		}
	};
	



	/// Use Temppath to first save to a temporary path and then at .close() move it to the final destination. 
	template< traits::character Ch = char, typename Traits = std::char_traits< Ch > >
	class Safe_ofstream : public std::basic_ofstream< Ch, Traits > {
		using stream_type				  = std::basic_ofstream< Ch, Traits >;
		Temppath							m_temp;

	public:
		Safe_ofstream()										=	delete;
		Safe_ofstream( const Safe_ofstream & )				=	delete;
		Safe_ofstream & operator=( const Safe_ofstream & )	=	delete;
		Safe_ofstream( Safe_ofstream && ) 					=	default;
		Safe_ofstream & operator=( Safe_ofstream && )		=	default;

		template< typename ...Extra_args >
		explicit Safe_ofstream( const std::filesystem::path & dest_, Extra_args && ...extras_ ) : 
			Safe_ofstream( Temppath{ dest_ }, std::forward< Extra_args >( extras_ )... ) 
		{}

		template< typename ...Extra_args >
		explicit Safe_ofstream( Temppath && temp_, Extra_args && ...extras_ ) : stream_type{}, m_temp( std::move( temp_ ) ) {
			errno = 0;
			stream_type::open( m_temp.temporary(), std::forward< Extra_args >( extras_ )... );
			if( !*this ) throw error_message( std20::format( 
				"Saving to temporary file '{}': could not open it{}. Final destination is '{}'.",
				to_string( m_temp.temporary() ),
				to_string( make_error_code( errno ) ),
				to_string( m_temp.destination() )
			) );
		}

		/// We are done: close the file and rename file to m_temp.destination().
		void close() {
			// Close the output stream before renaming.
			errno = 0;
			stream_type::close();
			if( !*this ) throw error_message( std20::format( 
				"Saving to temporary file '{}': could not save all contents{}. Final destination is '{}'.",
				to_string( m_temp.temporary() ),
				to_string( make_error_code( errno ) ),
				to_string( m_temp.destination() )
			) );

			// All went well: move the file from temp_dest to dest_. Or throw!
			m_temp.done_with_assert();
		}
	};




	template< typename Contents, traits::character Ch, typename Traits >
	void stream( 
		const std::filesystem::path		  & dest_, 
		const Contents					  & conts_ 
	) {
		// trunc erases previous file contents. 
		Safe_ofstream< Ch, Traits >			out( dest_, std::ofstream::trunc );
		stream( out, conts_ );
		out.close();
	}

}	// namespace pax
