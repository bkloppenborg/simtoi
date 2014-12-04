Ubuntu 12.04 build and installation instructions
========

The following are the minimial installation and build instructions for
getting liboi and SIMTOI operational on Ubuntu 12.04.5. These instructions
were written on 2014-Dec-02 against

    liboi commit ba34e05404901147d3b178223c57ddc3afda8672
    
and will only be updated for future versions by request.

## liboi

### Prerequisites

This version of Ubuntu does not have a OpenCL installable client driver,
thus we have to use the vendor-supplied version of OpenCL installed along side
the video drivers.

    sudo apt-get install build-essential g++ cmake git
    sudo apt-get install libccfits0 libccfits-dev
    sudo apt-get install libglu1-mesa libglu1-mesa-dev
    sudo apt-get install opencl-headers

### NVIDIA GPUs

    sudo apt-get install nvidia-current nvidia-opencl-dev
    sudo reboot

### AMD/ATI GPUs

The following instructions have not been tested, but are likely to be correct
from past experience.

    sudo apt-get install fglrx
    sudo reboot

### Building

    git clone https://github.com/bkloppenborg/liboi.git
    cd liboi
    git submodule update --init
    cd build
    cmake ..vim
    make
    
### Verifying functionality

    cd liboi/bin
    ./liboi_tests
    ./liboi_benchmark

## SIMTOI

### Prerequisites

In addition to the perquisites for liboi, the following packages need to be
installed.

    sudo apt-get install libqt4-dev liblapack-dev libblas-dev libglm-dev

### Building

    git clone https://github.com/bkloppenborg/simtoi.git
    cd simtoi
    git submodule update --init
    cd build
    cmake ..
    make
