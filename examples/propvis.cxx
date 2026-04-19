/************************************************************************

  PropVis

  $Id: propvis.cxx,v 1.2 1998/10/19 15:21:23 garland Exp $

 ************************************************************************/

#include <stdmix.h>
#include <mixio.h>

#include <MxSMF.h>
#include <MxStdPane.h>
#include <MxPropSlim.h>

class Pane : public MxStdPane
{
public:
    MxPropSlim *slim;

    Pane(MxStdModel *m0) : MxStdPane(m0) { slim=NULL; }

    void key(char c);
    void illustrate();
    void begin_redraw();
};

void Pane::key(char c)
{
    switch( c )
    {
    case MXKEY_NL:
    case MXKEY_CR:
	if( typein.length() > 0 )
	{
	    typein.add('\0');
	    slim->decimate(atoi(typein));
	    typein.reset();
	}
	break;

    case 'g':  slim->decimate(slim->valid_faces - 100); break;
    case 'G':  slim->decimate(slim->valid_faces - 1000); break;
    case 'h':  slim->decimate(1000); break;

    default:
	MxStdPane::key(c);
	return;
    }

    post_redraw();
}

void Pane::begin_redraw()
{
    MxStdPane::begin_redraw();
    glC(1.0, 1.0, 1.0);
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

bool init_application(MxGLApp *app)
{
    MxSMFReader smf;
    MxStdModel *m = new MxStdModel(100, 100);

    optind = 1;

    if( optind == app->argc )
    {
	const char *filename;
	filename = app->query_filename("Select file to view:",
				       ".", "*.smf", "");

	if( !filename ) return false;

	ifstream in(filename);
	smf.read(in, m);
	in.close();
    }
    else
    {
	for(; optind < app->argc; optind++)
	{
	    if( !strcmp(app->argv[optind], "-") )
	    {
		cerr << "-- Reading from stdin" << endl;
		smf.read(cin, m);
	    }
	    else
	    {
		cerr << "-- Reading file " << app->argv[optind] << endl;
		ifstream in(app->argv[optind]);
		if( !in.good() ) cerr << "   Failed to open file!" << endl;
		smf.read(in, m);
	    }
	}
    }


    if( m->normal_binding() == MX_UNBOUND )
    {
	m->normal_binding(MX_PERFACE);
	m->synthesize_normals();
    }

    Pane *pane = new Pane(m);

    cerr << "+ Initializing decimator ...";
    cerr.flush();
    pane->slim = new MxPropSlim(m);
    pane->slim->boundary_weight = 1000;
    pane->slim->initialize();
    cerr << " done" << endl;
    cerr << "+ Quadric dimension: " << pane->slim->dim() << endl;

    app->bind_pane(pane, "QSlim");
    return true;
}

MxGLApp app(init_application);
