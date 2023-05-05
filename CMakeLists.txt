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

cmake_minimum_required( VERSION 3.16 FATAL_ERROR )

find_package( ecbuild 3.5 REQUIRED HINTS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../ecbuild)

project( plume_examples VERSION 0.0.1 LANGUAGES CXX Fortran)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

ecbuild_find_package( NAME eckit  VERSION  1.18   REQUIRED )
ecbuild_find_package( NAME fckit  VERSION  0.10   REQUIRED )
ecbuild_find_package( NAME atlas  VERSION  0.31.1 REQUIRED )
ecbuild_find_package( NAME plume  VERSION  0.0.1  REQUIRED )

## Plugins
add_subdirectory(plugins)

## Plugins Data
set(DATA_PATH ${CMAKE_CURRENT_SOURCE_DIR}/data)
add_subdirectory(data)

## NWP Emulator
add_subdirectory(nwp_emulator)

# finalize
ecbuild_install_project( NAME ${PROJECT_NAME} )
ecbuild_print_summary()

