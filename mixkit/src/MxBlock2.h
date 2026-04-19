#ifndef MXBLOCK2_INCLUDED // -*- C++ -*-
#define MXBLOCK2_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxBlock2 provides typed access to 2D data blocks.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxBlock2.h,v 1.9 1998/10/26 21:08:41 garland Exp $

 ************************************************************************/

#include "MxBlock.h"

template<class T>
class MxBlock2 : public MxBlock<T>
{
private:
    uint W, H;

protected:
    MxBlock2() { }
    void init_block(uint w, uint h)
	{ W=w;  H=h;  MxBlock<T>::init_block(w*h); }
    void resize_block(uint w, uint h)
	{ W=w;  H=h;  MxBlock<T>::resize_block(w*h); }

    T&       raw(uint i, uint j)       { return MxBlock<T>::raw(j*W+i); }
    const T& raw(uint i, uint j) const { return MxBlock<T>::raw(j*W+i); }

#define __MXCK(i,j) AssertBound(i<W); AssertBound(j<H)

public:
    // Exported constructors/destructors
    // 
    MxBlock2(uint w, uint h) { init_block(w,h); }

    // Accessors
    //
    T&       ref(uint i, uint j)              { __MXCK(i,j); return raw(i,j); }
    const T& ref(uint i, uint j) const        { __MXCK(i,j); return raw(i,j); }
    T&       operator()(uint i, uint j)       { return ref(i,j); }
    const T& operator()(uint i, uint j) const { return ref(i,j); }

    uint width() const { return W; }
    uint height() const { return H; }

    void resize(uint w, uint h) { resize_block(w,h); }

#undef __MXCK
};


// MXBLOCK2_INCLUDED
#endif
