//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "../std/format.hpp"
#include <iterator>		// std::input_iterator


namespace pax {

	/// Default class to convert an object to a string. Utilizes std::format. 
	///
	template< typename T >
	struct To_string {
		static constexpr std::string to_string( const T & t_ ) {
			return std20::format( "{}", t_ );
		}

		static std::string to_string( 
			const T				  & t_, 
			const std::string_view	fmt_
		) {
			return std20::format( std20::runtime_format( fmt_ ), t_ );
		}
	};



	template< typename T >
	concept Has_iterator = std::is_array_v< T > || requires( T t ) {
		t.begin();
		t.end();
	};

	/// Default class to convert an object to a string. Utilizes std::format. 
	///
	template< Has_iterator Hasitr >
	struct To_string< Hasitr > {
		static constexpr std::string to_string( const Hasitr & t_ ) {
			using std::begin, std::end;
			return To_string< decltype( t_.begin() ) >::to_string( begin( t_ ), end( t_ ) );
		}

		static std::string to_string( 
			const Hasitr		  & t_, 
			const std::string_view	fmt_
		) {
			using std::begin, std::end;
			return To_string< decltype( t_.begin() ) >::to_string( begin( t_ ), end( t_ ), fmt_ );
		}
	};




	/// Returns a string formatted as "[" + to_string( *begin ) + ", " + ... + "]".
	/// – If ::to_string only has one argument, it is formated as a pointer. 
	template< std::input_iterator Itr >
		requires( not std::is_array_v< Itr > )
	struct To_string< Itr > {
		using base				  = To_string< decltype( *Itr{} ) >;

		static constexpr std::string to_string( const Itr itr_ ) {
			return std20::format( "{}", static_cast< const void* >( itr_ ) );
		}

		static constexpr std::string to_string(
			Itr						itr,
			const Itr				end_
		) {
			std::string				result;
			if( itr == end_ )		return { "[]" };
			else 					result = std::format( "[{}",            base::to_string( *itr ) );
			while( ++itr != end_ ) 	result = std::format( "{}, {}", result, base::to_string( *itr ) );
			return result + ']';
		}

		static constexpr std::string to_string(
			Itr						itr,
			const Itr				end_,
			const std::string_view	fmt_
		) {
			std::string				result;
			if( itr == end_ )		return { "[]" };
			else 					result = std::format( "[{}",            base::to_string( *itr, fmt_ ) );
			while( ++itr != end_ ) 	result = std::format( "{}, {}", result, base::to_string( *itr, fmt_ ) );
			return result + ']';
		}
	};


	template< typename T >
	concept Tuple_like_to_string = not Has_iterator< T > && requires( T t ) {
		get< 0 >( t );
		std::tuple_element_t< 0, T >{};
		std::tuple_size_v< T >;
	};

	/// Converts a tuple-like object to a string. 
	///
	template< Tuple_like_to_string Tuple >
	struct To_string< Tuple > {
	private:
		static constexpr std::size_t rank	  = std::tuple_size_v< Tuple >;
		using Indices						  = std::make_index_sequence< rank >;
		static constexpr std::size_t fmtSz	  = 64;
		static constexpr const char fmt[ fmtSz*4 - 1 ] = 
			"{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, "
			"{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, "
			"{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, "
			"{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}";
		static_assert( rank <= fmtSz );

		template< std::size_t ...I >
		static constexpr std::string to_string(
			const Tuple		  & t_, 
			std::integer_sequence< std::size_t, I... >
		) {
			static_assert( ( true && ... && ( I < rank ) ) );
			using std::get;
			return std20::format(
				fmt + ( fmtSz - rank )*4, 
				To_string< std::tuple_element_t< I, Tuple > >::to_string( get< I >( t_ ) )... 
			);
		}

	public:
		/// Convert to a string formed like "{ value1, value2, ... }". 
		/// Format is "{}".
		static constexpr std::string to_string( const Tuple & t_ ) {
			// return std20::format( "({})", to_string( t_, Indices{} ) );
			return std20::format( "{}", t_ );
		}
	};



	template< typename T, typename ...Attrs >
	constexpr std::string to_string(
		const T				  & t_, 
		Attrs				 && ...attrs_
	) {
		return To_string< T >::to_string( t_, std::forward< Attrs >( attrs_ )... );
	}

}	// namespace pax
