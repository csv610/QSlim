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

add_library(mix STATIC ${SRCS})
