set(CORE_SRCS mixmsg.cxx MxTimer.cxx MxCmdParser.cxx MxAsp.cxx)

set(MATH_SRCS MxMat2.cxx MxMat3.cxx MxMat4.cxx
            MxMat3-jacobi.cxx MxMat4-jacobi.cxx
            MxGeom3D.cxx MxTriProject.cxx mixmops.cxx)

set(MODEL_SRCS MxBlockModel.cxx MxStdModel.cxx MxSMF.cxx MxQMetric3.cxx
             MxQSlim.cxx MxStdSlim.cxx MxQMetric.cxx MxPropSlim.cxx
             MxEdgeFilter.cxx MxQMetric2.cxx MxLineModel.cxx
             MxOBJ.cxx MxOFF.cxx MxSTL.cxx)

set(DATA_SRCS MxHeap.cxx MxRaster.cxx)

if(USE_LIBTIFF)
    list(APPEND DATA_SRCS MxRaster-tiff.cxx)
endif()

set(SRCS ${CORE_SRCS} ${MATH_SRCS} ${MODEL_SRCS} ${DATA_SRCS})

if(USE_OPENGL)
    set(GL_SRCS MxArcball.cxx MxCamera.cxx MxQVis3.cxx
              MxStdRender.cxx MxGLDebug.cxx MxGLUtils.cxx)
    list(APPEND SRCS ${GL_SRCS})
endif()

if(USE_XFORMS)
    set(GUI_SRCS MxGLPane.cxx MxStdPane.cxx)
    list(APPEND SRCS ${GUI_SRCS})
endif()

add_library(mix STATIC ${SRCS})

if(USE_OPENGL)
    target_link_libraries(mix PUBLIC OpenGL::GL OpenGL::GLU X11::X11)
endif()

if(USE_XFORMS)
    target_include_directories(mix PRIVATE ${XFORMS_INCLUDE_DIR})
    target_link_libraries(mix PUBLIC ${XFORMS_LIBRARY})
endif()
