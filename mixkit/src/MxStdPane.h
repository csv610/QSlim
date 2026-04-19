#ifndef MXSTDPANE_INCLUDED // -*- C++ -*-
#define MXSTDPANE_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxStdPane

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxStdPane.h,v 1.13 1998/10/26 21:09:29 garland Exp $

 ************************************************************************/

#include "MxGLPane.h"
#include "MxStdModel.h"
#include "MxGeom3D.h"
#include "MxArcball.h"
#include "MxCamera.h"
#include "MxAsp.h"

class MxStdPane : public MxGLPane
{
private:
    GLdouble __proj_save[16];
    int selection_state;

protected:
    MxStdModel *m;
    MxArcball manip;
    MxBounds bounds;
    MxCamera camera;
    float light_position[4];

    MxDynBlock<char> typein;

    uint pick_something(int x, int y, double window, int mode);

public:
    bool will_draw_surface;
    bool will_draw_mesh;
    bool will_draw_points;
    bool will_draw_texture;
    bool will_illustrate;
    bool will_light_scene;
    bool ntsc_output;

    MxStdPane(MxStdModel *M)
	: typein(64)
	{
	    m = M;
	    will_draw_surface = true;
	    will_draw_mesh = false;
	    will_draw_points = false;
	    will_draw_texture = (m->texcoord_binding() != MX_UNBOUND);
	    will_illustrate = true;
	    will_light_scene = true;
	    ntsc_output = false;
	    selection_state = 0;

	    for(MxVertexID v=0; v<m->vert_count(); v++)
		bounds.add_point(m->vertex(v));
	    bounds.complete();

	    float aspect = 640.0f / 480.0f;  // guess at aspect ratio
	    camera.look_at(bounds, aspect);
	    manip.init(bounds, aspect);
	}


    void write_pane_config(ostream&);
    void bind_asp_vars(MxAspStore *);

    uint pick_face(int x, int y);
    uint pick_vertex(int x, int y);
    void move_light(float dx, float dy, float dz);

    void gl_init();
    void button(int kind, int which, int x, int y);
    void key(char);
    void redraw();
    void begin_redraw();
    void end_redraw();
    void illustrate();
    void size(int width, int height);

    void quit();
};

// MXSTDPANE_INCLUDED
#endif
