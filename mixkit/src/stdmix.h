#ifndef STDMIX_INCLUDED // -*- C++ -*-
#define STDMIX_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Standard base include file for the MIX library.  This defines various
  common stuff that is used elsewhere.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: stdmix.h,v 1.16 1998/10/26 21:09:41 garland Exp $

 ************************************************************************/

#ifdef HAVE_MIXCFG_H
#include "mixcfg.h"
#endif

#if !defined(MIX_ANSI_IOSTREAMS) && defined(WIN32)
#define MIX_ANSI_IOSTREAMS
#endif

#include <stdlib.h>
#ifdef HAVE_UNISTD_H
#  include <unistd.h>
#endif
#include <string.h>
#include <limits.h>
#if defined(MIX_ANSI_IOSTREAMS)
#  include <iostream>
using namespace std;
#else
#  include <iostream.h>
#endif

#ifdef MIX_NEED_BOOL
#  ifdef bool
#    undef bool
#  endif
typedef int bool;
const bool false = 0;
const bool true = 1;
#endif

#ifdef UINT_MAX
#  define MXID_NIL UINT_MAX
#else
#  define MXID_NIL 0xffffffffU
#endif

#if !defined(HAVE_UINT) && !defined(uint)
typedef unsigned int uint;
#endif

#if !defined(HAVE_USHORT) && !defined(ushort)
typedef unsigned short ushort;
#endif

#ifndef MIN
#  define MIN(a,b) (((a)>(b))?(b):(a))
#  define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef ABS
#  define ABS(x) (((x)<0)?-(x):(x))
#endif

inline bool streq(const char *a, const char *b) { return !strcmp(a,b); }

//
// Default equality class
//
class MxEQ {
public: bool operator==(const MxEQ& x) const { return this==&x; }
};

////////////////////////////////////////////////////////////////////////
//
// Optimization control, debugging, and error reporting facilities
//

//
// Safety levels:
//
//       -2 Reckless
//       -1 Optimized
//        0 Normal
//        1 Cautious
//        2 Paranoid
//
#ifndef SAFETY
// Default safety policy is to take the middle of the road
#define SAFETY 0
#endif

#include "mixmsg.h"

#define fatal_error(s) mxmsg_signal(MXMSG_FATAL, s, NULL, __FILE__, __LINE__)

#ifdef assert
#  undef assert
#endif

#if SAFETY >= 0
#  define assert(i) (i)?((void)NULL):mxmsg_signal(MXMSG_ASSERT, # i, \
						  NULL, __FILE__, __LINE__)
#  define CAREFUL(x) x
#else
#  define assert(i)
#  define CAREFUL(x)
#endif

#if SAFETY==2
#  define SanityCheck(t) assert(t)
#  define PARANOID(x) x
#else
#  define SanityCheck(t)
#  define PARANOID(x)
#endif

#if SAFETY > 0
#  define AssertBound(t)  assert(t)
#  define PRECAUTION(x) x
#else
#  define AssertBound(t)
#  define PRECAUTION(x)
#endif

////////////////////////////////////////////////////////////////////////
//
// Memory management stuff
//

enum MxAllocPolicy { MX_ALLOC_INPLACE };

inline void *operator new(size_t, void *spot, MxAllocPolicy)
{
    return spot;  // MX_ALLOC_INPLACE
}

// STDMIX_INCLUDED
#endif
