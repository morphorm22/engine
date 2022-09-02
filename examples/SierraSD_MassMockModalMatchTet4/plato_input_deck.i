// Optimizer service
begin service 1  
  code platomain
  number_processors 1
end service

// Service for calculating modal objective
begin service 2
  code sierra_sd
  number_processors 4
  // SierraSD needs caching turned on
  cache_state true
end service

// Service for calculating CAD parameter sensitivities
begin service 3
  code plato_esp
  // There are 3 design variables in the .csm file--this number should match that.
  number_processors 3
end service

begin service 4
  code plato_analyze
  number_processors 1
end service
      
begin criterion 1
  type modal_projection_error
  // The modal analysis (forward problem) will calculate 30 modes. Having this larger than the number of modes used to 
  // calculate the modal projection error will help the solver converge faster. 
  num_modes_compute 30
  // When we calculate the modal projection error we will only use modes 7, 8, 9, 10, 11, 12, 13
  modes_to_exclude 1 2 3 4 5 6 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30
  // This sideset defines where shape sensitivities will be calculated on the boundary of the model.
  shape_sideset 3
  // This file has the modal results we are trying to match.
  ref_data_file simple-out.exo
  // These nodesets each contain one node and specify where we will be measuring and trying to match the modes.
  match_nodesets 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66
  eigen_solver_shift -5e7
  camp_solver_tol 1e-4
  camp_max_iter 5000
end criterion

begin criterion 2
  type mass_properties
  cgx 3.669 weight 1
  cgy 6.331 weight 1
  cgz 3.0278 weight 1
  ixx 3.0885 weight 1
  ixy -0.97798 weight 1
  ixz -0.32077 weight 1
  iyx -0.97798 weight 1
  iyy 1.8547 weight 1
  iyz -1.0888 weight 1
  izx -0.32077 weight 1
  izy -1.0888 weight 1
  izz 3.1782 weight 1
  mass .04655525 weight 1
end criterion

begin scenario 1
  physics modal_response
  dimensions 3
  material 1
  tolerance 1e-8
  // This tells plato to convert the tet4 mesh coming out of the mesh generator into a tet10 mesh.
  // Tet10s do much better on these modal problems than tet4s.
  // convert_to_tet10 true
end scenario

begin scenario 2
  physics steady_state_mechanics
  dimensions 3
  material 1
  tolerance 1e-7
   loads 1
   boundary_conditions 1
end scenario

begin objective
  type weighted_sum
  criteria 1 2
  services 2 4
  shape_services 3 3
  scenarios 1 2
  weights 1 1
end objective

begin output
   service 2
   data dispx dispy dispz
end output
begin output
   service 4
   data dispx dispy dispz
end output
   
begin block 1
   material 1
   // Since we will be using tet10s we must specify it here.
   // element_type tet10
end block

begin material 1
   material_model isotropic_linear_elastic 
   poissons_ratio .42
   youngs_modulus 2.274e9
   mass_density 2.59e-4
end material

begin optimization_parameters  
   optimization_algorithm rol_bound_constrained
   rol_subproblem_model lin_more
   max_iterations 1000
   output_frequency 1
   normalize_in_aggregator true
   csm_file simple_mock.csm
   num_shape_design_variables 3
   optimization_type shape
   verbose true
   hessian_type zero
   // ks_max_trust_region_iterations 10
   // ks_trust_region_expansion_factor 4
   // ks_trust_region_contraction_factor .5
   // ks_initial_radius_scale .25
   // ks_max_radius_scale .9
   ks_min_trust_region_radius 1e-12
   ks_outer_gradient_tolerance 1e-11
   ks_outer_stationarity_tolerance 1e-11
   ks_outer_stagnation_tolerance 1e-11
   ks_outer_control_stagnation_tolerance 1e-11
   ks_outer_actual_reduction_tolerance 1e-11
   // ks_disable_post_smoothing true
   // ks_trust_region_ratio_low .05
   // ks_trust_region_ratio_mid .2
   // ks_trust_region_ratio_high .45
end optimization_parameters

begin mesh
   name simple_mock.exo
end mesh
begin boundary_condition 1
   type fixed_value
   location_type sideset
   location_name shape_sideset
   degree_of_freedom dispx dispy dispz
   value 0 0 0
end boundary_condition
begin load 1
   type traction
   location_type sideset
   location_name shape_sideset
   value 0 0 0
end load
