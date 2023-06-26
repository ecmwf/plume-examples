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

#include <iostream>
#include <string>

#include "atlas/field/Field.h"
#include "atlas/mesh/Mesh.h"
#include "atlas/functionspace/StructuredColumns.h"
#include "atlas/interpolation.h"

#include "plume/PluginCore.h"
#include "plume/Plugin.h"

namespace abl_extractor {


class PluginCoreAreaProbe : public plume::PluginCore {
public:
    PluginCoreAreaProbe(const eckit::Configuration& conf);

    ~PluginCoreAreaProbe();

    virtual void setup() override;

    virtual void run() override;

    virtual void teardown() override;

    constexpr static const char* type() { return "ABLextractor-plugincore"; }

private:

    // Atlas fields U and V
    atlas::Field fieldU_;
    atlas::Field fieldV_;

    // Bounding box (all vertical levels)
    double boundingBoxNorth_;
    double boundingBoxSouth_;
    double boundingBoxEast_;
    double boundingBoxWest_;

    atlas::Mesh outputMesh_;
    atlas::functionspace::StructuredColumns inputFuncSpace_;
    atlas::FunctionSpace outputFuncSpace_;    
    atlas::Field outputField_;

};
// ------------------------------------------------------

// ------------------------------------------------------
class PluginAreaProbe : public plume::Plugin {

public:
    PluginAreaProbe();

    ~PluginAreaProbe();

    plume::Protocol negotiate() override {
        plume::Protocol protocol;
        // protocol.requireAtlasVersion("0.32.0");
        protocol.requireAtlasField("U");
        protocol.requireAtlasField("V");
        return protocol;
    }

    // Return the static instance
    static const PluginAreaProbe& instance();

    std::string version() const override { return "0.0.1-AreaProbe"; }

    std::string gitsha1(unsigned int count) const override { return "undefined"; }

    virtual std::string plugincoreName() const override { return PluginCoreAreaProbe::type(); }
};
// ------------------------------------------------------

}  // namespace abl_extractor
