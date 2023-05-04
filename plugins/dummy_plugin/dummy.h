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
#include "plume/PluginCore.h"
#include "plume/Plugin.h"

namespace dummy_plugin {


// ------ Foo runnable plugincore that self-registers! -------
class PluginCoreDummy : public plume::PluginCore {
public:
    PluginCoreDummy(const eckit::Configuration& conf);

    ~PluginCoreDummy();

    void run() override { eckit::Log::info() << "Running a " << this->type() << "..." << std::endl; }

    constexpr static const char* type() { return "Dummy-plugincore"; }
};
// ------------------------------------------------------

// ------------------------------------------------------
class DummyPlugin : public plume::Plugin {

public:
    DummyPlugin();

    ~DummyPlugin();

    plume::Protocol negotiate() override {
        // Return an empty protocol
        return plume::Protocol();
    }

    // Return the static instance
    static const DummyPlugin& instance();

    std::string version() const override { return "0.0.1-Dummy"; }

    std::string gitsha1(unsigned int count) const override { return "undefined"; }

    virtual std::string plugincoreName() const override { return PluginCoreDummy::type(); }
};
// ------------------------------------------------------

}  // namespace dummy_plugin
