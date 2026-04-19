#ifndef MXQMETRIC2_INCLUDED // -*- C++ -*-
#define MXQMETRIC2_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  2D Quadric Error Metric

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.

  $Id: MxQMetric2.h,v 1.2 1998/10/26 21:09:18 garland Exp $

 ************************************************************************/

#include "MxMath.h"
#include "MxMat2.h"
#include "MxMat3.h"

class MxQuadric2
{
private:
    double a2, ab, ac;
    double     b2, bc;
    double         c2;

    double r;

    void init(double a, double b, double c, double len);

public:
    MxQuadric2() { clear(); }
    MxQuadric2(double a, double b, double c, double len=1.0)
	{ init(a, b, c, len); }
    MxQuadric2(const float *n, double c, double len=1.0)
	{ init(n[X], n[Y], c, len); }
    MxQuadric2(const double *n, double c, double len=1.0)
	{ init(n[X], n[Y], c, len); }
    MxQuadric2(const MxQuadric2& Q) { *this = Q; }

    Mat2 tensor() const;
    Vec2 vector() const { return Vec2(ac, bc); }
    double offset() const { return c2; }
    double length() const { return r; }
    Mat3 homogeneous() const;

    void clear(double val=0.0) { a2=ab=ac=b2=bc=c2=r=val; }
    MxQuadric2& operator=(const MxQuadric2& Q);
    MxQuadric2& operator+=(const MxQuadric2& Q);
    MxQuadric2& operator-=(const MxQuadric2& Q);
    MxQuadric2& operator*=(double s);

    double evaluate(double x, double y) const;
    double evaluate(const double *v) const { return evaluate(v[X], v[Y]); }
    double evaluate(const float *v) const { return evaluate(v[X], v[Y]); }

    double operator()(double x, double y) const { return evaluate(x,y); }
    double operator()(const double *v) const  { return evaluate(v[X], v[Y]); }
    double operator()(const float *v) const  { return evaluate(v[X], v[Y]); }

    bool optimize(Vec2& v) const;
    bool optimize(float *x, float *y) const;

    //    bool optimize(Vec2& v, const Vec2& v1, const Vec2& v2) const;
};

// MXQMETRIC2_INCLUDED
#endif
