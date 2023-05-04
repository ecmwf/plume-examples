! (C) Copyright 2023- ECMWF.
!
! This software is licensed under the terms of the Apache Licence Version 2.0
! which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
!
! In applying this licence, ECMWF does not waive the privileges and immunities
! granted to it by virtue of its status as an intergovernmental organisation nor
! does it submit to any jurisdiction.

module nwp_emulator_module
  
use iso_c_binding, only : c_null_char, c_bool
use fckit_log_module, only : log => fckit_log
use atlas_module, only: atlas_library, atlas_Field
use plume_module
use nwp_emulator_utils_module, only : field_generator


contains

subroutine initialise()
  call atlas_library%initialise()
end subroutine


subroutine execute()  
  
  ! plume structures
  type(plume_protocol) :: offers
  type(plume_manager) :: manager
  type(plume_data) :: data
  
  ! data
  type(field_generator) :: generator
  type(atlas_Field) :: field_U
  type(atlas_Field) :: field_V
  type(atlas_Field) :: field_U100
  type(atlas_Field) :: field_V100
  type(atlas_Field) :: field_TWC
  
  character(1024) :: plume_config_path
  integer :: iter
  logical(c_bool) :: is_requested
  
  call log%info("*** Running NWP emulator (Fortran) ***\n")
  
  CALL get_command_argument(1, plume_config_path)
  
  ! init
  call plume_check(plume_initialise())
  
  ! Define data offered by Plume
  call plume_check(offers%initialise())

  ! Note: parameters might be available "always" 
  ! (i.e. regardless of whether plugins require them or not)
  call plume_check(offers%offer_int("TSTEP", "always", "Simulation Step"))

  ! NWP Atlas Fields passed to plugins. They are passed "on-request"
  ! (i.e. passed to plugins only if required by active plugins)
  call plume_check(offers%offer_atlas_field("U", "on-request", "Dummy field representing U"))
  call plume_check(offers%offer_atlas_field("V", "on-request", "Dummy field representing V"))
  call plume_check(offers%offer_atlas_field("U100", "on-request", "Dummy field representing U100"))
  call plume_check(offers%offer_atlas_field("V100", "on-request", "Dummy field representing V100"))
  call plume_check(offers%offer_atlas_field("TWC", "on-request", "Dummy field representing TWC"))
  
  ! Negotiate with plugins
  ! At this point, plugins that succeed in the negotiation
  ! are "activated" (i.e. they are eligible to run)
  call plume_check(manager%initialise())
  call plume_check(manager%configure(trim(plume_config_path)//c_null_char))
  call plume_check(manager%negotiate(offers))  

  ! Generate NWP fields that Plume provides
  call generator%setup_function_space()
  field_U = generator%create_field_3d("U")
  field_V = generator%create_field_3d("V")
  field_U100 = generator%create_field_2d("U100")
  field_V100 = generator%create_field_2d("V100")
  field_TWC = generator%create_field_2d("TWC")

  ! Setup Plume data
  call plume_check(data%initialise())

  ! Initialise parameter
  call plume_check( data%create_int("TSTEP", 0) )

  ! Give Plume only the NWP fields requested by activated plugins
  call plume_check( manager%is_param_requested("U", is_requested) )
  if (is_requested .eqv. .true.) then
    call plume_check( data%provide_atlas_field_shared("U", field_U) )
  endif

  call plume_check( manager%is_param_requested("V", is_requested) )
  if (is_requested .eqv. .true.) then
    call plume_check( data%provide_atlas_field_shared("V", field_V) )  
  endif

  call plume_check( manager%is_param_requested("U100", is_requested) )
  if (is_requested .eqv. .true.) then
    call plume_check( data%provide_atlas_field_shared("U100", field_U100) )  
  endif

  call plume_check( manager%is_param_requested("V100", is_requested) )
  if (is_requested .eqv. .true.) then
    call plume_check( data%provide_atlas_field_shared("V100", field_V100) )  
  endif

  call plume_check( manager%is_param_requested("TWC", is_requested) )
  if (is_requested .eqv. .true.) then
    call plume_check( data%provide_atlas_field_shared("TWC", field_TWC) )  
  endif
  
  ! Feed plugins with the data
  call plume_check(manager%feed_plugins(data))
  
  ! Run the model for 10 iterations
  do iter=1,10
    call plume_check(manager%run())
    call plume_check( data%update_int("TSTEP", iter) )
  enddo
  
  ! Teardown as necessary
  call plume_check(data%finalise())
  call plume_check(manager%finalise())
  call plume_check(offers%finalise())
  call plume_check(plume_finalise())

  call field_U%final()
  call field_V%final()
  call field_U100%final()
  call field_V100%final()
  call field_TWC%final()
  
  call log%info("*** NWP emulator (Fortran) has completed. ***")
end subroutine execute


subroutine finalise()
  call atlas_library%finalise()
end subroutine

end module nwp_emulator_module


program nwp_emulator_program
use nwp_emulator_module
  call initialise()
  call execute()
  call finalise()
end program
    