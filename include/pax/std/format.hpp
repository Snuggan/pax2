//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <format>

namespace std {

#	if defined( __IS_GCC__ )
	constexpr auto dynamic_format( std::string_view fmt_ ) {
		return std::runtime_format( fmt_ );
	}
#	endif

}	// namespace std
