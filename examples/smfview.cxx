/************************************************************************

  SMFView

  This is a simple example program which will display an SMF model and
  allow the user to interact with it.  It can also be used as a simple
  prototype for writing more complex interactive programs.

  $Id: smfview.cxx,v 1.2 1998/10/28 16:27:34 garland Exp $

 ************************************************************************/

#include <stdmix.h>
#include <mixio.h>

#include <MxSMF.h>
#include <MxStdPane.h>


class Pane : public MxStdPane
{
public:
    Pane(MxStdModel *m0) : MxStdPane(m0) { }

};

bool init_application(MxGLApp *app)
{
    MxSMFReader smf;
    MxStdModel *m = new MxStdModel(100, 100);

    int optind = 1;

    if( optind == app->argc )
    {
	const char *filename;
	filename = app->query_filename("Select file to view:",
				       "../Models", "*.smf", "");

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
    app->bind_pane(pane, "SMFView");
    return true;
}

MxGLApp app(init_application);
