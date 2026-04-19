set(COMMON_SRCS cmdline.cxx)

set(PROGS smfclean smfstitch smfcat smfmeasure smfedgelist
        smf2ply smf2ldmesh smf2c smfsphere smfrefine smfquadric
        smfboundary smferror)

foreach(PROG ${PROGS})
    add_executable(${PROG} ${PROG}.cxx ${COMMON_SRCS})
    target_link_libraries(${PROG} mix)
endforeach()
