#ifndef MXRASTER_INCLUDED // -*- C++ -*-
#define MXRASTER_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxRaster

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxRaster.h,v 1.10 1998/10/26 21:09:23 garland Exp $

 ************************************************************************/

#include "MxBlock.h"

class MxRaster : private MxBlock<unsigned char>
{
private:
    uint W, H, nchan;

public:

    MxRaster(uint w, uint h, uint c)
	: MxBlock<unsigned char>(w*h*c)  { W=w;  H=h;  nchan=c; }

    uint width() const { return W; }
    uint height() const { return H; }
    uint channels() const { return nchan; }
    uint length() const { return MxBlock<unsigned char>::length(); }

#ifdef __GNUC__
    unsigned char& operator[](uint i)       { return raw(i); }
    unsigned char  operator[](uint i) const { return raw(i); }
#endif
    operator unsigned char *()
	{ return MxBlock<unsigned char>::operator unsigned char*(); }
    operator const unsigned char *() const
	{ return MxBlock<unsigned char>::operator const unsigned char*(); }

    unsigned char *pixel(uint i, uint j)
	{
	    AssertBound(i<W && j<H);
	    return ((unsigned char *)*this) + (j*W + i)*nchan;
	}

    const unsigned char *pixel(uint i, uint j) const
	{
	    AssertBound(i<W && j<H);
	    return ((const unsigned char *)*this) + (j*W + i)*nchan;
	}

    unsigned char *operator()(uint i, uint j) { return pixel(i,j); }
    const unsigned char *operator()(uint i, uint j) const {return pixel(i,j);}

    void reverse(uint start=0, uint end=MXID_NIL);
    void hflip();
    void vflip();
};

////////////////////////////////////////////////////////////////////////
//
// File I/O primitives for rasters.
// The basic format supported is PNM files.  Support for this format
// is built-in, and thus always available.
// 
extern ostream& pnm_write_header(ostream&,const MxRaster&, bool is_raw=true);
extern ostream& pnm_write_data(ostream&,const MxRaster&, bool is_raw=true);
inline ostream& pnm_write(ostream& out, const MxRaster& r, bool is_raw=true)
{ pnm_write_header(out,r,is_raw);  return pnm_write_data(out, r, is_raw); }

extern MxRaster *pnm_read(istream&);

// (Optional) support for TIFF files.
// If libtiff is not available, these procedures will always fail.
//
extern bool tiff_write(const char *filename, const MxRaster&);
extern MxRaster *tiff_read(const char *filename);

// MXRASTER_INCLUDED
#endif
