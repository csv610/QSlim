#ifndef MXGEOM3D_INCLUDED // -*- C++ -*-
#define MXGEOM3D_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  Handy 3D geometrical primitives

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxGeom3D.h,v 1.11 1998/10/26 21:09:00 garland Exp $

 ************************************************************************/

#include "MxVec3.h"
#include "MxVec4.h"

extern double triangle_area(const Vec3&, const Vec3&, const Vec3&);
extern Vec3 triangle_raw_normal(const Vec3&, const Vec3&, const Vec3&);
extern Vec3 triangle_normal(const Vec3&, const Vec3&, const Vec3&);
extern Vec4 triangle_plane(const Vec3&, const Vec3&, const Vec3&);
extern Vec4 triangle_raw_plane(const Vec3&, const Vec3&, const Vec3&);
extern double triangle_compactness(const Vec3&, const Vec3&, const Vec3&);

extern double triangle_project_point(const Vec3& v0, const Vec3& v1,
				     const Vec3& v2, const Vec3& v,
				     Vec3 *bary=nullptr);

extern void mx3d_box_corners(const Vec3& min, const Vec3& max, Vec3 *v);

class MxBounds
{
public:

    Vec3 min, max;
    Vec3 center;
    double radius;
    unsigned int points;

    void reset();
    void add_point(const double *v, bool will_update=true);
    void add_point(const float *v, bool will_update=true);
    void complete();
    void merge(const MxBounds&);

    MxBounds() { reset(); }
};



// MXGEOM3D_INCLUDED
#endif
