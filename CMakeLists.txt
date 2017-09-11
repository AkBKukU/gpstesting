# CMake file for Vulkan Tutorial
# by Shelby Jueden (AkBKukU)

cmake_minimum_required(VERSION 3.1)
project(wadmap)

# Set name of ouput binary. Multi line formated for other scripts. Lazy, line 10
set(
BIN_NAME
gpstest
)

# Add all files from the src folder
file(GLOB SOURCES "src/*.cpp")


# Flags for debug build
set(
	CMAKE_CXX_FLAGS_DEBUG 
	"${CMAKE_CXX_FLAGS_DEBUG} -d -Wall"
)
# Flags for Release build
set(
	CMAKE_CXX_FLAGS_RELEASE 
	"${CMAKE_CXX_FLAGS_RELEASE} -Wall"
)

# Set C++11
set (CMAKE_CXX_STANDARD 11)

# Setup binary output
add_executable(${BIN_NAME} ${SOURCES})

