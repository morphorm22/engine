begin service 1
  code platomain
  number_processors 1
end service

begin service 2
  code plato_analyze
  number_processors 1
end service

begin criterion 1
  type thermomechanical_compliance
  mechanical_weighting_factor 1.0
  thermal_weighting_factor 1e-4
  material_penalty_exponent 3.0
  minimum_ersatz_material_value 1e-8
end criterion

begin criterion 2
  type volume
end criterion

begin scenario 1
  physics steady_state_thermomechanics
  dimensions 3
  loads 1 2
  boundary_conditions 1 2 3 4
  material 1
  // Penalty function in Elliptic...
  material_penalty_exponent 3.0
  minimum_ersatz_material_value 1e-8
  // Linear solver
  linear_solver_tolerance 1e-10
end scenario

begin objective
  type weighted_sum
  criteria 1
  services 2
  scenarios 1
  weights 1.0e1
end objective

begin boundary_condition 1
   type fixed_value
   location_type sideset
   location_name fixed_xy
   degree_of_freedom dispx
   value 0.0
end boundary_condition

begin boundary_condition 2
   type fixed_value
   location_type sideset
   location_name fixed_xy
   degree_of_freedom dispy
   value 0.0
end boundary_condition

begin boundary_condition 3
   type fixed_value
   location_type sideset
   location_name fixed_z
   degree_of_freedom dispz
   value 0.0
end boundary_condition

begin boundary_condition 4
   type time_function
   location_type sideset
   location_name fixed_xy
   degree_of_freedom temp
   value 200
end boundary_condition

begin load 1
    type traction
    location_type sideset
    location_name load
    value 0 -40 0
end load

begin load 2
    type uniform_surface_flux
    location_type sideset
    location_name load
    value 0
end load

begin constraint 1
  criterion 2
  relative_target 0.5
  type less_than
  service 1
end constraint

begin block 1
   material 1
end block

begin material 1
   material_model isotropic_linear_thermoelastic
   youngs_modulus 75.0e3
   poissons_ratio .3
   thermal_expansivity 2.32e-5
   thermal_conductivity 16.0
   reference_temperature 100.0
end material

begin output
   service 2
   disable false
   native_service_output true
end output

begin optimization_parameters
   filter_radius_scale 2.5
   max_iterations 10
   output_frequency 1000
   optimization_algorithm rol_linear_constraint
   discretization density 
   initial_density_value .5
end optimization_parameters

begin mesh
   name cantilever.exo
end mesh

begin paths
   code PlatoMain PlatoMain
   code plato_analyze analyze_MPMD
end paths
