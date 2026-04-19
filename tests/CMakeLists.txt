cmake_minimum_required(VERSION 3.10)

# Math tests
add_executable(test_math test_math.cxx)
target_link_libraries(test_math mix)
add_test(NAME math_test COMMAND test_math)

# Data structure tests
add_executable(test_data test_data.cxx)
target_link_libraries(test_data mix)
add_test(NAME data_test COMMAND test_data)

# Removal order test
add_executable(test_removal_order test_removal_order.cxx)
target_link_libraries(test_removal_order mix)
add_test(NAME removal_order_test COMMAND test_removal_order)

# Protected vertices test
add_executable(test_protected_vertices test_protected_vertices.cxx)
target_link_libraries(test_protected_vertices mix)
add_test(NAME protected_vertices_test COMMAND test_protected_vertices)

# Integration test
find_package(Python3 REQUIRED)
add_test(NAME qslim_integration_test 
         COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_qslim_cli.py 
         --qslim $<TARGET_FILE:qslim>)

add_test(NAME qslim_primitives_test 
         COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_primitives.py 
         --qslim $<TARGET_FILE:qslim>)
