/************************************************************************

  Arcball rotation control -- The Arcball is a handy manipulator for
  spinning objects in 3D.  This particular controller also supports
  some limited camera and light movements as well.

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  The core arcball code was originally published in:

	"Arcball Rotation Control"
	by Ken Shoemake <shoemake@graphics.cis.upenn.edu>
	in "Graphics Gems IV", Academic Press, 1994.

  and was placed in the public domain.

  Originally written for IRIS GL, the OpenGL code is based on the
  modifications of Victor Ng-Thow-Hing <victorng@dgp.toronto.edu>.
  Over time, I've made various modifications including:

	- Porting from ANSI C to C++
	- Adding some improvements from Sebastian Grassia <spiff@cs.cmu.edu>
	- Nominal integration with the MixKit library

  $Id: MxArcball.cxx,v 1.17 1998/10/26 21:08:38 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "MxArcball.h"

enum AxisSet {NoAxes, CameraAxes, BodyAxes, OtherAxes, NSets};
enum BallType { ArcBall, TrackBall };
typedef Mat4 HMatrix;



/* ***** BallAux.c ***** */


Quat qOne(0,0,0,1);

/* Return quaternion product qL * qR.  Note: order is important!
 * To combine rotations, use the product Mul(qSecond, qFirst),
 * which gives the effect of rotating by qFirst then qSecond. */
Quat Qt_Mul(Quat& qL, Quat& qR)
{
    Quat qq;
    qq[W] = qL[W]*qR[W] - qL[X]*qR[X] - qL[Y]*qR[Y] - qL[Z]*qR[Z];
    qq[X] = qL[W]*qR[X] + qL[X]*qR[W] + qL[Y]*qR[Z] - qL[Z]*qR[Y];
    qq[Y] = qL[W]*qR[Y] + qL[Y]*qR[W] + qL[Z]*qR[X] - qL[X]*qR[Z];
    qq[Z] = qL[W]*qR[Z] + qL[Z]*qR[W] + qL[X]*qR[Y] - qL[Y]*qR[X];
    return (qq);
}


/* Construct rotation matrix from (possibly non-unit) quaternion.
 * Assumes matrix is used to multiply column vector on the left:
 * vnew = mat vold.  Works correctly for right-handed coordinate system
 * and right-handed rotations. */
HMatrix *Qt_ToMatrix(const Quat& q, HMatrix& out)
{
    double Nq = norm2(q);
    double s = (Nq > 0.0) ? (2.0 / Nq) : 0.0;

    double xs = q[X]*s,  ys = q[Y]*s,  zs = q[Z]*s;
    double wx = q[W]*xs, wy = q[W]*ys, wz = q[W]*zs;
    double xx = q[X]*xs, xy = q[X]*ys, xz = q[X]*zs;
    double yy = q[Y]*ys, yz = q[Y]*zs, zz = q[Z]*zs;

    out(X,X) = 1.0 - (yy + zz); out(Y,X) = xy + wz; out(Z,X) = xz - wy;
    out(X,Y) = xy - wz; out(Y,Y) = 1.0 - (xx + zz); out(Z,Y) = yz + wx;
    out(X,Z) = xz + wy; out(Y,Z) = yz - wx; out(Z,Z) = 1.0 - (xx + yy);
    out(X,W) = out(Y,W) = out(Z,W) = out(W,X) = out(W,Y) = out(W,Z) = 0.0;
    out(W,W) = 1.0;
    return ((HMatrix *)&out);
}

/* Return conjugate of quaternion. */
Quat Qt_Conj(Quat& q)
{
    Quat qq;
    qq[X] = -q[X]; qq[Y] = -q[Y]; qq[Z] = -q[Z]; qq[W] = q[W];
    return (qq);
}

/* Return vector formed from components */
inline Vec4 V3_(float x, float y, float z)
{
    return Vec4(x, y, z, 0);
}

/* Return norm of v, defined as sum of squares of components */
inline double V3_Norm(const Vec4& v)
{
    return ( v[X]*v[X] + v[Y]*v[Y] + v[Z]*v[Z] );
}

/* Return unit magnitude vector in direction of v */
Vec4 V3_Unit(const Vec4& v)
{
    static Vec4 u(0, 0, 0, 0);
    float vlen = sqrt(V3_Norm(v));
    if (vlen != 0.0) {
	u[X] = v[X]/vlen; u[Y] = v[Y]/vlen; u[Z] = v[Z]/vlen;
    }
    return (u);
}

/* Return version of v scaled by s */
inline Vec4 V3_Scale(const Vec4& v, float s)
{
    return Vec4(s*v[X], s*v[Y], s*v[Z], v[W]);
}

/* Return negative of v */
inline Vec4 V3_Negate(const Vec4& v)
{
    return Vec4(-v[X], -v[Y], -v[Z], 0);
}

/* Return sum of v1 and v2 */
inline Vec4 V3_Add(const Vec4& v1, const Vec4& v2)
{
    return Vec4(v1[X]+v2[X], v1[Y]+v2[Y], v1[Z]+v2[Z], 0);
}

/* Return difference of v1 minus v2 */
inline Vec4 V3_Sub(const Vec4& v1, const Vec4& v2)
{
    return Vec4(v1[X]-v2[X], v1[Y]-v2[Y], v1[Z]-v2[Z], 0);
}

/* Halve arc between unit vectors v0 and v1. */
Vec4 V3_Bisect(const Vec4& v0, const Vec4& v1)
{
    Vec4 v(0, 0, 0, 0);
    v = V3_Add(v0, v1);

    float Nv = V3_Norm(v);
    if (Nv < 1.0e-5) {
	v = V3_(0, 0, 1);
    } else {
	v = V3_Scale(v, 1/sqrt(Nv));
    }
    return (v);
}

/* Return dot product of v1 and v2 */
double V3_Dot(const Vec4& v1, const Vec4& v2)
{
    return (v1[X]*v2[X] + v1[Y]*v2[Y] + v1[Z]*v2[Z]);
}


/* Return cross product, v1 x v2 */
inline Vec4 V3_Cross(const Vec4& v1, const Vec4& v2)
{
    return Vec4(v1[Y]*v2[Z]-v1[Z]*v2[Y],
		v1[Z]*v2[X]-v1[X]*v2[Z],
		v1[X]*v2[Y]-v1[Y]*v2[X],
		0);
}



/* ***** Ball.h ***** */

typedef Vec4 *ConstraintSet;
struct BallData {
    Vec4 center;
    double radius, aspect;
    Quat qNow, qDown, qDrag;
    Vec4 vNow, vDown, vFrom, vTo, vrFrom, vrTo;
    HMatrix mNow, mDown;
    int showResult, dragging;
    ConstraintSet sets[NSets];
    int setSizes[NSets];
    AxisSet axisSet;
    int axisIndex;
    BallType ballType;
};

/* Public routines */
extern BallData *Ball_Alloc();
extern void Ball_Free(BallData *);
extern void Ball_Init(BallData *ball, double aspect);
extern void Ball_Place(BallData *ball, Vec4 center, double radius
		       /*, Quat orient*/);
extern void Ball_Mouse(BallData *ball, Vec4 vNow);
extern void Ball_UseSet(BallData *ball, AxisSet axisSet);
extern void Ball_ShowResult(BallData *ball);
extern void Ball_HideResult(BallData *ball);
extern void Ball_Update(BallData *ball, int al);
extern void Ball_Value(BallData *ball, HMatrix& mNow);
extern void Ball_BeginDrag(BallData *ball);
extern void Ball_EndDrag(BallData *ball);
extern void Ball_Draw(BallData *ball, int drawBall,
		      int drawConstraints, int drawArc);
extern int Ball_MouseOutsideSphere(BallData *ball, Vec4 vMouse);
extern void Ball_SetBallType(BallData *ball, BallType bt);
/* Private routines */
void DrawAnyArc(Vec4 vFrom, Vec4 vTo);
void DrawHalfArc(Vec4 n);
void Ball_DrawConstraints(BallData *ball);
void Ball_DrawDragArc(BallData *ball);
void Ball_DrawResultArc(BallData *ball);



/* ***** BallMath.c ***** */

/* Convert window coordinates to sphere coordinates. */
Vec4 MouseOnSphere(Vec4 mouse, Vec4 ballCenter, double ballRadius)
{
    Vec4 ballMouse;
    double mag;
    ballMouse[X] = (mouse[X] - ballCenter[X]) / ballRadius;
    ballMouse[Y] = (mouse[Y] - ballCenter[Y]) / ballRadius;
    mag = ballMouse[X]*ballMouse[X] + ballMouse[Y]*ballMouse[Y];
    if (mag > 1.0) {
	double scale = 1.0/sqrt(mag);
	ballMouse[X] *= scale; ballMouse[Y] *= scale;
	ballMouse[Z] = 0.0;
    } else {
	ballMouse[Z] = sqrt(1 - mag);
    }
    ballMouse[W] = 0.0;
    return (ballMouse);
}

/* Construct a unit quaternion from two points on unit sphere */
Quat Qt_FromBallPoints(Vec4 from, Vec4 to, BallType bt)
{
    Quat qu;
    qu[X] = from[Y]*to[Z] - from[Z]*to[Y];
    qu[Y] = from[Z]*to[X] - from[X]*to[Z];
    qu[Z] = from[X]*to[Y] - from[Y]*to[X];

    if (bt == ArcBall){
	qu[W] = from[X]*to[X] + from[Y]*to[Y] + from[Z]*to[Z];
    }
    else {
	float  a, s, mag;

	a = .5*acos(from[X]*to[X] + from[Y]*to[Y] + from[Z]*to[Z]);
	mag = sqrt(qu[X]*qu[X] + qu[Y]*qu[Y] + qu[Z]*qu[Z]);
	qu[W] = cos(a); s = sin(a);
	if (s != 0)
	  s /= mag;
	qu[X] *= s;  qu[Y] *= s; qu[Z] *= s;
    }

    return (qu);
}

/* Convert a unit quaternion to two points on unit sphere */
void Qt_ToBallPoints(Quat q, Vec4 *arcFrom, Vec4 *arcTo)
{
    double s;
    s = sqrt(q[X]*q[X] + q[Y]*q[Y]);
    if (s == 0.0) {
	*arcFrom = V3_(0.0, 1.0, 0.0);
    } else {
	*arcFrom = V3_(-q[Y]/s, q[X]/s, 0.0);
    }
    (*arcTo)[X] = q[W]*(*arcFrom)[X] - q[Z]*(*arcFrom)[Y];
    (*arcTo)[Y] = q[W]*(*arcFrom)[Y] + q[Z]*(*arcFrom)[X];
    (*arcTo)[Z] = q[X]*(*arcFrom)[Y] - q[Y]*(*arcFrom)[X];
    if (q[W] < 0.0) *arcFrom = V3_(-(*arcFrom)[X], -(*arcFrom)[Y], 0.0);
}

/* Force sphere point to be perpendicular to axis. */
Vec4 ConstrainToAxis(Vec4& loose, Vec4& axis)
{
    Vec4 onPlane = V3_Sub(loose, V3_Scale(axis, V3_Dot(axis, loose)));
    float norm = V3_Norm(onPlane);

    if (norm > 0.0) {
	if (onPlane[Z] < 0.0) onPlane = V3_Negate(onPlane);
	return ( V3_Scale(onPlane, 1/sqrt(norm)) );
    } /* else drop through */
    if (axis[Z] == 1) {
	onPlane = V3_(1.0, 0.0, 0.0);
    } else {
	onPlane = V3_Unit(V3_(-axis[Y], axis[X], 0.0));
    }
    return (onPlane);
}

/* Find the index of nearest arc of axis set. */
int NearestConstraintAxis(Vec4& loose, Vec4 *axes, int nAxes)
{
    Vec4 onPlane;
    float max, dot;
    int i, nearest;
    max = -1; nearest = 0;
    for (i=0; i<nAxes; i++) {
	onPlane = ConstrainToAxis(loose, axes[i]);
	dot = V3_Dot(onPlane, loose);
	if (dot>max) {
	    max = dot; nearest = i;
	}
    }
    return (nearest);
}



/* ***** Ball.c ***** */
/* Ken Shoemake, 1993 */
/* Modified by Victor Ng, Jan. 1996 for OpenGL */


#include "MxGLPane.h"

#define FALSE 0
#define TRUE 1

#define LG_NSEGS 4
#define NSEGS (1<<LG_NSEGS)
#define RIMCOLOR()    glColor3f(0.0f, 1.0f, 1.0f);
#define FARCOLOR()    glColor3f(0.8f, 0.5f, 0.0f);
#define NEARCOLOR()   glColor3f(1.0f, 0.8f, 0.0f);
#define DRAGCOLOR()   glColor3f(0.0f, 1.0f, 1.0f);
#define RESCOLOR()    glColor3f(0.8f, 0.0f, 0.0f);



Vec4 otherAxis(-0.48, 0.80, 0.36, 1);

BallData *Ball_Alloc()
{
    return (BallData *)malloc( sizeof(BallData) );
}

void Ball_Free(BallData *ball)
{
    free(ball);
}

/* Establish reasonable initial values for controller. */
void Ball_Init(BallData *ball, double aspect)
{
    ball->center = qOne;
    ball->radius = 1.0;
    ball->aspect = aspect;
    ball->ballType = ArcBall;
    ball->vDown = ball->vNow = qOne;
    ball->qDown = ball->qNow = qOne;

    ball->mNow = Mat4::I;
    ball->mDown = Mat4::I;

    ball->showResult = ball->dragging = FALSE;
    ball->axisSet = NoAxes;

    ball->sets[CameraAxes]     = (Vec4 *)&Mat4::I(0,0);  // Mat4::I.row(0)
    ball->setSizes[CameraAxes] = 3;

    ball->sets[BodyAxes] = (Vec4 *)&ball->mDown(0,0);  // mDown.row(0)
    ball->setSizes[BodyAxes] = 3;

    ball->sets[OtherAxes] = &otherAxis;
    ball->setSizes[OtherAxes] = 1;
}

/* Set the center and size of the controller. */
void Ball_Place(BallData *ball, Vec4 center, double radius)
{
    ball->center = center;
    ball->radius = radius;
}

/* Incorporate new mouse position. */
void Ball_Mouse(BallData *ball, Vec4 vNow)
{
    ball->vNow = vNow;
}

/* Choose a constraint set, or none. */
void Ball_UseSet(BallData *ball, AxisSet axisSet)
{
    /* if (!ball->dragging) */
	ball->axisSet = axisSet;
}

/* Begin drawing arc for all drags combined. */
void Ball_ShowResult(BallData *ball)
{
    ball->showResult = TRUE;
}

/* Stop drawing arc for all drags combined. */
void Ball_HideResult(BallData *ball)
{
    ball->showResult = FALSE;
}

/* Using vDown, vNow, dragging, and axisSet, compute rotation etc. */
/* To use closest constraint axis, set aI to -1 */
void Ball_Update(BallData *ball, int aI)
{
    int setSize = ball->setSizes[ball->axisSet];
    Vec4 *set = (Vec4 *)(ball->sets[ball->axisSet]);
    ball->vFrom = MouseOnSphere(ball->vDown, ball->center, ball->radius);
    ball->vTo = MouseOnSphere(ball->vNow, ball->center, ball->radius);
    if (ball->dragging) {
        if (ball->axisSet!=NoAxes) {
	    aI = (aI==-1) ? ball->axisIndex : aI;
            ball->vFrom = ConstrainToAxis(ball->vFrom, set[aI]);
            ball->vTo = ConstrainToAxis(ball->vTo, set[aI]);
        }
        ball->qDrag =Qt_FromBallPoints(ball->vFrom, ball->vTo, ball->ballType);
        ball->qNow = Qt_Mul(ball->qDrag, ball->qDown);
    } else {
        if (ball->axisSet!=NoAxes) {
            ball->axisIndex = (aI==-1) ?
	                      NearestConstraintAxis(ball->vTo, set, setSize) :
	                      aI;
        }
    }
    Qt_ToBallPoints(ball->qDown, &ball->vrFrom, &ball->vrTo);
    Qt_ToMatrix(Qt_Conj(ball->qNow), ball->mNow); /* Gives transpose for GL. */
}

/* Returns TRUE iff vMouse is a point off the arcball sphere */
int Ball_MouseOutsideSphere(BallData *ball, Vec4 vMouse)
{
    Vec4  vTest = MouseOnSphere(vMouse, ball->center, ball->radius);

    return vTest[Z] == 0.0;
}

/* Set the BallType: 'ArcBall' rotates through _twice_ the angle
 * between the two points on the sphere, and exhibits no
 * hysteresis. 'TrackBall' rotates through exactly the angle between
 * the two points on the sphere, like a physical trackball, and
 * consequently exhibits hysteresis. */
void Ball_SetBallType(BallData *ball, BallType bt)
{
    ball->ballType = bt;
}

/* Return rotation matrix defined by controller use. */
void Ball_Value(BallData *ball, HMatrix& mNow)
{
    mNow = ball->mNow;
}


/* Begin drag sequence. */
void Ball_BeginDrag(BallData *ball)
{
    ball->dragging = TRUE;
    ball->vDown = ball->vNow;
}

/* Stop drag sequence. */
void Ball_EndDrag(BallData *ball)
{
    ball->dragging = FALSE;
    ball->qDown = ball->qNow;

    ball->mDown = ball->mNow;
}

/* Draw the controller with all its arcs. */
void Ball_Draw(BallData *ball, int drawBall, int drawConstraints, int drawArc)
{
    GLdouble __save[16];
    float r = ball->radius;
    
    glPushAttrib(GL_LIGHTING_BIT|GL_CURRENT_BIT);
    glDisable(GL_LIGHTING);

    glMatrixMode(GL_PROJECTION);
    glGetDoublev(GL_PROJECTION_MATRIX, __save);	//  vs. glPushMatrix();
    glLoadIdentity();

    // Must use near and far clip planes at -1.0 and 1.0 or
    // risk clipping out the circle and arcs drawn.
    // glOrtho(-1.0,1.0,-1.0,1.0,-1.0,1.0);

    if (ball->aspect > 1)
	glOrtho(-ball->aspect, ball->aspect, -1.0, 1.0, -1.0, 1.0);
    else
	glOrtho(-1.0, 1.0, -1.0/ball->aspect, 1.0/ball->aspect, -1.0, 1.0);


    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glScalef(r,r,r);

    RIMCOLOR();
    if( drawBall ) {
	glBegin(GL_LINE_LOOP);
	for (int i=0; i < 36; i++)
	    glVertex3f(cos((float)i*2.0*M_PI/36.0),
		       sin((float)i*2.0*M_PI/36.0),0.0);
	glEnd();
    } 

    if( drawArc ) Ball_DrawResultArc(ball);
    if( drawConstraints ) Ball_DrawConstraints(ball);
    if( drawArc ) Ball_DrawDragArc(ball);

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(__save);	// vs. glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glPopAttrib();
}

/* Draw an arc defined by its ends. */
void DrawAnyArc(Vec4 vFrom, Vec4 vTo)
{
    int i;
    Vec4 pts[NSEGS+1];
    double dot;
    pts[0] = vFrom;
    pts[1] = pts[NSEGS] = vTo;
    for (i=0; i<LG_NSEGS; i++) pts[1] = V3_Bisect(pts[0], pts[1]);
    dot = 2.0*V3_Dot(pts[0], pts[1]);
    for (i=2; i<NSEGS; i++) {
	pts[i] = V3_Sub(V3_Scale(pts[i-1], dot), pts[i-2]);
    }
    glBegin(GL_LINE_STRIP);
    for (i=0; i<=NSEGS; i++)
	glVertex3fv((float *)&pts[i]);
    glEnd();
}

/* Draw the arc of a semi-circle defined by its axis. */
void DrawHalfArc(Vec4 n)
{
    Vec4 p, m;
    p[Z] = 0;
    if (n[Z] != 1.0) {
	p[X] = n[Y]; p[Y] = -n[X];
	p = V3_Unit(p);
    } else {
	p[X] = 0; p[Y] = 1;
    }
    m = V3_Cross(p, n);
    DrawAnyArc(p, m);
    DrawAnyArc(m, V3_Negate(p));
}

/* Draw all constraint arcs. */
void Ball_DrawConstraints(BallData *ball)
{
    ConstraintSet set;
    Vec4 axis;
    int i, axisI, setSize = ball->setSizes[ball->axisSet];
    if (ball->axisSet==NoAxes) return;
    set = ball->sets[ball->axisSet];
    for (axisI=0; axisI<setSize; axisI++) {
	if (ball->axisIndex!=axisI) {
	    if (ball->dragging) continue;
	    FARCOLOR();
	} else NEARCOLOR();
	axis = *(Vec4 *)&set[4*axisI];
	if (axis[Z]==1.0) {
	    glBegin(GL_LINE_LOOP);
	    for (i=0; i < 36; i++) 
		glVertex3f(cos((float)i*2.0*M_PI/36.0),sin((float)i*M_PI/36.0),0.0);
	    glEnd();
	} else {
	    DrawHalfArc(axis);
	}
    }
}

/* Draw "rubber band" arc during dragging. */
void Ball_DrawDragArc(BallData *ball)
{
    DRAGCOLOR();
    if (ball->dragging) DrawAnyArc(ball->vFrom, ball->vTo);
}

/* Draw arc for result of all drags. */
void Ball_DrawResultArc(BallData *ball)
{
    RESCOLOR();
    if (ball->showResult) DrawAnyArc(ball->vrFrom, ball->vrTo);
}



MxArcball::~MxArcball()
{
    if( ball ) Ball_Free(ball);
}

void MxArcball::init(const MxBounds& bounds, float aspect)
{
    if( ball ) Ball_Free(ball);  // Don't try to reuse old Balls
    ball = Ball_Alloc();
    Ball_Init(ball, aspect);

    Ball_UseSet(ball, NoAxes);
    Ball_SetBallType(ball, ArcBall);

    center[X] = bounds.center[X];
    center[Y] = bounds.center[Y];
    center[Z] = bounds.center[Z];
    center[W] = 1;

    radius = bounds.radius;
    cam[X] = cam[Y] = cam[Z] = 0.0f;
    dragging = NoDrag;

    Ball_Place(ball, qOne, 0.9 /* 0.75 */);
}

bool MxArcball::mouse_down(int button, int x, int y, int kind)
{
    mousex = startx = x;
    mousey = starty = y;

    if( kind&MX_SHIFTKEY )
    {
	dragging = LightAdjust;
    }
    else if( button == 1 )
    {
        int width, height;
        glGetViewport(NULL, NULL, &width, &height);

        vNow[X] = 2.0*startx/(float)width-1.0;
        vNow[Y] = -2.0*starty/(float)height+1.0;

        Ball_Mouse(ball, vNow);

        Ball_BeginDrag(ball);

        dragging = Spin;
    }
    else if( button == 2 )
    {
        dragging = Dolly;
    }
    else if( button == 3 )
    {
        dragging = Zoom;
    }

    return true;
}

bool MxArcball::mouse_up(int /*button*/, int /*x*/, int /*y*/, int /*kind*/)
{
    startx = mousex = 0;
    starty = mousey = 0;

    if( dragging == Spin )
        Ball_EndDrag(ball);

    dragging = NoDrag;

    return false;
}

bool MxArcball::motion(int x, int y)
{
    if( !dragging )
        return false;

    if( dragging == Spin )
    {
        mousex = x;
        mousey = y;

        int width, height;
        glGetViewport(NULL, NULL, &width, &height);

        vNow[X] = 2.0*mousex/(float)width-1.0;
        vNow[Y] = -2.0*mousey/(float)height+1.0;

        Ball_Mouse(ball, vNow);
    }
    else if( dragging == Dolly )
    {
	int width, height;
	glGetViewport(NULL, NULL, &width, &height);

	cam[X] += radius * (float)(x - mousex) / (float)width;
	cam[Y] += radius * (float)(mousey - y) / (float)height;

        mousex = x;
        mousey = y;
    }
    else if( dragging == Zoom )
    {
        cam[Z] += 0.02*radius*(float)(y - mousey);
        mousey = y;
    }
    else if( dragging == LightAdjust )
    {
	int width, height;
	glGetViewport(NULL, NULL, &width, &height);

	float light_pos[4];
	glGetLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	light_pos[X] += (x - mousex) / (float)width;
	light_pos[Y] += (mousey - y) / (float)height;
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
	mousex = x;
	mousey = y;
    }

    return true;
}

void MxArcball::apply()
{
    Ball_Update(ball, -1);

    GLint last_mode; glGetIntegerv(GL_MATRIX_MODE, &last_mode);

    Ball_Draw(ball, 0, 0, 0);


    glMatrixMode(GL_PROJECTION);
    glGetDoublev(GL_PROJECTION_MATRIX, __proj_save); // vs. glPushMatrix();
    glTranslatef(cam[X], cam[Y], cam[Z]);


    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

         // glLoadIdentity();
         // panning translation
         // gluLookAt()??

    HMatrix mNow;
    Ball_Value(ball, mNow);

    glTranslatef(center[X], center[Y], center[Z]);
    glMultMatrixd((double *)&mNow);
    glTranslatef(-center[X], -center[Y], -center[Z]);

    glMatrixMode((GLenum)last_mode);
}

void MxArcball::unapply()
{
    GLint last_mode; glGetIntegerv(GL_MATRIX_MODE, &last_mode);

    glMatrixMode(GL_PROJECTION);
    glLoadMatrixd(__proj_save);	// vs. glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    glMatrixMode((GLenum)last_mode);
}

void MxArcball::draw()
{
    Ball_Draw(ball, true, true, true);
}

double *MxArcball::get_qnow() { return ball->qNow; }
