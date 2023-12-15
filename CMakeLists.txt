# CMake project to allow standalone build of fhicl-cpp.  As part of the
# build procedure, it downloads, builds, and installs cetmodules,
# cetlib-except, hep-concurrency, and cetlib.
#
# Required products:
#
#   Boost 1.75
#   SQLite 3
#   oneTBB 2020
#
# I used UPS to provide the above, but it could also be achieved by
# using local installations.
#
# Testing is not included as part of this build as extra packages are
# required (e.g. Catch2).

cmake_minimum_required(VERSION 3.20.0 FATAL_ERROR)

set(FHICLCPP_SUITE_VERSION 4_18_01)
string(REPLACE "_" "." FHICLCPP_SUITE_VERSION_DOT ${FHICLCPP_SUITE_VERSION})

project(fhicl_cpp_standalone VERSION ${FHICLCPP_SUITE_VERSION_DOT})

set(CMAKE_CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)

#Changes default install path to be a subdirectory of the build dir.
#Can set build dir at configure time with -DCMAKE_INSTALL_PREFIX=/install/path
if(CMAKE_INSTALL_PREFIX STREQUAL "" OR CMAKE_INSTALL_PREFIX STREQUAL
  "/usr/local")
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}")
elseif(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME}")
endif()

#lets use CPM instead of ExternalProject_Add as it plays better with
#finding existing binary releases.
# file(
#   DOWNLOAD
#   https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.38.3/CPM.cmake
#   ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
#   EXPECTED_HASH SHA256=cc155ce02e7945e7b8967ddfaff0b050e958a723ef7aad3766d368940cb15494
# )
# include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

# These are the system default versions on alma9 as of 2023/11/01
# and seem to work
find_package(Boost 1.75 COMPONENTS filesystem REQUIRED)
find_package(SQLite3 3 REQUIRED)
find_package(TBB 2020 REQUIRED)


set(PACKAGES cetlib-except hep-concurrency cetlib fhicl-cpp)

include(ExternalProject)

ExternalProject_Add(cetmodules
  PREFIX cetmodules
  GIT_REPOSITORY https://github.com/FNALssi/cetmodules.git
  GIT_TAG 3.22.01
  CMAKE_ARGS -DBUILD_DOCS:BOOL=FALSE 
             -DCMAKE_INSTALL_PREFIX:STRING=${CMAKE_INSTALL_PREFIX}
)
set(PREVIOUS_PACKAGE cetmodules)

foreach(PACKAGE_TO_BUILD ${PACKAGES})
  ExternalProject_Add(${PACKAGE_TO_BUILD}
    PREFIX ${PACKAGE_TO_BUILD}
    GIT_REPOSITORY https://github.com/art-framework-suite/${PACKAGE_TO_BUILD}.git
    GIT_TAG FHICLCPP_SUITE_v${FHICLCPP_SUITE_VERSION}
    DEPENDS ${PREVIOUS_PACKAGE}
    STEP_TARGETS install
    CMAKE_ARGS --preset=default
               -DCMAKE_CXX_STANDARD=17
               -DCMAKE_PREFIX_PATH=${CMAKE_INSTALL_PREFIX}
               -DBUILD_TESTING:BOOL=FALSE
               -DCMAKE_INSTALL_PREFIX:STRING=${CMAKE_INSTALL_PREFIX}
  )

  set(PREVIOUS_PACKAGE ${PACKAGE_TO_BUILD})
endforeach()

#these directories have to exist or the target definition below fails
#this would all be a lot easier if we could use FetchContent... thanks cet.
FILE(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/include)
FILE(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/lib)

# The target that we're going to construct. We can't do this properly because
# cetlib's overrides will mangle with sensible usage of CPM or FetchContent_MakeAvailable
add_library(fhicl_cpp_standalone INTERFACE)

set(FHICL_CPP_INSTALL_STEP_TARGET fhicl-cpp-install)

if(NOT TARGET ${FHICL_CPP_INSTALL_STEP_TARGET})
  message(FATAL_ERROR "FHICL_CPP_INSTALL_STEP_TARGET: ${FHICL_CPP_INSTALL_STEP_TARGET} does not exist")
endif()

add_dependencies(fhicl_cpp_standalone ${FHICL_CPP_INSTALL_STEP_TARGET})
target_include_directories(fhicl_cpp_standalone INTERFACE 
  $<BUILD_INTERFACE:${CMAKE_INSTALL_PREFIX}/include> 
  $<INSTALL_INTERFACE:include>)
target_link_directories(fhicl_cpp_standalone INTERFACE 
  $<BUILD_INTERFACE:${CMAKE_INSTALL_PREFIX}/lib> 
  $<INSTALL_INTERFACE:lib>)
target_link_libraries(fhicl_cpp_standalone INTERFACE 
  -lfhiclcpp -lcetlib_except -lcetlib_sqlite SQLite::SQLite3 -lcetlib -lhep_concurrency Boost::filesystem TBB::tbb)
set_target_properties(fhicl_cpp_standalone PROPERTIES EXPORT_NAME standalone)

add_library(fhiclcpp::standalone ALIAS fhicl_cpp_standalone)

install(TARGETS fhicl_cpp_standalone
    EXPORT fhicl_cpp_standalone-targets)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/fhicl_cpp_standaloneConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(
  "${CMAKE_CURRENT_LIST_DIR}/cmake/Templates/fhicl_cpp_standaloneConfig.cmake.in"
  "${PROJECT_BINARY_DIR}/fhicl_cpp_standaloneConfig.cmake"
  INSTALL_DESTINATION cmake
  NO_SET_AND_CHECK_MACRO
  NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

install(EXPORT fhicl_cpp_standalone-targets
        NAMESPACE fhiclcpp::
        DESTINATION lib/cmake/fhicl_cpp_standalone)
install(FILES "${PROJECT_BINARY_DIR}/fhicl_cpp_standaloneConfigVersion.cmake"
              "${PROJECT_BINARY_DIR}/fhicl_cpp_standaloneConfig.cmake"
        DESTINATION lib/cmake/fhicl_cpp_standalone)
configure_file(${CMAKE_CURRENT_LIST_DIR}/cmake/Templates/setup.fhicl_cpp_standalone.sh.in 
  ${PROJECT_BINARY_DIR}/setup.fhicl_cpp_standalone.sh @ONLY)
install(PROGRAMS ${PROJECT_BINARY_DIR}/setup.fhicl_cpp_standalone.sh DESTINATION bin)



if(TESTS_ENABLED)
  add_executable(testfhiclcppstandalone testfhiclcppstandalone.cxx)
  target_link_libraries(testfhiclcppstandalone fhiclcpp::standalone)
endif()

