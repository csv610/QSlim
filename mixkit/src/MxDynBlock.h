#ifndef MXDYNBLOCK_INCLUDED // -*- C++ -*-
#define MXDYNBLOCK_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxDynBlocks are blocks that automatically grow to fit the data added
  to them.
  Modernized to use std::vector.

 ************************************************************************/

#include "MxBlock.h"

template<class T>
class MxDynBlock : public MxBlock<T>
{
public:
    MxDynBlock() { }
    MxDynBlock(uint n) : MxBlock<T>(n) { this->data.clear(); this->data.reserve(n); }

    T&       operator()(uint i)       { AssertBound(i<length()); return this->data[i]; }
    const T& operator()(uint i) const { AssertBound(i<length()); return this->data[i]; }

    uint length() const { return (uint)this->data.size(); }
    uint total_space() const { return (uint)this->data.capacity(); }
    T& last() { AssertBound(length()>0); return this->data.back(); }
    const T& last() const { AssertBound(length()>0); return this->data.back(); }

    uint add()
	{
	    this->data.push_back(T());
	    return length() - 1;
	}
    uint add(const T& t)
	{
	    this->data.push_back(t);
	    return length() - 1;
	}

    uint add(T&& t)
	{
	    this->data.push_back(std::move(t));
	    return length() - 1;
	}

    void push_back(const T& t) { this->data.push_back(t); }
    void push_back(T&& t) { this->data.push_back(std::move(t)); }

    void reset() { this->data.clear(); }
    T& drop() { T& val = this->data.back(); this->data.pop_back(); return val; }
    void drop(uint d) 
        { 
            for(uint i=0; i<d && !this->data.empty(); i++) 
                this->data.pop_back(); 
        }
    bool find(const T& t, uint *index=NULL)
	{
	    for(uint i=0; i<length(); i++)
		if( this->data[i] == t )
		{
		    if( index ) *index = i;
		    return true;
		}
	    return false;
	}
    void remove(uint i)
	{ 
            AssertBound(i<length()); 
            this->data[i] = std::move(this->data.back()); 
            this->data.pop_back(); 
        }
    void remove_inorder(uint i)
	{ 
            AssertBound(i<length());
            this->data.erase(this->data.begin() + i);
        }
};

template<class T, int T_SIZE>
class MxSizedDynBlock : public MxDynBlock<T>
{
public:   MxSizedDynBlock(uint n=T_SIZE) : MxDynBlock<T>(n) { }
};

// MXDYNBLOCK_INCLUDED
#endif
