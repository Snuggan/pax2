//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once


// Using std::format works well with clang 17. 
// It does not work with gcc 13.2, however. It does not know how to handle types like std::span, for example.
// So for now I stick with fmt::format.
#define PAX_EXTERNAL_USE_FMT	1


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

	/// When the formatter is not known at compile time, std::format may not be used as usual.
	/** Instead the format argument signals this case by having a specific type.		**/
	inline auto runtime_format( const std::string_view fmt_ ) {
		return std20::runtime( fmt_ );

		// This is proposed by http://wg21.link/P2918
		// return std20::runtime_format( fmt_ );
	}


	template< typename Ch, typename Traits >
	class basic_string_view2;

}	// namespace pax


namespace PAX_STD20 {
	template< typename Ch, typename Traits >
	struct std20::formatter< pax::basic_string_view2< Ch, Traits > > : ostream_formatter {};
}
