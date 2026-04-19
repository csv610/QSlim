include ../../mixkit/mix-config

SRCS = cmdline.cxx main.cxx output.cxx qslim.cxx
QVIS_SRCS = cmdline.cxx main.cxx output.cxx qvis.cxx
OBJS = $(SRCS:.cxx=.o)
QVIS_OBJS = $(QVIS_SRCS:.cxx=.o)


GUI_LIBS = $(XFORMS_LIBS) $(GL_LIBS) $(X_LIBS) $(TIFF_LIBS)

qslim : $(OBJS) ../../mixkit/src/libmix.a
	$(CXX) -o qslim $(OBJS) $(LDFLAGS) -lmix -lm

qvis : $(QVIS_OBJS) ../../mixkit/src/libmix.a
	$(CXX) -o qvis $(QVIS_OBJS) $(LDFLAGS) $(X_LDFLAGS) -lmix $(GUI_LIBS) -lm

all: qslim qvis

clean:
	-$(CLEAN)
	-/bin/rm -f qslim qvis qvis-2d
	-cat /dev/null > Makefile.dep


depend:
	makedepend -f Makefile.dep -- $(ALL_CXXFLAGS) -- $(SRCS) $(QVIS_SRCS)

-include Makefile.dep
