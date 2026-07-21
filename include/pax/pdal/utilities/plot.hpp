//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <pax/tables/meta.hpp>
#include <pdal/PointRef.hpp>
#include <array>
#include <string>
#include <string_view>


namespace pax {
	
	inline constexpr double lower_x( const pdal::BOX2D & box_ )	noexcept	{	return box_.minx;		}
	inline constexpr double upper_x( const pdal::BOX2D & box_ )	noexcept	{	return box_.maxx;		}
	inline constexpr double lower_y( const pdal::BOX2D & box_ )	noexcept	{	return box_.miny;		}
	inline constexpr double upper_y( const pdal::BOX2D & box_ )	noexcept	{	return box_.maxy;		}

	template< typename Box >
	constexpr double lower_x( const Box & box_ )	noexcept	{	return get< 0 >( std::get< 0 >( box_ ) );	}

	template< typename Box >
	constexpr double upper_x( const Box & box_ )	noexcept	{	return get< 0 >( std::get< 1 >( box_ ) );	}

	template< typename Box >
	constexpr double lower_y( const Box & box_ )	noexcept	{	return get< 1 >( std::get< 0 >( box_ ) );	}

	template< typename Box >
	constexpr double upper_y( const Box & box_ )	noexcept	{	return get< 1 >( std::get< 1 >( box_ ) );	}
	


	/// A simple container for the spacial data of a plot.
	class Plot : public std::array< double, 2 > {
	protected:
		using coord_type			  = double;
		using point_type			  = std::array< coord_type, 2 >;
		
	private:
		coord_type						m_radius{};
		
		enum class Inclusion			{	overlapped, contained, 	};
		static constexpr Inclusion		inclusion{ Inclusion::contained };
		static constexpr const char *	inclusion_id_[ 2 ] =	{	"overlapped", "contained"	};

		// Originally 32, changed to 40 in an update to pdal.
		static_assert( sizeof( pdal::PointRef ) == 40 || sizeof( pdal::PointRef ) == 32 );

		static constexpr coord_type square( const coord_type v_ )	noexcept		{	return v_*v_;	}
	

		/// Does the plot overlap any part of box_?
		template< typename BBox >
		constexpr bool overlapped( const BBox & bbox_ ) const noexcept {
			using std::get;
			return	( lower_x( bbox_ ) <  get< 0 >( *this ) + radius() )
				&&	( lower_y( bbox_ ) <  get< 1 >( *this ) + radius() )
				&&	( upper_x( bbox_ ) >  get< 0 >( *this ) - radius() )
				&&	( upper_y( bbox_ ) >  get< 1 >( *this ) - radius() );
		}


		/// Is the plot completely inside box_?
		template< typename BBox >
		constexpr bool contained( const BBox & bbox_ ) const noexcept {
			using std::get;
			return	( lower_x( bbox_ ) <= get< 0 >( *this ) - radius() )
				&&	( lower_y( bbox_ ) <= get< 1 >( *this ) - radius() )
				&&	( upper_x( bbox_ ) >= get< 0 >( *this ) + radius() )
				&&	( upper_y( bbox_ ) >= get< 1 >( *this ) + radius() );
		}


	public:
		constexpr Plot()							  = default;
		constexpr Plot( const Plot & )				  = default;
		constexpr Plot( Plot && )					  = default;
		constexpr Plot & operator=( const Plot & )	  = default;
		constexpr Plot & operator=( Plot && )		  = default;

		constexpr Plot(
			const coord_type		east_, 
			const coord_type		north_,
			const coord_type		radius_
		) noexcept : point_type{ east_, north_ }, m_radius{ radius_ } {}


		/// This is used to read values from a csv file.
		using Meta = Table_meta< Plot, coord_type, coord_type, coord_type >;
		static constexpr Meta table_meta()  							noexcept	{
			return { "east", "north", "radius" };
		}


		/// 
		static constexpr auto inclusion_id() noexcept {
			return inclusion_id_[ std::size_t( inclusion ) ];
		}
		
		
		/// Return the radius.
		constexpr coord_type radius() const noexcept {
			return m_radius;
		}


		/// Return the radius.
		constexpr void set_radius( const coord_type radius_ ) noexcept {
			m_radius = radius_;
		}


		/// Should the plot recieve points from file_ (with the bbox_)?
		template< typename BBox >
		constexpr bool in_box( const BBox & bbox_ ) const noexcept {
			if      constexpr( inclusion == Inclusion::overlapped )		return overlapped( bbox_ );
			else if constexpr( inclusion == Inclusion::contained )		return contained ( bbox_ );
		}


		/// Is pt_ inside the area?
		constexpr bool contains( const pdal::PointRef	pt_ ) const noexcept {
			using std::get;
			return	square( get< 0 >( *this ) - pt_.getFieldAs< coord_type >( pdal::Dimension::Id::X ) ) 
				+	square( get< 1 >( *this ) - pt_.getFieldAs< coord_type >( pdal::Dimension::Id::Y ) ) 
				<=  square( radius() );
		}
	};
	


	/// A simple container for the spacial data of a plot.
	class Plot_w_id : public Plot {
		std::string					m_plot_id{};

	public:
		constexpr Plot_w_id()								  = default;
		constexpr Plot_w_id( const Plot_w_id & )			  = default;
		constexpr Plot_w_id( Plot_w_id && )					  = default;
		constexpr Plot_w_id & operator=( const Plot_w_id & )  = default;
		constexpr Plot_w_id & operator=( Plot_w_id && )		  = default;

		constexpr Plot_w_id(
			const coord_type		east_, 
			const coord_type		north_,
			const coord_type		radius_,
			const std::string_view	plot_id_ = std::string_view{}
		) noexcept : Plot{ east_, north_, radius_ }, m_plot_id{ plot_id_ } {}


		/// This is used to read values from a csv file.
		using Meta = Table_meta< Plot_w_id, coord_type, coord_type, coord_type, std::string >;
		static constexpr Meta table_meta( const std::string_view id_col_ ) noexcept {
			return { "east", "north", "radius", id_col_ };
		}
		
		
		/// Return the radius.
		constexpr std::string id() 								const noexcept	{	return m_plot_id;	}
	};

}	// namespace pax
