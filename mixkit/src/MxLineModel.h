#ifndef MXLINEMODEL_INCLUDED // -*- C++ -*-
#define MXLINEMODEL_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxLineModel

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxLineModel.h,v 1.4 1999/02/18 15:23:50 garland Exp $

 ************************************************************************/

#include "MxDynBlock.h"
#include "MxGeoPrims.h"

typedef MxFaceID MxSegmentID;

class MxVertex2D : public MxEQ
{
public:
    float pos[2];

    MxVertex2D() { }
    MxVertex2D(float x, float y) { pos[0]=x; pos[1]=y; }
    MxVertex2D(const MxVertex2D& v) { *this = v; }

    MxVertex2D& operator=(const MxVertex2D& v)
	{ pos[0]=v.pos[0]; pos[1]=v.pos[1]; return *this; }

    operator const float*() const { return pos; }
    operator       float*()       { return pos; }
#ifdef __GNUC__
    float& operator[](int i)       { return pos[i]; }
    float  operator[](int i) const { return pos[i]; }
#endif
};

class MxSegment2D : public MxEQ
{
public:
    MxVertexID v[2];

    MxSegment2D() { }
    MxSegment2D(MxVertexID v0, MxVertexID v1) { v[0]=v0; v[1]=v1; }
    MxSegment2D(const MxSegment2D& s) { *this = s; }

    MxSegment2D& operator=(const MxSegment2D& s)
	{ v[0]=s.v[0]; v[1]=s.v[1]; return *this; }

    MxVertexID& operator[](int i)       { return v[i]; }
    MxVertexID  operator[](int i) const { return v[i]; }
};

inline ostream& operator<<(ostream& out, const MxVertex2D& v)
{
    return out << "v " << v[0] << " " << v[1];
}

inline ostream& operator<<(ostream& out, const MxSegment2D& s)
{
    return out << "l " << s[0]+1 << " " << s[1]+1;
}


class MxLineModel
{
private:
    MxDynBlock<MxVertex2D> vertices;
    MxDynBlock<MxSegment2D> segments;


public:
    MxLineModel(uint nvert, uint nseg) : vertices(nvert), segments(nseg)
	{ }

    uint vert_count() const { return vertices.length(); }
    uint segment_count() const { return segments.length(); }

    MxVertex2D&       vertex(uint i)       { return vertices(i); }
    const MxVertex2D& vertex(uint i) const { return vertices(i); }
    MxSegment2D&       segment(uint i)       { return segments(i); }
    const MxSegment2D& segment(uint i) const { return segments(i); }

    MxVertexID add_vertex(float, float);
    MxFaceID add_segment(MxVertexID, MxVertexID);
};




// MXLINEMODEL_INCLUDED
#endif
