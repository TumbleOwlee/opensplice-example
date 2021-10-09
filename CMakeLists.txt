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
 
# Set OpenSplice directories
message("Setting paths to OpenSplice")
if(WIN32)
    set(OS_BASE_DIR "${CMAKE_SOURCE_DIR}/opensplice/HDE/x86_64.win64")
    # Also append path to DLLs
    list(APPEND OS_LIB_DIR "${OS_BIN_DIR}")
elseif(UNIX)
    set(OS_BASE_DIR "${CMAKE_SOURCE_DIR}/opensplice/HDE/x86_64.linux")
else()
    message(FATAL_ERROR "Unsupported operating system")
endif()
# Set bin/lib/includes
list(APPEND OS_BIN_DIR "${OS_BASE_DIR}/bin") 
list(APPEND OS_LIB_DIR "${OS_BASE_DIR}/lib") 
list(APPEND OS_INCL_DIR "${OS_BASE_DIR}/include/dcps/C++/isocpp2" "${OS_BASE_DIR}/include/sys")

# Find generator
find_program(GENERATOR "idlpp" HINTS ${OS_BIN_DIR})
# Find libraries
find_library(DDSKERNEL "ddskernel" HINTS ${OS_LIB_DIR} REQUIRED)
find_library(DCPSISOCPP2 "dcpsisocpp2" HINTS ${OS_LIB_DIR} REQUIRED)

# Set library name
set(IDL_LIB "idl")
# Add IDL library
add_library(${IDL_LIB} STATIC "")
target_include_directories(${IDL_LIB} PUBLIC ${OS_INCL_DIR})

# Generate sources from IDLs
file(GLOB IDL_PATHS ${CMAKE_SOURCE_DIR}/idl/*.idl)
foreach(IDL_PATH IN LISTS IDL_PATHS)
    # Get IDL base
    get_filename_component(IDL_BASE ${IDL_PATH} NAME_WE)
    set(GEN_IDLS ${CMAKE_BINARY_DIR}/gen/${IDL_BASE}SplDcps.cpp)
    # Define command to generate sources
    add_custom_command(OUTPUT ${GEN_IDLS}
                       COMMAND ${GENERATOR} -I ${CMAKE_SOURCE_DIR}/idl -l isocpp2 -d ${CMAKE_BINARY_DIR}/gen ${IDL_PATH}
                       DEPENDS ${IDL_PATH}
                       VERBATISM)
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
target_include_directories(${PUB} PUBLIC ${CMAKE_BINARY_DIR}/gen ${OS_INCL_DIR})
target_link_libraries(${PUB} ${DCPSISOCPP2} ${DDSKERNEL} ${IDL_LIB})

# Set publisher name 
set(SUB "subscriber")
# Add executable
add_executable(${SUB} "src/subscriber.cpp")
target_include_directories(${SUB} PUBLIC ${CMAKE_BINARY_DIR}/gen ${OS_INCL_DIR})
target_link_libraries(${SUB} ${DCPSISOCPP2} ${DDSKERNEL} ${IDL_LIB})
 
# Copy necessary libraries
find_file(DCPSISOCPP2_LIB NAMES "dcpsisocpp2.dll" "libdcpsisocpp2.so" HINTS ${OS_LIB_DIR} REQUIRED)
find_file(DDSKERNEL_LIB NAMES "ddskernel.dll" "libddskernel.so" HINTS ${OS_LIB_DIR} REQUIRED)
find_file(SPLICED_LIB NAMES "spliced.dll" "libspliced.so" HINTS ${OS_LIB_DIR} REQUIRED)
add_custom_target("copy_libs" ALL
    COMMAND ${CMAKE_COMMAND} -E copy ${DCPSISOCPP2_LIB} ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${DDSKERNEL_LIB} ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E copy ${SPLICED_LIB} ${CMAKE_BINARY_DIR}
    DEPENDS ${PUB} ${SUB})