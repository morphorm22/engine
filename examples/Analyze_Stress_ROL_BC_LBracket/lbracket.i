#L-Bracket example stress constraint solved using ROL optimizer
#
begin service 1 
   code platomain 
   number_processors 1
end service

begin service 2
   code plato_analyze
   number_processors 1
   update_problem true
end service
   
begin criterion 1
   type stress_and_mass
   scmm_constraint_exponent 2
   stress_limit 3e6
   scmm_penalty_expansion_multiplier 1.5
   scmm_initial_penalty 1
   scmm_penalty_upper_bound 10000
end criterion
 
begin scenario 1
   physics steady_state_mechanics
   dimensions 3
   loads 1
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
   location_type nodeset
   location_name ss_1
   degree_of_freedom dispx dispy dispz
   value 0 0 0
end boundary_condition

begin load 1
   type traction
   location_type sideset
   location_name ss_2
   value 0 -1e6 0
end load
            
begin material 1
   material_model isotropic_linear_elastic
   poissons_ratio .3
   youngs_modulus 1e9
   mass_density 1e3
end material

begin block 1
   material 1
end block

begin optimization_parameters
   max_iterations 1
   filter_radius_absolute 0.05712
   number_buffer_layers 0
   verbose true
   write_restart_file false
   restart_iteration 0
   initial_density_value .5
   normalize_in_aggregator false
   optimization_algorithm rol_bound_constrained
   rol_subproblem_model lin_more
   reset_algorithm_on_update true
   hessian_type zero
   problem_update_frequency 1
   prune_mesh false
   number_refines 0
   output_frequency 500
end optimization_parameters

begin mesh
   name lbracket.exo
end mesh

