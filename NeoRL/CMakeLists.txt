cmake_minimum_required(VERSION 2.8)

project(NeoRL)

# This is only required for the script to work in the version control
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  find_package(GNUWIN)
endif()

# Box2D
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
include_directories("${CMAKE_CURRENT_BINARY_DIR}/Box2D/src/Box2D/Box2D")

# OpenCL
find_package(OpenCL REQUIRED)
message(STATUS "OpenCL inc: " ${OpenCL_INCLUDE_DIRS})
message(STATUS "OpenCL lib: " ${OpenCL_LIBRARIES})
include_directories(${OpenCL_INCLUDE_DIRS})

# SFML (relies on GnuWIN: FreeType and libjpeg)
# - http://gnuwin32.sourceforge.net/packages/freetype.htm
# - http://gnuwin32.sourceforge.net/packages/jpeg.htm
find_package(GNUWIN REQUIRED)
find_package(FREETYPE REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})
find_package(JPEG REQUIRED)
include_directories(${JPEG_INCLUDE_DIR})

set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML COMPONENTS graphics window system REQUIRED)
include_directories(${SFML_INCLUDE_DIR})

file(GLOB_RECURSE LINK_SRC
  "source/*.h"
  "source/*.cpp"
)

add_executable(NeoRL ${LINK_SRC})

set_property(TARGET NeoRL PROPERTY CXX_STANDARD 11)
set_property(TARGET NeoRL PROPERTY CXX_STANDARD_REQUIRED ON)

target_link_libraries(NeoRL
                      ${OpenCL_LIBRARIES}
                      ${SFML_LIBRARIES}
                      ${SFML_DEPENDENCIES}
                      ${FREETYPE_LIBRARIES}
                      ${JPEG_LIBRARIES})

include_directories("${PROJECT_SOURCE_DIR}/source")
