//	Copyright (c) 2014-2022, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "filter.hpp"
#include "function.hpp"
#include "point-aggregator.hpp"

#include <vector>


namespace pax::metrics {

	/// Encapsulates a specified filter and calculation pair. 
	class Function_filter {
		template< typename ...Nilsson >
		static Function_filter metric_id_divide( const std::string_view id_, Nilsson && ...nilsson_ ) {
			const auto			divide = id_.find( function_filter_divider );
			if( divide >= id_.size() )
				throw error_message( std20::format( "No divisor '{}' of function and filter parts found in '{}'.", function_filter_divider, id_ ) );

			return Function_filter(
				Function( id_.substr( 0, divide ) ), 
				Filter( id_.substr( divide + 1 ), std::forward< Nilsson >( nilsson_ )... )
			);
		}
	
		template< typename Out >
		static Out & print_sets( Out & out_, const std::string_view pre_ ) {
			for( const auto name : { "basic-linear", "inka-berries", "extra-allt" } ) {
				const auto items = create_set( name, 9.99 );
				out_ << pre_ << "    " << name << ": [";
				for( const auto item : items )		out_ << item << " ";
				out_ << "]\n";
			}
			return out_;
		}

		Filter					m_filter;
		Function				m_function;

	public:
		static constexpr char	function_filter_divider		=	'_';

		constexpr Function_filter(
			const Function		function_, 
			const Filter		filter_ 
		) noexcept : m_filter{ filter_ }, m_function{ function_ } {}

		constexpr Function_filter( const Function_filter & )				=	default;
		constexpr Function_filter & operator=( const Function_filter & )	=	default;

		/// Construct a Function_filter (a metric-filter pair) as defined by a text string. 
		/** The textual format is: ´function_filter' or 'collection-id'.		**/
		Function_filter( const std::string_view id_ ) 
			: Function_filter{ metric_id_divide( id_ ) } {}
	
		/// Construct a Function_filter (a metric-filter pair) as defined by a text string. 
		/** The textual format is: ´function_filter' or 'collection-id'.		**/
		Function_filter(
			const std::string_view		id_, 
			const metrics_value_type	nilsson_ 
		) : Function_filter{ metric_id_divide( id_, nilsson_ ) } {}
	
		metrics_value_type calculate( Point_aggregator & acc_ )	const {
			return m_function( acc_.ordered_span( m_filter ) );
		}

		Function function()							const noexcept	{	return m_function;		}
		Filter filter()								const noexcept	{	return m_filter;		}
		
		bool operator==( const Function_filter o_ )	const noexcept	{
			return	( m_function == o_.m_function )
				&&	( m_filter	 == o_.m_filter   );
		}
		
		bool operator< ( const Function_filter o_ )	const noexcept	{
			return	( m_filter != o_.m_filter )	? ( m_filter   < o_.m_filter   )
												: ( m_function < o_.m_function );
		}
		
		friend std::string to_string( const Function_filter	ff_ ) {
			return std20::format( "{}{}{}", to_string( ff_.m_function ), function_filter_divider, to_string( ff_.m_filter ) );
		}
		
		template< typename Out >
		friend Out & operator<<(
			Out						  & out_,
			const Function_filter		ff_
		) {
			out_ << to_string( ff_ );
			return out_;
		}
	
		template< typename Out >
		static void help( Out & out_, const std::string_view pre_ = "\t" ) {
			out_	<< pre_ << "A metric identifier has two parts:\n"
					<< pre_ << "    function" << function_filter_divider << "filter\n";
			Filter::help( out_, pre_ );
			out_	<< pre_ << "The possible functions are:\n";
			Function::help( out_, pre_ );
			out_	<< pre_ << "Examples:\n"
					<< pre_ << "    count_all\n"
					<< pre_ << "    extra-allt\n"
					<< pre_ << "    count_1ret_ge180cm\n"
					<< pre_ << "    count_all_ge500cm_lt1000cm\n"
					<< pre_ << "    p95_1ret_ge180cm\n";
			out_	<< pre_ << "Preconfigured sets (where 999cm symbolizes the nilson level):\n";
			print_sets( out_, pre_ );
		}


		static std::vector< Function_filter > create_set( const std::string_view id_, const metrics_value_type nilsson_ ) {
			if       ( id_ == "basic-linear" ) {
				return { 
					{ Function::count(),	Filter:: all()				},
					{ Function::variance(),	Filter:: all_ge( nilsson_ )	},
					{ Function::p( 30 ),	Filter:: all_ge( nilsson_ )	},
					{ Function::p( 80 ),	Filter:: all_ge( nilsson_ )	},
					{ Function::p( 95 ),	Filter:: all_ge( nilsson_ )	},
					{ Function::count(),	Filter::ret1_ge( nilsson_ )	},
					{ Function::count(),	Filter::ret1()				}
				};

			} else if( id_ == "inka-berries" ) {
				return { 
					{ Function::L3(),		Filter:: all()				},
					{ Function::p( 30 ),	Filter:: all()				}, 
					{ Function::count(),	Filter::ret1_ge( nilsson_ )	}, 
					{ Function::count(),	Filter::ret1()				}, 
					{ Function::mean2(),	Filter::ret1()				}
				};

			} else if( id_ == "extra-allt" ) {
				std::vector< Function_filter >	collection{};

				for( const Filter filter : {
					Filter:: all_ge( 5 ), Filter:: all_ge( 10 ), Filter:: all_ge( 15 ),
					Filter::ret1_ge( 5 ), Filter::ret1_ge( 10 ), Filter::ret1_ge( 15 )
				} )	collection.emplace_back( Function::count(), filter );

				for( const Function function : {
					Function::count(),	Function::mean(),	Function::mean2(),	Function::variance(), 
					Function::skewness(),					Function::kurtosis(), 
					Function::p( 10 ),	Function::p( 20 ),	Function::p( 30 ),	Function::p( 40 ),	Function::p( 50 ), 
					Function::p( 60 ),	Function::p( 70 ),	Function::p( 80 ),	Function::p( 90 ),	Function::p( 95 ), 
					Function::L2(),   	Function::L3(),   	Function::L4(),   	Function::mad()
				} ) for( const auto filter : { 
					Filter::all(),		Filter::all_ge( nilsson_ ), 
					Filter::ret1(),		Filter::ret1_ge( nilsson_ )
				} )	collection.emplace_back( function, filter );
				
				std::sort( collection.begin(), collection.end() );
				return collection;
			}
			
			return { { id_, nilsson_ } };
		}


		template< traits::string S, std::size_t N >
		static std::vector< Function_filter > create_set( const std::span< S, N > ids_, const metrics_value_type nilsson_ ) {
			std::vector< Function_filter >	collection;
			for( auto id : ids_ ) {
				const auto items = create_set( id, nilsson_ );
				collection.insert( collection.end(), items.begin(), items.end() );
			}

			std::sort( collection.begin(), collection.end() );
			const auto dummy = Function_filter( "count_all" );
			collection.resize( std::unique( collection.begin(), collection.end() ) - collection.begin(), dummy );
			return collection;
		}
	};

	static_assert( sizeof( Function_filter ) == 8 ); 



	/// Takes a bunch of strings and converts them to a vector of Function_filter. Or throws.
	template< typename Id >
	std::vector< Function_filter > metric_set( const Id & id_, const metrics_value_type nilsson_ ) {
		return Function_filter::create_set( id_, nilsson_ );
	}


	template< typename Out >
	Out & operator<<(
		Out									  & out_, 
		const std::span< Function_filter >	  & items_
	) {
		return std20::format_to( out_, "{}", items_ );
	}

}	// namespace pax::metrics

namespace pax {
	template< typename >			struct Serialize;
	
	template<>
	struct Serialize< metrics::Function_filter > {
		template< typename J >
		static void assign( J & j_, const metrics::Function_filter t_ ) {
			j_ = {
				{	"value",		to_string( t_ )		},
				{	"function",		"<see below>"		},
				{	"filter",		"<see below>"   	}
			};
			Serialize< metrics::Function >::assign( j_[ "function" ], t_.function() );
			Serialize< metrics::Filter   >::assign( j_[ "filter"   ], t_.filter() );
		}
	};
}	// namespace pax
