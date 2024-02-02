

#pragma once


#define __PAX_PREVERSION__STRINGIFY( x ) #x
#define __PAX_PREVERSION__FILE2( a ) __PAX_PREVERSION__STRINGIFY( version/version_ ## a )
#define __PAX_PREVERSION__FILE( a ) __PAX_PREVERSION__FILE2( a )

#include __PAX_PREVERSION__FILE( __PAX_SYSTEM__ )
