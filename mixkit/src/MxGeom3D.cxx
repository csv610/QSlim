/************************************************************************

  Handy 3D geometrical primitives

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxGeom3D.cxx,v 1.11 1998/10/26 21:08:59 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxGeom3D.h"

Vec3 triangle_raw_normal(const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
    Vec3 a = v2 - v1;
    Vec3 b = v3 - v1;
    return a^b;
}

double triangle_area(const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
    return 0.5 * norm(triangle_raw_normal(v1, v2, v3));
}

Vec3 triangle_normal(const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
    Vec3 n = triangle_raw_normal(v1, v2, v3);
    unitize(n);

    return n;
}

Vec4 triangle_plane(const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
    Vec3 n = triangle_normal(v1, v2, v3);

    return Vec4(n, -(n*v1));
}

Vec4 triangle_raw_plane(const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
    Vec3 n = triangle_raw_normal(v1, v2, v3);

    return Vec4(n, -(n*v1));
}

const double FOUR_ROOT3 = 6.928203230275509;

double triangle_compactness(const Vec3& v1, const Vec3& v2, const Vec3& v3)
{
    double L1 = norm2(v2 - v1);
    double L2 = norm2(v3 - v2);
    double L3 = norm2(v1 - v3);

    return FOUR_ROOT3 * triangle_area(v1, v2, v3) / (L1+L2+L3);
}

void mx3d_box_corners(const Vec3& min, const Vec3& max, Vec3 *v)
{
    v[0] = min;
    v[1] = Vec3(min[X], max[Y], min[Z]);
    v[2] = Vec3(max[X], min[Y], min[Z]);
    v[3] = Vec3(max[X], max[Y], min[Z]);
    v[4] = Vec3(min[X], min[Y], max[Z]);
    v[5] = Vec3(min[X], max[Y], max[Z]);
    v[6] = Vec3(max[X], min[Y], max[Z]);
    v[7] = max;
}

////////////////////////////////////////////////////////////////////////
//
// Basic bounding volumes.
// This has been imported pretty much directly from GFX
//

void MxBounds::reset()
{
    min[X] = min[Y] = min[Z] = HUGE;
    max[X] = max[Y] = max[Z] = -HUGE;

    center[X] = center[Y] = center[Z] = 0.0;
    radius = 0.0;

    points = 0;
}

void MxBounds::add_point(const double *v, bool will_update)
{
    if( v[X] < min[X] ) min[X] = v[X];
    if( v[Y] < min[Y] ) min[Y] = v[Y];
    if( v[Z] < min[Z] ) min[Z] = v[Z];

    if( v[X] > max[X] ) max[X] = v[X];
    if( v[Y] > max[Y] ) max[Y] = v[Y];
    if( v[Z] > max[Z] ) max[Z] = v[Z];


    if( will_update )
    {
	center += Vec3(v);
	points++;
    }
}

void MxBounds::add_point(const float *v, bool will_update)
{
    if( v[X] < min[X] ) min[X] = v[X];
    if( v[Y] < min[Y] ) min[Y] = v[Y];
    if( v[Z] < min[Z] ) min[Z] = v[Z];

    if( v[X] > max[X] ) max[X] = v[X];
    if( v[Y] > max[Y] ) max[Y] = v[Y];
    if( v[Z] > max[Z] ) max[Z] = v[Z];


    if( will_update )
    {
	center += Vec3(v);
	points++;
    }
}

void MxBounds::complete()
{
    center /= (double)points;

    Vec3 R1 = max-center;
    Vec3 R2 = min-center;

    radius = MAX(norm(R1), norm(R2));
}

void MxBounds::merge(const MxBounds& b)
{
    add_point(b.min, false);
    add_point(b.max, false);
    points += b.points;

    Vec3 dC = b.center - center;
    double dist = norm(dC);

    if( dist + b.radius > radius )
    {
	// New sphere does not lie within old sphere
	center += b.center;
	center /= 2;

	dist /= 2;
	radius = MAX(dist+radius, dist+b.radius);
    }
}
