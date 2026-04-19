add_executable(propslim propslim.cxx)
target_link_libraries(propslim mix)

if(USE_OPENGL AND USE_XFORMS)
    add_executable(smfview smfview.cxx)
    target_link_libraries(smfview mix)

    add_executable(propvis propvis.cxx)
    target_link_libraries(propvis mix)
endif()
