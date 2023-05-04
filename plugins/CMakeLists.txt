# 
#  (C) Copyright 2023- ECMWF.
# 
#  This software is licensed under the terms of the Apache Licence Version 2.0
#  which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
# 
#  In applying this licence, ECMWF does not waive the privileges and immunities
#  granted to it by virtue of its status as an intergovernmental organisation nor
#  does it submit to any jurisdiction.
# 

## Examples plugins

# Extract ABL in an area
add_subdirectory(abl_extractor)         # C++
add_subdirectory(abl_extractor_fort)    # Fortran

# Minimal Dummy plugin
add_subdirectory(dummy_plugin)          # C++
add_subdirectory(dummy_plugin_fort)     # Fortran

# Check u,v range
add_subdirectory(uv_bound_check)        # C++ 
add_subdirectory(uv_bound_check_fort)   # Fortran

# Wind turbine model
add_subdirectory(wind_turbine)          # C++
add_subdirectory(wind_turbine_fort)     # Fortran