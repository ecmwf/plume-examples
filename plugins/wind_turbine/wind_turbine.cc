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


// ---- wind turbine
WindTurbine::WindTurbine(const eckit::Configuration& conf) :

    // WT name
    ID_{ conf.getInt("ID") },

    // WT coords
    lat_{ conf.getDouble("lat") },
    lon_{ conf.getDouble("lon") },

    // WT params
    hubHeight_{ conf.getDouble("hub_height") },
    radius_{ conf.getDouble("radius") },
    Cp_{ conf.getDouble("power_coeff") },
    cutoffMax_{ conf.getDouble("cutoff_max") },
    cutoffMin_{ conf.getDouble("cutoff_min") },

    // Rho at hub height
    rhoHub_{ conf.getDouble("rho_hub") },
    
    // Nearest Grid Point
    nearestPointID_{0},
    minDistanceLocal_{0.0},
    minRankGlob_{0} {
}

WindTurbine::~WindTurbine(){}


void WindTurbine::setupClosestGP(atlas::Field lonLatField) {

    auto lonLatArray = atlas::array::make_view<double,2>(lonLatField);
    double pointLat;
    double pointLon;
    double minDist2 = 1e12;
    int minID;

    // Brute-force loop
    for (int iPt = 0; iPt < lonLatArray.shape(0); iPt++) {
        pointLon   = lonLatArray(iPt, 0);
        pointLat   = lonLatArray(iPt, 1);
        auto dist2 = (lat_ - pointLat) * (lat_ - pointLat) +
                     (lon_ - pointLon) * (lon_ - pointLon);
        if (dist2 < minDist2) {
            minID    = iPt;
            minDist2 = dist2;
        }
    }

    double minDist = sqrt(minDist2);

    // mpi info
    size_t size = eckit::mpi::comm().size();
    size_t rank = eckit::mpi::comm().rank();

    // Now we need to find the (global) nearest
    std::pair<double, int> minDistRankLocal(minDist, rank);
    std::pair<double, int> minDistRankGlob(0.0, 0.0);
    eckit::mpi::comm().allReduce(minDistRankLocal, minDistRankGlob, eckit::mpi::minloc());
    size_t minRankGlob = minDistRankGlob.second;

    // Set nearest GP info
    nearestPointID_ = minID;
    minDistanceLocal_ = minDist;
    minRankGlob_ = minRankGlob;

    std::cout << "(" << rank << "/" << size << ") => "
              << "WT-ID: " << ID_
              << ", nearestPointID=" << nearestPointID_
              << ", minRankGlob=" << minRankGlob_ << std::endl;

}


void WindTurbine::calculatePower(atlas::Field fieldU, atlas::Field fieldV) {

    size_t size = eckit::mpi::comm().size();
    size_t rank = eckit::mpi::comm().rank();

    // only the rank with global min distance will do the calculation..
    if (rank == minRankGlob_) {

        // horizontal vel components
        auto arrayU = atlas::array::make_view<double,3>(fieldU);
        auto arrayV = atlas::array::make_view<double,3>(fieldV);

        auto hubU_ms = arrayU(nearestPointID_, 0, 0);
        auto hubV_ms = arrayV(nearestPointID_, 0, 0);

        // vel module at hub (approximated!)
        auto hubUmag_ms = sqrt(hubU_ms * hubU_ms + hubV_ms * hubV_ms);

        // calc power output
        double powerWatts;

        // min cutoff speed
        if (hubUmag_ms < cutoffMin_) {
            powerWatts = 0.0;
        } else if (hubUmag_ms > cutoffMax_) {
            powerWatts = 0.0;
        } else {

            double dynPressure = 3.14159 * radius_ * radius_;
            powerWatts = 0.5 * rhoHub_ * dynPressure * pow(hubUmag_ms, 3) * Cp_;

            std::cout << "(" << rank << "/" << size << "), "
                    << "WT-ID: " << ID_ << " --> U_hub: " << hubUmag_ms  << ", P_wt: " << powerWatts                   
                    << std::endl;
        }
    }
}

// ----------------- WindTurbinePluginCore ------------------
static plume::PluginCoreBuilder<WindTurbinePluginCore> WindTurbinePluginCoreBuilder;

WindTurbinePluginCore::WindTurbinePluginCore(const eckit::Configuration& conf) : PluginCore(conf) {

    // Wind turbines
    auto wtConfigs = conf.getSubConfigurations("wind_turbines");
    for (auto wtConfig: wtConfigs) {
        windTurbines_.push_back( WindTurbine(wtConfig) );
    }

}

WindTurbinePluginCore::~WindTurbinePluginCore() {}

void WindTurbinePluginCore::setup() {

    fieldU_ = modelData().getAtlasFieldShared("U100");
    fieldV_ = modelData().getAtlasFieldShared("V100");

    // function space
    auto lonlat = fieldU_.functionspace().lonlat();

    // find closest Grid Point to each WT
    for (auto& wt: windTurbines_) {
        wt.setupClosestGP(lonlat);
    }
};


void WindTurbinePluginCore::run() {
    for (auto& wt: windTurbines_) {
        wt.calculatePower(fieldU_, fieldV_);
    }
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
