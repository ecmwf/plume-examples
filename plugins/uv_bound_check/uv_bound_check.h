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

#include "atlas/array.h"
#include "atlas/field/Field.h"
#include "atlas/field/detail/FieldImpl.h"

#include "plume/PluginCore.h"
#include "plume/Plugin.h"

namespace uv_bound_check {


class PluginCoreUVBoundCheck : public plume::PluginCore {
public:
    PluginCoreUVBoundCheck(const eckit::Configuration& conf);
    ~PluginCoreUVBoundCheck();
    void setup() override;
    void run() override;
    constexpr static const char* type() { return "UV-bound-check-plugincore"; }

private:
    bool inRange(const atlas::Field& field) const;
    double lBound_;
    double uBound_;
};


class UVBoundCheckPlugin : public plume::Plugin {
public:
    UVBoundCheckPlugin();
    ~UVBoundCheckPlugin();

    plume::Protocol negotiate() override {
        plume::Protocol protocol;
        protocol.requireAtlasField("U100");
        protocol.requireAtlasField("V100");
        return protocol;
    }

    // Return the static instance
    static const UVBoundCheckPlugin& instance();
    std::string version() const override { 
        return "0.0.1-UV-bound-check"; 
    }

    std::string gitsha1(unsigned int count) const override { 
        return "undefined";
    }

    virtual std::string plugincoreName() const override {
        return PluginCoreUVBoundCheck::type();
    }
};


}  // namespace uv_bound_check
