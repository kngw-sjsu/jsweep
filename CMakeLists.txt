#
# Root level CMakeLists.txt for j_Sweeper
# Read the README.md for how to build (only for Windows right now)
#
# The following variables must be set on the command line:
#       CMAKE_BUILD_TYPE: Debug | Release
#       wxWidgets_ROOT_DIR: <absolute or relative path to the "wxWidgets-x.y.z" directory>
#       wxWidgets_LIB_DIR: <absolute or relative path to the "wxWidgets-x.y.z/lib/gcc_lib" directory>
#       nlohmann_json_DIR: <absolute or relative path to the directory containing "nlohmann_jsonConfig.cmake">
#       MinGW_lib_DIR: <absolute or relative path to the "MinGW/lib" directory>
#           (might be unnecessary if no libraries stored in the ADDITIONAL_LIBS variable are from MinGW.
#

cmake_minimum_required(VERSION 3.12)
project(jsweep)

# Use C++14
set(CMAKE_CXX_STANDARD 14)

# Disable console window
set(CMAKE_CXX_FLAGS -mwindows)

# Configuration based on OS
if (WIN32)
    message(STATUS "Building on Microsoft Windows...")
    if(${CMAKE_BUILD_TYPE} MATCHES Debug)
        message(STATUS "Building under debug mode...")
        set(wxWidgets_CONFIGURATION mswud)
    elseif(${CMAKE_BUILD_TYPE} MATCHES Release)
        message(STATUS "Building under release mode...")
        set(wxWidgets_CONFIGURATION mswu)
        add_definitions(-DwxDEBUG_LEVEL=0)
    else()
        message(FATAL_ERROR "Invalid build type specified. Terminating...")
    endif()

# UNIX Build still not available
elseif(UNIX)
    message(STATUS "Building on UNIX...")
endif()

# Load wxWidgets
find_package(wxWidgets REQUIRED html xml core base)
# message(${wxWidgets_LIBRARIES})
include(${wxWidgets_USE_FILE})

# Load nlohmann JSON for Modern C++
find_package(nlohmann_json 3.2.0 REQUIRED)

# Integration!
add_executable(jsweep)
add_subdirectory(src)
add_subdirectory(res)
find_library(ADDITIONAL_LIBS libuxtheme.a ${MinGW_lib_DIR}) # without this, a lot of undefined reference happens
target_include_directories(jsweep PRIVATE src)
target_link_libraries(jsweep
        ${wxWidgets_LIBRARIES}
        ${ADDITIONAL_LIBS}
        nlohmann_json::nlohmann_json
)
