//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once


// Using std::format works well with clang 18. It does not work with gcc 13.2 or 14, however.
// So I stick with fmt::format until gcc implements std::runtime_format as defined in C++26.
#define PAX_USE_STD_FORMAT			( ( __cplusplus > 202302L ) || false )
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

	namespace fmt {
		/// When the format string is not known at compile time, std::format may not be used as usual.
		/** Instead the format argument signals this case by having a specific type.		**/
		inline auto runtime_format( const std::string_view fmt_ )	{	return fmt::runtime( fmt_ );		}
	}	// namespace std20
#endif
