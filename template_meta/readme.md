# C++ template metaprogramming demo

This repo contains some template metaprogramming exercises in C++, from the some basic examples up to more complex problems.

## Required software
 * Recommended editor: CLion or VS Code
    * Useful extensions for VS Code:
        * C/C++ (ms-vscode.cpptools)
        * C/C++ Extension Pack (ms-vscode.cpptools-extension-pack)
        * CMake (twxs.cmake)
        * CMake Tools (ms-vscode.cmake-tools)
 * `cmake`: version 2.6 or newer
 * c++ compiler with c++20 support
    * `g++`: version 11.0.1 or newer OR
    * `clang`: version 10.0.0 or newer
 * Optionally may be preinstalled: `boost 1.77.0`
    * make sure that `$BOOST_ROOT_DIR` is set correctly
    * if not installed, then cmake will download and install it

## Setup
* Generate `Makefile` by running `cmake .` in the `template_meta` directory.
* Install Boost by running `make Boost`. This step may need to be run with `sudo` and may take a couple of minutes to complete. ~186 MB will be downloaded, after unpacking Boost will take up ~1 GB of disk space.
* Optional: regenerate `Makefile` by running `cmake .` in the `template_meta` directory.
* Now the `Makefile` should be up to date.

## Compiling the subproject
* To compile all subprojects run `make all` (or simply `make`).
The resulting executables will have the name `{project}.out`
* Since the projects are using template metaprogramming all programming errors should be displayed during compile time. Running the executables does not have any effect.
* To build individual projects, use `make {project}`, where the possible projects are:
   * `std`
   * `mpl`
   * `hana`