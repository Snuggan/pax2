//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once


// Using std::format works well with clang 18. 
// It does not work with gcc 13.2, however. 
// So I stick with fmt::format until I have a gcc that handles C++26.
#define PAX_EXTERNAL_USE_FMT	( __cplusplus <= 202302L )


#if PAX_EXTERNAL_USE_FMT
	//	https://fmt.dev/latest/index.html
	//	https://github.com/fmtlib/fmt
#	define FMT_HEADER_ONLY
#	include "../external/fmt/format.h"
#	include "../external/fmt/chrono.h"
#	include "../external/fmt/ostream.h"
#	include "../external/fmt/ranges.h"
#	define PAX_STD20 fmt
#else
#	include <format>
#	define PAX_STD20 std
#endif

namespace std20 = PAX_STD20;

namespace pax {
	template< typename Ch, typename Traits >
	class basic_string_view2;
}	// Namespace pax

namespace PAX_STD20 {
#	if PAX_EXTERNAL_USE_FMT

		template< typename Ch, typename Traits >
		struct formatter< pax::basic_string_view2< Ch, Traits > > : basic_ostream_formatter< Ch > {};

		/// When the formatter is not known at compile time, std::format may not be used as usual.
		/** Instead the format argument signals this case by having a specific type.		**/
		inline auto runtime_format( const std::string_view fmt_ ) {
			return fmt::runtime( fmt_ );
		}

#	else

		template< typename Ch, typename Traits >
		struct formatter< pax::basic_string_view2< Ch, Traits > > : formatter< std::basic_string_view< Ch >, Ch > {};

#	endif
}	// namespace PAX_STD20
