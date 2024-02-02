//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <type_traits>


namespace pax::asprs {

	using coordinate_type			=	double;			
	using raw_coordinate_type		=	std::int32_t;			
	using intensity_type			=	std::uint16_t;
	using return_number_type		=	std::uint8_t;
	using classification_type		=	std::uint8_t;
	using scan_angle_rank_type		=	std::int8_t;			
	using user_data_type			=	std::uint8_t;
	using point_source_id_type		=	std::uint16_t;
	using gps_time_type				=	double;				
	using color_type				=	std::uint16_t;

	enum class Classification : classification_type {
		never_classified			=   0,	// 00000:	Created, never classified.
		unspecified					=   1,	// 00001:	Unspecified, also includes any incorrect classes.
		ground						=   2,	// 00010:	Point on ground.
		low_vegetation				=   3,	// 00011:	Low vegetation.
		medium_vegetation			=   4,	// 00100:	Medium vegetation.
		high_vegetation				=   5,	// 00101:	High vegetation.
		building					=   6,	// 00110:	Building.
		low_point_noise				=   7,	// 00111:	Low-point (noise), point under ground level.
	//	reserved					=   8,	// 01000:	
		water						=   9,	// 01001:	Point on water.
		rail						=  10,	// 01010:	reserved for ASPRS Definition
		road						=  11,	// 01011:	reserved for ASPRS Definition
	//	reserved					=  12,	// 01100:	
		wire_guard					=  13,	//	Wire – Guard (Shield)
		wire_conductor				=  14,	//	Wire – Conductor (Phase), 
		transmission_tower			=  15,	//	Transmission Tower, 
		wire_struct_connector		=  16,	//	Wire-Structure Connector
		bridge						=  17,	//	Point on bridges (only classification level 3).
		high_noise					=  18,	//	Hight noise: point over ground, vegetation, building (for example clouds).
		overhead_structure			=  19,	//	Conveyors, mining equipment, traffic lights, etc.
		ignored_ground				=  20,	//	E.g., breakline proximity.
		snow						=  21,	//	
		temporal_exclusion			=  22,	//	Features excluded due to changes over time between data sources – e.g., 
											//	water levels, landslides, permafrost.
	};

	enum class Classification_mask : classification_type {
		classified_mask				=  30,
		lowbit_mask					=  31,
		synthetic_mask				=  32,
		keypoint_mask				=  64,
		withheld_mask				= 128,
		highbit_mask				= synthetic_mask | keypoint_mask | withheld_mask,
	};

	constexpr bool has( const Classification c_, const Classification_mask m_ ) noexcept {
		using type = std::underlying_type_t< Classification >;
		return type( c_ ) & type( m_ );
	}


	/// Returns Classification masked with Classification::lowbit_mask (without high bit stuff). 
	constexpr Classification main_classification( const Classification c_ ) noexcept {
		using type = std::underlying_type_t< Classification >;
		return Classification( type( c_ ) & type( Classification_mask::lowbit_mask ) );
	}


	/// Is the point a valid point when processing lantmateriet data?
	constexpr bool normal_lm_filter( const Classification c_ )      noexcept    {
		switch( main_classification( c_ ) ) {
			case Classification::never_classified: 	// Created, never classified.
			case Classification::unspecified: 		// Unspecified, also includes any incorrect classes.
			case Classification::ground: 			return true;
			default:								return false;
		}
	}
	
	
	/// Is the point 'synthetic'?
	/** If true, then this point was created by a technique other than LIDAR collection, 
		such as digitized from a photogrammetric stereo model.							**/
	constexpr bool is_synthetic( const Classification c_ ) 		    noexcept
	{	return  has( c_, Classification_mask::synthetic_mask );					}


	/// Is the point a 'key-point'?
	/** If true, this point is considered to be a model key-point and thus generally should 
		not be witheld in a thinning algorithm.										    **/
	constexpr bool is_keypoint( const Classification c_ ) 	    	noexcept
	{	return  has( c_, Classification_mask::keypoint_mask );					}


	/// Is the point 'withheld'?
	/** If true, this point should not be included in processing (synonymous with Deleted).	**/
	constexpr bool is_withheld( const Classification c_ ) 	    	noexcept
	{	return  has( c_ , Classification_mask::withheld_mask );					}


	/// Are all the high bits cleared?
	/** \returns the same as !( is_synthetic() || is_key_point() || is_withheld() )		**/
	constexpr bool is_unmasked( const Classification c_ ) 		    noexcept
	{	return !has( c_, Classification_mask::highbit_mask );					}
	
	
	/// Is the classification other than never_classified or unspecified?
	constexpr bool is_classified( const Classification c_ ) 		noexcept
	{	return  has( c_, Classification_mask::classified_mask );				}
	
	
	/// Is the point classified as 'ground'?
	constexpr bool is_ground( const Classification c_ ) 			noexcept
	{	return main_classification( c_ ) == Classification::ground;				}
	
	
	/// Is the point classified as 'vegetation'?
	constexpr bool is_vegetation( const Classification c_ )		    noexcept    {
		switch( main_classification( c_ ) ) {
			case Classification::low_vegetation:
			case Classification::medium_vegetation:
			case Classification::high_vegetation:	return true;
			default:								return false;
		}
	}

}	// namespace pax::asprs
