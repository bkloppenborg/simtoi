simtoi
======

The SImulation and Modeling Tool for Optical Interferometry

## Installing prerequisites

If you are building SIMTOI from scratch, you will need a C, C++, and Fortran
compiler along with the CMake build system. On a Debian-based system these
can be installed using `apt-get`:

    sudo apt-get install build-essential cmake gfortran

If you intend to use Git to checkout the repository, you also need to install it

    sudo apt-get install git

All of the minimization engines presently used in SIMTOI use LAPACK and/or BLAS
for solving linear systems of equations. SIMTOI also requires ccfits, cfitsio
QT4 and an OpenGL library. These can all be installed via. `apt-get`:

    sudo apt-get install liblapack-dev libblas-dev libccfits0 libccfits-dev libqt4-dev libglu1-mesa libglu1-mesa-dev

In order to run SIMTOI you must also have an installation of OpenCL for your graphics
card. If you have an NVidia GPU simply install the video card drivers. On ATI GPUs
you must install *both* the drivers *and* the [AMD APP SDK](http://developer.amd.com/tools/heterogeneous-computing/amd-accelerated-parallel-processing-app-sdk/).

Lastly, SIMTOI has optional support for [MultiNest 2.17](http://ccpforge.cse.rl.ac.uk/gf/project/multinest/)
(a Bayesian nested sampling minimization engine). As the MultiNest API has significantly changed
in the last few revisions, SIMTOI may not support the latest MultiNest. Please pay close attention
to version numbers!

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
