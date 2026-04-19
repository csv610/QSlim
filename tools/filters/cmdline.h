#ifndef CMDLINE_INCLUDED // -*- C++ -*-
#define CMDLINE_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Command line interface for the SMF filters.

  Copyright (C) 1998 Michael Garland, All Rights Reserved.
  
  $Id: cmdline.h,v 1.7 1998/10/27 01:06:27 garland Exp $

 ************************************************************************/

#include <MxStdModel.h>
#include <MxSMF.h>

typedef void (*option_processor)(int, char *optarg);

extern MxStdModel *process_cmdline(int argc, char *argv[],
				   const char *options=NULL,
				   option_processor proc=NULL);

extern void process_cmdline_only(int argc, char *argv[],
				 const char *options=NULL,
				 option_processor proc=NULL);

extern MxStdModel *read_model_from_file(const char *filename,
					MxSMFReader *smf=NULL,
					MxStdModel *m=NULL);

extern void output_final_model(MxStdModel *, bool was_modified=false);

// CMDLINE_INCLUDED
#endif
