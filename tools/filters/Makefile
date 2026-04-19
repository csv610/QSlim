include ../../mixkit/mix-config

COMMON_SRCS = cmdline.cxx
COMMON_OBJS = $(COMMON_SRCS:.cxx=.o)
ALL_SRCS = $(COMMON_SRCS) smfclean.cxx smfstitch.cxx smfcat.cxx \
           smfmeasure.cxx smfedgelist.cxx \
           smf2ply.cxx smf2ldmesh.cxx smf2c.cxx smfsphere.cxx smfrefine.cxx \
           smfquadric.cxx smfboundary.cxx smferror.cxx
PROGS = smfclean smfstitch smfcat smfmeasure smfedgelist \
        smf2ply smf2ldmesh smf2c smfsphere smfrefine smfquadric \
        smfboundary smferror

all: $(PROGS)

smfclean: smfclean.o $(COMMON_OBJS)
	$(CXX) -o smfclean smfclean.o $(COMMON_OBJS) $(LDFLAGS) -lmix -lm

smfstitch: smfstitch.o $(COMMON_OBJS)
	$(CXX) -o smfstitch smfstitch.o $(COMMON_OBJS) $(LDFLAGS) -lmix -lm

smfcat: smfcat.o $(COMMON_OBJS)
	$(CXX) -o smfcat smfcat.o $(COMMON_OBJS) $(LDFLAGS) -lmix -lm

smfmeasure: smfmeasure.o $(COMMON_OBJS)
	$(CXX) -o smfmeasure smfmeasure.o $(COMMON_OBJS) $(LDFLAGS) -lmix -lm

smfedgelist: smfedgelist.o $(COMMON_OBJS)
	$(CXX) -o smfedgelist smfedgelist.o $(COMMON_OBJS) $(LDFLAGS) -lmix -lm

smf2ply: smf2ply.o $(COMMON_OBJS)
	$(CXX) -o smf2ply smf2ply.o $(COMMON_OBJS) $(LDFLAGS) -lmix -lm

smf2ldmesh: smf2ldmesh.o $(COMMON_OBJS)
	$(CXX) -o smf2ldmesh smf2ldmesh.o $(COMMON_OBJS) $(LDFLAGS) -lmix -lm

smf2c: smf2c.o $(COMMON_OBJS)
	$(CXX) -o smf2c smf2c.o $(COMMON_OBJS) $(LDFLAGS) -lmix -lm

smfsphere: smfsphere.o $(COMMON_OBJS)
	$(CXX) -o smfsphere smfsphere.o $(COMMON_OBJS) $(LDFLAGS) -lmix -lm

smfrefine: smfrefine.o $(COMMON_OBJS)
	$(CXX) -o smfrefine smfrefine.o $(COMMON_OBJS) $(LDFLAGS) -lmix -lm

smfquadric: smfquadric.o $(COMMON_OBJS)
	$(CXX) -o smfquadric smfquadric.o $(COMMON_OBJS) $(LDFLAGS) -lmix -lm

smfboundary: smfboundary.o $(COMMON_OBJS)
	$(CXX) -o smfboundary smfboundary.o $(COMMON_OBJS) $(LDFLAGS) -lmix -lm

smferror: smferror.o $(COMMON_OBJS)
	$(CXX) -o smferror smferror.o $(COMMON_OBJS) $(LDFLAGS) -lmix -lm

strip:
	strip $(PROGS)

clean:
	-$(CLEAN)
	-/bin/rm -f $(PROGS)
	-cat /dev/null > Makefile.dep

depend:
	makedepend -f Makefile.dep -- $(ALL_CXXFLAGS) -- $(ALL_SRCS)

-include Makefile.dep
