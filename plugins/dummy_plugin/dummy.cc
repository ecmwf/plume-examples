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

#include "dummy.h"
#include <iostream>


namespace dummy_plugin {

REGISTER_LIBRARY(DummyPlugin)

DummyPlugin::DummyPlugin() : Plugin("DummyPlugin"){};

DummyPlugin::~DummyPlugin(){};

const DummyPlugin& DummyPlugin::instance() {
    static DummyPlugin instance;
    return instance;
}
//--------------------------------------------------------------


// PluginCoreDummy
static plume::PluginCoreBuilder<PluginCoreDummy> plugincoreBuilderFoo;

PluginCoreDummy::PluginCoreDummy(const eckit::Configuration& conf) : PluginCore(conf) {}

PluginCoreDummy::~PluginCoreDummy() {}

//--------------------------------------------------------------


}  // namespace dummy_plugin
