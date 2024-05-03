//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once


// Using std::format works well with clang 18. 
// It does not work with gcc 13.2, however. I think it will with gcc 14. 
// So I stick with fmt::format until I have a gcc that handles C++26.
#define PAX_USE_STD_FORMAT			( __cplusplus > 202302L )


#if PAX_USE_STD_FORMAT
#	define std20 std
#else
#	define std20 fmt
#endif


namespace std20 {

	// template< class T, class CharT = char >
	// struct formatter;

}
