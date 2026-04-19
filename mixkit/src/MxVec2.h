#ifndef MXVEC2_INCLUDED // -*- C++ -*-
#define MXVEC2_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  2D Vector class

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxVec2.h,v 1.10 1998/10/26 21:09:35 garland Exp $

 ************************************************************************/

#include "MxMath.h"

#ifdef MIX_USE_TVEC
#include "MxVector.h"

template<class T>
class MxTVec2 : public MxVBlock<T, 2>
{
public:
    MxTVec2() { }
    MxTVec2(T x, T y) { (*this)[0]=x; (*this)[1]=y; }
    MxTVec2(const MxVBlock<T, 2>& v) : MxVBlock<T, 2>(v) { }
    MxTVec2(const float *v) {(*this)[0]=v[0];(*this)[1]=v[1];}
    MxTVec2(const double *v) {(*this)[0]=v[0];(*this)[1]=v[1];}
};

typedef MxTVec2<double> Vec2;


#else

class Vec2 {
private:
    double elt[2];

protected:
    inline void copy(const Vec2& v);

public:
    //
    // Standard constructors
    //
    Vec2(double x=0, double y=0) { elt[0]=x; elt[1]=y; }
    Vec2(const Vec2& v) { copy(v); }
    Vec2(const float *v) { elt[0]=v[0]; elt[1]=v[1]; }
    Vec2(const double *v) { elt[0]=v[0]; elt[1]=v[1]; }

    //
    // Access methods
    //
    double& operator()(uint i)       { AssertBound(i<2); return elt[i]; }
    double  operator()(uint i) const { AssertBound(i<2); return elt[i]; }
#ifdef __GNUC__
    double& operator[](uint i)       { return elt[i]; }
    double  operator[](uint i) const { return elt[i]; }
#endif

    operator double*() { return elt; }
    operator const double*() const { return elt; }

    //
    // Comparison operators
    //
    inline bool operator==(const Vec2& v) const;
    inline bool operator!=(const Vec2& v) const;

    //
    // Assignment and in-place arithmetic methods
    //
    inline void set(double x, double y) { elt[0]=x; elt[1]=y; }
    inline Vec2& operator=(const Vec2& v);
    inline Vec2& operator+=(const Vec2& v);
    inline Vec2& operator-=(const Vec2& v);
    inline Vec2& operator*=(double s);
    inline Vec2& operator/=(double s);

    //
    // Binary arithmetic methods
    //
    inline Vec2 operator+(const Vec2& v) const;
    inline Vec2 operator-(const Vec2& v) const;
    inline Vec2 operator-() const;

    inline Vec2 operator*(double s) const;
    inline Vec2 operator/(double s) const;
    inline double operator*(const Vec2& v) const;
};



////////////////////////////////////////////////////////////////////////
//
// Method definitions
//

inline void Vec2::copy(const Vec2& v)
{
    elt[0]=v.elt[0]; elt[1]=v.elt[1];
}

inline bool Vec2::operator==(const Vec2& v) const
{
    double dx=elt[X]-v[X],  dy=elt[Y]-v[Y];
    return (dx*dx + dy*dy) < FEQ_EPS2;
}

inline bool Vec2::operator!=(const Vec2& v) const
{
    double dx=elt[X]-v[X],  dy=elt[Y]-v[Y];
    return (dx*dx + dy*dy) > FEQ_EPS2;
}

inline Vec2& Vec2::operator=(const Vec2& v)
{
    copy(v);
    return *this;
}

inline Vec2& Vec2::operator+=(const Vec2& v)
{
    elt[0] += v[0];   elt[1] += v[1];
    return *this;
}

inline Vec2& Vec2::operator-=(const Vec2& v)
{
    elt[0] -= v[0];   elt[1] -= v[1];
    return *this;
}

inline Vec2& Vec2::operator*=(double s)
{
    elt[0] *= s;   elt[1] *= s;
    return *this;
}

inline Vec2& Vec2::operator/=(double s)
{
    elt[0] /= s;   elt[1] /= s;
    return *this;
}

inline Vec2 Vec2::operator+(const Vec2& v) const
{
    return Vec2(elt[0]+v[0], elt[1]+v[1]);
}

inline Vec2 Vec2::operator-(const Vec2& v) const
{
    return Vec2(elt[0]-v[0], elt[1]-v[1]);
}

inline Vec2 Vec2::operator-() const
{
    return Vec2(-elt[0], -elt[1]);
}

inline Vec2 Vec2::operator*(double s) const
{
    return Vec2(elt[0]*s, elt[1]*s);
}

inline Vec2 Vec2::operator/(double s) const
{
    return Vec2(elt[0]/s, elt[1]/s);
}

inline double Vec2::operator*(const Vec2& v) const
{
    return elt[0]*v[0] + elt[1]*v[1];
}

#endif

// Make scalar multiplication commutative
inline Vec2 operator*(double s, const Vec2& v) { return v*s; }



////////////////////////////////////////////////////////////////////////
//
// Primitive function definitions
//

inline double norm(const Vec2& v) { return sqrt(v[0]*v[0] + v[1]*v[1]); }
inline double norm2(const Vec2& v) { return v[0]*v[0] + v[1]*v[1]; }

inline double unitize(Vec2& v)
{
    double l=norm2(v);
    if( l!=1.0 && l!=0.0 )
    {
	l = sqrt(l);
	v /= l;
    }
    return l;
}



////////////////////////////////////////////////////////////////////////
//
// Misc. function definitions
//

inline ostream& operator<<(ostream& out, const Vec2& v)
{
    return out << v[0] << " " << v[1];
}

inline istream& operator>>(istream& in, Vec2& v)
{
    return in >> v[0] >> v[1];
}

// MXVEC2_INCLUDED
#endif
