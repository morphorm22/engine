begin service 1
   code platomain
   number_processors 1
end service

begin service 2
   code plato_analyze
   number_processors 1
   update_problem true
end service

begin service 3
  code plato_esp
  number_processors 5
end service

begin criterion 1
   type volume 
end criterion

begin scenario 1
   physics steady_state_mechanics
   dimensions 3
#   loads 1
   boundary_conditions 1
   material 1
   linear_solver_tolerance 1e-6
end scenario   

begin objective
   type weighted_sum
   criteria 1 
   services 2 
   scenarios 1 
   weights 1 
end objective

begin output
   service 2
   data dispx dispy dispz vonmises
end output

begin boundary_condition 1
    type fixed_value
    location_type sideset
    location_name center_axis
    degree_of_freedom dispx dispz dispy
    value 0 0 0
end boundary_condition


begin load 1
    type traction
    location_type sideset
    location_name left_axis
    value 0 0 0
end load


begin block 1
   material 1
end block

begin optimization_parameters
   optimization_type shape
   optimization_algorithm rol_linear_constraint

   check_gradient true
   rol_gradient_check_perturbation_scale 0.01
   rol_gradient_check_steps 12
   problem_update_frequency 1

   csm_file rocker.csm
   num_shape_design_variables 5
   verbose true

end optimization_parameters

begin mesh
   name rocker.exo
end mesh

