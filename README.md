# Plume-examples
plume-examples are example plugins for the Plume plugin system. Data interface is provided by Atlas.
 - Plume: https://github.com/ecmwf/plume
 - Atlas: https://github.com/ecmwf/atlas

## Description
plume-examples demonstrate how to use plume API (both C++ and Fortran) for various types of plugins, including point time-series extraction (for a wind turbine model) and a simplified anomaly detection algorithm.

### Example Plugins
**Inportant**: To be noted that these example plugins are intended for illustration purposes only and are not optimised for any real-life applications.

 - **abl_extractor**: Plugin that extracts a 3D crop of wind field (U and V components) on a selected region

 - **dummy_plugin**: A minimal plugin that shows the main structure of a plume plugin (and internal plugin "core")

 - **uv_bound_check**: It checks that the values of 2 fields (U and V at h=100m) are within a pre-defined range

 - **wind_turbine**: A plugins that implements a simple wind turbine model (using U100 field as input)

### NWP Emulator
This repository also contains an emulator of a NWP (Numerical Weather Prediction) model that demonstrates how to use Plume API on the model side. The NWP emulator is available in both C++ and Fortran.

## Requirements
Build dependencies:

- C/C++ compiler (C++17)
- Fortran 2008 compiler
- CMake >= 3.16 --- For use and installation see http://www.cmake.org/
- ecbuild >= 3.5 --- ECMWF library of CMake macros (https://github.com/ecmwf/ecbuild)

Runtime dependencies:
  - eckit >= 1.18.0 (https://github.com/ecmwf/eckit)
  - fckit >= 0.10.0 (https://github.com/ecmwf/fckit)
  - Atlas >= 0.31.1 (https://github.com/ecmwf/atlas)
  - Plume >=  0.0.1 (https://github.com/ecmwf/plume)

## Installation
Plume-examples employs an out-of-source build/install based on CMake.
Make sure ecbuild is installed and the ecbuild executable script is found ( `which ecbuild` ).
Now proceed with installation as follows:

```bash
# Environment --- Edit as needed
srcdir=$(pwd)
builddir=build
installdir=$HOME/local  

# 1. Create the build directory:
mkdir $builddir
cd $builddir

# 2. Run CMake
ecbuild --prefix=$installdir -- \
  -Deckit_ROOT=<path/to/eckit/install> \
  -Dfckit_ROOT=<path/to/fckit/install> \
  -Dplume_ROOT=<path/to/plume/install> \
  -Datlas_ROOT=<path/to/atlas/install> $srcdir

# 3. Compile / Install
make -j10
make install
```

## Run the example plugins
To run the C++ NWP emulator + plugins:
```bash
cd $builddir
./bin/run_nwp_emulator.sh cpp
```
Or alternatively, to run the Fortran NWP emulator + plugins:
```bash
./bin/run_nwp_emulator.sh fortran
```