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

# Plume Fortran macro
include(${plume_INTERFACE_PLUGIN_CMAKE_FILE})


plume_plugin_interface(
    PLUGIN_TEMPLATE
        ${CMAKE_CURRENT_SOURCE_DIR}/dummy.F90.in
    PLUGIN_NAME 
        "DummyPlugin_F"
    PLUGIN_VERSION 
        "0.1.0"
    PLUGIN_SHA 
        "undefined"
    PLUGIN_REQUIRED_PARAMS
    PLUGINCORE_NAME
        "DummyPluginCore_F"
)


# Example of Fortran plugin (through interface plugin)
ecbuild_add_library(
    TARGET dummy_plugin_fortran

    SOURCES
        ${INTERFACE_PLUGIN_SOURCES}

    PUBLIC_INCLUDES
       $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
       $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>

    PRIVATE_INCLUDES
        "${eckit_INCLUDE_DIRS}"
        "${CMAKE_SOURCE_DIR}"

    PRIVATE_LIBS
        eckit
        eckit_mpi
        fckit
        atlas_f
        plume_f
        plume_plugin
)
