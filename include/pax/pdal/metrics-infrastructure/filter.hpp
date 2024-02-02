//	Copyright (c) 2014-2022, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "_general.hpp"
#include <pax/reporting/error_message.hpp>

#include <string_view>
#include <limits>	// std::numeric_limits


namespace pax::metrics {

	class Filter {

		using limit_type			  = std::uint16_t;
		static constexpr auto Min	  =	std::numeric_limits< limit_type >::lowest();
		static constexpr auto Max	  =	std::numeric_limits< limit_type >::max();
		static_assert( Min == 0 );

		static constexpr limit_type to_limit_type( metrics_value_type v_ )	noexcept	{
			return limit_type( ( v_ >= Max ) ? Max : ( v_ <= Min ) ? Min : v_ );
		}

		limit_type						m_min, m_max;
		bool							m_first_only;
	
		static constexpr Filter parse( const std::string_view id0_ ) {
			std::string_view	id_ = "<incorrect>";
			const bool			is_1ret = id0_.starts_with( "1ret" );
			if( is_1ret )							id_ = id0_.substr( 4 );
			else if( id0_.starts_with( "all" ) )	id_ = id0_.substr( 3 );

			limit_type			min = Min;
			if( const auto		num = parse_limit( id_, "_ge" ); num.size() ) {
				min	  = to_cm( num );
				id_	  = id_.substr( 5 + num.size() );
			}

			limit_type			max = Max;
			if( const auto		num = parse_limit( id_, "_lt" ); num.size() ) {
				max	  = to_cm( num );
				id_	  = id_.substr( 5 + num.size() );
			}

			return ( id_.empty() )	? Filter( is_1ret, min, max )	// String parsed ok.
									: Filter( is_1ret, Max, Min );	// Will generate an exception.
		}

		/// Replace the Nilsson level marker {}, if any, with to_cm( nilsson_ ).
		/// Not constexpr due to std20::format in older compilators.
		static Filter parse( const std::string_view id_, const metrics_value_type nilsson_ ) {
			if( const auto 		i = id_.find( '}' ); ( i < id_.size() ) && ( i > 0 ) && ( id_[ i-1 ] == '{' ) ) {
				const auto 		temp = std20::format( "{}{}{}", id_.substr( 0, i-1 ), to_cm( nilsson_ ), id_.substr( i+1 ) );
				return parse( temp, nilsson_ );
			}
			return parse( id_ );
		}
		
		static constexpr limit_type to_cm( metrics_value_type v_ )	noexcept	{
			return to_limit_type( v_*100 + 0.5 );	// To cm.
		}

		static constexpr limit_type to_cm( const std::string_view num_ ) {
			metrics_value_type	temp{};
			for( const unsigned n : num_ )	temp = temp*10 + ( n - '0' );
			return to_limit_type( temp );
		}

		static constexpr metrics_value_type from_cm( const limit_type limit_ ) noexcept {
			return	( limit_ == Min )	?	-std::numeric_limits< metrics_value_type >::infinity() 
				:	( limit_ == Max )	?	+std::numeric_limits< metrics_value_type >::infinity() 
				:							limit_/metrics_value_type( 100 );
		}


		// Returns NNN in preNNNcm if NNN is an integer. Returns an empty view if not.
		static constexpr std::string_view parse_limit( std::string_view text_, const std::string_view pre_ ) noexcept {
			const auto where_cm = text_.find( "cm" );
			if( !text_.starts_with( pre_ ) || ( where_cm >= text_.size() ) || ( text_.size() <= pre_.size() + 2 ) )
				return std::string_view{};

			const std::string_view num = text_.substr( pre_.size(), where_cm - 3 );
			for( const char c : num )	if( ( c < '0' ) || ( c > '9' ) ) return std::string_view{};

			return num;
		}

		constexpr Filter(
			const bool 					first_only_, 
			const limit_type			min_, 
			const limit_type			max_
		) noexcept : m_min{ min_ }, m_max{ max_ }, m_first_only{ first_only_ } {}
	
	public:
		constexpr Filter( const Filter & )				=	default;
		constexpr Filter & operator=( const Filter & )	=	default;

		/// Construct a filter as defined by a text string. 
		/** The format is: ´all|1ret[_ge<int>cm][_lt<int>cm]´.	**/
		Filter( const std::string_view id_ ) 
			: Filter{ parse( id_ ) } {
				if( m_min >= m_max ) 
					throw error_message( std20::format( "Could not parse filter string '{}'.", id_ ) );
			}

		/// Construct a filter as defined by a text string. There might be a "{}" to be replaced by the nilsson_ value.
		/** The format is: ´all|1ret[_ge<int>cm][_lt<int>cm]´.	**/
		Filter( const std::string_view id_, const metrics_value_type nilsson_ )
			: Filter{ parse( id_, nilsson_ ) } {
				if( m_min >= m_max ) 
					throw error_message( std20::format( "Could not parse filter string '{}'.", id_ ) );
			}

		static constexpr Filter all()						  noexcept	{	return Filter{ false, Min, Max };			}
		static constexpr Filter ret1()						  noexcept	{	return Filter{ true,  Min, Max };			}
		
		/// Argument min_ i meter.
		static constexpr Filter all_ge( metrics_value_type min_ )	noexcept	{
			return Filter{ false, to_cm( min_ ), Max };
		}
		
		/// Argument min_ i meter.
		static constexpr Filter ret1_ge( metrics_value_type min_ )  noexcept	{
			return Filter{ true,  to_cm( min_ ), Max };
		}

		constexpr bool first_only()						const noexcept	{	return m_first_only;						}
		constexpr metrics_value_type min_level()		const noexcept	{	return from_cm( m_min );					}
		constexpr metrics_value_type max_level()		const noexcept	{	return from_cm( m_max );					}
		
		constexpr bool operator==( const Filter o_ )	const noexcept	{
			return	( m_first_only	== o_.m_first_only	)
				&&	( m_min			== o_.m_min			)
				&&	( m_max			== o_.m_max			);
		}
		
		constexpr bool operator< ( const Filter o_ )	const noexcept	{
			return	( m_first_only	!= o_.m_first_only	)	? ( m_first_only < o_.m_first_only	)
				:	( m_min			!= o_.m_min			)	? ( m_min		 < o_.m_min			)
				:										 	  ( m_max		 < o_.m_max			);
		}
	
		/// The textual representation of the filter. 
		friend std::string to_string( const Filter f_ ) {
			return std20::format( "{}{}{}", 
				f_.m_first_only ? "1ret" : "all", 
				( f_.m_min == Min ) ? std::string() : std20::format( "_ge{}cm", f_.m_min ), 
				( f_.m_max == Max ) ? std::string() : std20::format( "_lt{}cm", f_.m_max ) 
			);
		}

		template< typename Out >
		friend Out & operator<<(
			Out						  & out_,
			const Filter				filter_
		) {
			return out_ << to_string( filter_ );
		}
	
		template< typename Out >
		static void help( Out & out_, const std::string_view pre_ ) {
			out_	<< pre_ << "The filter format is either of:\n"
					<< pre_ << "    all[_geMINcm][_ltMAXcm] (all points within the optional limits)\n"
					<< pre_ << "    1ret[_geMINcm][_ltMAXcm] (first returns within the optional limits)\n"
			;
		}
	};

	static_assert( sizeof( Filter ) == 6 ); 

}	// namespace pax::metrics

namespace pax {
	template< typename >			struct Serialize;
	
	template<>
	struct Serialize< metrics::Filter > {
		template< typename J >
		static void assign( J & j_, const metrics::Filter t_ ) {
			j_ = {
				{	"value",		to_string( t_ )										},
				{	"points",		t_.first_only() ? "first returns" : "all points"	},
				{	"min_level",	t_.min_level()										},
				{	"max_level",	t_.max_level()										}
			};
		}
	};
}	// namespace pax
