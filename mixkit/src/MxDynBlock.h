#ifndef MXDYNBLOCK_INCLUDED // -*- C++ -*-
#define MXDYNBLOCK_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxDynBlocks are blocks that automatically grow to fit the data added
  to them.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxDynBlock.h,v 1.12 1998/10/26 21:08:49 garland Exp $

 ************************************************************************/

#include "MxBlock.h"

template<class T>
class MxDynBlock : public MxBlock<T>
{
private:
    uint fill;

public:
    MxDynBlock() { this->init_block(8); fill=0; }
    MxDynBlock(uint n) { this->init_block(n); fill=0; }

    T&       operator()(uint i)       { AssertBound(i<fill); return this->ref(i); }
    const T& operator()(uint i) const { AssertBound(i<fill); return this->ref(i); }

    uint length() const { return fill; }
    uint total_space() const { return MxBlock<T>::length(); }
    T& last() { AssertBound(fill>0); return this->raw(fill-1); }
    const T& last() const { AssertBound(fill>0); return this->raw(fill-1); }

    uint add()
	{
	    if( length()==total_space() )  this->resize(total_space() * 2);
	    return fill++;
	}
    uint add(const T& t)
	{
	    uint i=add();
	    this->raw(i) = t;
	    return i;
	}

    void reset() { fill = 0; }
    T& drop() { return this->raw(--fill); }
    void drop(uint d) { fill -= d; }
    bool find(const T& t, uint *index=NULL)
	{
	    for(uint i=0; i<fill; i++)
		if( this->raw(i) == t )
		{
		    if( index ) *index = i;
		    return true;
		}
	    return false;
	}
    void remove(uint i)
	{ AssertBound(i<fill); fill--; this->raw(i) = this->raw(fill); }
    void remove_inorder(uint i)
	{ memmove(&this->raw(i), &this->raw(i+1), (--fill - i)*sizeof(T)); }
};

template<class T, int T_SIZE>
class MxSizedDynBlock : public MxDynBlock<T>
{
public:   MxSizedDynBlock(uint n=T_SIZE) : MxDynBlock<T>(n) { }
};

// MXDYNBLOCK_INCLUDED
#endif
