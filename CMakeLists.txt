cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(GNUInstallDirs)

project(nanobdd)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

add_library(${PROJECT_NAME}
  src/Nanobdd.cpp
  src/Bdd.cpp
  src/Cache.cpp
  src/NodeTable.cpp
)

target_link_libraries(${PROJECT_NAME}
  tbb
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
    # where the library itself will look for its internal headers
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    # ${CMAKE_CURRENT_SOURCE_DIR}/include
  PUBLIC
    # where top-level project will look for the library's public headers
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}>
    # where external projects will look for the library's public headers
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set(public_headers
  include/nanobdd/nanobdd.h
  include/nanobdd/Bdd.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${public_headers}")

install(TARGETS ${PROJECT_NAME}
  EXPORT "${PROJECT_NAME}Targets"
  # these get default values from GNUInstallDirs, no need to set them
  #RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # bin
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib
  # except for public headers, as we want them to be inside a library folder
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME} # include/SomeLibrary
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME} # include
)
include(CMakePackageConfigHelpers)

install(
  EXPORT ${PROJECT_NAME}Targets
  FILE nanobddTargets.cmake
  NAMESPACE nanobdd::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/nanobdd
)
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/nanobdd.pc.in" "${CMAKE_CURRENT_BINARY_DIR}/nanobdd.pc" @ONLY)
install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/nanobdd.pc"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
)