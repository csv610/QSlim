#ifndef MXGLPANE_INCLUDED // -*- C++ -*-
#define MXGLPANE_INCLUDED
#if !defined(__GNUC__)
#  pragma once
#endif

/************************************************************************

  MxGLPane

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxGLPane.h,v 1.21 1999/01/22 19:45:12 garland Exp $

 ************************************************************************/

#ifdef WIN32
//
// Include MFC stuff now so that <windows.h> doesn't get included for GL
//
#  define VC_EXTRALEAN
#  include <afxwin.h>	    // MFC core and standard components
#  include <afxext.h>	    // MFC extensions
#endif

#include "MxGL.h"
#include "MxRaster.h"

#define MX_BUTTONDOWN 1
#define MX_BUTTONUP 2
#define MX_POINTERMOTION 3
// Keep lowest digit unused to avoid clashes with the event symbols
#define MX_CTRLKEY 0x10
#define MX_SHIFTKEY 0x20

// Specific ASCII keycodes
//
// BUG: This is not an appropriate home for this key stuff.
//      At some point, we should consolidate this and related items
//      into a unified event module.
//
#define MXKEY_BS  0x08
#define MXKEY_DEL 0x7f
#define MXKEY_NL  0x0a
#define MXKEY_CR  0x0d

#if defined(WIN32)

// Handy alternative to RGB()
inline COLORREF fRGB(float r, float g, float b)
{
    return RGB((uint)(r*255.0f), (uint)(g*255.0f), (uint)(b*255.0f));
}

class MxGLPane : public CFrameWnd
{
private:
    HGLRC gl_context;
    int pixel_format;
    CDC *active_dc;

    bool set_pixel_format(HDC);
    bool create_glcontext(HDC);

    void handle_mouse(int kind, UINT flags, CPoint point);
    void do_redraw(CDC& dc);

protected:  // MFC virtual function overrides
    BOOL PreCreateWindow(CREATESTRUCT& cs);

protected:  // MFC event handlers
    afx_msg void OnPaint();
    afx_msg BOOL OnEraseBkgnd(CDC *);
    afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnDestroy();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLButtonDown(UINT flags, CPoint point);
    afx_msg void OnLButtonUp(UINT flags, CPoint point);
    afx_msg void OnRButtonDown(UINT flags, CPoint point);
    afx_msg void OnRButtonUp(UINT flags, CPoint point);
    afx_msg void OnMButtonDown(UINT flags, CPoint point);
    afx_msg void OnMButtonUp(UINT flags, CPoint point);
    afx_msg void OnMouseMove(UINT flags, CPoint point);
    afx_msg void OnChar(UINT ch, UINT repcount, UINT flags);
    afx_msg void OnTimer(UINT timer_id);
    DECLARE_MESSAGE_MAP ()

public:
    float background_color[3];

public:
    MxGLPane();

    void create(char *name);
    inline void make_current(HDC dc) { wglMakeCurrent(dc, gl_context); }
    inline void finish(HDC dc) { SwapBuffers(dc); }
    void get_canvas_geometry(int *x, int *y, int *w, int *h);
    void draw_string(int x, int y, const char *str);

    MxRaster *grab_snapshot();
    void snapshot_ppm(const char *name=NULL);
    void snapshot_tiff(const char *name=NULL);

    virtual void gl_init();
    virtual void begin_redraw();
    virtual void end_redraw();

    void post_redraw();
    void require_redraw();
    virtual void redraw();
    virtual void illustrate();
    virtual void key(char);
    virtual void button(int kind, int which, int x, int y);
    virtual void size(int width, int height);

    unsigned int add_timer(unsigned int id, unsigned int millisecs);
    bool remove_timer(unsigned int id);
    virtual void timer(unsigned int);
};

class MxGLApp : public CWinApp
{

public:
    BOOL InitInstance(); // MFC interface


    static MxGLApp *application;
    static bool (*init_hook)(MxGLApp *);

    int argc;
    char **argv;

    MxGLApp(bool (*h)(MxGLApp *)=NULL)
    {
	application = this;
	init_hook = h;
    }

    void bind_pane(MxGLPane *, char *name);
    const char *query_filename(const char *prompt, const char *cwd,
			       const char *filter, const char *default_name);

    virtual bool init_application() { return true; }
};


#else
#ifdef MIX_USE_XFORMS
#ifdef MIX_USE_X11_FORMS
#include <X11/forms.h>
#else
#include <forms.h>
#endif
#endif

#ifdef MIX_USE_XFORMS
class MxGLPane
{
private:
    FL_FORM *glpane;
    FL_OBJECT *canvas;
    GC gc;

    static MxGLPane *glpane_for_form(FL_FORM *);
    static void glpane_for_form(FL_FORM *, MxGLPane *);
    static MxGLPane *glpane_for_object(FL_OBJECT *);

    static int cb_mapnotify(FL_OBJECT *, Window, int, int, XEvent *, void *);
    static int cb_configure(FL_OBJECT *, Window, int, int, XEvent *, void *);
    static int    cb_expose(FL_OBJECT *, Window, int, int, XEvent *, void *);
    static int    cb_button(FL_OBJECT *, Window, int, int, XEvent *, void *);
    static int       cb_key(FL_OBJECT *, Window, int, int, XEvent *, void *);

public:
    float background_color[3];

public:
    MxGLPane();
    virtual ~MxGLPane();

    void create(char *name,int *x=NULL,int *y=NULL,int *w=NULL,int *h=NULL);
    inline void make_current()
	{
	    glXMakeCurrent(fl_display,
			   fl_get_canvas_id(canvas),
			   fl_get_glcanvas_context(canvas));
	}
    inline void finish() {glXSwapBuffers(fl_display,fl_get_canvas_id(canvas));}
    void get_canvas_geometry(int *x, int *y, int *w, int *h);
    void draw_string(int x, int y, char *str)
	{
	    XDrawString(fl_display, fl_get_canvas_id(canvas), gc,
			x, y+canvas->bw, str, strlen(str));
	}

    MxRaster *grab_snapshot();
    void snapshot_ppm(const char *name=NULL);
    void snapshot_tiff(const char *name=NULL);

    virtual void gl_init();
    virtual void begin_redraw();
    virtual void end_redraw();

    void post_redraw();
    void require_redraw();
    virtual void redraw();
    virtual void illustrate();
    virtual void key(char);
    virtual void button(int kind, int which, int x, int y);
    virtual void size(int width, int height);
    virtual void timer(unsigned int);
};

class MxGLApp
{
public:
    static MxGLApp *application;
    static bool (*init_hook)(MxGLApp *);

    int argc;
    char **argv;

    MxGLApp(bool (*h)(MxGLApp *)=NULL)
	{
	    application = this;
	    init_hook = h;
	}

    void bind_pane(MxGLPane *, char *name);
    const char *query_filename(const char *prompt, const char *cwd,
			       const char *filter, const char *default_name);

    virtual bool init_application() { return true; }
};
#endif
#endif

// MXGLPANE_INCLUDED
#endif
