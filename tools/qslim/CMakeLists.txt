add_executable(qslim cmdline.cxx main.cxx output.cxx qslim.cxx)
target_link_libraries(qslim mix)

if(USE_OPENGL AND USE_XFORMS)
    add_executable(qvis cmdline.cxx main.cxx output.cxx qvis.cxx)
    target_link_libraries(qvis mix)
endif()
