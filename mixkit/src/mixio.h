#ifndef MIXIO_INCLUDED // -*- C++ -*-
#define MIXIO_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Standard IO header for MixKit programs.  It deals with things like
  the unpleasantness of multiple IO packages in VC++.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: mixio.h,v 1.4 1998/10/26 21:09:38 garland Exp $

 ************************************************************************/

#if defined(MIX_ANSI_IOSTREAMS)
#include <fstream>

#else
#include <fstream.h>

#endif



#ifdef WIN32
//
// Win32 provides support for POSIX low-level I/O routines.
// However, for some reason Microsoft felt compelled to prefix the
// function names with underscores.
//

#include <io.h>

inline int dup(int fd) { return _dup(fd); }

#endif

// MIXIO_INCLUDED
#endif
