# fhicl-cpp-standalone

This package enables a monolithic build of the [fhicl-cpp](https://github.com/art-framework-suite/fhicl-cpp) package.  There is no C++ code in this repository--instead, all of the dependencies of `fhicl-cpp` supplied by the [art-framework-suite](https://github.com/art-framework-suite) are built and installed while building this package.  These packages include (in dependency order):

- [cetlib-except](https://github.com/art-framework-suite/cetlib-except)
- [hep-concurrency](https://github.com/art-framework-suite/hep-concurrency)
- [cetlib](https://github.com/art-framework-suite/cetlib)
- [fhicl-cpp](https://github.com/art-framework-suite/fhicl-cpp)

Upon cloning this repository, you may build it by simply invoking:

```console
$ git clone https://github.com/art-framework-suite/fhicl-cpp-standalone.git
$ cmake fhicl-cpp-standalone
$ make [-j <n>]
```

## Prerequisites

Before invoking `cmake` you must have:

- CMake 3.21 or newer (see [here](#fetch-cmake-3-21))
- A compiler that supports C++17
- A reasonably modern version of Boost
- oneTBB
- SQLite 3

Depending on where these packages are installed, you may need to adjust the `cmake` command line to instruct CMake as to where it can find those packages.

## Using fhicl-cpp-standalone in your own project

Add the following cmake code to pull in and construct the `fhiclcpp::fhiclcpp` CMake target that
you can link against.

```
# Uncomment if you aren't already using CPM in your project
# file(
#   DOWNLOAD
#   https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.38.3/CPM.cmake
#   ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
#   EXPECTED_HASH SHA256=cc155ce02e7945e7b8967ddfaff0b050e958a723ef7aad3766d368940cb15494
# )
# include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)
CPMFindPackage(
    NAME fhicl_cpp_standalone
    GIT_TAG develop
    VERSION 4.18.01
    GITHUB_REPOSITORY luketpickering/fhicl-cpp-standalone
)

target_link_libraries(mytarget PRIVATE fhiclcpp::standalone)
```

Alternatively if you want to use a binary release/build of fhicl-cpp-standalone,
make sure that `fhicl_cpp_standalone_ROOT` points to the installation prefix of a release of
this package, and then use the same CMake as above. If you see something like:

```
-- fhicl-cpp-standalone Found:
-- --  fhicl_cpp_standalone_INCLUDE_DIR: /root/software/fhicl-cpp-standalone/build/Linux/include
-- --      fhicl_cpp_standalone_BIN_DIR: /root/software/fhicl-cpp-standalone/build/Linux/bin
-- --      fhicl_cpp_standalone_LIB_DIR: /root/software/fhicl-cpp-standalone/build/Linux/lib
-- --      fhicl_cpp_standalone_VERSION: 4.18.01
-- CPM: using local package fhicl_cpp_standalone@4.18.01
```

then you will know it has picked up the 'local' install of fhiclcpp and not
pulled down a new version of the repo (also the build of your dependent package
wont need to build all of these dependencies).

### Fetch CMake 3.21

If you just want a local build of cmake for this package and do not want to update
your system cmake, you can try the below recipe.

```
CMAKEVERSION=3.21.7
cd /path/to/repo
wget https://github.com/Kitware/CMake/archive/refs/tags/v${CMAKEVERSION}.tar.gz
tar -zxf v${CMAKEVERSION}.tar.gz
cd CMake-${CMAKEVERSION}
./bootstrap
make -j 8
export PATH=$(pwd)/bin:${PATH}
which cmake; cmake --version
```

## Caveats

The preferred way of building and distributing `fhicl-cpp` is through Spack, which will correctly manage the dependencies without needing this standalone repository.  However, this approach is permissible so long as you do not try to use the monolithic build of `fhicl-cpp` along with products that are linked against a standard build of `fhicl-cpp`.  Doing so will likely lead to violations of the one-definition rule.
