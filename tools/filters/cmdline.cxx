/************************************************************************

  Command line processing for the standard SMF filters.

  Copyright (C) 1998 Michael Garland, All Rights Reserved.
  
  $Id: cmdline.cxx,v 1.7 1998/10/27 01:06:27 garland Exp $

 ************************************************************************/

#include <stdmix.h>
#include <mixio.h>
#include <MxSMF.h>

#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#else
#  include <getopt.h>
#endif

#include "cmdline.h"

static const char *base_options="o:h";

static char *output_filename = nullptr;
static const char *custom_usage = nullptr;

static const char *base_usage =
"Usage: [options] [input files]\n"
"\n"
"Options:\n"
"  -o <file>   Specify output filename\n"
"  -h          Display this help message\n"
"\n";

void set_usage_string(const char *usage)
{
    custom_usage = usage;
}

MxStdModel *read_model_from_file(const char *filename,
				 MxSMFReader *smf,
				 MxStdModel *m)
{
    bool local_smfreader = false;

    if( !smf )
    {
	smf = new MxSMFReader;
	local_smfreader = true;
    }

    if( !strcmp(filename, "-"))
	m = smf->read(cin, m);
    else
    {
	ifstream in(filename);
	if( !in.good() )  fatal_error("Failed to open input file");
	m = smf->read(in, m);
    }

    if( local_smfreader ) delete smf;

    return m;
}

void process_cmdline_only(int argc, char *argv[],
			    const char *user_options, option_processor proc)
{
    const char *options = base_options;
    char *dyn_options = nullptr;

    if( user_options )
    {
	int base_len = strlen(base_options);
	int user_len = strlen(user_options);

	dyn_options = new char[base_len + user_len + 1];
	strcpy(dyn_options, user_options);
	strcpy(dyn_options+user_len, base_options);
	dyn_options[base_len+user_len] = '\0';
        options = dyn_options;
    }

    int opt;

    while( (opt = getopt(argc, argv, options)) != EOF )
    {
	switch( opt )
	{
	case 'o':
	    output_filename = optarg;
	    break;
	case 'h':
	    if( custom_usage )
		cerr << endl << custom_usage << endl;
	    else
		cerr << endl << base_usage << endl;
	    exit(0);
	    break;

	default:
	    if( proc )
		(*proc)(opt, optarg);
	    break;
	}
    }
    
    if (dyn_options) delete[] dyn_options;
}

MxStdModel *process_cmdline(int argc, char *argv[],
			    const char *user_options, option_processor proc)
{
    process_cmdline_only(argc, argv, user_options, proc);

    MxSMFReader smf;
    MxStdModel *m = new MxStdModel(64, 64);

    if( optind==argc )
	smf.read(cin, m);

    for(; optind<argc; optind++)
	read_model_from_file(argv[optind], &smf, m);

    if( m->face_count() == 0 && m->vert_count() == 0 )
    {
	cerr << argv[0] << ": Input model is empty." << endl;
	delete m;
	m = NULL;
    }

    return m;
}

void output_final_model(MxStdModel *m, bool was_modified)
{
    if( was_modified )
	m->compact_vertices();

    MxSMFWriter writer;

    if( !output_filename )
	writer.write(cout, *m);
    else
    {
	ofstream out(output_filename);
	writer.write(out, *m);
    }
}
