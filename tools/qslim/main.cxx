/************************************************************************

  Main QSlim file.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: main.cxx,v 1.19 1999/03/17 17:00:13 garland Exp $

 ************************************************************************/

#include <stdmix.h>
#include <mixio.h>
#include "qslim.h"
#include "MxOBJ.h"
#include "MxOFF.h"
#include "MxSTL.h"

// Configuration variables and initial values
//
unsigned int face_target = 0;
bool will_use_fslim = false;
int placement_policy = MX_PLACE_OPTIMAL;
double boundary_weight = 1000.0;
int weighting_policy = MX_WEIGHT_AREA;
bool will_record_history = false;
double compactness_ratio = 0.0;
double meshing_penalty = 1.0;
bool will_join_only = false;
bool be_quiet = false;
OutputFormat output_format = OFF;
char *output_filename = NULL;

// Globally visible variables
//
MxSMFReader *smf = NULL;
MxStdModel *m = NULL;
MxQSlim *slim = NULL;
MxEdgeQSlim *eslim = NULL;
MxFaceQSlim *fslim = NULL;
QSlimLog *history = NULL;
MxDynBlock<MxEdge> *target_edges = NULL;

const char *slim_copyright_notice =
"Copyright (C) 1998 Michael Garland.  See \"COPYING.txt\" for details.";

const char *slim_version_string = "2.0";

static ostream *output_stream = NULL;

static
bool qslim_smf_hook(char *op, int, char *argv[], MxStdModel& m)
{
    if( streq(op, "e") )
    {
	if( !target_edges )
	    target_edges = new MxDynBlock<MxEdge>(m.vert_count() * 3);

	uint i = target_edges->add();

	(*target_edges)[i].v1 = atoi(argv[0]) - 1;
	(*target_edges)[i].v2 = atoi(argv[1]) - 1;

	return true;
    }

    return false;
}

bool (*unparsed_hook)(char *, int, char*[], MxStdModel&) = qslim_smf_hook;

void slim_print_banner(ostream& out)
{
    out << "QSlim surface simplification software." << endl
	<< "Version " << slim_version_string << " "
	<< "[Built " << __DATE__ << "]." << endl
	<< slim_copyright_notice << endl;
}

void slim_init()
{
    if( !slim )
    {
	if( will_use_fslim )
	    slim = fslim = new MxFaceQSlim(*m);
	else
	    slim = eslim = new MxEdgeQSlim(*m);
    }
    else
    {
	if( will_use_fslim )
	    fslim = (MxFaceQSlim *)slim;
	else
	    eslim = (MxEdgeQSlim *)slim;
    }

    slim->placement_policy = placement_policy;
    slim->boundary_weight = boundary_weight;
    slim->weighting_policy = weighting_policy;
    slim->compactness_ratio = compactness_ratio;
    slim->meshing_penalty = meshing_penalty;
    slim->will_join_only = will_join_only;

    if( eslim && target_edges )
    {
	eslim->initialize(*target_edges, target_edges->length());
    }
    else
	slim->initialize();

    if( will_record_history )
    {
	if( !eslim )
	    mxmsg_signal(MXMSG_WARN,
			 "History only available for edge contractions.");
	else
	{
	    history = new QSlimLog(100);
	    eslim->contraction_callback = slim_history_callback;
	}
    }
}

#define CLEANUP(x)  if(x) { delete x; x=NULL; }

void slim_cleanup()
{
    CLEANUP(smf);
    CLEANUP(m);
    CLEANUP(slim);
    eslim = NULL;
    fslim = NULL;
    CLEANUP(history);
    CLEANUP(target_edges);
    if( output_stream != &cout )
    	CLEANUP(output_stream);
}

static bool strend(const char *s, const char *suffix)
{
    size_t slen = strlen(s);
    size_t suffixlen = strlen(suffix);
    if (suffixlen > slen) return false;
    
    const char *p = s + slen - suffixlen;
    while (*p) {
        if (tolower(*p) != tolower(*suffix)) return false;
        p++; suffix++;
    }
    return true;
}

void infer_output_format()
{
    if( !output_filename ) return;

    if      ( strend(output_filename, ".mmf") ) { output_format = MMF; will_record_history = true; }
    else if ( strend(output_filename, ".pm")  ) { output_format = PM;  will_record_history = true; }
    else if ( strend(output_filename, ".log") ) { output_format = LOG; will_record_history = true; }
    else if ( strend(output_filename, ".smf") ) output_format = SMF;
    else if ( strend(output_filename, ".iv")  ) output_format = IV;
    else if ( strend(output_filename, ".vrml")) output_format = VRML;
    else if ( strend(output_filename, ".obj") ) output_format = OBJ;
    else if ( strend(output_filename, ".off") ) output_format = OFF;
    else if ( strend(output_filename, ".ply") ) output_format = PLY;
}

void output_preamble()
{
    if( output_format==MMF || output_format==LOG )
	output_current_model();
}

static
void setup_output()
{
    if( !output_stream )
    {
	if( output_filename )
	    output_stream = new ofstream(output_filename);
	else
	    output_stream = &cout;
    }
}

void output_current_model()
{
    setup_output();

    MxSMFWriter writer;
    writer.write(*output_stream, *m);
}

static
void cleanup_for_output()
{
    // First, mark stray vertices for removal
    //
    for(uint i=0; i<m->vert_count(); i++)
	if( m->vertex_is_valid(i) && m->neighbors(i).length() == 0 )
	    m->vertex_mark_invalid(i);

	// Compact vertex array so only valid vertices remain
    m->compact_vertices();
}

void output_final_model()
{
    setup_output();

    switch( output_format )
    {
    case MMF:
	output_regressive_mmf(*output_stream);
	break;

    case LOG:
	output_regressive_log(*output_stream);
	break;

    case PM:
	output_progressive_pm(*output_stream);
	break;

    case IV:
	cleanup_for_output();
	output_iv(*output_stream);
	break;

    case VRML:
	cleanup_for_output();
	output_vrml(*output_stream);
	break;

    case SMF:
	cleanup_for_output();
	output_current_model();
	break;

    case OBJ:
	cleanup_for_output();
	output_obj(*output_stream);
	break;

    case OFF:
	cleanup_for_output();
	output_off(*output_stream);
	break;

    case PLY:
	cleanup_for_output();
	output_ply(*output_stream);
	break;
    }


}

void input_file(const char *filename)
{
    if( streq(filename, "-") )
    {
	m = smf->read(cin, m);
    }
    else
    {
	ifstream in(filename);
	if( !in.good() )
	    mxmsg_signal(MXMSG_FATAL, "Failed to open input file", filename);
	
        if (strend(filename, ".obj"))
        {
            MxOBJReader reader;
            m = reader.read(in, m);
        }
        else if (strend(filename, ".off"))
        {
            MxOFFReader reader;
            m = reader.read(in, m);
        }
        else if (strend(filename, ".stl"))
        {
            MxSTLReader reader;
            m = reader.read(in, m);
        }
        else // Default to SMF
        {
	    m = smf->read(in, m);
        }
	in.close();
    }
}

static
MxDynBlock<char*> files_to_include(2);

void defer_file_inclusion(char *filename)
{
    files_to_include.add(filename);
}

void include_deferred_files()
{
    for(uint i=0; i<files_to_include.length(); i++)
	input_file(files_to_include[i]);
}

void slim_history_callback(const MxPairContraction& conx, float cost)
{
    history->add(conx);
}
