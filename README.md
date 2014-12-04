SIMTOI
======

The SImulation and Modeling Tool for Optical Interferometry (SIMTOI) is an open 
source (GPL v3) software package for simulating and modeling optical interferometric
observations of complex astrophysical sources that are not easily described
by analytic expressions.

# 1. Installing prerequisites

The requirements for SIMTOI are:

* Compiler with C++11 support (gcc/g++ v4.6.3 or later)
* A graphics card with OpenCL 1.1 (or later)
* OpenGL Mathematics Library (glm)
* cmake 2.8 or higher
* git
* LAPACK and BLAS
* QT4
* cfitsio and ccfits libraries for FITS and OIFITS handling

We detail below how to install each of these for OSX or GNU/Linux.

## 1.1 Apple/OSX 

Prior to installing `SIMTOI` follow the instructions for building and testing
[liboi](https://github.com/bkloppenborg/liboi) for your platform.

### 1.1.1 OpenGL and OpenCL

OpenGL and OpenCL come pre-installed with the system.
To be able to use OpenCL 1.1, you just need to use OS X 10.7 or higher.

Note however that support for SIMTOI on Intel Integrated or Intel Iris
graphics is currently being explored. We have successfully built and
used liboi on Intel HD Graphics 4000 devices; however, there are a few
lingering OpenGL issues which we must resolve.

### 1.1.2 Compiler and libraries

We have sucessfully used MacPorts to install the prerequisites for SIMTOI. 

### 1.1.2.1 gcc

OS X uses clang as its default compiler and /opt/local/bin/gcc is a
wrapper for clang, and not actually a true gcc. clang support of C++11
is spotty so it is advised to use gcc 4.7 instead.

### 1.1.2.2 Libraries

OS X already includes LAPACK and BLAS, so there is no need to install these.

    sudo port install cmake cfitsio git qt4-mac glm

Please note, depending on the status of QT within macport, you may also need 
to install the `qt4-mac-devel` package.

Next you will need to download and install ccfits. You can do this using 
commands similar to the following

    wget http://heasarc.gsfc.nasa.gov/fitsio/CCfits/CCfits-2.4.tar.gz
    tar xvzf CCfits-2.4.tar.gz
    cd CCfits

At this point you need to specify that you want to use the Macports gcc compiler
and not clang:

    export CC=/opt/local/bin/gcc-mp-4.7
    export CXX=/opt/local/bin/g++-mp-4.7

    ./configure --prefix=/opt/local --with-cfitsio=/opt/local
    make
    sudo make install

## 1.2 Debian-based GNU/Linux (including Ubuntu)

Prior to installing `SIMTOI` follow the instructions for building and testing
[liboi](https://github.com/bkloppenborg/liboi) for your platform.

### 1.2.1 OpenGL and OpenCL

To enable OpenCL-OpenGL interoperability you should also install the
mesa OpenGL library and the basic OpenCL headers:

     sudo apt-get install libglu1-mesa libglu1-mesa-dev opencl-headers

For OpenCL capabilities you need to install drivers for your device,
and an OpenCL Installable Client Driver (ICD) loader, which are vendor
dependent:

#### 1.2.1.1 NVIDIA

On NVidia hardware, simply install the video card drivers (either from your
package manager or from NVidia directly) which also contain the OpenGL and 
OpenCL libraries.

     sudo apt-get install nvidia-331 nvidia-opencl-icd-331 nvidia-331-uvm nvidia-modprobe nvidia-opencl-dev 

where you should (probably) choose the most current drivers rather than the 
`nvidia-331` branch. 

If you prefer, you can also install download the drivers directly from the 
NVIDIA website instead.

#### 1.2.1.2 AMD GPUs and AMD CPUs:

On ATI hardware the installation is a multi-stage process. First install
the video card drivers.

    sudo apt-get install fglrx fglrx-dev

Next you need to download and install the 
[AMD APP SDK](http://developer.amd.com/tools/heterogeneous-computing/amd-accelerated-parallel-processing-app-sdk/).

#### 1.2.1.3 Intel CPUs and GPUs

Neither CPUs or Intel GPUs are fully supported at this time. Please contact us
if you wish to assist with development on these platforms.

### 1.2.2 Compiler and libraries

Recent version of Debian-based GNU/Linux should meet the requirements of C++11
support for their gcc/g++ compiler.

All the libraries can be installed with:

    sudo apt-get install build-essential g++ cmake libccfits0 libccfits-dev git libqt4-dev liblapack-dev libblas-dev libglu1-mesa libglu1-mesa-dev libglm-dev

# 2. Installation

## 2.1 Checkout / getting a copy of SIMTOI source code

After installing the aforementioned prerequisites, you simply need to checkout
a copy of SIMTOI and initialize the submodules for [liboi](https://github.com/bkloppenborg/liboi),
 [textio](https://github.com/bkloppenborg/textio), and [ccoifits](https://github.com/bkloppenborg/ccoifits).
To do this simply run the following commands:

    git clone https://github.com/bkloppenborg/simtoi.git
    cd simtoi
    git submodule update --init

## 2.2 Building instructions

After you have installed the prerequisites and checked out SIMTOI and 
submodules, complilation is easy:

    cd build
    cmake ..
    make

During the `cmake ..` step CMake will attempt to find the necessary libraries.
It may generate errors if something is not found. The error messages are often
clear, but sometimes a bit cryptic. If you need help, please ask.

Please note that you can compile SIMTOI using multiple cores by specifying
`make -jN` where `N` is the number of cores in your machine.

## 2.3 Additional minimizer: MultiNest

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


# Licensing and Acknowledgements


SIMTOI includes the following libraries as part of its distribution:
* [OpenCL Interferometry Library (liboi)](https://github.com/bkloppenborg/liboi)
* [textio](https://github.com/bkloppenborg/textio)
* [ccoifits](https://github.com/bkloppenborg/ccoifits).
* [json-cpp](https://github.com/open-source-parsers/jsoncpp)
* [levmar](http://www.ics.forth.gr/~lourakis/levmar/)

SIMTOI supports, but our repositories do not include, MultiNest (a Bayesian 
nested sampling minimization engine).
* [MultiNest](http://ccpforge.cse.rl.ac.uk/gf/project/multinest/)

SIMTOI is free software, distributed under the 
[GNU Lesser General Public License (Version 3)](<http://www.gnu.org/licenses/lgpl.html). 

If you use this software as part of a scientific publication, please cite the 
following works:

Kloppenborg, B.; Baron, F. (2012) "SIMTOI: SImulation and Modeling Tool for 
Optical Interferometry" (Version X).  Available from 
<https://github.com/bkloppenborg/simtoi>.

Kloppenborg, B.; Baron, F. (2012), "LibOI: The OpenCL Interferometry Library"
(Version X). Available from  <https://github.com/bkloppenborg/liboi>.

If you use the _levmar_ minmizer, see their instruction for including 
[a suitable reference](http://www.ics.forth.gr/~lourakis/levmar/bibentry.html).

If you use the _MultiNest_ minimizer see reference instructions on the 
[_MultiNest_ website](http://ccpforge.cse.rl.ac.uk/gf/project/multinest/)

