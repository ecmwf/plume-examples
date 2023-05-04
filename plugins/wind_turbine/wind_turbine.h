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

#include <math.h>
#include <iostream>
#include <string>

#include "eckit/mpi/Comm.h"

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "atlas/field/detail/FieldImpl.h"
#include "atlas/functionspace/StructuredColumns.h"

#include "plume/PluginCore.h"
#include "plume/Plugin.h"

namespace wind_turbine_plugin {


// Class containing the Wind Turbine information
class WindTurbineInfo {

public:
    WindTurbineInfo(const eckit::Configuration& conf) :  

        // Horns-Rev coords
        lat_{ conf.getDouble("lat") },
        lon_{ conf.getDouble("lon") },

        // wind turbine params
        hubHeight_{ conf.getDouble("hub_height") },
        radius_{ conf.getDouble("radius") },
        Cp_{ conf.getDouble("power_coeff") },
        cutoffMax_{ conf.getDouble("cutoff_max") },
        cutoffMin_{ conf.getDouble("cutoff_min") },

        // rho at hub height
        rhoHub_{ conf.getDouble("rho_hub") } {}

    ~WindTurbineInfo() {}

    double& lat() { return lat_; }
    double& lon() { return lon_; }
    double& hubHeight() { return hubHeight_; }
    double& radius() { return radius_; }
    double& Cp() { return Cp_; }
    double& cutoffMax() { return cutoffMax_; }
    double& cutoffMin() { return cutoffMin_; }
    double& rhoHub() { return rhoHub_; }

private:

    // coordinates
    double lat_;
    double lon_;

    // wind turbine parameters
    double hubHeight_;
    double radius_;
    double Cp_;
    double cutoffMax_;
    double cutoffMin_;

    // atmospheric params
    double rhoHub_;
};

class WindTurbinePluginCore : public plume::PluginCore {

public:
    WindTurbinePluginCore(const eckit::Configuration& conf);

    ~WindTurbinePluginCore();

    virtual void setup() override;

    void run() override;

    virtual void teardown() override{
        // nothing to do here..
    };

    constexpr static const char* type() { return "WindTurbine"; }

private:
    // Local nearest grid point to the wind turbine
    void nearestPoint(atlas::Field lonLatField);

    // Power output of the wind turbine
    double calcPower(double hubUmag_ms);


private:
    // wind turbine information
    WindTurbineInfo windTurbine_;

    // atlas fields U and V
    atlas::Field fieldU_;
    atlas::Field fieldV_;

    // nelev
    int nelev_;

    int nearestPointID_;
    double minDistanceLocal_;

    // global
    size_t minRankGlob_;
};
// ------------------------------------------------------

// ------------------------------------------------------
class WindTurbinePlugin : public plume::Plugin {

public:
    WindTurbinePlugin();

    ~WindTurbinePlugin();

    plume::Protocol negotiate() override {
        plume::Protocol protocol;
        protocol.requireAtlasField("U100");
        protocol.requireAtlasField("V100");
        return protocol;
    }

    // Return the static instance
    static const WindTurbinePlugin& instance();

    std::string version() const override { return "0.0.1-WT"; }

    std::string gitsha1(unsigned int count) const override { return "undefined"; }

    virtual std::string plugincoreName() const override { return WindTurbinePluginCore::type(); }
};
// ------------------------------------------------------

}  // namespace wind_turbine_plugin
