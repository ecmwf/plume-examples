
! (C) Copyright 2023- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.

module nwp_emulator_utils_module

use iso_c_binding
use fckit_mpi_module
use atlas_module, only: atlas_GridDistribution, &
                        atlas_Partitioner, &
                        atlas_meshgenerator, &
                        atlas_functionspace_structuredcolumns, &
                        atlas_real, &
                        atlas_grid, &
                        atlas_config, &
                        atlas_field, &
                        atlas_library, &
                        atlas_reducedgaussiangrid, &
                        atlas_fs_strcol => atlas_functionspace_StructuredColumns

public :: field_generator

! A helper class for creating Atlas Fields used by the NWP emulator 
type field_generator
    type(atlas_fs_strcol) :: fs  ! function space    
    integer :: grid_size_x = 32  ! grid size X
    integer :: grid_size_y = 64  ! grid size Y
    integer :: n_halo = 3        ! N halo points
    integer :: n_levels = 19     ! N vertical levels for 3D fields
    integer :: n_variables = 1   ! n variables per field

    logical :: fs_initialised = .false.
contains  
    procedure, public :: setup_function_space   => field_generator__create_function_space
    procedure, public :: create_field   => field_generator__create_field
    procedure, public :: create_field_2d   => field_generator__create_field_2d
    procedure, public :: create_field_3d   => field_generator__create_field_3d
    procedure, public :: finalise   => field_generator__finalise
end type

interface

function vortex_roll_interface(lon, lat, t) result(roll) bind(C, name="vortex_roll")
    use iso_c_binding, only: c_double
    real(c_double), value :: lon
    real(c_double), value :: lat
    real(c_double), value :: t
    real(c_double) :: roll
end function

end interface

contains

subroutine field_generator__create_function_space(this)
    class(field_generator) :: this
    type(atlas_grid) :: grid
    type(atlas_config) :: config
    type(atlas_griddistribution) :: distribution
    type(fckit_mpi_comm) :: comm
    integer :: grid_size_vec(this%grid_size_x)
    integer :: size

    grid_size_vec = this%grid_size_y
    grid = atlas_reducedgaussiangrid( grid_size_vec )

    comm = fckit_mpi_comm()
    size = comm%size()

    config = atlas_config()
    call config%set("type", "checkerboard")
    call config%set("bands", size)
    distribution = atlas_GridDistribution(grid, config)
    call config%final()

    this%fs = atlas_functionspace_structuredcolumns(grid, distribution, halo=this%n_halo, levels=this%n_levels)

    this%fs_initialised = .true.
end subroutine

function field_generator__create_field(this, name, n_levels) result(field)
    class(field_generator) :: this
    character(*) :: name
    integer :: n_levels
    type(atlas_Field) :: field

    type(atlas_Field) :: lon_lat_field
    real(c_double), pointer :: field_view(:,:,:)
    real(c_double), pointer :: lon_lat(:,:)

    real(c_double) :: lat
    real(c_double) :: lon
    real(c_double) :: t
    real(c_double) :: val  

    ! create a field
    field = this%fs%create_field(name=name, kind=atlas_real(c_double), variables=this%n_variables, levels=n_levels)

    call field%data(field_view)

    ! fill-in field with vortex rollup
    lon_lat_field = this%fs%xy()
    call lon_lat_field%data(lon_lat)

    do i_pt=1,this%fs%size()
        lon = lon_lat(1, i_pt)
        lat = lon_lat(2, i_pt)
        do i_lvl=1,n_levels
            do i_var=1,this%n_variables
            t = real(i_lvl-1)/real(n_levels)
            val = vortex_roll_interface(lon, lat, t)
            field_view(i_var, i_lvl, i_pt) = val
            enddo
        enddo
    enddo
end function

! create a 2D field
function field_generator__create_field_2d(this, name) result(field)
    class(field_generator) :: this
    character(*) :: name
    type(atlas_Field) :: field
    field = this%create_field(name, 1)
end function

! create a 3D field
function field_generator__create_field_3d(this, name) result(field)
    class(field_generator) :: this
    character(*) :: name
    type(atlas_Field) :: field
    field = this%create_field(name, this%n_levels)
end function

subroutine field_generator__finalise(this)
    class(field_generator) :: this
    if (this%fs_initialised) then
    call this%fs%final()
    endif
end subroutine

end module nwp_emulator_utils_module