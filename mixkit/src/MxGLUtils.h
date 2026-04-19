#ifndef MXGLUTILS_INCLUDED // -*- C++ -*-
#define MXGLUTILS_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxGLUtils

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxGLUtils.h,v 1.2 1998/10/26 21:08:58 garland Exp $

 ************************************************************************/

#include "MxBlock.h"
#include "MxGL.h"
#include "MxRaster.h"


extern void mxgl_default_texinit(MxRaster *tex, bool will_draw_texture=true);
extern void mxgl_default_init();
extern GLenum mxgl_matrix_for_mode(GLenum mode);

extern void mxgl_pick_begin(int x, int y, double window);
extern uint mxgl_pick_complete();


class MxGLPicker
{
private:
    MxBlock<GLuint> buffer;

public:
    MxGLPicker(int x, int y, double window);
    uint complete();
};


//
// The OpenGL projection matrix stack is often very small.  For instance,
// it's only 2 levels deep on an Impact.  So we often wind up having to
// manage that stack manually.  And that's exactly what this little
// class is for.
//
class MxGLMatrixCache
{
private:
    GLdouble saved_mat[16];
    GLenum mode, saved_mode;

    void save_mode() { glGetIntegerv(GL_MATRIX_MODE, (GLint *)&saved_mode); }
    void restore_mode() { glMatrixMode(saved_mode); }

public:
    MxGLMatrixCache(GLenum m=GL_PROJECTION) { mode=m; }

    void save_matrix()
	{ glGetDoublev(mxgl_matrix_for_mode(mode), saved_mat); }

    void restore_matrix()
	{
	    save_mode();
	    glMatrixMode(mode);  glLoadMatrixd(saved_mat);
	    restore_mode();
	}
};




// MXGLUTILS_INCLUDED
#endif
