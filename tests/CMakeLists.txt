cmake_minimum_required(VERSION 3.10)

# Math tests
add_executable(test_math test_math.cxx)
target_link_libraries(test_math mix)
add_test(NAME math_test COMMAND test_math)

# Data structure tests
add_executable(test_data test_data.cxx)
target_link_libraries(test_data mix)
add_test(NAME data_test COMMAND test_data)

# Integration test
find_package(Python3 REQUIRED)
add_test(NAME qslim_integration_test 
         COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/test_qslim_cli.py 
         --qslim $<TARGET_FILE:qslim>)
