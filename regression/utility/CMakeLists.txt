set( utilityFolder "regression/utility/")
add_custom_target(utility-python-files ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/${utilityFolder} ${CMAKE_BINARY_DIR}/${utilityFolder}
    DEPENDS ${MY_TARGET})
