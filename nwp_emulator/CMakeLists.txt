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

# NWP_emulators

ecbuild_add_library( TARGET nwp_utils
  SOURCES
    nwp_utils.h
    nwp_utils.cc
  PUBLIC_LIBS
    atlas
)

## C++
ecbuild_add_executable( TARGET nwp_emulator.x
  SOURCES
    nwp_emulator.cc
  LIBS
    nwp_utils
    plume_plugin_manager
)

## Fortran
ecbuild_add_executable( TARGET nwp_emulator_fort.x
  SOURCES
    nwp_utils.F90
    nwp_emulator.F90
  LIBS
    nwp_utils
    plume_f
)

# Script to run the emulators
ecbuild_configure_file(run_nwp_emulator.sh.in
                       ${CMAKE_BINARY_DIR}/bin/run_nwp_emulator.sh
)