# CMake project to allow standalone build of fhicl-cpp.  As part of the
# build procedure, it downloads, builds, and installs cetmodules,
# cetlib-except, hep-concurrency, and cetlib.
#
# Required products:
#
#   Boost 1.82.0
#   SQLite 3
#   oneTBB 2021.9.0
#
# I used UPS to provide the above, but it could also be achieved by
# using local installations.
#
# Testing is not included as part of this build as extra packages are
# required (e.g. Catch2).

cmake_minimum_required(VERSION 3.18.2 FATAL_ERROR)

project(fhicl-cpp-standalone VERSION 4.18.01)

include(ExternalProject)

set(PACKAGES cetmodules cetlib-except hep-concurrency cetlib fhicl-cpp)
set(FHICLCPP_SUITE_VERSION FHICLCPP_SUITE_v4_18_01)
list(LENGTH PACKAGES PACKAGES_LEN)

set(TOP_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR})
ExternalProject_Add(cetmodules
  PREFIX cetmodules
  GIT_REPOSITORY https://github.com/FNALssi/cetmodules.git
  GIT_TAG 3.22.01
  CMAKE_ARGS -DBUILD_DOCS:BOOL=FALSE -DCMAKE_INSTALL_PREFIX:STRING=${TOP_BUILD_DIR}/cetmodules-install
)
set(PREFIX_PATH "${TOP_BUILD_DIR}/cetmodules-install")

MATH(EXPR MAX_INDEX "${PACKAGES_LEN}-1")
foreach(I RANGE 1 ${MAX_INDEX})
  MATH(EXPR J "${I}-1")
  list(GET PACKAGES ${I} PACKAGE_TO_BUILD)
  list(GET PACKAGES ${J} DEPENDENCY)

  ExternalProject_Add(${PACKAGE_TO_BUILD}
    PREFIX ${PACKAGE_TO_BUILD}
    GIT_REPOSITORY https://github.com/art-framework-suite/${PACKAGE_TO_BUILD}.git
    GIT_TAG ${FHICLCPP_SUITE_VERSION}
    DEPENDS ${DEPENDENCY}
    LIST_SEPARATOR |
    CMAKE_ARGS --preset=default
               -DCMAKE_PREFIX_PATH=${PREFIX_PATH}
               -DBUILD_TESTING:BOOL=FALSE
               -DCMAKE_INSTALL_PREFIX:STRING=${TOP_BUILD_DIR}/${PACKAGE_TO_BUILD}-install
  )
  string(APPEND PREFIX_PATH "|${TOP_BUILD_DIR}/${PACKAGE_TO_BUILD}-install")
endforeach()
