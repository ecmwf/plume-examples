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

#include "eckit/config/YAMLConfiguration.h"
#include "eckit/config/LocalConfiguration.h"

#include "atlas/runtime/Log.h"
#include "atlas/runtime/AtlasTool.h"

#include "plume/Manager.h"
#include "plume/data/ModelData.h"
#include "plume/data/ParameterCatalogue.h"

#include "nwp_utils.h"

using atlas::Log;

class NWPEmulator : public atlas::AtlasTool {

    int execute(const Args& args) override {

        Log::info() << "*** Running NWP emulator ***" << std::endl;

        // Plume manager configuration
        eckit::YAMLConfiguration plumeConfig(eckit::PathName{plumeConfigPath_});
        plume::Manager::configure(plumeConfig);

        // Define data offered by Plume
        plume::Protocol offers;

        // Example of parameters to pass from the model to plugins

        // Note: parameters might be available "always" 
        // (i.e. regardless of whether plugins require them or not)
        offers.offerInt("TSTEP", "always", "Simulation Step");

        // NWP Atlas Fields passed to plugins. They are passed "on-request"
        // (i.e. passed to plugins only if required by active plugins)
        offers.offerAtlasField("U", "on-request", "Dummy field representing U");
        offers.offerAtlasField("V", "on-request", "Dummy field representing V");
        offers.offerAtlasField("U100", "on-request", "Dummy field representing U100");
        offers.offerAtlasField("V100", "on-request", "Dummy field representing U100");
        offers.offerAtlasField("TWC", "on-request", "Dummy field representing TWC");

        // Negotiate with plugins.
        // At this point, plugins that succeed in the negotiation
        // are "activated" (i.e. they are eligible to run)
        plume::Manager::negotiate(offers);

        // Setup field generator (to generate dummy NWP fields)
        nwp_util::FieldGenerator field_gen;
        field_gen.setupFunctionSpace();

        atlas::Field fieldU = field_gen.createField3D("U");
        atlas::Field fieldV = field_gen.createField3D("V");
        atlas::Field fieldU100 = field_gen.createField2D("U100");
        atlas::Field fieldV100 = field_gen.createField2D("V100");
        atlas::Field fieldTWC = field_gen.createField2D("TWC");

        // Setup Plume data
        plume::data::ModelData data;

        // Initialise parameter
        data.createInt("TSTEP", 0);

        // Give Plume only the NWP fields requested by activated plugins
        if (plume::Manager::isParamRequested("U")){
            data.provideAtlasFieldShared("U", fieldU.get());
        }

        if (plume::Manager::isParamRequested("V")){   
            data.provideAtlasFieldShared("V", fieldV.get());
        }

        if (plume::Manager::isParamRequested("U100")){
            data.provideAtlasFieldShared("U100", fieldU100.get());
        }

        if (plume::Manager::isParamRequested("V100")){
            data.provideAtlasFieldShared("V100", fieldV100.get());
        }

        if (plume::Manager::isParamRequested("TWC")){
            data.provideAtlasFieldShared("TWC", fieldV100.get());
        }


        // Feed plugins with the data
        plume::Manager::feedPlugins(data);

        // Run the model for 10 iterations
        for (int iStep=0; iStep<10; iStep++){

            // Update parameters as necessary
            data.updateInt("TSTEP", iStep);

            plume::Manager::run();
        }

        // Teardown Plume and plugins
        plume::Manager::teardown();

        Log::info() << "*** NWP emulator has completed. ***" << std::endl;
        Log::info() << std::endl;

        return 0;

    };

public:
        NWPEmulator(int argc, char** argv): atlas::AtlasTool(argc, argv) {
        plumeConfigPath_ = argv[1];
    }

private:

    char* plumeConfigPath_;
};


int main(int argc, char** argv) {
    ASSERT_MSG(argc >= 2, "NOTE: this example is for illustration purposes only, error checks are missing!");
    NWPEmulator emulator(argc, argv);
    return emulator.start();
}
