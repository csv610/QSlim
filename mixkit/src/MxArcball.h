#ifndef MXARCBALL_INCLUDED // -*- C++ -*-
#define MXARCBALL_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxArcball

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxArcball.h,v 1.10 1998/10/26 21:08:39 garland Exp $

 ************************************************************************/

#include "MxVec4.h"
#include "MxMat4.h"
#include "MxManipulator.h"

typedef Vec4 Quat;
struct BallData;


class MxArcball : public MxManipulator
{
private:
    int mousex, mousey;
    int startx, starty;
    BallData *ball;
    Quat center;
    float radius;

    enum {NoDrag=0, Spin, Dolly, Zoom, LightAdjust} dragging;

    double __proj_save[16];

public:
    Vec4 vNow;
    float cam[3];

public:
    MxArcball() { ball=NULL; }
    MxArcball(const MxBounds& bounds, float aspect)
      { ball=NULL; init(bounds,aspect); }
    ~MxArcball();
    void init(const MxBounds& bounds, float aspect);


    bool mouse_down(int which, int x, int y, int kind=1);
    bool mouse_up(int which, int x, int y, int kind=2);
    bool motion(int x, int y);

    void apply();
    void unapply();
    void draw();

    double *get_qnow();
};

// MXARCBALL_INCLUDED
#endif
