# Set version requirement
cmake_minimum_required(VERSION 3.20)

# Define project
project(opensplice-example
	VERSION 0.0.1
	DESCRIPTION "Simple basic example using Vortex OpenSplice"
	LANGUAGES C CXX)

# Set default to Debug
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
	set(CMAKE_BUILD_TYPE Debug)
endif()
message("Build type: ${CMAKE_BUILD_TYPE}")

# Check if OSPL_HOME is set
if("$ENV{OSPL_HOME}" STREQUAL "")
	message(FATAL_ERROR "OSPL_HOME is not set!")
endif()

# Set OpenSplice directories
message("Setting paths to OpenSplice")
if(WIN32)
	set(LIB_PATH_ENV "PATH")
elseif(UNIX)
	set(LIB_PATH_ENV "LD_LIBRARY_PATH")
else()
	message(FATAL_ERROR "Unsupported operating system")
endif()
# Set bin/lib/includes
list(APPEND OSPL_BIN_DIR "$ENV{OSPL_HOME}/bin") 
list(APPEND OSPL_LIB_DIR "$ENV{OSPL_HOME}/lib" "$ENV{OSPL_HOME}/bin") 
list(APPEND OSPL_INCL_DIR "$ENV{OSPL_HOME}/include/dcps/C++/isocpp2" "$ENV{OSPL_HOME}/include/sys")

# Find generator
find_program(GENERATOR "idlpp" HINTS ${OSPL_BIN_DIR})
# Find libraries
find_library(DDSKERNEL "ddskernel" HINTS ${OSPL_LIB_DIR} REQUIRED)
find_library(DCPSISOCPP2 "dcpsisocpp2" HINTS ${OSPL_LIB_DIR} REQUIRED)

# Set library name
set(IDL_LIB "idl")
# Add IDL library
add_library(${IDL_LIB} STATIC "")
target_include_directories(${IDL_LIB} PUBLIC ${OSPL_INCL_DIR})

# Generate sources from IDLs
file(GLOB IDL_PATHS ${CMAKE_SOURCE_DIR}/idl/*.idl)
foreach(IDL_PATH IN LISTS IDL_PATHS)
	# Get IDL base
	get_filename_component(IDL_BASE ${IDL_PATH} NAME_WE)
	set(GEN_IDLS ${CMAKE_BINARY_DIR}/gen/${IDL_BASE}SplDcps.cpp)
	# Define command to generate sources
	add_custom_command(OUTPUT ${GEN_IDLS}
		COMMAND ${CMAKE_COMMAND} -E env "${LIB_PATH_ENV}=${OSPL_LIB_DIR}" ${GENERATOR} -I ${CMAKE_SOURCE_DIR}/idl -l isocpp2 -d ${CMAKE_BINARY_DIR}/gen ${IDL_PATH}
		DEPENDS ${IDL_PATH}
		VERBATIM)
	# Add target to execute command
	add_custom_target(${IDL_BASE} ALL DEPENDS ${GEN_IDLS})
	# Add dependencies and sources to library
	add_dependencies(${IDL_LIB} ${IDL_BASE})
	# Collect all generated sources
	list(APPEND IDL_LIB_SRCS ${GEN_IDLS})
endforeach()
# Add sources based on generated IDL sources
target_sources(${IDL_LIB} PUBLIC ${IDL_LIB_SRCS})

# Set publisher name 
set(PUB "publisher")
# Add executable
add_executable(${PUB} "src/publisher.cpp")
target_include_directories(${PUB} PUBLIC ${CMAKE_BINARY_DIR}/gen ${OSPL_INCL_DIR})
target_link_libraries(${PUB} ${DCPSISOCPP2} ${DDSKERNEL} ${IDL_LIB})

# Set publisher name 
set(SUB "subscriber")
# Add executable
add_executable(${SUB} "src/subscriber.cpp")
target_include_directories(${SUB} PUBLIC ${CMAKE_BINARY_DIR}/gen ${OSPL_INCL_DIR})
target_link_libraries(${SUB} ${DCPSISOCPP2} ${DDSKERNEL} ${IDL_LIB})

# Copy necessary libraries
find_file(DCPSISOCPP2_LIB NAMES "dcpsisocpp2.dll" "libdcpsisocpp2.so" HINTS ${OSPL_LIB_DIR} REQUIRED)
find_file(DDSKERNEL_LIB NAMES "ddskernel.dll" "libddskernel.so" HINTS ${OSPL_LIB_DIR} REQUIRED)
find_file(SPLICED_LIB NAMES "spliced.dll" "libspliced.so" HINTS ${OSPL_LIB_DIR} REQUIRED)
find_file(DDSI2_LIB NAMES "ddsi2.dll" "libddsi2.so" HINTS ${OSPL_LIB_DIR} REQUIRED)
add_custom_target("copy_libs" ALL
	COMMAND ${CMAKE_COMMAND} -E copy ${DCPSISOCPP2_LIB} ${CMAKE_BINARY_DIR}
	COMMAND ${CMAKE_COMMAND} -E copy ${DDSKERNEL_LIB} ${CMAKE_BINARY_DIR}
	COMMAND ${CMAKE_COMMAND} -E copy ${SPLICED_LIB} ${CMAKE_BINARY_DIR}
	COMMAND ${CMAKE_COMMAND} -E copy ${DDSI2_LIB} ${CMAKE_BINARY_DIR}
	DEPENDS ${PUB} ${SUB})
