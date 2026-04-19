/************************************************************************

  Interactive QSlim program.  This version of QSlim actually opens up
  a simple window pane and let's you interact with the model.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: qvis.cxx,v 1.17 1998/10/28 19:13:48 garland Exp $

 ************************************************************************/

#include <stdmix.h>
#include <mixio.h>
#include <MxStdPane.h>
#include <MxRaster.h>
#include <MxVector.h>
#include "qslim.h"

float surface_color[3] = {0.569f, 0.447f, 0.315f};  //{0.668, 0.605, 0.528};
float background_color[3] = {0.3f, 0.3f, 1.0f};

class Pane : public MxStdPane
{
public:
    bool will_draw_quadrics;
    bool will_draw_edges;
    double quadric_radius;

    uint selected_vertex;
    
    Pane(MxStdModel *M) : MxStdPane(M)
        {
            will_draw_quadrics = false;
            will_draw_edges = false;
            quadric_radius = 0.75;

	    selected_vertex = MXID_NIL;
        }

    void key(char);
    void button(int kind, int which, int x, int y);
    void begin_redraw();
    void redraw();
    void illustrate();
    void draw_quadric(uint v);

    void adapt_to_target(uint);
    uint compute_default_target();
};

static void colorize(MxStdModel& m)
{
    uint limit=0;

    switch( m.color_binding() )
    {
    case MX_PERFACE:
        m.color_binding(MX_PERVERTEX); limit=m.vert_count(); break;
    case MX_UNBOUND:
        m.color_binding(MX_PERFACE); limit=m.face_count(); break;
    default:
        m.color_binding(MX_UNBOUND); return;
    }

    for(uint i=0; i<limit; i++)
        m.add_color((float)random1(), (float)random1(), (float)random1());
}

uint Pane::compute_default_target()
{
    uint target = 1000;

    if( slim->valid_faces <= 300 ) target = 100;
    else if( slim->valid_faces <= 1000 ) target = 300;

    return target;
}

void Pane::adapt_to_target(uint target)
{
    if( target > m->face_count() )  target=m->face_count();

    if( target < slim->valid_faces )
	slim->decimate(target);
    else if( target > slim->valid_faces && history )
    {
	while( slim->valid_faces < target && history->length() > 0 )
	{
	    MxPairContraction& conx = history->drop();
	    ((MxEdgeQSlim *)slim)->apply_expansion(conx);
	}
    }
}

void Pane::button(int kind, int which, int x, int y)
{
    if( kind&MX_CTRLKEY && (kind&0xf)==MX_BUTTONDOWN && which==1 )
    {
        selected_vertex = pick_vertex(x,y);
        post_redraw();
    }
    else
        MxStdPane::button(kind, which, x, y);
}


void Pane::key(char c)
{
    switch( c )
    {
    case 'q':
	slim_cleanup();
        delete this;
	exit(0);
        break;

    case 'b':  adapt_to_target(slim->valid_faces + 100); break;
    case 'g':  adapt_to_target(slim->valid_faces - 100); break;
    case 'h':  adapt_to_target(slim->valid_faces - 1); break;
    case 'B':  adapt_to_target(m->face_count()); break;

    case MXKEY_NL:
    case MXKEY_CR:
	if( typein.length() > 0 )
	{
	    typein.add('\0');
	    adapt_to_target(atoi(typein));
	    typein.reset();
	}
        break;

    case 't':  will_draw_edges = !will_draw_edges; break;
    case 'e':  will_draw_quadrics = !will_draw_quadrics; break;
    case 'c':  colorize(*m); break;
    case '<':  quadric_radius /= 2; break;
    case '>':  quadric_radius *= 2; break;

    default:  MxStdPane::key(c); return;
    }

    post_redraw();
}


void Pane::illustrate()
{
    MxStdPane::illustrate();

    if( !will_illustrate ) return;
    char buf[128];
    sprintf(buf, "%uv : %uf",
	    slim?slim->valid_verts:m->vert_count(),
	    slim?slim->valid_faces:m->face_count());
    draw_string(10, 10, buf);
}

void Pane::draw_quadric(uint v)
{
    mx_draw_quadric(((MxQSlim*)slim)->vertex_quadric(v),
		    quadric_radius, m->vertex(v));
}

void Pane::begin_redraw()
{
    glClearColor(::background_color[0], ::background_color[1],
		 ::background_color[2], 0.0f);
    MxStdPane::begin_redraw();
}

void Pane::redraw()
{
    if( will_draw_edges && eslim )
	glOffsetForMesh();

    glColor3fv(surface_color);
    MxStdPane::redraw();

    if( will_draw_edges && eslim )
    {
        glPushAttrib(GL_LIGHTING_BIT);
        glDisable(GL_LIGHTING);
        glC(0.0, 1.0, 0.0);

        glBegin(GL_LINES);
        for(uint i=0; i<eslim->edge_count(); i++)
        {
	    const MxQSlimEdge *e = eslim->edge(i);
	    glV(m->vertex(e->v1));
	    glV(m->vertex(e->v2));
        }
        glEnd();

        glPopAttrib();
    }

    if( will_draw_quadrics )
    {
        glPushAttrib(GL_LIGHTING_BIT);
        glDisable(GL_COLOR_MATERIAL);
        mx_quadric_shading(MX_GREEN_ELLIPSOIDS);
        for(VID v=0; v<m->vert_count(); v++)
            if( m->vertex_is_valid(v) )
		draw_quadric(v);

        glPopAttrib();
    }
    else if( selected_vertex != MXID_NIL &&
	     m->vertex_is_valid(selected_vertex) )
    {
        glPushAttrib(GL_LIGHTING_BIT);
        glDisable(GL_COLOR_MATERIAL);
        mx_quadric_shading(MX_GREEN_ELLIPSOIDS);
	draw_quadric(selected_vertex);
        glPopAttrib();
    }
}

bool init_application(MxGLApp *app)
{
    smf = new MxSMFReader;

    smf->asp_store()->defvar("surface_color", MXASP_FLOAT, surface_color, 3);
    smf->asp_store()->defvar("background_color", MXASP_FLOAT,
			     background_color, 3);

    process_cmdline(app->argc, app->argv);

    // No files were specified on the command line,
    // present the user with a file selector.
    //
    if( m->face_count() == 0 )
    {
	const char *filename;
	filename = app->query_filename("Select file to view:",
				       ".", "*.smf", "");

	if( !filename ) return false;

	smf->unparsed_hook = unparsed_hook;
	input_file(filename);
    }

    if( m->normal_binding() == MX_UNBOUND )
    {
        m->normal_binding(MX_PERFACE);
        m->synthesize_normals();
    }

    slim_init();

    Pane *pane = new Pane(m);
    app->bind_pane(pane, "QVis");


    pane->bind_asp_vars(smf->asp_store());

    include_deferred_files();

    return true;
}

MxGLApp app(init_application);
