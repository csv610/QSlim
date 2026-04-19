/************************************************************************

  MxStdPane

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxStdPane.cxx,v 1.22 1999/01/22 19:45:31 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxStdPane.h"
#include "MxGLUtils.h"

#define MX_SELECT_NONE 0
#define MX_SELECT_FACE 1
#define MX_SELECT_VERT 2

void MxStdPane::bind_asp_vars(MxAspStore *store)
{
    store->defvar("cam.fovy", MXASP_DOUBLE, &camera.fovy);
    store->defvar("cam.aspect", MXASP_DOUBLE, &camera.aspect);
    store->defvar("cam.znear", MXASP_DOUBLE, &camera.znear);
    store->defvar("cam.zfar", MXASP_DOUBLE, &camera.zfar);
    store->defvar("cam.from", MXASP_DOUBLE, (double*)camera.from, 3);
    store->defvar("cam.at", MXASP_DOUBLE, (double *)camera.at, 3);
    store->defvar("cam.up", MXASP_DOUBLE, (double *)camera.up, 3);

    store->defvar("light.pos", MXASP_FLOAT, light_position, 3);

    store->defvar("bounds.min", MXASP_DOUBLE, (double*)bounds.min, 3);
    store->defvar("bounds.max", MXASP_DOUBLE, (double*)bounds.max, 3);
    store->defvar("bounds.ctr", MXASP_DOUBLE, (double*)bounds.center, 3);
    store->defvar("bounds.radius", MXASP_DOUBLE, &bounds.radius, 1);

    store->defvar("manip.vnow", MXASP_DOUBLE, (double*)manip.vNow, 4);
    store->defvar("manip.cam", MXASP_FLOAT, (double*)manip.cam, 3);

    store->defvar("will_draw_surface", MXASP_BOOL, &will_draw_surface);
    store->defvar("will_draw_mesh", MXASP_BOOL, &will_draw_mesh);
    store->defvar("will_draw_points", MXASP_BOOL, &will_draw_points);
    store->defvar("will_draw_texture", MXASP_BOOL, &will_draw_texture);
    store->defvar("will_illustrate", MXASP_BOOL, &will_illustrate);
    store->defvar("will_light_scene", MXASP_BOOL, &will_light_scene);
    store->defvar("ntsc_output", MXASP_BOOL, &ntsc_output);

    extern float mx_mesh_color[3];
    store->defvar("mesh_color", MXASP_FLOAT, mx_mesh_color, 3);
}

void MxStdPane::write_pane_config(ostream& out)
{
    out << "set cam.fovy " << camera.fovy << endl;
    out << "set cam.aspect " << camera.aspect << endl;
    out << "set cam.znear " << camera.znear << endl;
    out << "set cam.zfar " << camera.zfar << endl;

    out << "set cam.from " << camera.from[X] << " "
	<< camera.from[Y] << " " << camera.from[Z] << endl;
    out << "set cam.at " << camera.at[X] << " "
	<< camera.at[Y] << " " << camera.at[Z] << endl;
    out << "set cam.up " << camera.up[X] << " "
	<< camera.up[Y] << " " << camera.up[Z] << endl;

    glGetLightfv(GL_LIGHT0, GL_POSITION, light_position);
    out << "set light.pos" << light_position[X] << " "
	<< light_position[Y] << " " << light_position[Z] << endl;

    out << "set manip.vnow " << manip.vNow[X] << " " << manip.vNow[Y] << " "
	<< manip.vNow[Z] << " " << manip.vNow[W] << endl;
    out << "set manip.cam " << manip.cam[X] << " " << manip.cam[Y] << " "
	<< manip.cam[Z] << endl;
}

void MxStdPane::gl_init()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // camera.apply();
    glMatrixMode(GL_MODELVIEW);

    MxGLPane::gl_init();

    glGetLightfv(GL_LIGHT0, GL_POSITION, light_position);
    CAREFUL(mxgl_check_errors("at end of MxStdPane::gl_init"));

    mxgl_default_texinit(m->texmap(), will_draw_texture);
}

uint MxStdPane::pick_something(int x, int y, double window, int mode)
{
    selection_state = mode;

    MxGLPicker pick(x, y, window);
    begin_redraw();  redraw();  end_redraw();

    selection_state = MX_SELECT_NONE;
    return pick.complete();
}

uint MxStdPane::pick_face(int x, int y)
{
    return pick_something(x, y, 1.0, MX_SELECT_FACE);
}

uint MxStdPane::pick_vertex(int x, int y)
{
    return pick_something(x, y, 16.0, MX_SELECT_VERT);
}

void MxStdPane::move_light(float dx, float dy, float dz)
{
    light_position[0] += dx;
    light_position[1] += dy;
    light_position[2] += dz;
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

void MxStdPane::size(int width, int height)
{
    MxGLPane::size(width, height);
    
    camera.look_at(bounds, (float)width / (float)height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //camera.apply();
}

void MxStdPane::button(int kind, int which, int x, int y)
{
    switch( kind&0xf )
    {
    case MX_BUTTONDOWN:
        if( manip.mouse_down(which, x, y, kind) )
            post_redraw();
        break;

    case MX_BUTTONUP:
        if( manip.mouse_up(which, x, y, kind) )
            post_redraw();
        break;

    case MX_POINTERMOTION:
        if( manip.motion(x, y) )
            post_redraw();
        break;
    }
}

void MxStdPane::key(char c)
{
    switch( c )
    {
    case 'm':
        will_draw_mesh = !will_draw_mesh;
        if( will_draw_mesh )
            glOffsetForMesh();
        else
            glNoOffset();
        break;

    case 'L':
	if( will_light_scene )
	    glDisable(GL_LIGHTING);
	else
	    glEnable(GL_LIGHTING);
	will_light_scene = !will_light_scene;
	break;

    case 'p': will_draw_points = !will_draw_points; break;

    case 'x':  snapshot_ppm(); return;
    case 'X':  snapshot_tiff(); return;
	
    case 's':
        will_draw_surface = !will_draw_surface;
        break;

    case 't':
        will_draw_texture = !will_draw_texture;
	if( m->texmap() )
	{
	    if( will_draw_texture ) glEnable(GL_TEXTURE_2D);
	    else glDisable(GL_TEXTURE_2D);
	}
        break;

    case 'i':  will_illustrate = !will_illustrate; break;

    case 'n':
        if(m->normal_binding()==MX_PERFACE)  m->normal_binding(MX_PERVERTEX);
        else  m->normal_binding(MX_PERFACE);
        m->synthesize_normals();
        break;

    case 'V':
	ntsc_output = !ntsc_output;
	if( ntsc_output )
	{
	    glEnable(GL_LINE_SMOOTH);
	    glLineWidth(1.5);
	}
	else
	{
	    glDisable(GL_LINE_SMOOTH);
	    glLineWidth(1.0);
	}
	break;

    case 'D': write_pane_config(cerr); return;

    case 'q':
	quit();
	break;

    case MXKEY_DEL: case MXKEY_BS: if(typein.length()) typein.drop(); break;
    case '@':  typein.reset(); break;

    default:
	if( c>='0' && c<='9' )
	    typein.add(c);
	else
	    return;

	break;
    }

    post_redraw();
}

void MxStdPane::begin_redraw()
{
    MxGLPane::begin_redraw();

    if( will_draw_mesh ) glOffsetForMesh(); else glNoOffset();
    if( will_light_scene ) glEnable(GL_LIGHTING); else glDisable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    // Conserve stack space by avoiding glPushMatrix
    glGetDoublev(GL_PROJECTION_MATRIX, __proj_save);
    camera.apply();

    glMatrixMode(GL_MODELVIEW);
    manip.apply();

    // Use a pure white surface color when we're texturing
    if( will_draw_texture )
	glC(1.0, 1.0, 1.0);
    else
	// glC(0.5, 0.5, 0.5);
	glC(0.569, 0.447, 0.315);

    CAREFUL(mxgl_check_errors("at end of MxStdPane::begin_redraw"));
}

void MxStdPane::end_redraw()
{
    manip.unapply();
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(__proj_save);	// Explicit save to save stack space
    glMatrixMode(GL_MODELVIEW);
    MxGLPane::end_redraw();
    CAREFUL(mxgl_check_errors("at end of MxStdPane::end_redraw"));
}

void MxStdPane::redraw()
{
    if( !selection_state )
    {
	if( will_draw_surface )
	{
	    mx_render_model(*m);
	    if( will_draw_mesh )
		mx_draw_mesh(*m);
	}
	else if( will_draw_mesh )
	    mx_draw_wireframe(*m);

	if( will_draw_points )
	    mx_draw_pointcloud(*m);
    }
    else if( selection_state == MX_SELECT_FACE )
	mx_render_model(*m);
    else if( selection_state == MX_SELECT_VERT )
	mx_draw_pointcloud(*m);
}

void MxStdPane::illustrate()
{
    if( !will_illustrate )  return;

    char buf[128];
    int w, h;
    get_canvas_geometry(NULL, NULL, &w, &h);

    if( typein.length() )
    {
	buf[0] = '>';
	buf[1] = ' ';
	strncpy(buf+2, typein, typein.length());
	buf[typein.length() + 2] = '\0';

	draw_string(10, h-30, buf);
    }
}

#ifdef WIN32

void MxStdPane::quit()
{
    DestroyWindow();
}

#else

void MxStdPane::quit()
{
    exit(0);
}

#endif
