/************************************************************************

  TIFF support is optional.  Only PNM support is guaranteed to be
  available.  Since it is optional, the TIFF code is contained here in a
  separate source file.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxRaster-tiff.cxx,v 1.5 1999/01/22 17:26:12 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxRaster.h"


#ifdef MIX_HAVE_LIBTIFF
#include <tiffio.h>

////////////////////////////////////////////////////////////////////////
//
// TIFF output
//
static
bool __tiff_write(TIFF *tif, const MxRaster& img)
{
    TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, img.width());
    TIFFSetField(tif, TIFFTAG_IMAGELENGTH, img.height());

    TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, img.channels());
    TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

    // Turn on LZW compression.  Shhhhh!  Don't tell Unisys!
    TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
    //
    // Predictors:
    //     1 (default) -- No predictor
    //     2           -- Horizontal differencing
    TIFFSetField(tif, TIFFTAG_PREDICTOR, 2);

    uint32 scanline_size = img.channels() * img.width();
    if( TIFFScanlineSize(tif) != scanline_size )
    {
	mxmsg_signal(MXMSG_WARN,
		     "Mismatch with library's expected scanline size!",
		     "tiff_write");
	return false;
    }

    TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, TIFFDefaultStripSize(tif, -1));

    char *scanline_buf = new char[scanline_size];

    const unsigned char *scanline = img;
    for(uint32 y=0; y<img.height(); y++)
    {
	memcpy(scanline_buf, scanline, scanline_size);
	// NOTE: TIFFWriteScanline modifies the buffer you pass it.
	//       Thus, we need to copy stuff out of the raster first.
	TIFFWriteScanline(tif, scanline_buf, y, 0);
	scanline += scanline_size;
    }
    delete[] scanline_buf;

    return true;
}

bool tiff_write(const char *filename, const MxRaster& img)
{
    TIFF *tif = TIFFOpen(filename, "w");
    if( !tif ) return false;

    bool result = __tiff_write(tif, img);

    TIFFClose(tif);

    return result;
}

////////////////////////////////////////////////////////////////////////
//
// TIFF input
//
static
void unpack_tiff_raster(uint32 *raster, MxRaster *img, uint npixels)
{
    unsigned char *pix = img[0];
    
    for(uint i=0; i<npixels; i++)
    {
	*pix++ = TIFFGetR(raster[i]);
	*pix++ = TIFFGetG(raster[i]);
	*pix++ = TIFFGetB(raster[i]);
    }
}

static
MxRaster *__tiff_read(TIFF *tif)
{
    uint32 w, h;
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);

    uint npixels = w*h;

    uint32 *raster = (uint32 *)_TIFFmalloc(npixels * sizeof(uint32));
    if( !raster ) return NULL;

    TIFFReadRGBAImage(tif, w, h, raster, true);

    MxRaster *img = new MxRaster(w, h, 3);
    unpack_tiff_raster(raster, img, npixels);

    _TIFFfree(raster);

    return img;
}

MxRaster *tiff_read(const char *filename)
{
    TIFF *tif = TIFFOpen(filename, "r");
    if( !tif ) return NULL;

    MxRaster *img = __tiff_read(tif);

    TIFFClose(tif);

    return img;
}

#else

bool tiff_write(const char *, const MxRaster&) { return false; }
MxRaster *tiff_read(const char *) { return NULL; }

#endif
