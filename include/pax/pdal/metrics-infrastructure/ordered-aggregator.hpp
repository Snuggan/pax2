//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <cassert>
#include <span>
#include <vector>
#include <algorithm>


namespace pax { 
	
	/// Maintains an ordered vector (sorted ascending).
	/** If you get all your values and then want ordered access, you might as well use a vector and sort it.
		If you repeatedly add some values and want ordered access to them before adding more, use this container.
		- The container is not (for efficiency reasons) in an ordered state after every insert. 
		- Access to elements are only through std::span(). This is to ensure that access is always to sorted elements.
	**/
	template< std::floating_point T >
	class Ordered_vector : std::vector< T > {
	private:
		using Base						  = std::vector< T >;
		std::size_t							m_ordered{};

		constexpr bool is_ordered()								const noexcept	{	return m_ordered == Base::size();	}

		/// Sort the container (you need rarely to call this, as span() does it).
		constexpr void order()										  noexcept	{
			if( !is_ordered() ) {
				// Order the trailing set (values recently pushed).
				std::sort( Base::begin() + m_ordered, Base::end() );

				// Merge the main and trailing sets.
				std::inplace_merge( Base::begin(), Base::begin() + m_ordered, Base::end() );

				// All values are now in main set.
				m_ordered = Base::size();
			}
		}

	public:
		using value_type				  = const T;
		using Base::empty;
		using Base::size;
		using Base::reserve;
		using Base::capacity;
		using Base::shrink_to_fit;
		using Base::push_back;

		constexpr Ordered_vector()										=	default;
		constexpr Ordered_vector( const Ordered_vector & )				=	default;
		constexpr Ordered_vector( Ordered_vector && ) 					=	default;
		constexpr Ordered_vector & operator=( const Ordered_vector & )	=	default;
		constexpr Ordered_vector & operator=( Ordered_vector && ) 		=	default;

		template< std::floating_point U, std::size_t N >
		explicit constexpr Ordered_vector( const std::span< U, N > data_ ) : Base{}		{
			push_back( data_ );
			order();		// In case we want to create a const Ordered_vector.
		}

		/// Push a bunch of values.
		template< std::floating_point U, std::size_t N >
		constexpr void push_back( const std::span< U, N > data_ )						{
			Base::insert( Base::end(), data_.begin(), data_.end() );
		}

		/// Get a span of the [sorted] container elements.
		constexpr std::span< value_type > ordered_span()		 	   		  noexcept	{
			order();
			return { Base::data(), Base::size() };
		}

		/// If sorted, get a span of the container elements. Throws, otherwise. 
		constexpr std::span< value_type > ordered_span()				const noexcept	{
			assert( is_ordered() && "Ordered_vector: ordered access needed, but const container is not ordered" );
			return { Base::data(), Base::size() };
		}
	};


	template< typename T >	struct Serialize;

	template< typename T >
	struct Serialize< Ordered_vector< T > > : Serialize< std::vector< T > > {
		template< typename ...Format >
		static auto to_string( std::vector< T > & ov_, Format && ...fmt_ ) {
			ov_.order();
			return to_string_container( ov_ , std::forward< Format >( fmt_ )... );
		}

		template< typename ...Format >
		static auto to_string( const std::vector< T > & ov_, Format && ...fmt_ ) {
			return to_string_container( ov_ , std::forward< Format >( fmt_ )... );
		}
	};

}	// namespace pax
