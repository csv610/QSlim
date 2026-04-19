#
# Makefile for building the MixKit library
# $Id: Makefile,v 1.40.2.1 1999/03/17 19:43:52 garland Exp $
#

include ../mix-config

CORE_SRCS = mixmsg.cxx MxTimer.cxx MxCmdParser.cxx MxAsp.cxx

MATH_SRCS = MxMat2.cxx MxMat3.cxx MxMat4.cxx \
            MxMat3-jacobi.cxx MxMat4-jacobi.cxx \
	    MxGeom3D.cxx MxTriProject.cxx mixmops.cxx

MODEL_SRCS = MxBlockModel.cxx MxStdModel.cxx MxSMF.cxx MxQMetric3.cxx \
             MxQSlim.cxx MxStdSlim.cxx MxQMetric.cxx MxPropSlim.cxx \
             MxEdgeFilter.cxx MxQMetric2.cxx MxLineModel.cxx

DATA_SRCS = MxHeap.cxx MxRaster.cxx MxRaster-tiff.cxx

# These modules require OpenGL or Mesa
GL_SRCS = MxArcball.cxx MxCamera.cxx MxQVis3.cxx \
          MxStdRender.cxx MxGLDebug.cxx MxGLUtils.cxx

# These modules require XForms under X11 or MFC under Win32
GUI_SRCS = MxGLPane.cxx MxStdPane.cxx 

SRCS = $(CORE_SRCS) $(MATH_SRCS) $(MODEL_SRCS) $(DATA_SRCS) \
       $(GL_SRCS) $(GUI_SRCS)

OBJS = $(SRCS:.cxx=.o)

########################################################################

libmix.a: $(OBJS)
	$(AR) cru libmix.a $(OBJS)
	$(RANLIB) libmix.a

clean:
	-$(CLEAN)
	-/bin/rm -f libmix.a
	-cat /dev/null > Makefile.dep

depend:
	makedepend -f Makefile.dep -- $(ALL_CXXFLAGS) -- $(SRCS)

-include Makefile.dep
#sinclude Makefile.dep     # Alternative syntax for some makes
