/*
 * (C) Copyright 2023- ECMWF.
 *
 * This software is licensed under the terms of the Apache Licence Version 2.0
 * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *
 * In applying this licence, ECMWF does not waive the privileges and immunities
 * granted to it by virtue of its status as an intergovernmental organisation nor
 * does it submit to any jurisdiction.
 */

#include "uv_bound_check.h"
#include <iostream>


namespace uv_bound_check {



static plume::PluginCoreBuilder<PluginCoreUVBoundCheck> PluginCoreUVBoundCheckBuilder;

PluginCoreUVBoundCheck::PluginCoreUVBoundCheck(const eckit::Configuration& conf) : PluginCore(conf) {
    lBound_ = conf.getDouble("lower_bound");
    uBound_ = conf.getDouble("upper_bound");
}

PluginCoreUVBoundCheck::~PluginCoreUVBoundCheck() {}

void PluginCoreUVBoundCheck::setup() {}

void PluginCoreUVBoundCheck::run() {

    eckit::Log::info() << "Checking ranges.." << std::endl;

    // Check U field    
    if (!inRange( modelData().getAtlasFieldShared("U100") )) {
        eckit::Log::warning() << "U100 values not in range!" << std::endl;
    }

    // Check V field
    if (!inRange( modelData().getAtlasFieldShared("V100") )) {
        eckit::Log::warning() << "V100 values not in range!" << std::endl;
    }
}


bool PluginCoreUVBoundCheck::inRange(const atlas::Field& field) const {
    atlas::array::ArrayView<const double,3> arr = atlas::array::make_view<const double,3>( field );
    bool res = true;
    for (int idx = 0; idx < arr.size(); idx++) {
        auto v = *(arr.data() + idx);
        if ((v < lBound_) || (v > uBound_)) {
            return false;
        }
    }
    return true;
}

// ------------------------------------------------------

// ------------------------------------------------------
REGISTER_LIBRARY(UVBoundCheckPlugin)

UVBoundCheckPlugin::UVBoundCheckPlugin() : Plugin("UVBoundCheckPlugin"){};

UVBoundCheckPlugin::~UVBoundCheckPlugin(){};

const UVBoundCheckPlugin& UVBoundCheckPlugin::instance() {
    static UVBoundCheckPlugin instance;
    return instance;
}
// ------------------------------------------------------

}  // namespace uv_bound_check
