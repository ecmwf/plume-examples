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

#include "abl_extractor.h"
#include <iostream>
#include <fstream>

#include "atlas/array.h"
#include "atlas/meshgenerator.h"
#include "atlas/grid.h"
#include "atlas/functionspace/NodeColumns.h"
#include "atlas/domain/Domain.h"
#include "atlas/output/Gmsh.h"


namespace abl_extractor {

REGISTER_LIBRARY(PluginAreaProbe)

PluginAreaProbe::PluginAreaProbe() : Plugin("ABLextractor"){};

PluginAreaProbe::~PluginAreaProbe(){};

const PluginAreaProbe& PluginAreaProbe::instance() {
    static PluginAreaProbe instance;
    return instance;
}
//--------------------------------------------------------------


// PluginCoreAreaProbe
static plume::PluginCoreBuilder<PluginCoreAreaProbe> pluginCorelBuilderAreaProbe;

PluginCoreAreaProbe::PluginCoreAreaProbe(const eckit::Configuration& conf) : PluginCore(conf) {
    boundingBoxNorth_ = conf.getDouble("bounding_box_north_deg");
    boundingBoxSouth_ = conf.getDouble("bounding_box_south_deg");
    boundingBoxEast_ =  conf.getDouble("bounding_box_east_deg");
    boundingBoxWest_ =  conf.getDouble("bounding_box_west_deg");
}

PluginCoreAreaProbe::~PluginCoreAreaProbe() {}

void PluginCoreAreaProbe::setup() {
    fieldU_ = modelData().getAtlasFieldShared("U");
    fieldV_ = modelData().getAtlasFieldShared("V");

    // field function space
    inputFuncSpace_ = fieldU_.functionspace();
    atlas::Grid inputGrid = inputFuncSpace_.grid();
    atlas::idx_t levels = inputFuncSpace_.levels();

    // target mesh and functional space
    atlas::RectangularLonLatDomain rd({boundingBoxWest_,boundingBoxEast_}, {boundingBoxSouth_,boundingBoxNorth_});
    atlas::Grid areaGrid(inputGrid, rd); 

    atlas::MeshGenerator outputMeshGen_("structured");
    outputMesh_ = outputMeshGen_.generate(areaGrid, atlas::grid::MatchingPartitioner(inputFuncSpace_));
    outputFuncSpace_ = atlas::functionspace::NodeColumns{outputMesh_};

    // define target field
    outputField_ = outputFuncSpace_.createField<double>(atlas::option::name("target") | atlas::option::levels(levels) | atlas::option::variables(1));
}


void PluginCoreAreaProbe::run() {

    // setup interpolation
    atlas::util::Config scheme;
    scheme.set("type", "structured-linear2D");
    atlas::Interpolation interpolation(scheme, inputFuncSpace_, outputFuncSpace_);

    // execute interpolation
    interpolation.execute(fieldU_, outputField_);

    // write out to gmsh for testing/debugging
    atlas::output::Gmsh gmshSrc("source.msh");
    atlas::MeshGenerator meshgenSrc("structured");
    atlas::Mesh meshSrc = meshgenSrc.generate(inputFuncSpace_.grid());
    gmshSrc.write(meshSrc);
    gmshSrc.write(fieldU_);

    // interpolated mesh and field (within lat/lon domain)
    atlas::output::Gmsh gmsh("interpolated.msh");
    gmsh.write(outputMesh_);
    gmsh.write(outputField_);

};


void PluginCoreAreaProbe::teardown() {
    // nothing to do here..
};


//--------------------------------------------------------------


}  // namespace abl_extractor