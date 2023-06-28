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
class WindTurbine {

public:

    WindTurbine(const eckit::Configuration& conf);

    ~WindTurbine();

    void setupClosestGP(atlas::Field lonLatField);

    void calculatePower(atlas::Field fieldU, atlas::Field fieldV);

    const double& lat() { return lat_; }
    const double& lon() { return lon_; }
    const double& hubHeight() { return hubHeight_; }
    const double& radius() { return radius_; }
    const double& Cp() { return Cp_; }
    const double& cutoffMax() { return cutoffMax_; }
    const double& cutoffMin() { return cutoffMin_; }
    const double& rhoHub() { return rhoHub_; }

private:

    // WT ID
    int ID_;

    // WT coordinates
    double lat_;
    double lon_;

    // Wind turbine parameters
    double hubHeight_;
    double radius_;
    double Cp_;
    double cutoffMax_;
    double cutoffMin_;

    // Atmospheric params
    double rhoHub_;

    // Nearest Grid Point info
    int nearestPointID_;
    double minDistanceLocal_;
    size_t minRankGlob_;

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

    // Wind turbines
    std::vector<WindTurbine> windTurbines_;

    // Atlas fields U and V
    atlas::Field fieldU_;
    atlas::Field fieldV_;

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
