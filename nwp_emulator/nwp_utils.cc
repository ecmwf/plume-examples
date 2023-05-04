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

#include <memory>
#include <vector>

#include "atlas/option.h"
#include "atlas/parallel/mpi/mpi.h"
#include "atlas/runtime/Log.h"

#include "atlas/array/ArrayView.h"
#include "atlas/grid/Grid.h"
#include "atlas/grid/StructuredGrid.h"
#include "atlas/grid/detail/grid/Gaussian.h"
#include "atlas/grid/Distribution.h"
#include "atlas/grid/Vertical.h"
#include "atlas/util/function/VortexRollup.h"
#include "atlas/util/CoordinateEnums.h"

#include "atlas/util/function/VortexRollup.h"

#include "nwp_utils.h"

using atlas::Log;

namespace nwp_util {

void FieldGenerator::setupFunctionSpace() {

    long n_procs = atlas::mpi::comm().size();

    // ------------------ grid ------------------
    std::vector<long> gaussian_x(gridSizeX_,gridSizeY_);
    atlas::StructuredGrid grid = atlas::grid::detail::grid::reduced_gaussian( gaussian_x );
    atlas::grid::Distribution distribution(grid, 
                                           atlas::util::Config("type", "checkerboard") | 
                                           atlas::util::Config("bands", n_procs));

    // ----------- function space ---------------
    fs_ = atlas::functionspace::StructuredColumns(grid, 
                                                    distribution, 
                                                    atlas::util::Config("halo", nHalo_) | 
                                                    atlas::util::Config("levels", nLevels_) );

}

atlas::Field FieldGenerator::createField3D(const std::string& name){
    return createField(name, nLevels_);
}

atlas::Field FieldGenerator::createField2D(const std::string& name){
    return createField(name, 1);
}

atlas::Field FieldGenerator::createField(const std::string& name, int n_levels){
    atlas::Field field_source = fs_.createField<double>(atlas::option::name(name) | 
                                                        atlas::option::variables(1) | 
                                                        atlas::option::levels(n_levels));
    
    auto lonlat = atlas::array::make_view<double,2>(fs_.xy());
    auto source = atlas::array::make_view<double,3>(field_source);

    int n_vertical_lev = source.shape(1);
    int n_vars = source.shape(2);

    // fill-in field with vortex rollup
    for (atlas::idx_t i_pt = 0; i_pt < fs_.size(); i_pt++) {
        for (atlas::idx_t i_ver = 0; i_ver < n_levels; i_ver++) {
            for (atlas::idx_t i_var = 0; i_var < n_vars; i_var++) {

                double t = double(i_ver)/n_levels;
                double lon = lonlat(i_pt, atlas::LON);
                double lat = lonlat(i_pt, atlas::LAT);
                double val = atlas::util::function::vortex_rollup(lon, lat, t);

                source(i_pt, i_ver, i_var) = val;
            }
        }
    }

    Log::info() << "Created field: " << name << ", with shape" << field_source.shape() << std::endl;

    return field_source;
}

} // namespace nwp_util



extern "C" {

double vortex_roll(double lon, double lat, double t) {
    return atlas::util::function::vortex_rollup(lon, lat, t);
}

}