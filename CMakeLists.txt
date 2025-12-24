cmake_minimum_required(VERSION 4.0)
project(nanobdd)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include(GNUInstallDirs)
include(CTest)

option(NANOBDD_BUILD_EXAMPLES "Build example binaries in examples/ and wire them into ctest" ON)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif ()

find_package(JNI REQUIRED)
find_package(TBB CONFIG REQUIRED)

add_library(${PROJECT_NAME} SHARED
  src/Nanobdd.cpp
  src/Bdd.cpp
  src/Cache.cpp
  src/NodeAllocator.cpp
  src/NodeTable.cpp
  src/capi.cpp
  java/src/main/java/org/snlab/jni/org_snlab_jni_NanoBDD.cpp
)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        TBB::tbb
        PRIVATE
  ${JNI_LIBRARIES}
)

target_include_directories(${PROJECT_NAME}
  PRIVATE
        # where the library itself will look for its internal headers
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${JNI_INCLUDE_DIRS}
  PUBLIC
        # where top-level project will look for the library's public headers
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}>
        # where external projects will look for the library's public headers
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set(public_headers
  include/nanobdd/nanobdd.h
  include/nanobdd/Bdd.h
  include/nanobdd/Node.h
  include/nanobdd/capi.h
  java/src/main/java/org/snlab/jni/org_snlab_jni_NanoBDD.h
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

if (NANOBDD_BUILD_EXAMPLES)
  set(NANOBDD_EXAMPLE_SOURCES
          examples/simple.cpp
          examples/gc.cpp
          examples/parallel.cpp
  )

  set(nanobdd_example_targets)

  foreach (example_src IN LISTS NANOBDD_EXAMPLE_SOURCES)
    get_filename_component(example_name ${example_src} NAME_WE)
    set(target_name "nanobdd_example_${example_name}")

    add_executable(${target_name} ${example_src})
    target_link_libraries(${target_name} PRIVATE ${PROJECT_NAME})
    target_include_directories(${target_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

    if (BUILD_TESTING)
      add_test(NAME ${target_name} COMMAND ${target_name})
      set_tests_properties(${target_name} PROPERTIES LABELS examples)
    endif ()

    list(APPEND nanobdd_example_targets ${target_name})
  endforeach ()

  add_custom_target(run_examples
          COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure -L examples
          DEPENDS ${nanobdd_example_targets}
          WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
          USES_TERMINAL
  )
endif ()

# Tests
if (BUILD_TESTING)
  add_executable(nanobdd_node_table_test
          tests/node_table_slab_test.cpp
  )
  target_link_libraries(nanobdd_node_table_test PRIVATE ${PROJECT_NAME})
  target_include_directories(nanobdd_node_table_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
  add_test(NAME nanobdd_node_table_test COMMAND nanobdd_node_table_test)
endif ()
