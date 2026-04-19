/************************************************************************

  MxRaster

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxRaster.cxx,v 1.8 1999/01/22 17:26:12 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxRaster.h"


////////////////////////////////////////////////////////////////////////
//
// Raster manipulation
//

// !!BUG: This could probably be made significantly faster.
//
void MxRaster::reverse(uint start, uint end)
{
    if(end >= length()) end = length() - channels();

    uint i=start;
    uint j=end;
    unsigned char t;
    
    while(i<j)
    {
	for(uint k=0; k<channels(); k++)
	{
	    t=(*this)[i+k];
	    (*this)[i+k] = (*this)[j+k];
	    (*this)[j+k] = t;
	}
	i += channels();
	j -= channels();
    }
}

void MxRaster::hflip()
{
     uint i = 0;
     uint j = channels()*(width()-1);

     while( i<length() )
     {
	 reverse(i, i+j);
	 i += j + channels();
     }
}

void MxRaster::vflip()
{
    reverse();
    hflip();
}

////////////////////////////////////////////////////////////////////////
//
// PNM output
//
ostream& pnm_write_header(ostream& out, const MxRaster& r, bool is_raw)
{
    return out << "P" << (is_raw?r.channels()*2:r.channels())
	       << " " << r.width() << " " << r.height() << " 255" << endl;
}

ostream& pnm_write_data(ostream& out, const MxRaster& r, bool is_raw)
{
    if( is_raw )
	out.write((const char *)(const unsigned char *)r, r.length());
    else
    {
	for(uint i=0; i<r.length(); i++)
	{
	    out << (uint)(r[i]);
	    if( i%6 ) out << " ";
	    else      out << endl;
	}
    }

    return out;
}

////////////////////////////////////////////////////////////////////////
//
// PNM input
//
static
istream& pnm_skip(istream& in)
{
    for(;;)
    {
	in >> ws;
	if( in.peek() == '#' )
	    in.ignore(65535, '\n');
	else
	    return in;
    }
}

static
void pnm_read_ascii(istream& in, MxRaster& img)
{
    unsigned char *current = img;
    uint val;

    for(uint j=0; j<img.height(); j++) for(uint i=0; i<img.width(); i++)
        for(uint k=0; k<img.channels(); k++)
        {
            pnm_skip(in) >> val;
            *current++ = (unsigned char)val;
        }
}

static
void pnm_read_ascii(istream& in, MxRaster& img, float scale)
{
    unsigned char *current = img;
    float val;

    for(uint j=0; j<img.height(); j++) for(uint i=0; i<img.width(); i++)
        for(uint k=0; k<img.channels(); k++)
        {
            pnm_skip(in) >> val;
            *current++ = (unsigned char)(val*scale);
        }
}

static
void pnm_read_raw(istream& in, MxRaster& img)
{
    char c;  in.get(c);  // extract 1 whitespace character

    //
    // Is this guaranteed to read all the requested bytes?
    //
    in.read((char *)(unsigned char *)img, img.length());
}

MxRaster *pnm_read(istream& in)
{
    unsigned char P, N;

    in >> P >> N;

    if( P!='P' )  return NULL;

    uint width, height, maxval;
    pnm_skip(in) >> width;
    pnm_skip(in) >> height;
    pnm_skip(in) >> maxval;

    bool is_raw = false;
    uint channels = N - '0';
    if( channels > 3 )
    {
	channels /= 2;
	is_raw = true;
    }

    if( channels > 3 )  return NULL;

    MxRaster *img = new MxRaster(width, height, channels);

    if( is_raw )
    {
	if( maxval>255 )  return NULL;
	if( maxval < 255 )
	    mxmsg_signal(MXMSG_WARN, "Ignoring scaled raw PNM data.");

	pnm_read_raw(in, *img);
    }
    else if( maxval==255 )
	pnm_read_ascii(in, *img);
    else
	pnm_read_ascii(in, *img, 255.0f/(float)maxval);

    return img;
}
