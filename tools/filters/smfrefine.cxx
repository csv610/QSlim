/************************************************************************

  smfrefine

  This filter refines a model by iteratively splitting edges.

  Copyright (C) 1998 Michael Garland, All Rights Reserved.
  
  $Id: smfrefine.cxx,v 1.3 1998/10/27 01:08:53 garland Exp $

 ************************************************************************/

#include <stdmix.h>

#include <MxSMF.h>
#include <MxGeom3D.h>
#include <MxEdgeFilter.h>
#include <MxVector.h>

#include "cmdline.h"

static float max_sqrlen = HUGE;
static MxBounds bounds;
static float noise_level = 0.0;
static float noise_scale;
static float diameter;

static const char *options = "t:n:";
static const char *usage_string =
"-t <n>		Set the maximum allowable edge length.\n"
"-n <L>		Set the noise level to L% of the model diameter [default=0].\n"
"\n";

static
void process_options(int opt, char *optarg)
{
    switch( opt )
    {
    case 't':  max_sqrlen = atof(optarg); max_sqrlen*=max_sqrlen; break;
    case 'n':  noise_level = atof(optarg)/100; break;
    }
}

class RefineFilter : public MxEdgeFilter
{
public:
    RefineFilter(MxStdModel *m) : MxEdgeFilter(m) { }

    void filter_target_edge(MxRankedEdge *);
};

void RefineFilter::filter_target_edge(MxRankedEdge *edge)
{
    uint vnew = split_edge(edge);

    if( noise_level > 0 )
    {
	float delta[3];

	delta[0] = random1() * noise_scale;
	delta[1] = random1() * noise_scale;
	delta[2] = random1() * noise_scale;

	mxv_addinto(m->vertex(vnew), delta, 3);
    }
}


int main(int argc, char *argv[])
{
    MxStdModel *m = process_cmdline(argc, argv, options, process_options);
    if( !m ) return 0;

    bounds.reset();
    for(uint i=0; i<m->vert_count(); i++)
	bounds.add_point(m->vertex(i));
    bounds.complete();

    diameter = sqrt(mxv_L2(bounds.max, bounds.min, 3));
    noise_scale = diameter * noise_level;
    if( noise_level > 0 )
    {
	cerr << "Noise level = " << noise_level*100 << "%" << endl;
	cerr << "            = " << noise_scale << " units" << endl;
    }

    RefineFilter filter(m);
    filter.initialize();
    filter.filter_above_rank(max_sqrlen);

    output_final_model(m, true);
    delete m;
    return 0;
}
