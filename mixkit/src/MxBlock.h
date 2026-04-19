#ifndef MXBLOCK_INCLUDED // -*- C++ -*-
#define MXBLOCK_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxBlock provides typed access a contiguous block of elements.
  Modernized to use std::vector.

 ************************************************************************/

#include <vector>
#include <cstring>
#include <algorithm>

template<class T>
class MxBlock : public MxEQ
{
protected:
    std::vector<T> data;

    MxBlock() { }
    void init_block(uint n) { data.assign(n, T()); }
    void resize_block(uint n) { data.resize(n, T()); }
    void free_block() { data.clear(); }

    T&       raw(uint i)       { return data[i]; }
    const T& raw(uint i) const { return data[i]; }

public:
    MxBlock(uint n) : data(n) { }
    virtual ~MxBlock() { }

    // Standard accessors
    //
    T&       ref(uint i)              { AssertBound(i<data.size()); return data[i]; }
    const T& ref(uint i) const        { AssertBound(i<data.size()); return data[i]; }
    T&       operator()(uint i)       { return ref(i);   }
    const T& operator()(uint i) const { return ref(i);   }
    
    T&       operator[](uint i)       { return data[i]; }
    const T& operator[](uint i) const { return data[i]; }

    operator const T*() const { return data.data(); }
    operator       T*()       { return data.data(); }
    uint length() const { return (uint)data.size(); }

    // Primitive methods for altering the data block
    //
    void resize(uint n) { resize_block(n); }
    void bitcopy(const T *array, uint n) // copy bits directly
	{ 
            uint count = std::min((uint)data.size(), n);
            if (count > 0) std::memcpy(data.data(), array, count*sizeof(T)); 
        }
    void copy(const T *array, const uint n) // copy using assignment operator
	{ 
            uint count = std::min((uint)data.size(), n);
            for(uint i=0; i<count; i++) data[i] = array[i]; 
        }
    void bitcopy(const MxBlock<T>& b) { bitcopy(b, b.length()); }
    void copy(const MxBlock<T>& b) { copy(b, b.length()); }
};

// MXBLOCK_INCLUDED
#endif
