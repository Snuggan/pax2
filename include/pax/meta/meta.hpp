//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "version.hpp"
#include "../std/format.hpp"
#include <thread>				// std::thread::hardware_concurrency
#include <chrono>				// std::formatter< std::chrono::sys_time< Duration > >


namespace pax {
	
	class Meta2 {
		std::string		m_name, m_usage, m_title, m_description;
		
	public:
		std::string_view name()			const noexcept	{	return m_name;								}
		std::string_view usage()		const noexcept	{	return m_usage;								}
		std::string_view title()		const noexcept	{	return m_title;								}
		std::string_view description()	const noexcept	{	return m_description;						}
		std::string		 info()			const			{
			return std20::format( "\033[1m{}\033[0m, part of {}, compiled {} ({})\n\n",
				m_name, metadata::project::named_version, metadata::compilation::dateTtime, metadata::compilation::status
			)	+ std20::format( "Copyright:  {}\n", metadata::copyright )
				+ std20::format( "License:    {}\n", metadata::license )
			//	+ std20::format( "Author:     {}\n", metadata::author )
			//	+ std20::format( "Maintainer: {}\n", metadata::maintainer )
				+ std20::format( "Repository: {}\n", metadata::project::repository )
				+ std20::format( "\nResources used:\n{}\n{}\n{}\n{}\n{}\n{}\n", 
					metadata::fast_float::named_version, 
					metadata::fmt::named_version, 
					metadata::gdal::named_version, 
					metadata::json::named_version,
					metadata::pdal::named_version,
					metadata::type_name_rt::named_version
				);
		}

		Meta2(
			const std::string_view	name_,
			const std::string_view	usage_ = "",
			const std::string_view	title_ = "",
			const std::string_view	description_ = ""
		) : m_name{ name_ }, m_usage{ usage_ }, m_title{ title_ }, m_description{ description_ } {}
	};



	template< typename T > struct Serialize;
	
	template<> struct Serialize< Meta2 > {
		template< typename J >
		static void assign(
			J								  & j_, 
			const Meta2						  & meta_
		) noexcept {
			static constexpr const std::string_view resources[] = {
				metadata::cmake::named_version,
				metadata::doctest::named_version,
				metadata::fast_float::named_version, 
				metadata::fmt::named_version,
				metadata::gdal::named_version,
				metadata::json::named_version,
				metadata::pdal::named_version,
				metadata::type_name_rt::named_version
			};

			j_ = {
				{	"compilation",		{
					{	"base",					std20::format( "{}, compiled {} ({})",
												metadata::project::named_version, 
												metadata::compilation::dateTtime, 
												metadata::compilation::status 
					) },
					{	"copyright",			metadata::copyright							},
					{	"license",				metadata::license							},
					{	"repository",			metadata::project::repository				},
					{	"c++",					metadata::compilation::cpp_standard			},
					{	"compiler",				metadata::compilation::named_version		},
					{	"flags",				metadata::compilation::flags				},
					{	"processor",			metadata::compilation::processor			},
					{	"resources",			resources									},
					{	"standard_library",		metadata::compilation::standard_library		},
					{	"system",		{
						{	"name",				metadata::system::named_version				},
						{	"is_unix",			metadata::system::unix						}
					}}
				}},
				{	"execution",		{
					{	"concurrency",			std::thread::hardware_concurrency()			},
					{	"time",					std20::format( "{:%FT%T}Z", 
						std::chrono::floor< std::chrono::seconds >( std::chrono::system_clock::now() ) )	},
					{	"tool",			{
						{	"author",			metadata::author							},
						{	"copyright",		metadata::copyright							},
						{	"description",		meta_.description()							},
						{	"license",			metadata::license							},
						{	"maintainer",		metadata::maintainer						},
						{	"title",			meta_.title()								},
						{	"tool",				meta_.name()								}
					}}
				}}
			};
		}
	};

}	// namespace pax
