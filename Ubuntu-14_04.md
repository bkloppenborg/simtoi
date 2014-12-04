Ubuntu 14.04 build and installation instructions
========

The following are the minimial installation and build instructions for
getting liboi and SIMTOI operational on Ubuntu 14.04. These instructions
were written on 2014-Dec-02 against

    liboi commit ba34e05404901147d3b178223c57ddc3afda8672
    
and will only be updated for future versions by request.

## liboi

### Prerequisites

    sudo apt-get install build-essential g++ cmake libccfits0 libccfits-dev git
    sudo apt-get install libglu1-mesa libglu1-mesa-dev
    sudo apt-get install opencl-headers ocl-icd-libopencl1

### NVIDIA GPUs

    sudo apt-get install nvidia-331 nvidia-331-uvm nvidia-modprobe nvidia-opencl-icd-331
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
    cmake ..
    make
    
### Verifying functionality

    cd liboi/bin
    ./liboi_tests
    ./liboi_benchmark

## SIMTOI


