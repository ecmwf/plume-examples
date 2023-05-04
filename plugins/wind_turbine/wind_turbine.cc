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

#include "wind_turbine.h"
#include <iostream>


namespace wind_turbine_plugin {


// ----------------- WindTurbinePluginCore ------------------
static plume::PluginCoreBuilder<WindTurbinePluginCore> WindTurbinePluginCoreBuilder;

WindTurbinePluginCore::WindTurbinePluginCore(const eckit::Configuration& conf) : PluginCore(conf), windTurbine_(conf) {

}

WindTurbinePluginCore::~WindTurbinePluginCore() {}

void WindTurbinePluginCore::setup() {

    fieldU_ = modelData().getAtlasFieldShared("U100");
    fieldV_ = modelData().getAtlasFieldShared("V100");

    // function space
    auto functSpace = fieldU_.functionspace();

    // mpi info
    size_t size = eckit::mpi::comm().size();
    size_t rank = eckit::mpi::comm().rank();

    // (Local) nearest point to WT
    nearestPoint(functSpace.lonlat());

    std::cout << "(" << rank << "/" << size << ") => "
              << "nearestPointID =" << nearestPointID_ << ", minDistanceLocal =" << minDistanceLocal_ << std::endl;

    // Now we need to find the (global) nearest
    std::pair<double, int> minDistRankLocal(minDistanceLocal_, rank);
    std::pair<double, int> minDistRankGlob(0.0, 0.0);
    eckit::mpi::comm().allReduce(minDistRankLocal, minDistRankGlob, eckit::mpi::minloc());

    std::cout << "(" << rank << "/" << size << ") => "
              << "minDistRankGlob.first=" << minDistRankGlob.first
              << ", minDistRankGlob.second=" << minDistRankGlob.second << std::endl;

    // working rank (rank containing global min)
    minRankGlob_ = minDistRankGlob.second;
};


void WindTurbinePluginCore::run() {

    size_t size = eckit::mpi::comm().size();
    size_t rank = eckit::mpi::comm().rank();

    // only the rank with global min distance will do the calculation..
    if (rank == minRankGlob_) {

        // horizontal vel components
        auto arrayU = atlas::array::make_view<double,3>(fieldU_);
        auto arrayV = atlas::array::make_view<double,3>(fieldV_);

        auto hubU_ms = arrayU(nearestPointID_, 0, 0);
        auto hubV_ms = arrayV(nearestPointID_, 0, 0);

        // vel module at hub (approximated!)
        auto hubUmag_ms = sqrt(hubU_ms * hubU_ms + hubV_ms * hubV_ms);

        // calc power output
        auto powerWatts = calcPower(hubUmag_ms);

        std::cout << "(" << rank << "/" << size << "), "
                  << " --> U_hub = " << hubUmag_ms 
                  << ", P_wt = " << powerWatts                   
                  << std::endl;
    }
}


void WindTurbinePluginCore::nearestPoint(atlas::Field lonLatField) {
    auto lonLatArray = atlas::array::make_view<double,2>(lonLatField);
    double pointLat;
    double pointLon;
    double minDist2 = 1e12;
    int minID;
    for (int iPt = 0; iPt < lonLatArray.shape(0); iPt++) {
        pointLon   = lonLatArray(iPt, 0);
        pointLat   = lonLatArray(iPt, 1);
        auto dist2 = (windTurbine_.lat() - pointLat) * (windTurbine_.lat() - pointLat) +
                     (windTurbine_.lon() - pointLon) * (windTurbine_.lon() - pointLon);
        if (dist2 < minDist2) {
            minID    = iPt;
            minDist2 = dist2;
        }
    }
    nearestPointID_   = minID;
    minDistanceLocal_ = sqrt(minDist2);
}


// power output of the WT
double WindTurbinePluginCore::calcPower(double hubUmag_ms) {

    // min cutoff speed
    if (hubUmag_ms < windTurbine_.cutoffMin()) {
        return 0.0;
    }

    //  max cutoff speed
    if (hubUmag_ms > windTurbine_.cutoffMax()) {
        return 0.0;
    }

    double dynPressure = 3.14159 * windTurbine_.radius() * windTurbine_.radius();
    return 0.5 * windTurbine_.rhoHub() * dynPressure * pow(hubUmag_ms, 3) * windTurbine_.Cp();
}

// ------------------------------------------------------

// ------------------------------------------------------
REGISTER_LIBRARY(WindTurbinePlugin)

WindTurbinePlugin::WindTurbinePlugin() : Plugin("WindTurbinePlugin"){};

WindTurbinePlugin::~WindTurbinePlugin(){};

const WindTurbinePlugin& WindTurbinePlugin::instance() {
    static WindTurbinePlugin instance;
    return instance;
}
// ------------------------------------------------------

}  // namespace wind_turbine_plugin
