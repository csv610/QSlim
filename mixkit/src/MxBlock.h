#ifndef MXBLOCK_INCLUDED // -*- C++ -*-
#define MXBLOCK_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxBlock provides typed access a contiguous block of elements.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxBlock.h,v 1.17 1999/02/18 15:24:33 garland Exp $

 ************************************************************************/

template<class T>
class MxBlock : public MxEQ
{
private:
    uint N;
    T *block;

protected:
    MxBlock() { }
    void init_block(uint n)
	{
	    // Allocate memory for block
	    N=n; block = (T *)malloc(sizeof(T)*n);

	    // Initialize each entry
	    for(uint i=0; i<n; i++) new((void *)&block[i], MX_ALLOC_INPLACE) T;
	}
    void resize_block(uint n)
	{
	    T *old = block;

	    // Allocate new block
	    block = (T *)realloc(old, sizeof(T)*n);

	    // Initialize all the newly allocated entries
	    for(uint i=N; i<n; i++) new((void *)&block[i], MX_ALLOC_INPLACE) T;

	    N = n;
	}
    void free_block()
	{
#ifdef WIN32
	    // The Microsoft compiler has a problem with the 
	    // standard syntax below.  For some reason,
	    // expanding it into the following pointer-based
	    // version makes it work.  Don't ask me why.
	    //
	    for(uint i=0; i<N; i++)  { T *p = &block[i]; p->~T(); }
	    free(block);
#else
	    // Call the relevant destructors for each element before
	    // freeing the block.  Has now effect for types like 'int'.
	    //
	    for(uint i=0; i<N; i++)  block[i].~T();
	    free(block);
#endif
	}

    T&       raw(uint i)       { return block[i]; }
    const T& raw(uint i) const { return block[i]; }


public:
    MxBlock(uint n) { init_block(n); }
    ~MxBlock() { free_block(); }

    // Standard accessors
    //
    T&       ref(uint i)              { AssertBound(i<N);return block[i]; }
    const T& ref(uint i) const        { AssertBound(i<N);return block[i]; }
    T&       operator()(uint i)       { return ref(i);   }
    const T& operator()(uint i) const { return ref(i);   }
#ifdef __GNUC__
    // Most compilers can apply the casting operators and then use the
    // standard array [].  GCC (2.7.2) seems to need explicit operators.
    //
    T&       operator[](uint i)       { return block[i]; }
    const T& operator[](uint i) const { return block[i]; }
#endif

    operator const T*() const { return block; }
    operator       T*()       { return block; }
    uint length() const { return N; }

    // Primitive methods for altering the data block
    //
    void resize(uint n) { resize_block(n); }
    void bitcopy(const T *array, uint n) // copy bits directly
	{ memcpy(block, array, MIN(n,N)*sizeof(T)); }
    void copy(const T *array, const uint n) // copy using assignment operator
	{ for(uint i=0; i<MIN(n,N); i++) block[i] = array[i]; }
    void bitcopy(const MxBlock<T>& b) { bitcopy(b, b.length()); }
    void copy(const MxBlock<T>& b) { copy(b, b.length()); }
};

// MXBLOCK_INCLUDED
#endif
