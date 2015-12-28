cmake_minimum_required(VERSION 2.8)

project(NeoRL)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
  message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

include_directories("${PROJECT_SOURCE_DIR}/source")

# This is only required for the script to work in the version control
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}")

include(ExternalProject)
ExternalProject_Add(
  Box2D
  PREFIX Box2D
  GIT_REPOSITORY https://github.com/erincatto/Box2D.git
  GIT_TAG v2.3.1
  UPDATE_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  )
include_directories("Box2D/src/Box2D/Box2D")

find_package(OpenCL REQUIRED)

message(STATUS "OpenCL inc: " ${OpenCL_INCLUDE_DIRS})
message(STATUS "OpenCL lib: " ${OpenCL_LIBRARIES})
 
include_directories(${OpenCL_INCLUDE_DIRS})

find_package(SFML REQUIRED)

file(GLOB_RECURSE LINK_SRC
  "source/*.h"
  "source/*.cpp"
)
 
add_executable(NeoRL ${LINK_SRC})

target_link_libraries(NeoRL ${OpenCL_LIBRARIES})

