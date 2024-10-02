//	Copyright (c) 2014-2022, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <pax/math/metrics/ordered.hpp>
#include <pax/math/metrics/summary.hpp>
#include <pax/reporting/error_message.hpp>
#include <pax/textual/from_string.hpp>

#include <string_view>


namespace pax::metrics {

	class Function {
		
		enum function : std::uint8_t {
			f_count, f_mean, f_mean2, f_variance, f_skewness, f_kurtosis, 
			f_L2, f_L3, f_L4, f_mad, f_pN, f_unidentified,
			f_with_args = f_pN, 
			f_last		= f_pN, 
		};

		// Check if v_ starts with an integer and return a std::span to it. 
		// - Returns std::span< sp_.begin(), sp_.begin() if there is no such.
		static constexpr std::string_view parse_int( const std::string_view v_ ) noexcept {
			const bool		negative = !v_.empty() && ( v_.front() == '-' );
			auto			ptr = v_.begin() + negative;
			const auto	 	end = v_.end();
			while( ( ptr < end ) && ( *ptr >= '0' ) && ( *ptr <= '9' ) )		++ptr;
			return { v_.begin(), ( ptr > v_.begin() + negative ) ? ptr : v_.begin() };
		}
		
		struct Id_description
			{	const char * id;	const char * descr;					};
		static constexpr const Id_description id_descr[ f_last + 1 ] =	{
			{	"count",	"Number of values."							}, 
			{	"mean",		"Mean of the values."						}, 
			{	"mean2",	"Mean of the values^2."						}, 
			{	"variance",	"Sample variance."							}, 
			{	"skewness",	"Sample skewness."							}, 
			{	"kurtosis",	"Sample kurtosis."							}, 
			{	"L2",		"L2-moment (L-scale)."						}, 
			{	"L3",		"L3-moment (L-skewness)."					}, 
			{	"L4",		"L4-moment (L-kurtosis)."					}, 
			{	"mad",		"Median absolute deviation."				}, 
			{	"p{}",		"Percentile {}, where {} is in [0, 100]."	}
		};

		/// Returns the function number of the identified metric calculation. Throws if none was found.
		static constexpr Function parse( const std::string_view id_ )						{
			for( std::size_t i{}; i<f_with_args; ++i )
				if( id_ == id_descr[ i ].id )
					return Function{ function( i ) };

			// Check if percentile.
			if( const auto num = parse_int( id_.substr( 1 ) ); num.size() && ( id_.size() == num.size() + 1 ) && ( id_.front() == 'p' ) ) {
				const auto		perc = from_string< int >( num );
				if( ( perc >= 0 ) && ( perc <= 100 ) && ( id_.size() == num.size() + 1 ) )
					return Function{ f_pN, percentile_type( perc ) };
			}

			return Function{ f_unidentified };
		}

		using percentile_type		  = std::uint8_t;
		percentile_type					m_percentile{};
		function						m_function;
		
		constexpr Function(
			const function				f_, 
			const percentile_type		p_ = 0
		) noexcept : m_percentile{ p_ }, m_function{ f_ } {}

	public:
		constexpr Function( const Function & )				=	default;
		constexpr Function & operator=( const Function & )	=	default;

		/// Construct a metric calculation object as defined by a text string. 
		/// Throws if the string can't be parsed correctly, so every Function instance is valid. 
		Function( const std::string_view id_ ) : Function{ parse( id_ ) } {
			if( m_function == f_unidentified )
				throw error_message( std20::format( "Metric function identification failed: could not properly parse '{}'", id_ ) );
		}
		
		static constexpr Function count()							  noexcept	{	return Function( f_count );		}
		static constexpr Function mean()							  noexcept	{	return Function( f_mean );		}
		static constexpr Function mean2()							  noexcept	{	return Function( f_mean2 );		}
		static constexpr Function variance()						  noexcept	{	return Function( f_variance );	}
		static constexpr Function skewness()						  noexcept	{	return Function( f_skewness );	}
		static constexpr Function kurtosis()						  noexcept	{	return Function( f_kurtosis );	}
		static constexpr Function L2()								  noexcept	{	return Function( f_L2 );		}
		static constexpr Function L3()								  noexcept	{	return Function( f_L3 );		}
		static constexpr Function L4()								  noexcept	{	return Function( f_L4 );		}
		static constexpr Function mad()								  noexcept	{	return Function( f_mad );		}
		static constexpr Function p( unsigned p_ )					  noexcept	{
			return Function( f_pN, ( p_ <= 100 ) ? p_ : 100 );
		}

		/// Return the description string of the Function. 
		constexpr auto description()							const noexcept	{
			return id_descr[ int( m_function ) ].descr;
		}
		

		/// Calculate the metric for data_. 
		template< typename T >
		constexpr T operator()( const std::span< T > ordered_ )	const			{
			using namespace ordered;
			switch( m_function ) {
				case f_count: 			return					 	 ordered_.size();
				case f_mean: 			return pax::mean			( summary< 1 >( ordered_ ) );
				case f_mean2: 			return pax::mean< 2 >		( summary< 2 >( ordered_ ) );
				case f_variance: 		return pax::sample_variance	( summary< 2 >( ordered_ ) );
				case f_skewness: 		return pax::sample_skewness	( summary< 3 >( ordered_ ) );
				case f_kurtosis: 		return pax::sample_kurtosis	( summary< 4 >( ordered_ ) );
				case f_L2:				return L_moment< 2 >		( ordered_ );
				case f_L3:				return L_moment< 3 >		( ordered_ );
				case f_L4:				return L_moment< 4 >		( ordered_ );
				case f_mad: 			return median_mad			( ordered_ ).mad();
				case f_pN: 				return percentile			( ordered_, m_percentile );
				case f_unidentified:	return std::numeric_limits< T >::quiet_NaN();
			}
			// A final return statement is required by gcc.
			return std::numeric_limits< T >::quiet_NaN();
		}
		
		constexpr bool operator==( const Function f_ )			const noexcept	{
			return	( m_function   == f_.m_function   )
				&&	( m_percentile == f_.m_percentile );
		}
		
		constexpr bool operator< ( const Function f_ )			const noexcept	{
			return	( m_function != f_.m_function )	? ( m_function   < f_.m_function )
													: ( m_percentile < f_.m_percentile );
		}
	
	
		/// The textual representation of the filter. 
		friend std::string to_string( const Function f_ )						{
			switch( f_.m_function ) {
				case f_pN: 			return std20::format( "p{}", f_.m_percentile );
				default: 			return id_descr[ int( f_.m_function ) ].id;
			}
		}

		template< typename Out >
		static void help( Out & out_, const std::string_view pre_ ) 			{
			for( auto i : id_descr ) if( i.descr[ 0 ] != 0 )
				out_ << std20::format( "{}    {:10}{}\n", pre_, i.id, i.descr );
		}

		template< typename Out >
		friend Out & operator<<(
			Out					  & out_,
			Function				f_
		) {
			return out_ << to_string( f_ );
		}
	};

	static_assert( sizeof( Function ) == 2 ); 

}	// namespace pax::metrics



namespace pax {
	template< typename >			struct Serialize;
	
	template<>
	struct Serialize< metrics::Function > {
		template< typename J >
		static void assign( J & j_, const metrics::Function t_ )				{
			j_ = {
				{	"value",		to_string( t_ )			},
				{	"description",	t_.description()		}
			};
		}
	};
}	// namespace pax
