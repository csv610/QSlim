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
				   const char *options=nullptr,
				   option_processor proc=nullptr);

extern void process_cmdline_only(int argc, char *argv[],
				 const char *options=nullptr,
				 option_processor proc=nullptr);

extern MxStdModel *read_model_from_file(const char *filename,
					MxSMFReader *smf=nullptr,
					MxStdModel *m=nullptr);

extern void output_final_model(MxStdModel *, bool was_modified=false);
extern void set_usage_string(const char *usage);

// CMDLINE_INCLUDED
#endif
