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

- CMake 3.18 or newer
- A compiler that supports C++17
- A reasonably modern version of Boost
- oneTBB
- SQLite 3

Depending on where these packages are installed, you may need to adjust the `cmake` command line to instruct CMake as to where it can find those packages.

## Caveats

The preferred way of building and distributing `fhicl-cpp` is through Spack, which will correctly manage the dependencies without needing this standalone repository.  However, this approach is permissible so long as you do not try to use the monolithic build of `fhicl-cpp` along with products that are linked against a standard build of `fhicl-cpp`.  Doing so will likely lead to violations of the one-definition rule.
