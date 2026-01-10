//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "algorithm.hpp"	// struct Newline
#include <string_view>


#define DOCTEST_ASCII_CHECK_EQ( __1__, __2__ )	DOCTEST_FAST_CHECK_EQ( pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )
#define DOCTEST_ASCII_CHECK_NE( __1__, __2__ )	DOCTEST_FAST_CHECK_NE( pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )
#define DOCTEST_ASCII_WARN_EQ ( __1__, __2__ )	DOCTEST_FAST_WARN_EQ ( pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )
#define DOCTEST_ASCII_WARN_NE ( __1__, __2__ )	DOCTEST_FAST_WARN_NE ( pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )


namespace std {

	/// Concatenate a string and a string view.
	template< typename Ch, typename Traits, typename Allocator >
	[[nodiscard]] constexpr auto operator+(
		const basic_string< remove_cv_t< Ch >, Traits, Allocator >	  & str_,
		const basic_string_view< Ch, Traits >							view_
	) {
		return basic_string< remove_cv_t< Ch >, Traits, Allocator >( str_ )+= view_;
	}

	/// Concatenate a string view and a string.
	template< typename Ch, typename Traits, typename Allocator >
	[[nodiscard]] constexpr auto operator+(
		const basic_string_view< Ch, Traits >							view_, 
		const basic_string< remove_cv_t< Ch >, Traits, Allocator >	  & str_
	) {
		return basic_string< remove_cv_t< Ch >, Traits, Allocator >( view_ )+= str_ ;
	}

}	// namespace std



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

	/// Returns true iff c is any ao the linebreak characters LF or CR.
	static constexpr auto is_newline  = []( const unsigned c )	noexcept {
		// The first part of the test is redundant, but is thought to quicken up the test in most cases.
		return ( c <= Ascii::CR ) && ( ( c == Ascii::LF ) || ( c == Ascii::CR ) );
	};
	
	/// Returns 2 if { LF, CR } or { CR, LF }, returns 1 if c is LF or CR, and returns 0 otherwise.
	static constexpr auto newlines( const unsigned c, const unsigned c2 )			noexcept {
		// ( c^c2 ) == 0x7 signifies either { LF, CR } or { CR, LF }:
		return is_newline( c ) ? 1u + ( ( c^c2 ) == 0x7 ) : 0u;
	}
	
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
		using std::size, std::begin, std::end;
		using 				Ch	  = typename S::value_type;
		using				Str	  = std::basic_string     < Ch >;
		using				StrV  = std::basic_string_view< Ch >;

		auto				itr	  = begin( str_ );
		const auto			stop  = end  ( str_ );
		StrV				sub{};
		Str					result{};
							result.reserve( size( str_ ) );

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



	/// Returns 2 if `view_` starts with `"\n\r"` or `"\r\n"`; 1 if `'\n'` or `'\r'`; and 0 otherwise.
	template< traits::string V >
	[[nodiscard]] constexpr std::size_t starts_with(  
		const V							& v_, 
		Newline 
	) noexcept {
		if constexpr( traits::extent_v< V > > 1 ) {
			using std::data, std::size;
			return	( size( v_ ) > 1 )	? newlines  ( v_[ 0 ], v_[ 1 ] )
				:	  size( v_ )		? is_newline( v_[ 0 ] )
				:						  0;
		} else if constexpr( traits::extent_v< V > == 1 ) {
			return is_newline( v_[ 0 ] );
		} else {
			return 0;
		}
	}

	/// Returns 2 if `view_` ends with `"\n\r"` or `"\r\n"`; 1 if `'\n'` or `'\r'`; and 0 otherwise.
	template< traits::string V >
	[[nodiscard]] constexpr std::size_t ends_with(  
		const V							& v_, 
		Newline 
	) noexcept {
		if constexpr( traits::extent_v< V > > 1 ) {
			using std::data, std::size;
			auto			s = size ( v_ );
			if constexpr( traits::character_array< V > ) 
				s -= bool( s ) && !v_[ s - 1 ];		// To remove possible trailing '\0'.

			const auto last = data( v_ ) + s - ( s > 0 );
			return	( s > 1 )	? newlines  ( *last, *( last - 1 ) )
				:	bool( s )	? is_newline( v_[ 0 ] )
				:	0u;
		} else if constexpr( traits::extent_v< V > == 1 ) {
			return is_newline( v_[ 0 ] );
		} else {
			return 0u;
		}
	}




	template< traits::string V >
	[[nodiscard]] constexpr std::size_t length( V && v_ )		noexcept	{
		using 				std::size;
		if constexpr( traits::character_array< V > )	return std::basic_string_view( v_ ).size();
		else											return size( v_ );
	}


	/// Returns a reference to the last item. 
	/// UB, if v_ has a dynamic size that is zero.
	template< traits::string V >
	[[nodiscard]] constexpr auto & back( const V & v_ )			noexcept	{
		using 				std::data;
		const auto			sz = length( v_ );
		assert( sz && "back( strv ) requires size( strv ) > 0" );
		return *( data( v_ ) + sz - 1 );
	}


	/// Returns a basic_string_view of the first i_ elements of v_.
	///	- If i_ > size( v_ ), basic_string_view( v_ ) is returned.
	template< traits::string V >
	[[nodiscard]] constexpr auto first( 
		V				 && v_, 
		const std::size_t 	i_ = 1 
	) noexcept {
		using				std::data;
		const auto			sz = length( v_ );
		return std::basic_string_view( data( v_ ), std::min( i_, sz ) );
	}

	/// Returns a basic_string_view of the last i_ elements of v_.
	///	- If i_ > size( v_ ), basic_string_view( v_ ) is returned.
	template< traits::string V >
	[[nodiscard]] constexpr auto last( 
		V				 && v_, 
		const std::size_t 	i_ = 1 
	) noexcept {
		using std::data;
		using				std::data;
		const auto			sz = length( v_ );
		return ( i_ < sz )	? std::basic_string_view( data( v_ ) + sz - i_, i_ )
							: std::basic_string_view( v_ );
	}

	/// Returns a basic_string_view of v_ but the first i_.
	///	- If i_ > size( v_ ), an empty basic_string_view( end( v_ ) ) is returned.
	template< traits::string V >
	[[nodiscard]] constexpr auto not_first( 
		V				 && v_, 
		const std::size_t 	i_ = 1 
	) noexcept {
		using std::data;
		const auto			sz = length( v_ );
		return ( i_ < sz )	? std::basic_string_view( data( v_ ) + i_, sz - i_ )
							: std::basic_string_view( data( v_ ) + sz, 0 );
	}

	/// Returns a basic_string_view of all elements of v_ except the last i_.
	///	- If i_ > size( v_ ), an empty basic_string_view( begin( v_ ) ) is returned.
	template< traits::string V >
	[[nodiscard]] constexpr auto not_last( 
		V				 && v_, 
		const std::size_t 	i_ = 1 
	) noexcept {
		using std::data;
		const auto			sz = length( v_ );
		return std::basic_string_view( data( v_ ), ( i_ < sz ) ? sz - i_ : 0u );
	}

	/// Returns a basic_string_view of `size_` elements in `v_` starting with `offset_`.
	///	- If `offset_ < 0`, `offset_ += size( v_ )` is used (the offset is seen from the back), 
	///	- If `offset_ + size_ >= size( v_ )`: returns `not_first( v_, offset_ )`.
	template< traits::string V >
	[[nodiscard]] constexpr auto subview( 
		V					 && v_, 
		const std::ptrdiff_t 	offset_, 
		const std::size_t 		size_ 
	) noexcept {
		using 					std::data, std::size;
		const auto				sz = size( v_ );
		const std::size_t 		offset	=	( offset_ >= 0 )					? std::min( std::size_t( offset_ ), sz ) 
										:	( std::size_t( -offset_ ) < sz )	? sz - std::size_t( -offset_ )
										:										  std::size_t{};

		return std::basic_string_view( data( v_ ) + offset, std::min( length( v_ ) - offset, size_ ) );
	}



	/// Return the beginning of v_ up to but not including the first until_this_.
	/// - If no until_this_ is found, v_ is returned.
	template< traits::string V, typename U >
	constexpr auto until(  
		V					 && v_, 
		U					 && until_this_ 
	) noexcept {
		return first( v_, find( v_, until_this_ ) );
	}



	/// Returns `v_`, possibly excluding a leading `t_`. 
	/// Returns a [non-owning] string view into v_.
	template< traits::string V >
	[[nodiscard]] constexpr auto trim_front( 
		const V							  & v_, 
		const traits::value_type_t< V >		t_ 
	) noexcept {
		return not_first( v_, starts_with( v_, t_ ) );
	}

	/// Returns `v_` possibly excluding a trailing `t_`. 
	/// Returns a [non-owning] string view into v_.
	template< traits::string V >
	[[nodiscard]] constexpr auto trim_back( 
		const V							  & v_, 
		const traits::value_type_t< V >		t_ 
	) noexcept {
		return not_last( v_, ends_with( v_, t_ ) );
	}



	/// Returns `v_`, but excluding any leading elements `v` that satisfy `p_( v )`.
	/// Returns a [non-owning] string view into v_.
	template< typename Pred, traits::string V >
		requires( std::predicate< Pred, traits::value_type_t< V > > )
	[[nodiscard]] constexpr auto trim_first( 
		const V		  & v_, 
		Pred		 && p_ 
	) noexcept {
		using std::begin, std::end;
		auto			itr = begin( v_ );
		auto			e   = end  ( v_ );
		if constexpr( traits::character_array< V > ) 
			e -= ( itr != e ) && !*( e - 1 );			// To remove possible trailing '\0'.

		while( ( itr != e ) && p_( *itr ) )				++itr;
		return std::basic_string_view{ itr, e };
	}

	/// Returns `v_`, but excluding all leading `t_`, if any.
	/// Returns a [non-owning] string view into v_.
	template< traits::string V >
	[[nodiscard]] constexpr auto trim_first( 
		const V							  & v_, 
		const traits::value_type_t< V >   & t_ 
	) noexcept {
		return trim_first( v_, [ & t_ ]( const traits::value_type_t< V > & t ){ return t == t_; } );
	}

	/// Returns `v_`, but excluding a leading `'\n'`, `'\r'`, `"\n\r"`, or `"\r\n"`. 
	/// Returns a [non-owning] string view into v_.
	template< traits::string V >
	[[nodiscard]] constexpr auto trim_first( 
		const V		  & v_, 
		Newline 
	) noexcept {
		return not_first( v_, starts_with( v_, Newline{} ) );
	}

	/// Returns `v_`, but excluding any trailing elements `v` that satisfy `p_( v )`.
	/// Returns a [non-owning] string view into v_.
	template< typename Pred, traits::string V >
		requires( std::predicate< Pred, traits::value_type_t< V > > )
	[[nodiscard]] constexpr auto trim_last( 
		const V		  & v_, 
		Pred		 && p_ 
	) noexcept {
		using std::begin, std::end;
		auto			itr = end  ( v_ );
		const auto		b   = begin( v_ );
		if constexpr( traits::character_array< V > ) 
			itr -= ( itr != b ) && !*( itr - 1 );		// To remove possible trailing '\0'.

		while( ( --itr != b ) && p_( *itr ) );
		return std::basic_string_view{ b, itr + 1 };
	}

	/// Returns `v_`, but excluding all trailing `t_`, if any.
	/// Returns a [non-owning] string view into v_.
	template< traits::string V >
	[[nodiscard]] constexpr auto trim_last( 
		const V							  & v_, 
		const traits::value_type_t< V >		t_ 
	) noexcept {
		return trim_last( v_, [ & t_ ]( const traits::value_type_t< V > & t ){ return t == t_; } );
	}

	/// Returns `v_`, but excluding a trailing `'\n'`, `'\r'`, `"\n\r"`, or `"\r\n"`. 
	/// Returns a [non-owning] string view into v_.
	template< traits::string V >
	[[nodiscard]] constexpr auto trim_last( 
		const V		  & v_, 
		Newline 
	) noexcept {
		return not_last( v_, ends_with( v_, Newline{} ) );
	}

	/// Returns `v_`, but without any leading or trailing values `v` that satisfy `p_( v )`.
	/// Returns a [non-owning] string view into v_.
	template< traits::string V, typename T >
	[[nodiscard]] constexpr auto trim( 
		const V		  & v_, 
		T			 && p_ 
	) noexcept {
		return trim_last( trim_first( v_, p_ ), p_ );
	}



	/// Split v_ into two parts: before `at_` and after (but not including) `at_ + n_`.
	/// - If `at_ >= size( v_ )`, then `{ v_, last( v_, 0 ) }` is returned.
	/// Returns a pair of [non-owning] string views into v_.
	template< traits::string V >
	[[nodiscard]] constexpr auto split_at( 
		const V							  & v_, 
		const std::size_t 					at_, 
		const std::size_t 					n_ = 1 
	) noexcept {
		// first() and not_first() handle the case if at_ + n_ >= size( v_ ).
		return std::pair{ first( v_, at_ ), not_first( v_, at_ + n_ ) };
	}

	/// Split `view_` into two parts: before and after the first `x_`, not including it.
	/// Returns a pair of [non-owning] string views into v_.
	template< traits::string V >
	[[nodiscard]] constexpr auto split_by( 
		const V							  & v_, 
		const traits::value_type_t< V >		item_ 
	) noexcept {
		return split_at( v_, find( v_, item_ ) );
	}

	/// Split the v_ into two parts: before and after (but not including) `by_`.
	/// - If `begin( by_ ) <= begin( v_ )`, the first string view returned is `first( v_, 0 )`.
	///	- If `end( by_ )   >= end( v_ )`,   the second string view returned is `last( v_, 0 )`.
	/// Returns a pair of [non-owning] string views into v_.
	template< traits::string V, traits::string By >
	[[nodiscard]] constexpr auto split_by(
		const V							  & v_, 
		By								 && by_
	) noexcept {
		using std::size;
		auto				s = size( by_ );
		if constexpr( traits::character_array< V > ) 
			s -= bool( s ) && !by_[ s - 1 ];		// To remove possible trailing '\0'.

		return split_at( v_, find( v_, by_ ), s );
	}

	/// Split into two parts: before and after the first newline (`'\n'`, `'\r'`, `"\n\r"`, or `"\r\n"`), but not including it.
	/// Returns a pair of [non-owning] string views into v_.
	template< traits::string V >
	[[nodiscard]] constexpr auto split_by(
		const V							  & v_, 
		Newline 
	) noexcept {
		const std::size_t 	i = find( v_, is_newline );
		return split_at( v_, i, starts_with( not_first( v_, i ), Newline{} ) );
	}



	/// A class to simplify iterating using ´split_by´. It uses views, so the original string must remain static.
	/// - Example usage: ´for( const auto item : String_view_splitter( "A\nNumber\nof\nRows", Newline{} ) ) { ... }´. 
	/// - The Divider type may be any that is accepted by ´split_by( ..., Divider )´. 
	/// - String_view_splitter is constexpr [and never throws]. 
	template< traits::character Char, typename Divider, typename Traits = std::char_traits< std::remove_const_t< Char > > >
	class String_view_splitter {
		class End						{};
		using Value					  = std::basic_string_view< std::remove_const_t< Char >, Traits >;
		Value							m_str;
		Divider							m_divider;
		

		class iterator {
			std::pair< Value, Value >	m_parts;
			Divider						m_divider;

		public:
			constexpr iterator( const Value str_, const Divider divider_ )	noexcept :
				m_parts{ split_by( str_, divider_ ) }, m_divider{ divider_ } {}

			/// Iterate to next item. 
			constexpr iterator & operator++()	noexcept		{
				m_parts = split_by( m_parts.second, m_divider );
				return *this;
			}

			/// Get the string_view of the present element. 
			constexpr Value operator*()			const noexcept	{	return m_parts.first;									}

			/// Does *not* check equality! Only checks if we are done iterating. 
			constexpr bool operator==( End )	const noexcept	{	return m_parts.first.data() == m_parts.second.data();	}
		};
		
	public:
		constexpr String_view_splitter( const Value str_, const Divider divider_ ) 	noexcept :
			m_str{ str_ }, m_divider{ divider_ } {}

		constexpr iterator begin()				const noexcept	{	return { m_str, m_divider };							}
		constexpr End end()						const noexcept	{	return {};												}
	};

	template< traits::string S, typename D >
	String_view_splitter( S &&, D ) 
		-> String_view_splitter< traits::element_type_t< S >, D, typename std::remove_cvref_t< S >::traits_type >;

	template< traits::character Ch, typename D >
	String_view_splitter( Ch *, D ) -> String_view_splitter< std::remove_reference_t< Ch >, D >;



	/// Return the first newline used in view_ (`"\n"`, `"\r"`, `"\n\r"`, or `"\r\n"`).
	/// - If none is found, `"\n"` is returned.
	template< traits::string V >
	[[nodiscard]] constexpr auto identify_newline( const V & str_ ) noexcept {
		using my_view = std::basic_string_view< traits::value_type_t< V > >;
		static constexpr const my_view			 	res = { "\n\r\n" };
		const auto 									temp = not_first( str_, find( str_, Newline{} ) );
		const std::size_t 							sz = starts_with( temp, Newline{} );
		return	sz ? subview( res, temp.front() == '\r', sz ) : first( res, 1 );
	}



	/// Calculate the Luhn sum (a control sum).
	/// – UB if any character is outside ['0', '9'].
	/// - https://en.wikipedia.org/wiki/Luhn_algorithm
	template< traits::string V >
	[[nodiscard]] constexpr std::size_t luhn_sum( const V & v_ ) noexcept {
		using std::begin, std::end;
		auto				b = begin( v_ );
		auto				e = end  ( v_ );
		if constexpr( traits::character_array< V > ) 
			e -= ( b != e ) && !*( e - 1 );			// To remove possible trailing '\0'.

		static constexpr char	twice[] = { 0, 2, 4, 6, 8, 1, 3, 5, 7, 9 };
		std::size_t				sum{};
		bool					one{ true };
		while( b != e )			sum += ( one = !one ) ? ( *( b++ ) - '0' ) : twice[ *( b++ ) - '0' ];
		return sum;
	}

}