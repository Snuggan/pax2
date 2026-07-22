//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "box.hpp"
#include <string>
#include <span>


namespace pax {

	template< typename T, typename ... P >	struct Table_meta;
	template< floating F >					struct Object_meta;


	
	/// Implements an object with coordinates and a scalar value.
	/// It can have any rank you please, but two is probably usual.
	template< floating F, std::size_t N >							requires( is_static< N > )
	class Circle {
		Point< F, N >												m_center{};
		F															m_radius{};

		constexpr const Point< F, N > & center()					const noexcept	{	return m_center;	}
		
	public:
		static constexpr std::size_t 				rank		  = N;
		using 										Pt			  = Point< F, N >;
		using value_type = Pt::value_type;

		constexpr Circle()									 	 = default;
		constexpr Circle( const Circle & )						  = default;
		constexpr Circle( Circle && )							  = default;
		constexpr Circle & operator=( const Circle & )			  = default;
		constexpr Circle & operator=( Circle && )				  = default;

		constexpr Circle( const Point< F, N > & center_, const F radius_ ) noexcept
			: m_center( center_ ), m_radius{ ( radius_ >= 0 ) ? radius_ : -radius_ } {}

		constexpr Circle(
			const F					east_,
			const F					north_,
			const F					radius_
		) noexcept requires( rank == 2 ) : Circle( Pt{ east_, north_ }, radius_ ) {}


		constexpr value_type radius()								const noexcept	{	return m_radius;	}

		/// The center coordinates.
		constexpr bool operator==( const Circle & c_ ) 				const noexcept	{
			return ( center() == c_.center() ) && ( radius() == c_.radius() );
		}

		/// The center coordinates.
		friend constexpr const Pt & center( const Circle & c_ ) 	noexcept		{	return c_.center();	}

		/// The minimal coordinates of a bounding box of circle's.
		friend constexpr Pt min( const Circle & c_ )				noexcept		{
			auto [ ... c ]		  = c_.center();
			return { ( c - c_.radius() ) ... };
		}

		/// The maximal coordinates of a bounding box of circle's.
		friend constexpr Pt max( const Circle & c_ )				noexcept		{
			auto [ ... c ]		  = c_.center();
			return { ( c + c_.radius() ) ... };
		}

		/// The point is inside, but not on, the circle.
		constexpr bool strictly_inside( const Point< F, N > & pt_ )	const noexcept	{
			return distance2( center(), pt_ ) <  radius()*radius();
		}

		/// The point is either inside or on the circle.
		constexpr bool inside_or_on( const Point< F, N > & pt_ )	const noexcept	{
			return distance2( center(), pt_ ) <= radius()*radius();
		}

		/// Box contents to std::string.
		explicit constexpr operator std::string() 					const			{
			return std::format( "{{{}, {}}}", center(), radius() );
		}

		/// Stream a box contents.
		template< typename Out >
		friend constexpr Out & operator<<(
			Out				  & out_,
			const Circle	  & c_
		) {
			return out_ << std::string( c_ );
		}
	};
	
	using Circle2d			  = Circle< double, 2 >;
	using Circle3d			  = Circle< double, 3 >;

	template< floating F, std::size_t N, floating F0 >
	Circle( const Point< F, N > &, F0 ) -> Circle< F, N >;


	/// This is used to read Circle values from a csv file using Text_table.
	template< floating F >
	struct Object_meta< Circle< F, 2 > > {
		static constexpr auto value = Table_meta< Circle< F, 2 >, F, F, F >{ "east", "north", "radius" };
	};


	/// The point is either inside or on the circle.
	template< floating F, std::size_t N >							requires( is_static< N > )
	constexpr bool contains( 
		const Circle< F, N >	  & circle_,
		const Point< F, N >		  & pt_
	) noexcept {
		return	circle_.inside_or_on( pt_ );
	}


	/// Does the Box contain the Circle_? (They may touch.)
	template< floating F, std::size_t N >							requires( is_static< N > )
	constexpr bool contains( 
		const Box< F, N >		  & box_, 
		const Circle< F, N >	  & circle_ 
	) noexcept {
		return	all_le( min( box_    ),	min( circle_ ) )
			&&	all_le( max( circle_ ),	max( box_    ) );
	}


	/// Does the Box contain the Circle_? (They may touch.)
	template< floating F, std::size_t N >							requires( is_static< N > )
	constexpr bool overlap( 
		const Box< F, N >		  & box_, 
		const Circle< F, N >	  & circle_ 
	) noexcept {
		return	all_lt( min( box_    ), max( circle_ ) )
			&&	all_lt( min( circle_ ), max( box_    ) );
	}
	



	/// A simple container for the spacial data of a plot/circle with an id.
	template< floating F, std::size_t N >
	class Circle_w_id : public Circle< F, N > {
		std::string						m_id{};
		using Base					  = Circle< F, N >;

	public:
		constexpr Circle_w_id()									  = default;
		constexpr Circle_w_id( const Circle_w_id & )			  = default;
		constexpr Circle_w_id( Circle_w_id && )					  = default;
		constexpr Circle_w_id & operator=( const Circle_w_id & )  = default;
		constexpr Circle_w_id & operator=( Circle_w_id && )		  = default;

		constexpr Circle_w_id(
			const Base				  & circle_,
			const std::string_view		id_
		) noexcept : Base{ circle_ }, m_id{ id_ } {}

		constexpr Circle_w_id(
			const Base::Pt			  & center_,
			const F						radius_,
			const std::string_view		id_
		) noexcept : Base{ center_, radius_ }, m_id{ id_ } {}

		constexpr Circle_w_id(
			const F						east_,
			const F						north_,
			const F						radius_,
			const std::string_view		id_
		) noexcept requires( N == 2 ) : Circle_w_id( Base( { east_, north_ }, radius_ ), id_ ) {}
	
		/// Return the [unique] id. No other circle/plot may have the same. 
		constexpr const std::string & id() 							const noexcept	{	return m_id;	}
	};
	
	using Circle_w_id2d		  = Circle_w_id< double, 2 >;
	using Circle_w_id3d		  = Circle_w_id< double, 3 >;

	template< floating F, std::size_t N, floating F0, traits::string Str >
	Circle_w_id( const Point< F, N > &, F0, Str ) -> Circle_w_id< F, N >;
	

	/// This is used to read values from a csv file using Text_table.
	template< floating F >
	struct Object_meta< Circle_w_id< F, 2 > > {
		static constexpr auto value = Table_meta< Circle_w_id< F, 2 >, F, F, F, std::string >
			{ "east", "north", "radius", "id" };
	};

}	// namespace pax
