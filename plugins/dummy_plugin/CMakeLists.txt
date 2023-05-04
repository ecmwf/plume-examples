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

# Foo plugin
ecbuild_add_library(
    TARGET dummy_plugin
    SOURCES
        dummy.h
        dummy.cc
    PUBLIC_INCLUDES
       $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>
       $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src>
    PRIVATE_INCLUDES
        "${eckit_INCLUDE_DIRS}"
        "${CMAKE_SOURCE_DIR}"
    PRIVATE_LIBS
        eckit
        atlas
        plume_plugin
)