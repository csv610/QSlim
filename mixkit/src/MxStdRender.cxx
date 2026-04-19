/************************************************************************

  Since we don't want to have to rewrite rendering loops every time we
  want to draw a model, this module provides centralized drawing code.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxStdRender.cxx,v 1.13 1998/10/26 21:09:30 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxGL.h"
#include "MxStdModel.h"

////////////////////////////////////////////////////////////////////////
//
// Globally visible variables.  These exist for quasi-easy
// customization.  Eventually, we'll have a more flexible system.
//

float mx_mesh_color[3] = {0.05f, 0.05f, 0.05f};
float mx_alt_mesh_color[3] = {0.8f, 0.1f, 0.1f};



////////////////////////////////////////////////////////////////////////
//
// Currently, we create custom routines for each possible combination
// of normal and color bindings. This approach might eventually lead
// to excessive code bloat.  Be careful!
//
// Note that the whole point of this is to avoid conditional branching
// in the rendering loop.  Currently, we have 2: one for the loop
// condition and one to test face validity.  This is minimal.
//

#define _0(x) 
#define _V(x) glV(m.vertex(x))
#define _N(x) glN(m.normal(x))
#define _C(x) glC(m.color(x))

#define _CV(x) _C(x),_V(x)
#define _NV(x) _N(x),_V(x)
#define _NC(x) _N(x),_C(x)
#define _NCV(x) _N(x),_C(x),_V(x)


#define defdraw(fn, per_face, per_vert) \
static void fn(MxStdModel& m) \
{ \
    glBegin(GL_TRIANGLES); \
    for(MxFaceID f=0; f<m.face_count(); f++) \
    { \
	if( !m.face_is_valid(f) ) continue; \
	MxVertexID v1=m.face(f)(0), v2=m.face(f)(1), v3=m.face(f)(2); \
	per_face(f); \
	per_vert(v1); \
	per_vert(v2); \
	per_vert(v3); \
    } \
    glEnd(); \
}

defdraw(draw_c0n0, _0 , _V)
defdraw(draw_c0n1, _N , _V)
defdraw(draw_c0n2, _0 , _NV)

defdraw(draw_c1n0, _C , _V)
defdraw(draw_c1n1, _NC, _V)
defdraw(draw_c1n2, _C , _NV)

defdraw(draw_c2n0, _0 , _CV)
defdraw(draw_c2n1, _N , _CV)
defdraw(draw_c2n2, _0 , _NCV)


static inline void emit_face(MxStdModel& m, MxFaceID f)
{
    if( m.normal_binding()==MX_PERFACE )  glN(m.normal(f));
    if( m.color_binding()==MX_PERFACE ) glC(m.color(f));
}

static inline void emit_vertex(MxStdModel& m, MxVertexID v)
{
    if( m.normal_binding()==MX_PERVERTEX ) glN(m.normal(v));
    if( m.color_binding()==MX_PERVERTEX ) glC(m.color(v));
    if( m.texcoord_binding()==MX_PERVERTEX) glT(m.texcoord(v));
    glV(m.vertex(v));
}

static
void draw_fallback(MxStdModel& m)
{
    glBegin(GL_TRIANGLES);
    for(MxFaceID f=0; f<m.face_count(); f++)
    {
	if( !m.face_is_valid(f) ) continue;
	MxVertexID v1=m.face(f)(0), v2=m.face(f)(1), v3=m.face(f)(2);
	emit_face(m,f);
	emit_vertex(m,v1);
	emit_vertex(m,v2);
	emit_vertex(m,v3);
    }
    glEnd();
}

typedef void (*render_func)(MxStdModel&);

#define TABLE_SIZE 12

static render_func render_table[12] = {
    draw_c0n0, draw_c0n1, draw_c0n2, NULL,

    draw_c1n0, draw_c1n1, draw_c1n2, NULL,

    draw_c2n0, draw_c2n1, draw_c2n2, NULL,
};


static
void render_faces_for_selection(MxStdModel& m)
{
    for(MxFaceID f=0; f<m.face_count(); f++)
    {
	if( !m.face_is_valid(f) ) continue;
	MxVertexID v1=m.face(f)(0), v2=m.face(f)(1), v3=m.face(f)(2);
	glLoadName(f);
	glBegin(GL_TRIANGLES);
	emit_face(m, f); // emit normal in case backface culling is on
	glV(m.vertex(v1));
	glV(m.vertex(v2));
	glV(m.vertex(v3));
	glEnd();
    }
}

static
void render_points_for_selection(MxStdModel& m)
{
    for(MxVertexID v=0; v<m.vert_count(); v++)
    {
	if( !m.vertex_is_valid(v) )  continue;
	glLoadName(v);
	glBegin(GL_POINTS);
	glV(m.vertex(v));
	glEnd();
    }
}

static inline int get_mode()
{
    int mode; glGetIntegerv(GL_RENDER_MODE, &mode); return mode;
}

void mx_render_model(MxStdModel& m)
{

    if( get_mode() == GL_SELECT ) { render_faces_for_selection(m);  return; }

    uint index = (m.texcoord_binding() << 4) |
	         (m.color_binding() << 2) | m.normal_binding();

    if( index >= TABLE_SIZE )
	draw_fallback(m);
    else
    {
	render_func fn = render_table[index];
	if( fn ) (*fn)(m);
	else fatal_error("mx_render_model: Illegal model properties.");
    }
}

static
inline void emit_faces_only(MxStdModel& m)
{
    glBegin(GL_TRIANGLES);
    for(MxFaceID f=0; f<m.face_count(); f++)
    {
	if( !m.face_is_valid(f) ) continue;
	glV(m.corner(f, 0));
	glV(m.corner(f, 1));
	glV(m.corner(f, 2));
    }
    glEnd();
}

static
inline void emit_points_only(MxStdModel& m)
{
    glBegin(GL_POINTS);
    for(MxVertexID v=0; v<m.vert_count(); v++)
    {
	if( !m.vertex_is_valid(v) )  continue;
	glV(m.vertex(v));
    }
    glEnd();
}

void mx_draw_mesh(MxStdModel& m, double *color)
{
    if( get_mode() == GL_SELECT ) return;


    glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    if( color ) glColor3dv(color); else glColor3fv(mx_mesh_color);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    emit_faces_only(m);

    glPopAttrib();
}

void mx_draw_wireframe(MxStdModel& m, double *color)
{
    GLfloat bkg_color[4];
    glGetFloatv(GL_COLOR_CLEAR_VALUE, bkg_color);

    glPushAttrib(GL_ENABLE_BIT | GL_POLYGON_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glNoOffset();
    if( color ) glColor3dv(color); else glColor3fv(mx_mesh_color);
    emit_faces_only(m);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glOffsetForMesh();
    glC(bkg_color[0], bkg_color[1], bkg_color[2]);
    emit_faces_only(m);

    glPopAttrib();
}

void mx_draw_pointcloud(MxStdModel& m)
{
    if( get_mode() == GL_SELECT ) { render_points_for_selection(m);  return; }

    glPushAttrib(GL_ENABLE_BIT | GL_POINT_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glPointSize(2.0);

    emit_points_only(m);

    glPopAttrib();
}

void mx_draw_boundaries(MxStdModel& m)
{
    glPushAttrib(GL_LIGHTING_BIT);
    glDisable(GL_LIGHTING);
    glC(0.8, 0.8, 0.1);

    glBegin(GL_LINES);

    MxVertexList star;
    MxFaceList faces;
    for(MxVertexID i=0; i<m.vert_count(); i++)
    {
	star.reset();
	m.collect_vertex_star(i, star);

	for(uint j=0; j<star.length(); j++)
	    if( i < star(j) )
	    {
		faces.reset();
		m.collect_edge_neighbors(i, star(j), faces);
		if( faces.length() == 1 )
		{ glV(m.vertex(i)); glV(m.vertex(star(j))); }
	    }
    }

    glEnd();

    glPopAttrib();
}
