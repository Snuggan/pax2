//	Copyright (c) 2014-2022, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "filter.hpp"
#include "ordered-aggregator.hpp"
#include <pax/math/metrics/ordered.hpp>


namespace pax::metrics {

	class Point_aggregator {
		Ordered_vector< metrics_value_type >	m_all{}, m_firsts{};
		
		constexpr auto narrow(
			const Filter						  & filter_, 
			std::span< const metrics_value_type >	data_
		) const noexcept {
			// Keep only values belove the max level.
			data_ = data_.first  ( ordered::count_lt( data_, filter_.max_level() ) );
			// Keep only values above the min level.
			data_ = data_.subspan( ordered::count_lt( data_, filter_.min_level() ) );
			return data_;
		}

	public:
		using value_type = std::remove_const_t< typename Ordered_vector< metrics_value_type >::value_type >;

		Point_aggregator()											=	default;
		Point_aggregator( const Point_aggregator & )				=	default;
		Point_aggregator( Point_aggregator && )						=	default;
		Point_aggregator & operator=( const Point_aggregator & )	=	default;
		Point_aggregator & operator=( Point_aggregator && )			=	default;

		/// Ordinary number are treated as points with no first returns. 
		template< std::floating_point T, std::size_t N >
		Point_aggregator( const std::span< T, N > data_ ) : m_all{ data_ }, m_firsts{} {}

		/// Push a value. 
		void push_back(
			const value_type		z_, 
			const bool				is_first_return_
		) {
			m_all.push_back( z_ );
			if( is_first_return_ )	m_firsts.push_back( z_ );
		}

		/// Push another point. 
		template< typename Pt >
		void push_back( const Pt & pt_ )			{
			push_back( height( pt_ ), is_first_return( pt_ ) );
		}

		auto empty()										const noexcept	{	return m_all.empty();		}

		void reserve( std::size_t capacity_ )		{
			m_all   .reserve( capacity_ );	
			m_firsts.reserve( capacity_ );
		}

		void shrink_to_fit()						{
			m_all   .shrink_to_fit();	
			m_firsts.shrink_to_fit();
		}

		/// Return a std::span of z values as specified by filter_. 
		/** Warning: if you push more points you might invalidate the returned std::span!	**/
		auto ordered_span( const Filter filter_ )				  noexcept	{
			return narrow( filter_, filter_.first_only() ? m_firsts.ordered_span() : m_all.ordered_span() );
		}

		/// Return a std::span of z values as specified by filter_. 
		/** Warning: if you push more points you might invalidate the returned std::span!	**/
		auto ordered_span( const Filter filter_ )			const			{
			return narrow( filter_, filter_.first_only() ? m_firsts.ordered_span() : m_all.ordered_span() );
		}
	};

}	// namespace pax::metrics
