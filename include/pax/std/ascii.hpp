//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <pax/types/point-stuff/contiguous.hpp>
#include <string_view>


#define DOCTEST_ASCII_CHECK_EQ( __1__, __2__ )	DOCTEST_FAST_CHECK_EQ( pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )
#define DOCTEST_ASCII_CHECK_NE( __1__, __2__ )	DOCTEST_FAST_CHECK_NE( pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )
#define DOCTEST_ASCII_WARN_EQ ( __1__, __2__ )	DOCTEST_FAST_WARN_EQ ( pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )
#define DOCTEST_ASCII_WARN_NE ( __1__, __2__ )	DOCTEST_FAST_WARN_NE ( pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )


namespace pax {

	/// Name some of the control characters.
	enum Ascii : unsigned {
		NUL	= 0x00,		null				= NUL,	// \0
		SOH	= 0x01,		start_of_header		= SOH,
		STX	= 0x02,		start_of_text		= STX,
		ETX	= 0x03,		end_of_text			= ETX,
		EOT	= 0x04,		end_of_transmission	= EOT,
		ENQ	= 0x05,		enquiry				= ENQ,
		ACK	= 0x06,		acknowledge			= ACK,
		BEL	= 0x07,		bell				= BEL,	// \a
		BS	= 0x08,		backspace			= BS,	// \b
		HT	= 0x09,		horizontal_tab		= HT,	// \t
		LF	= 0x0a,		line_feed			= LF,	new_line = LF,	// \n
		VT	= 0x0b,		vertical_tab		= VT,	// \v
		FF	= 0x0c,		form_feed			= FF,	new_page = FF,	// \f
		CR	= 0x0d,		carriage_return		= CR,	// \r
		SO	= 0x0e,		shift_out			= SO,
		SI	= 0x0f,		shift_in			= SI,
		DLE	= 0x10,		data_link_escape	= DLE,
		DC1	= 0x11,		device_control_1	= DC1,
		DC2	= 0x12,		device_control_2	= DC2,
		DC3	= 0x13,		device_control_3	= DC3,
		DC4	= 0x14,		device_control_4	= DC4,
		NAK	= 0x15,		negative_acknowledge = NAK,
		SYN	= 0x16,		synchronous_idle	= SYN,
		ETB	= 0x17,		end_of_transmission_block = ETB,
		CAN	= 0x18,		cancel				= CAN,
		EM	= 0x19,		end_of_medium		= EM,
		SUB	= 0x1a,		substitute			= SUB,
		ESC	= 0x1b,		escape				= ESC,
		FS	= 0x1c,		file_separator		= FS,
		GS	= 0x1d,		group_separator		= GS,
		RS	= 0x1e,		record_separator	= RS,
		US	= 0x1f,		unit_separator		= US,
		SP	= 0x20,		space				= SP,
		DEL	= 0x7f,		delete_				= DEL,
		UCPO= 0x2400,	unicode_control_pictures_offset = UCPO,
	};


	/// Returns a descriptive string view for invisible characters and an empty one for visible ones.
	template< traits::character Ch2 >
	static constexpr std::basic_string_view< Ch2 > ascii_control_name( const Ch2 c_ )	noexcept {
		using string_view	  = std::basic_string_view< Ch2 >;
		static constexpr		unsigned		specialsN = 33;
		static constexpr		string_view		control[ specialsN ] = { 
			"\\0",   	"<SOH>", 	"<STX>", 	"<ETX>", 	"<EOT>", 	"<ENQ>", 	"<ACK>", 	"\\a",
			"\\b",   	"\\t",   	"\\n",   	"\\v",   	"\\f",   	"\\r",   	"<SO>",  	"<SI>",
			"<DLE>", 	"<DC1>", 	"<DC2>", 	"<DC3>", 	"<DC4>", 	"<NAK>", 	"<SYN>", 	"<ETB>", 
			"<CAN>", 	"<EM>",  	"<SUB>", 	"\\e",   	"<FS>",  	"<GS>",  	"<RS>",  	"<US>",
			" "
		};
		switch( c_ ) {
			case '\\': 			return "\\\\";
			case '"': 			return "\"";
			case Ascii::DEL:	return "<DEL>";
			default:			return ( unsigned( c_ ) >= specialsN ) ? string_view{} : control[ unsigned( c_ ) ];
		}
	}
	
	/// Returns a descriptive string view for invisible characters and an empty one for visible ones.
	template< traits::character Ch2 >
	static constexpr std::basic_string_view< Ch2 > uu_control_name( const Ch2 c_ )		noexcept {
		using string_view	  = std::basic_string_view< Ch2 >;
		static constexpr		unsigned		specialsN = 33;
		static constexpr		string_view		control[ specialsN ] = { 
			"\u2400",   "\u2401",	"\u2402",	"\u2403",	"\u2404",	"\u2405",	"\u2406",	"\u2407",
			"\u2408",   "\u2409",	"\u240a",	"\u240b",	"\u240c",	"\u240d",	"\u240e",	"\u240f",
			"\u2410",   "\u2411",	"\u2412",	"\u2413",	"\u2414",	"\u2415",	"\u2416",	"\u2417",
			"\u2418",   "\u2419",	"\u241a",	"\u241b",	"\u241c",	"\u241d",	"\u241e",	"\u241f",
			"\u2423"
		};	// https://en.wikipedia.org/wiki/Unicode_control_characters
		
		switch( c_ ) {
			case '\\': 			return "\\";
			case '"': 			return "\"";
			case Ascii::DEL:	return "\u2421";
			default:			return ( unsigned( c_ ) >= specialsN ) ? string_view{} : control[ unsigned( c_ ) ];
		}
	}


	template< traits::character Ch >
	constexpr std::basic_string< Ch > as_ascii( const Ch c_ )	noexcept	{
		const auto view 	= ascii_control_name( c_ );
		return view.size()	? std::basic_string< Ch >{ view }
							: std::basic_string< Ch >{{ c_ }};
	}

	template< traits::string S >
	constexpr auto as_ascii( const S & str_ )					noexcept	{
		using std::begin;
		using 				Ch	  = typename S::value_type;
		using				Str	  = std::basic_string     < Ch >;
		using				StrV  = std::basic_string_view< Ch >;

		auto				itr	  = begin( str_ );
		const auto			stop  = no_nullchar_end( str_ );
		StrV				sub{};
		Str					result{};
							result.reserve( stop - itr );

		while( itr != stop ) {
			auto itr2	  = itr;
			while( ( ( sub = ascii_control_name( *itr2 ) ).size() == 0u ) && ( ++itr2 != stop ) );
			if( itr2 != itr ) {			// A [bunch of] visible character[s]. 
				result	 +=	StrV{ itr, itr2 };
				itr		  = itr2;
			} else if( itr2 != stop ) {	// An invisible character is substituted by a string. 
				result	 +=	sub;
				++itr;
			}
		}
		return result;
	}

	template< traits::character Ch >
	constexpr std::basic_string< Ch > as_ascii( const Ch *c_ )	noexcept	{
		return as_ascii( std::basic_string_view( c_ ) );
	}

}	// namespace pax