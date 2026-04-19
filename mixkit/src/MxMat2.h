#ifndef MXMAT2_INCLUDED // -*- C++ -*-
#define MXMAT2_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  2x2 Matrix class

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxMat2.h,v 1.8 1998/10/26 21:09:05 garland Exp $

 ************************************************************************/

#include "MxMath.h"
#include "MxVec2.h"

class Mat2
{
private:
    Vec2 row[2];

protected:

    inline void copy(const Mat2& m);
    inline Vec2 col(int i) const {return Vec2(row[0][i],row[1][i]);}

public:
    // Standard matrices
    static Mat2 I;
    static Mat2 zero;
    static Mat2 unit;

    // Standard constructors
    Mat2() { copy(zero); }
    Mat2(double a, double b, double c, double d)
	{ row[0][0]=a; row[0][1]=b; row[1][0]=c; row[1][1]=d; }
    Mat2(const Vec2& r0,const Vec2& r1) { row[0]=r0; row[1]=r1; }
    Mat2(const Mat2& m) { copy(m); }

    // Access methods
    // M(i, j) == row i;col j
    double& operator()(int i, int j)       { return row[i][j]; }
    double  operator()(int i, int j) const { return row[i][j]; }
    const Vec2& operator[](int i) const { return row[i]; }

    operator       double*()       { return row[0]; }
    operator const double*() const { return row[0]; }

    // Assignment methods
    inline Mat2& operator=(const Mat2& m) { copy(m); return *this; }
    inline Mat2& operator+=(const Mat2& m);
    inline Mat2& operator-=(const Mat2& m);

    inline Mat2& operator*=(double s);
    inline Mat2& operator/=(double s);


    // Arithmetic methods
    inline Mat2 operator+(const Mat2& m) const;
    inline Mat2 operator-(const Mat2& m) const;
    inline Mat2 operator-() const;

    inline Mat2 operator*(double s) const;
    inline Mat2 operator/(double s) const;
    Mat2 operator*(const Mat2& m) const;

    inline Vec2 operator*(const Vec2& v) const; // [x y]

    // Matrix operations
    double det();
    Mat2 transpose();
    double invert(Mat2&);


    // Input/Output methods
    friend ostream& operator<<(ostream&, const Mat2&);
    friend istream& operator>>(istream&, Mat2&);
};



inline void Mat2::copy(const Mat2& m)
{
    row[0] = m.row[0]; row[1] = m.row[1];
}

inline Mat2& Mat2::operator+=(const Mat2& m)
{
    row[0] += m.row[0]; row[1] += m.row[1];
    return *this;
}

inline Mat2& Mat2::operator-=(const Mat2& m)
{
    row[0] -= m.row[0]; row[1] -= m.row[1];
    return *this;
}

inline Mat2& Mat2::operator*=(double s)
{
    row[0] *= s; row[1] *= s;
    return *this;
}

inline Mat2& Mat2::operator/=(double s)
{
    row[0] /= s; row[1] /= s;
    return *this;
}

inline Mat2 Mat2::operator+(const Mat2& m) const
{
    return Mat2(row[0]+m.row[0],
		row[1]+m.row[1]);
}

inline Mat2 Mat2::operator-(const Mat2& m) const
{
    return Mat2(row[0]-m.row[0],
		row[1]-m.row[1]);
}

inline Mat2 Mat2::operator-() const
{
    return Mat2(-row[0], -row[1]);
}

inline Mat2 Mat2::operator*(double s) const
{
    return Mat2(row[0]*s, row[1]*s);
}

inline Mat2 Mat2::operator/(double s) const
{
    return Mat2(row[0]/s, row[1]/s);
}

inline Vec2 Mat2::operator*(const Vec2& v) const
{
    return Vec2(row[0]*v, row[1]*v);
}

inline ostream& operator<<(ostream& out, const Mat2& M)
{
    return out << M.row[0] << endl  << M.row[1];
}

inline istream& operator>>(istream& in, Mat2& M)
{
    return in >> M.row[0] >> M.row[1];
}

// MXMAT2_INCLUDED
#endif
