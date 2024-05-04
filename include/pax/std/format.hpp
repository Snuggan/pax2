//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once


// Using std::format works well with clang 18. 
// It does not work with gcc 13.2, however. I think it will with gcc 14. 
// So I stick with fmt::format until I have a gcc that handles C++26.
#define PAX_USE_STD_FORMAT			( __cplusplus > 202302L )


#if PAX_USE_STD_FORMAT
#	include <format>
#	define std20 std
#else
	//	https://fmt.dev/latest/index.html
	//	https://github.com/fmtlib/fmt
#	define FMT_HEADER_ONLY
#	include "../external/fmt/format.h"
#	include "../external/fmt/chrono.h"
#	include "../external/fmt/ostream.h"
#	include "../external/fmt/ranges.h"
#	define std20 fmt
#endif


namespace pax {

	template< typename Ch, typename Traits >
	class basic_string_view2;

}	// Namespace pax


namespace std20 {

	template< typename Ch, typename Traits >
	struct formatter< pax::basic_string_view2< Ch, Traits >, Ch > 
		:  formatter< std::basic_string_view < Ch, Traits > > {};

#	if !PAX_USE_STD_FORMAT

		// For fmt 10.2.2
		template< typename Ch, typename Traits >
		struct detail::is_std_string_like< pax::basic_string_view2< Ch, Traits > > : std::true_type {};

		/// When the formatter is not known at compile time, std::format may not be used as usual.
		/** Instead the format argument signals this case by having a specific type.		**/
		inline auto runtime_format( const std::string_view fmt_ ) {
			return fmt::runtime( fmt_ );
		}

#	endif
}	// namespace std20
