cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(GNUInstallDirs)

project(nanobdd)

add_library(${PROJECT_NAME}
  ${LIBRARY_HEADERS}
  ${LIBRARY_SOURCE}
)

target_sources(${PROJECT_NAME}
  PRIVATE
    src/Nanobdd.cpp
    src/NodeTable.cpp
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
    # where the library itself will look for its internal headers
    ${CMAKE_CURRENT_SOURCE_DIR}/src
  PUBLIC
    # where top-level project will look for the library's public headers
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    # where external projects will look for the library's public headers
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set(public_headers
    include/nanobdd.h
)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${public_headers}")

install(TARGETS ${PROJECT_NAME}
  EXPORT "${PROJECT_NAME}Targets"
  # these get default values from GNUInstallDirs, no need to set them
  #RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # bin
  #LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib
  #ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib
  # except for public headers, as we want them to be inside a library folder
  PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME} # include/SomeLibrary
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # include
)