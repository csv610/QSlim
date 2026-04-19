/************************************************************************

  MxGLUtils

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxGLUtils.cxx,v 1.3 1999/01/07 21:27:29 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxGLUtils.h"


void mxgl_default_texinit(MxRaster *tex, bool will_draw_texture)
{
    if( tex )
    {
	if( will_draw_texture ) glEnable(GL_TEXTURE_2D);
        // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//         glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
//         glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage(tex);
	// gluMipmaps(tex);
    }
}

void mxgl_default_init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    GLfloat mat_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat mat_diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat mat_specular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 40);

    GLfloat light_pos[] = {0.0f, 0.0f, 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glEnable(GL_LIGHT0);

    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glColor3f(0.5f, 0.5f, 0.5f);
    glClearColor(0.3f, 0.3f, 1.0f, 0.0f);

    CAREFUL(mxgl_check_errors("at end of mxgl_default_init()"));
}


GLenum mxgl_matrix_for_mode(GLenum mode)
{
    switch( mode )
    {
    case GL_MODELVIEW:  return GL_MODELVIEW_MATRIX;
    case GL_PROJECTION: return GL_PROJECTION_MATRIX;
    case GL_TEXTURE:    return GL_TEXTURE_MATRIX;
    }

    return GL_FALSE;
}

#define pick_zmax MXID_NIL


MxGLPicker::MxGLPicker(int x, int y, double window)
    : buffer(128)
{
    GLint vp[4];

    glMatrixMode(GL_PROJECTION);
    glGetIntegerv(GL_VIEWPORT, vp);
    glSelectBuffer(128, buffer);
    glRenderMode(GL_SELECT);

    gluPickMatrix(x, vp[3] - y, window, window, vp);
    glInitNames();
    glPushName(MXID_NIL);
}

uint MxGLPicker::complete()
{
    glFlush();

    GLint nhits = glRenderMode(GL_RENDER);
    GLuint hit = MXID_NIL;
    GLuint zmin = pick_zmax;
    GLuint *ptr = buffer;

    for(uint i=0; i<nhits; i++)
    {
	GLuint nnames = *ptr++;
	assert( nnames==1 ); // assume name stack is always 1 deep

	GLuint cur_zmin = *ptr++;
	GLuint cur_zmax = *ptr++;

	if( cur_zmin < zmin )
	{
	    zmin = cur_zmin;
	    hit = *ptr;
	}
	ptr++;
    }

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();	// get rid of the pick matrix

    return hit;
}
