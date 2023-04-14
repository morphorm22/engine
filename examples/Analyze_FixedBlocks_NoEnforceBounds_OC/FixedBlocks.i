begin service 1
   code platomain
   number_processors 1
end service

begin service 2
   code plato_analyze
   number_processors 1
   device_ids 0
end service
   
begin criterion 1
   type mechanical_compliance 
end criterion
 
begin criterion 2
   type volume 
end criterion
      
begin scenario 1
   physics steady_state_mechanics
   dimensions 3
   loads 1
   boundary_conditions 1
   material 1
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
   location_name ns1
   degree_of_freedom dispx dispy dispz
   value 0 0 0
end boundary_condition

begin load 1
   type traction
   location_type sideset
   value 0 0 -1e5
   location_name ss1
end load
         
begin constraint 1
   criterion 2
   relative_target .15
   service 1
   scenario 1
end constraint
   
begin material 1
   material_model isotropic_linear_elastic
   poissons_ratio .33
   youngs_modulus 1e9
end material

begin block 1
   material 1
   element_type tet4
end block

begin block 2
  material 1
  element_type tet4
end block 2

begin optimization_parameters
   max_iterations 10
   filter_radius_scale 3
   verbose true
   write_restart_file true
   number_buffer_layers 0
   fixed_block_ids 2
   optimization_algorithm oc
   enforce_bounds false
   output_frequency 0
end optimization_parameters

begin mesh
  name FixedBlocks.exo
end mesh

