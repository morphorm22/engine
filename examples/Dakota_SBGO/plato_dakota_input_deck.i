# W93 Design A

environment
  tabular_data
    tabular_data_file = 'surrogate_based_global_optim_dakota.dat'
  top_method_pointer = 'SBGO'

method
  id_method = 'SBGO'
  surrogate_based_global
    model_pointer = 'SURROGATE'
    method_pointer = 'MOGA'
    max_iterations = 10
    replace_points
  output quiet

method
  id_method = 'MOGA'
  moga
    seed = 10983
    population_size = 300
    max_function_evaluations = 20000
    initialization_type unique_random
    crossover_type shuffle_random
        num_offspring = 2 num_parents = 2
        crossover_rate = 0.8
    mutation_type replace_uniform
        mutation_rate = 0.1
    fitness_type domination_count
    replacement_type elitist
    niching_type distance 0.02
    postprocessor_type
        orthogonal_distance 0.01
    convergence_type metric_tracker
        percent_change = 0.01 num_generations = 10
  output normal
  scaling

model
    id_model = 'SURROGATE'
    surrogate global
        dace_method_pointer = 'SAMPLING'
        gaussian_process surfpack
        # optimization_method 'local'
        # correction additive zeroth_order
        # find_nugget = 2
        # export_model
        # filename_prefix = 'my_surrogate'
        # formats binary_archive

method
    id_method = 'SAMPLING'
    sampling
      samples = 12
      seed = 777
      sample_type lhs
      model_pointer = 'TRUTH'

model
    id_model = 'TRUTH'
    single
      interface_pointer = 'TRUE_FN'

variables
  continuous_design = 2
    initial_point 2.0   2.0
    lower_bounds  2.0   1.6
    upper_bounds  3.0   2.4
    descriptors   'Py'  'Px'

interface
    id_interface = 'TRUE_FN'
    analysis_drivers = 'plato_dakota_plugin'
    direct
    asynchronous evaluation_concurrency 3

responses
    descriptors 'Compliance' 'Volume'
    objective_functions = 2
    primary_scales = 1.0 1.0
    no_gradients
    no_hessians

