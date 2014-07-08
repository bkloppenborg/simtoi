simtoi
======

The SImulation and Modeling Tool for Optical Interferometry

## Installing prerequisites

### Build environment

If you are building SIMTOI from scratch, you will need a C, C++, and Fortran
compiler along with the CMake build system. On a Debian-based system these
can be installed using `apt-get`:

    sudo apt-get install build-essential cmake gfortran

If you intend to use Git to checkout the repository, you also need to install it

    sudo apt-get install git

All of the minimization engines presently used in SIMTOI use LAPACK and/or BLAS
for solving linear systems of equations. SIMTOI also requires ccfits, cfitsio
QT4 and an OpenGL library. These can all be installed via. `apt-get`:

    sudo apt-get install liblapack-dev libblas-dev libccfits0 libccfits-dev 
        libqt4-dev libglu1-mesa libglu1-mesa-dev

### OpenGL and OpenCL

In order to run SIMTOI you must also have an installation of OpenCL for your 
graphics card. 

#### NVidia hardware

On *NVidia* hardware, simply install the video card drivers (either from your
package manager or from NVidia directly). Both OpenGL and OpenCL are installed 
automatically. On Ubuntu, this can be done with a command similar to the
following:

    sudo apt-get install nvidia-331 nvidia-opencl-icd-331 nvidia-opencl-dev 

where you should (probably) choose the most current drivers rather than the 
`nvidia-331` branch. Please note that I have not tested installations involving
`bumblebee` (a.k.a. Optimus for Linux).

#### ATI hardware

On *ATI* hardware the installation is a multi-stage process. First install
the video card drivers (either from your package manager or from ATI directly).
For example, on Ubuntu simply install the `fglrx` package:

    sudo apt-get install fglrx fglrx-dev

Next you need to download and install the 
[AMD APP SDK](http://developer.amd.com/tools/heterogeneous-computing/amd-accelerated-parallel-processing-app-sdk/).

When you compile SIMTOI pay close attention to the OpenGL drivers that were 
found. If the driver path does not contain `fglrx`, (e.g. `/usr/lib/fglrx/libGL.so`)
then try changing the `CMAKE_LIBRARY_PATH` in `simtoi/CMakeLists.txt` to 
the directory in which `libgl.so` is installed. For example:

    set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/lib/fglrx)

Related links: 
http://askubuntu.com/questions/205112/how-do-i-get-amd-intel-hybrid-graphics-drivers-to-work


#### Intel Integrated / Iris Graphics



## Optional prerequisites

### MultiNest

SIMTOI supports the [MultiNest](http://ccpforge.cse.rl.ac.uk/gf/project/multinest/)
Bayesian nested sampling minimization engine. In order to enable support,
download, compile, and install the *CMake* version of MultiNest.
SIMTOI has been tested with MultiNest v3.7, but should work with more
recent version as well (as long as the API hasn't changed). SIMTOI will 
automatically attempt to locate MultiNest. If it is found, you will get a 
message similar to the following during the CMake stage:

    # cmake ..
    ...
    -- Found MULTINEST: /usr/local/lib/libmultinest.so;/usr/lib/liblapack.so;/usr/lib/libblas.so 
    -- MultiNest found
    --  SIMTOI will include the MultiNest minimizer

## Checkout / getting a copy of SIMTOI source code

After installing the aforementioned prerequisites, you simply need to checkout
a copy of SIMTOI and initialize the submodules for [liboi](https://github.com/bkloppenborg/liboi),
 [textio](https://github.com/bkloppenborg/textio), and [ccoifits](https://github.com/bkloppenborg/ccoifits).
To do this simply run the following commands:

    git clone https://github.com/bkloppenborg/simtoi
    cd simtoi
    git submodule update --init

## Building instructions

After you have installed the prerequisites and checked out SIMTOI and submodules, complilation
is easy:

    cd build
    cmake ..
    make

During the `cmake ..` step CMake will attempt to find the necessary libraries.
It may generate errors if something is not found. The error messages are often
clear, but sometimes a bit cryptic. If you need help, please ask.

Please note that you can compile SIMTOI using multiple cores by specifying
`make -jN` where `N` is the number of cores in your machine.

## External Libraries

SIMTOI includes the following libraries as part of its distribution:
* [OpenCL Interferometry Library (liboi)](https://github.com/bkloppenborg/liboi)
* [textio](https://github.com/bkloppenborg/textio)
* [ccoifits](https://github.com/bkloppenborg/ccoifits).
* [json-cpp](http://sourceforge.net/projects/jsoncpp/)
* [levmar](http://www.ics.forth.gr/~lourakis/levmar/)

SIMTOI supports, but our repositories do not include MultiNest (a Bayesian nested
sampling minimization engine).
* [MultiNest](http://ccpforge.cse.rl.ac.uk/gf/project/multinest/)

## Licensing and Acknowledgements

SIMTOI is free software, distributed under the [GNU Lesser General Public License (Version 3)](<http://www.gnu.org/licenses/lgpl.html). 

If you use this software as part of a scientific publication, please cite the following works:

Kloppenborg, B.; Baron, F. (2012) "SIMTOI: SImulation and Modeling Tool for Optical Interferometry" (Version X).  Available from  <https://github.com/bkloppenborg/simtoi>.

Kloppenborg, B.; Baron, F. (2012), "LibOI: The OpenCL Interferometry Library"
(Version X). Available from  <https://github.com/bkloppenborg/liboi>.

If you use the _levmar_ minmizer, see their instruction for including [a suitable reference](http://www.ics.forth.gr/~lourakis/levmar/bibentry.html).

If you use the _MultiNest_ minimizer see reference instructions on the [_MultiNest_ website](http://ccpforge.cse.rl.ac.uk/gf/project/multinest/)
