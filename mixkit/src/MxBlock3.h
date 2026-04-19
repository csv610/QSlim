#ifndef MXBLOCK3_INCLUDED // -*- C++ -*-
#define MXBLOCK3_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxBlock3 provides typed access to 3D data blocks.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxBlock3.h,v 1.5 1998/10/26 21:08:42 garland Exp $

 ************************************************************************/

#include "MxBlock.h"

template<class T>
class MxBlock3 : public MxBlock<T>
{
private:
    uint W, H, D;

protected:
    MxBlock3() { }

    void init_block(uint w, uint h, uint d)
	{ W=w; H=h; D=d;  MxBlock<T>::init_block(w*h*d); }
    void resize_block(uint w, uint h, uint d)
	{ W=w; H=h; D=d;  MxBlock<T>::resize_block(w*h*d); }
    void free_block() { MxBlock<T>::free_block(); }

    T& raw(uint i, uint j, uint k) { return MxBlock<T>::raw(k*W*H + j*W + i); }
    const T& raw(uint i, uint j, uint k) const
		{ return MxBlock<T>::raw(k*W*H + j*W + i); }

#define __MXCK(i,j,k) AssertBound(i<W); AssertBound(j<H); AssertBound(k<D)

public:
    MxBlock3(uint w, uint h, uint d) { init_block(w, h, d); }
    ~MxBlock3() { free_block(); }

    T&       ref(uint i,uint j,uint k)       {__MXCK(i,j,k);return raw(i,j,k);}
    const T& ref(uint i,uint j,uint k) const {__MXCK(i,j,k);return raw(i,j,k);}
    T&       operator()(uint i,uint j,uint k)       { return ref(i,j,k); }
    const T& operator()(uint i,uint j,uint k) const { return ref(i,j,k); }

    uint width() const { return W; }
    uint height() const { return H; }
    uint depth() const { return D; }

    void resize(uint w, uint h, uint d) { resize_block(w,h,d); }

#undef __MXCK
};

// MXBLOCK3_INCLUDED
#endif
