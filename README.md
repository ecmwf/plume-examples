# Plume-plugins
:warning: This software is still under heavy development and not 
yet ready for operational use

plume-plugins is a collection of example plugins for the plume plugin system (https://github.com/ecmwf/plume).

## Description
plume-plugins demonstrate how to use plume API (both C++ and Fortran) for various types of plugins, including point time-series extraction (for a wind turbine model) and a simplified anomaly detection algorithm.

### Example Plugins
**Inportant**: To be noted that these example plugins are intended for illustration purposes only and are not optimised for any real-life application.

 - **abl_extractor**: Plugin that extracts a 3D crop of wind field (U and V components) on a selected region

 - **dummy_plugin**: A minimal plugin that shows the main structure of a plume plugin (and internal plugin "core")

 - **uv_bound_check**: It checks that the values of 2 fields (U and V at h=100m) are within a pre-defined range

 - **wind_turbine**: A plugins that implements a simple wind turbine model (using U100 field as input)

 ### NWP Emulator
This repository also contains an NWP (Numerical Weather Prediction) model "emulator" that demonstrates how to use Plume API on the model side. The NWP emulator is available in both C++ and Fortran.
