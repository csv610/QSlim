include ../mixkit/mix-config

PROGS = propslim
GUI_PROGS = smfview propvis

default: $(PROGS)

all: default $(GUI_PROGS)

propslim: propslim.o ../mixkit/src/libmix.a
	$(CXX) -o propslim propslim.o $(LDFLAGS) -lmix -lm

GUI_LIBS = $(XFORMS_LIBS) $(GL_LIBS) $(X_LIBS)

smfview : smfview.o ../mixkit/src/libmix.a
	$(CXX) -o smfview smfview.o $(LDFLAGS) $(X_LDFLAGS) -lmix $(GUI_LIBS) -lm


propvis : propvis.o ../mixkit/src/libmix.a
	$(CXX) -o propvis propvis.o $(LDFLAGS) $(X_LDFLAGS) -lmix $(GUI_LIBS) -lm

clean:
	-$(CLEAN)
	-/bin/rm -f $(PROGS) $(GUI_PROGS)
	-cat /dev/null > Makefile.dep

-include Makefile.dep
