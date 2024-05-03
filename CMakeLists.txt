
cmake_minimum_required( VERSION 3.12 )

project( "pax2" 
	VERSION		1.0.0	# Always use 3 numbers -- no fewer, no more!
	DESCRIPTION	"A personal library of handy things and pdal modules."
	LANGUAGES	CXX
)


## Set the c++ standard required.
set( CMAKE_CXX_STANDARD 				23 )
set( CMAKE_CXX_STANDARD_REQUIRED 		ON )
set( CMAKE_CXX_EXTENSIONS 				OFF )


list( APPEND PAX_FLAGS
#	-v									# Verbose.
	-O3									# Turn on all sorts of optimizations.
	-msse3								# i386 and x86-64 Options: use sse.
	-pedantic -Wall -Wextra -Werror		# Maximum number of checks.
	-Wno-unused-function				# What is unused in one target might be used in another...
	-Wno-unknown-attributes				# Allow unknown attributes, such as [[likely]].
	-Wno-ambiguous-reversed-operator
)


#set( PAX_USE_SANITIZE 				1 )
list( APPEND PAX_SANITIZE
	-fexperimental-library
	-fno-omit-frame-pointer
	-fsanitize-memory-track-origins
	-fsanitize=address 
	-fsanitize=alignment
	-fsanitize=bounds
	-fsanitize=enum
	-fsanitize=integer-divide-by-zero
	-fsanitize=memory 					# Untried, see https://m-peko.github.io/craft-cpp/posts/be-wise-sanitize-keeping-your-cpp-code-free-from-bugs/
	-fsanitize=null
	-fsanitize=nullability
	-fsanitize=nullability				# Not available in my gcc.
	-fsanitize=return
	-fsanitize=signed-integer-overflow
	-fsanitize=undefined
	-fsanitize=unreachable
	-fsanitize=unsigned-integer-overflow
	-fsanitize=vptr
)


if( ${CMAKE_CXX_COMPILER_ID} MATCHES "Clang" )	
	add_definitions( -D__IS_CLANG__=1 )
	list( APPEND PAX_FLAGS
		# No clang-specific flags.
	)
	list( REMOVE_ITEM PAX_SANITIZE
		-fsanitize-memory-track-origins	# Untried, see https://m-peko.github.io/craft-cpp/posts/be-wise-sanitize-keeping-your-cpp-code-free-from-bugs/
		-fsanitize=memory				# Untried, see https://m-peko.github.io/craft-cpp/posts/be-wise-sanitize-keeping-your-cpp-code-free-from-bugs/
		-fsanitize=signed-integer-overflow
		-fsanitize=undefined
	)
elseif( ${CMAKE_CXX_COMPILER_ID} MATCHES "GNU" )
	add_definitions( -D__IS_GCC__=1 )
	list( APPEND PAX_FLAGS
		# No gcc-specific flags.
	)
	list( REMOVE_ITEM PAX_SANITIZE
		-fsanitize=nullability			# Not available in my gcc.
		-fsanitize=undefined
	)
endif()

add_compile_options( ${PAX_FLAGS} )

set( PAX_DIRECTORY "include/pax" )

# Find the required libraries
include_directories( "/opt/local/include/" )
include_directories( "/usr/local/include/" )
include_directories( "${PAX_DIRECTORY}/.." )



#################################################
################# Get metadata ##################
#################################################


### Project metadata
list( APPEND PROJECT_VERSION_ARRAY ${PROJECT_VERSION_MAJOR} ${PROJECT_VERSION_MINOR} ${PROJECT_VERSION_PATCH} ${PROJECT_VERSION_TWEAK} )
list( JOIN PROJECT_VERSION_ARRAY ", " PROJECT_VERSION_ARRAY )

string( TIMESTAMP PROJECT_UNIXTIME %s UTC )
string( TIMESTAMP PROJECT_DATE_TIME "%Y-%m-%dT%H:%M:%SZ" )
string( TIMESTAMP PROJECT_COPYRIGHT "©2014–%Y, Peder Axensten" )
string( REPLACE " " "\", \"" PROJECT_CXX_FLAGS "${CMAKE_CXX_FLAGS}" )

set( IS_UNIX    false )
set( IS_WINDOWS false )
if( ${UNIX} ) 
	set( IS_UNIX true )
endif()
if( ${WIN32} ) 
	set( IS_WINDOWS true )
endif()

list( SORT PAX_FLAGS CASE INSENSITIVE )
string( JOIN "\",\n\t\t\t\t\"" PAX_FLAGS_NICE ${PAX_FLAGS} )

message( STATUS "############### Versions" )

### Version info: revision and changeset.
find_package( Git )
if( GIT_EXECUTABLE )
	execute_process( COMMAND
		"${GIT_EXECUTABLE}" describe --match=NeVeRmAtCh --always --abbrev=40
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		OUTPUT_VARIABLE PAX_PROJECT_CHANGESET_SHA1
		ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE )
	execute_process( COMMAND
		"${GIT_EXECUTABLE}" rev-list HEAD --count
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		OUTPUT_VARIABLE PAX_PROJECT_REVISION
		ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE )
	execute_process( COMMAND
		"${GIT_EXECUTABLE}" log -1 --format=%h
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		OUTPUT_VARIABLE PAX_PROJECT_CHANGESET
		ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE )
	execute_process( COMMAND
		"${GIT_EXECUTABLE}" rev-parse --abbrev-ref HEAD
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		OUTPUT_VARIABLE PAX_PROJECT_VARIANT
		ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE )
	execute_process( COMMAND
		"${GIT_EXECUTABLE}" config --get remote.origin.url
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		OUTPUT_VARIABLE PAX_PROJECT_REPOSITORY
		ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE )
endif()

set( UUID_DNS_NAMESPACE 6ba7b810-9dad-11d1-80b4-00c04fd430c8 )
string( UUID PROJECT_UUID_MD5  NAMESPACE ${UUID_DNS_NAMESPACE} NAME www.pax.se TYPE MD5 )
string( UUID PROJECT_UUID_SHA1 NAMESPACE ${UUID_DNS_NAMESPACE} NAME www.pax.se TYPE SHA1 )
message( STATUS "Project:        ${PROJECT_NAME} ${PROJECT_VERSION}r${PAX_PROJECT_REVISION} (${CMAKE_BUILD_TYPE})" )

### System metadata
message( STATUS "System:         ${CMAKE_SYSTEM}" )
if( ${CMAKE_SYSTEM_VERSION} MATCHES "([0-9]+)(\.([0-9]+)(\.([0-9]+)))" )
	list( APPEND CMAKE_SYSTEM_VERSION_ARRAY ${CMAKE_MATCH_1} ${CMAKE_MATCH_3} ${CMAKE_MATCH_5} )
	list( JOIN CMAKE_SYSTEM_VERSION_ARRAY ", " CMAKE_SYSTEM_VERSION_ARRAY )
endif()
set( PAX_SYSTEM							${CMAKE_SYSTEM} )
set( PAX_SYSTEM_NAME					${CMAKE_SYSTEM_NAME} )
set( PAX_SYSTEM_RELEASE					${CMAKE_SYSTEM_VERSION} )
set( PAX_SYSTEM_ALIAS					${CMAKE_SYSTEM_NAME} )
if( EXISTS "/etc/lsb-release" )
	file( READ "/etc/lsb-release" PAX_SYSTEM_INFO )
	string( REGEX MATCH "DISTRIB_DESCRIPTION=\"([^\n]*)\"" _ ${PAX_SYSTEM_INFO} )
	set( PAX_SYSTEM 					"${CMAKE_MATCH_1} (${CMAKE_SYSTEM})" )
	string( REGEX MATCH "DISTRIB_ID=([^\n]*)" _ ${PAX_SYSTEM_INFO} )
	set( PAX_SYSTEM_NAME				"${CMAKE_MATCH_1}" )
	string( REGEX MATCH "DISTRIB_RELEASE=([^\n]*)" _ ${PAX_SYSTEM_INFO} )
	set( PAX_SYSTEM_RELEASE				"${CMAKE_MATCH_1}" )
	string( REGEX MATCH "DISTRIB_CODENAME=([^\n]*)" _ ${PAX_SYSTEM_INFO} )
	set( PAX_SYSTEM_ALIAS				"${CMAKE_MATCH_1}" )
endif()


### Compiler metadata
set( PAX_CPP_LIBRARY					<undetermined> )
if( ${__GLIBCXX__} )					# Does not work.
	set( PAX_CPP_LIBRARY				libstdc++ )
elseif( ${_LIBCPP_VERSION} )			# Does not work.
	set( PAX_CPP_LIBRARY				libc++ )
endif()
message( STATUS "C++ compiler:   ${CMAKE_CXX_COMPILER} (${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION})" )
if( ${CMAKE_CXX_COMPILER_VERSION} MATCHES "([0-9]+)(\.([0-9]+)(\.([0-9]+)))" )
	list( APPEND CMAKE_CXX_COMPILER_VERSION_ARRAY ${CMAKE_MATCH_1} ${CMAKE_MATCH_3} ${CMAKE_MATCH_5} )
	list( JOIN CMAKE_CXX_COMPILER_VERSION_ARRAY ", " CMAKE_CXX_COMPILER_VERSION_ARRAY )
endif()


### Cmake metadata
list( APPEND CMAKE_VERSION_ARRAY ${CMAKE_MAJOR_VERSION} ${CMAKE_MINOR_VERSION} ${CMAKE_PATCH_VERSION} ${CMAKE_TWEAK_VERSION} )
list( JOIN CMAKE_VERSION_ARRAY ", " CMAKE_VERSION_ARRAY )


### Sanitize metadata
if( ${PAX_USE_SANITIZE} )
	# Compile with a memory error detector (twice as slow runtime)
	# In shell: export ASAN_OPTIONS="detect_odr_violation=0 alloc_dealloc_mismatch=0"
	# https://github.com/google/sanitizers/wiki/AddressSanitizer
	message( STATUS "############### SANITIZE" )
	add_compile_options( ${PAX_SANITIZE} )
	add_link_options( ${PAX_SANITIZE} )
endif()


### Doctest metadata
set( DOCTEST "${PAX_DIRECTORY}/external/doctest/doctest.h" )
file( READ ${DOCTEST} doctest_h )
if( doctest_h MATCHES "DOCTEST_VERSION_MAJOR ([0-9]+)" )
	list( APPEND DOCTEST_VERSION_ARRAY ${CMAKE_MATCH_1} )
	if( doctest_h MATCHES "DOCTEST_VERSION_MINOR ([0-9]+)" )
		list( APPEND DOCTEST_VERSION_ARRAY ${CMAKE_MATCH_1} )
		if( doctest_h MATCHES "DOCTEST_VERSION_PATCH ([0-9]+)" )
			list( APPEND DOCTEST_VERSION_ARRAY ${CMAKE_MATCH_1} )
		endif()
	endif()
endif()
list( JOIN DOCTEST_VERSION_ARRAY "."  DOCTEST_VERSION )
list( JOIN DOCTEST_VERSION_ARRAY ", " DOCTEST_VERSION_ARRAY )
message( STATUS "doctest:        ${DOCTEST_VERSION}" )


### fmt metadata
#set( FORMAT "${PAX_DIRECTORY}/external/fmt/core.h" )
set( FORMAT "${PAX_DIRECTORY}/external/fmt/base.h" )
file( READ ${FORMAT} fmt_version_h )
if( fmt_version_h MATCHES "FMT_VERSION ([0-9]+)([0-9][0-9])([0-9][0-9])" )
	math( EXPR FMT_VERSION_MAJOR ${CMAKE_MATCH_1} )	# Use math to skip leading zeros if any.
	math( EXPR FMT_VERSION_MINOR ${CMAKE_MATCH_2} )
	math( EXPR FMT_VERSION_PATCH ${CMAKE_MATCH_3} )
	list( APPEND FMT_VERSION_ARRAY ${FMT_VERSION_MAJOR} ${FMT_VERSION_MINOR} ${FMT_VERSION_PATCH} )
	list( JOIN FMT_VERSION_ARRAY "."  FMT_VERSION )
	list( JOIN FMT_VERSION_ARRAY ", " FMT_VERSION_ARRAY )
endif ()
message( STATUS "fmt:            ${FMT_VERSION}" )


### GDAL metadata
find_package( GDAL 2.1 REQUIRED )	# For all targets
if( ${GDAL_FOUND} )
	include_directories( ${GDAL_INCLUDE_DIR} )
	message( STATUS "gdal:           ${GDAL_VERSION}" )
	if( ${GDAL_VERSION} MATCHES "([0-9]+)(\.([0-9]+)(\.([0-9]+)))" )
		list( APPEND GDAL_VERSION_ARRAY ${CMAKE_MATCH_1} ${CMAKE_MATCH_3} ${CMAKE_MATCH_5} )
		list( JOIN GDAL_VERSION_ARRAY ", " GDAL_VERSION_ARRAY )
	endif()
else()
	message( FATAL_ERROR "The gdal library is not found [in the required version]." )
endif()


### PDAL metadata
set( BUILD_PLUGIN_TILEDB		ON )		# See https://pdal.io/en/latest/faq.html
find_package( PDAL 2.4.0 REQUIRED CONFIG )
if( ${PDAL_FOUND} )
	include_directories( ${PDAL_INCLUDE_DIRS} )
	list( GET PDAL_LIBRARY_DIRS 0 PDAL_LIBRARY_DIRS_FIRST )
	message( STATUS "pdal:           ${PDAL_VERSION}" )
	if( ${PDAL_VERSION} MATCHES "([0-9]+)(\.([0-9]+)(\.([0-9]+)))" )
		list( APPEND PDAL_VERSION_ARRAY ${CMAKE_MATCH_1} ${CMAKE_MATCH_3} ${CMAKE_MATCH_5} )
		list( JOIN PDAL_VERSION_ARRAY ", " PDAL_VERSION_ARRAY )
	endif()
else()
	message( FATAL_ERROR "The PDAL library is required, but not found." )
endif()


### nlohmann/json metadata
set( NLOHMANN_JSON "${PAX_DIRECTORY}/external/nlohmann-json/json.hpp" )
file( READ ${NLOHMANN_JSON} pegtl )
if( pegtl MATCHES "NLOHMANN_JSON_VERSION_MAJOR ([0-9]+)" )
	list( APPEND NLOHMANN_JSON_VERSION_ARRAY ${CMAKE_MATCH_1} )
	if( pegtl MATCHES "NLOHMANN_JSON_VERSION_MINOR ([0-9]+)" )
		list( APPEND NLOHMANN_JSON_VERSION_ARRAY ${CMAKE_MATCH_1} )
		if( pegtl MATCHES "NLOHMANN_JSON_VERSION_PATCH ([0-9]+)" )
			list( APPEND NLOHMANN_JSON_VERSION_ARRAY ${CMAKE_MATCH_1} )
		endif ()
	endif ()
endif ()
list( JOIN NLOHMANN_JSON_VERSION_ARRAY "."  NLOHMANN_JSON_VERSION )
list( JOIN NLOHMANN_JSON_VERSION_ARRAY ", " NLOHMANN_JSON_VERSION_ARRAY )
message( STATUS "nlohmann/json:  ${NLOHMANN_JSON_VERSION}" )

### Generate metadata file
add_definitions( -D__PAX_SYSTEM__=${CMAKE_SYSTEM_NAME} )
configure_file (
	"${PROJECT_SOURCE_DIR}/${PAX_DIRECTORY}/meta/version/version.in.hpp"
	"${PROJECT_SOURCE_DIR}/${PAX_DIRECTORY}/meta/version/version_${CMAKE_SYSTEM_NAME}"
)



#################################################
################ Setup targets ##################
#################################################

message( STATUS " " )
message( STATUS "############### Targets" )

### Agregate libraries
set( PAX_BASE_LIBS	${GDAL_LIBRARY}	${PDAL_LIBRARIES} )
set( PAX_TOOL_LIBS	${PAX_BASE_LIBS} )


### Setup the 'pax' library target.
file( GLOB_RECURSE LIB_SOURCE_FILES_LIBS		"${PROJECT_SOURCE_DIR}/include/*.lib.c*" )
set( CMAKE_MACOSX_RPATH OFF )	# Handles problem: dyld[80443]: Library not loaded: @rpath/liblibpax2.dylib
add_library( libpax2 SHARED ${LIB_SOURCE_FILES_LIBS} ${LIB_SOURCE_FILES_EXTERNALS} )
install( TARGETS libpax2 DESTINATION /usr/local/lib )


### Setup the doctest targets.
# One for each directory in "tests/*" "tests-ideas/*". There are many doctest targets, so they may compile in parallell.
file( GLOB INCLUDE_DIRS0 "tests/*" )
file( GLOB MAIN_DOCTEST "*/doctest-main.cpp" )
if( MAIN_DOCTEST )
	message( STATUS "# Tests" )
else()
	message( SEND_ERROR "No main.doctest.cpp file was found. No doctests can be built." )
endif()
FOREACH( include_dir ${INCLUDE_DIRS0} )
	if( IS_DIRECTORY ${include_dir} )
		list( APPEND DOCTEST_PROJS ${include_dir} )
		file( GLOB_RECURSE TEMP_SOURCE_FILES "${include_dir}/*.doctest.cpp" )
		if( TEMP_SOURCE_FILES )									# If the directory have any .doctest.cpp files...
			list( APPEND TEMP_SOURCE_FILES ${MAIN_DOCTEST} )	# Add the main file.
			string( REGEX REPLACE "^.*/(include|tests|tests-ideas)/" "" doctest_name ${include_dir} )
			add_executable( ${doctest_name} ${TEMP_SOURCE_FILES} )
			target_link_libraries( ${doctest_name} ${PAX_BASE_LIBS} )	# Removed libpax2
			# Comment the next line to impede tests to run automagically. 
			add_custom_command( TARGET ${doctest_name} COMMENT "Run doctest in ${doctest_name}" POST_BUILD COMMAND ${doctest_name} )
			list( APPEND DOCTEST_PROJS ${doctest_name} )
			message( STATUS "${doctest_name}" )
		endif()
	endif()
ENDFOREACH()

# Target 'doctest' (and its alias 'test'): all doctest sub-projects, as defined above.
add_custom_target( doctest DEPENDS ${DOCTEST_PROJS} )
add_custom_target( test DEPENDS doctest )


### Setup the individual tools targets.
message( STATUS "# Tools" )
file( GLOB PAX_TOOL_SOURCES "tools/*.cpp" )
foreach( pax_tool_source ${PAX_TOOL_SOURCES} )
	# From https://stackoverflow.com/questions/14306642/adding-multiple-executables-in-cmake
	get_filename_component( pax_tool ${pax_tool_source} NAME_WE )	# NAME_WE: Name without Extension
	add_executable( ${pax_tool} ${pax_tool_source} )
	target_link_libraries( ${pax_tool} ${PAX_TOOL_LIBS} )			# Removed libpax2
	install( TARGETS ${pax_tool} DESTINATION /usr/local/bin )
	set( PAX_TOOLS ${PAX_TOOLS} ${pax_tool} )
	message( STATUS "${pax_tool}" )
endforeach( pax_tool_source )

# Target 'tools' (all tools generated by the loop, above)
add_custom_target( tools DEPENDS ${PAX_TOOLS} )


### Setup the pdal modules targets.
message( STATUS "# pdal modules" )
file( GLOB PAX_PDAL_SOURCES "pdal-plugins/pdal_plugin_*.cpp" )
foreach( pax_pdal_source ${PAX_PDAL_SOURCES} )
	# From https://stackoverflow.com/questions/14306642/adding-multiple-executables-in-cmake
	get_filename_component( pax_pdal_tool ${pax_pdal_source} NAME_WE )	# NAME_WE: Name without Extension
	add_library(             ${pax_pdal_tool} SHARED  ${pax_pdal_source} )
	target_link_libraries(   ${pax_pdal_tool} PRIVATE ${PAX_TOOL_LIBS} ${PDAL_LIBRARIES} )
	target_link_directories( ${pax_pdal_tool} PRIVATE ${PDAL_LIBRARY_DIRS} )
	install( TARGETS ${pax_pdal_tool} DESTINATION ${PDAL_LIBRARY_DIRS_FIRST} )
	set( PAX_PDAL ${PAX_PDAL} ${pax_pdal_tool} )
	message( STATUS "${pax_pdal_tool}" )
endforeach( pax_pdal_source )

# message( STATUS "" )
# message( STATUS "PAX_TOOL_LIBS:            ${PAX_TOOL_LIBS}" )
# message( STATUS "PDAL_VERSION:             ${PDAL_VERSION}" )
# message( STATUS "PDAL_INCLUDE_DIRS:        ${PDAL_INCLUDE_DIRS}" )
# message( STATUS "PDAL_LIBRARIES:           ${PDAL_LIBRARIES}" )
# message( STATUS "PDAL_LIBRARY_DIRS:        ${PDAL_LIBRARY_DIRS}" )
# message( STATUS "PDAL_LIBRARY_DIRS_FIRST:  ${PDAL_LIBRARY_DIRS_FIRST}" )
# message( STATUS "PDAL_DEFINITION:          ${PDAL_DEFINITION}" )


## Target 'pdal': all pdal drivers generated by above.
add_custom_target( pdal DEPENDS ${PAX_PDAL} )


### A target of all targets: 'everything'.
add_custom_target( everything DEPENDS doctest tools pdal )


message( STATUS " " )
message( STATUS "############### Winding down" )
