/************************************************************************

  MxGLPane

  Copyright (C) 1998 Michael Garland.  See "COPYING.txt" for details.
  
  $Id: MxGLPane.cxx,v 1.29 1999/01/22 21:55:26 garland Exp $

 ************************************************************************/

#include "stdmix.h"
#include "mixio.h"
#include "MxMath.h"
#include "MxGLPane.h"

uint mxgl_default_pane_width = 800;
uint mxgl_default_pane_height = 600;

#ifdef WIN32

MxGLApp *MxGLApp::application = NULL;
bool (*MxGLApp::init_hook)(MxGLApp *) = NULL;

void MxGLApp::bind_pane(MxGLPane *pane, char *name)
{
    m_pMainWnd = pane;
    pane->create(name);
    m_pMainWnd->ShowWindow(m_nCmdShow);
    m_pMainWnd->UpdateWindow();
}

const char *MxGLApp::query_filename(const char *prompt, const char *cwd,
				    const char *filter, const char *default_name)
{
    static CString filename;
    char *ext = strchr(filter, '.');
    if( !ext ) ext = ""; else ext++;

    CString converted_cwd(cwd);
    for(int i=0; i<converted_cwd.GetLength(); i++)
	if( converted_cwd[i] == '/' )  converted_cwd.SetAt(i, '\\');

    CFileDialog dlg(true, ext, NULL,
	OFN_HIDEREADONLY,
	"||");
    dlg.m_ofn.lpstrInitialDir = converted_cwd;

    if( dlg.DoModal() == IDOK )
    {
	filename = dlg.GetPathName();
	return filename;
    }
    else
	return NULL;
}

#include "MxDynBlock.h"

BOOL MxGLApp::InitInstance()
{
    bool result;

    // Split command line into argc/argv 
    MxDynBlock<char *> args(10);
    char *cmdline = strdup(m_lpCmdLine);  // Don't modify original
    const char *ws=" \t";
    char *token = strtok(cmdline, ws);
    while( token )
    {
	args.add(token);
	token = strtok(NULL, ws);
    }

    argc = args.length() + 1;
    argv = new char*[argc];
    argv[0] = strdup(m_pszAppName);
    for(int i=1; i<argc; i++) { argv[i]=args[i-1]; }

    if( MxGLApp::init_hook )
	result = (init_hook)(this);
    else
	result = application->init_application();

    return (BOOL)result;    
}


BEGIN_MESSAGE_MAP(MxGLPane, CFrameWnd)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_WM_LBUTTONDOWN()
    ON_WM_LBUTTONUP()
    ON_WM_RBUTTONDOWN()
    ON_WM_RBUTTONUP()
    ON_WM_MBUTTONDOWN()
    ON_WM_MBUTTONUP()
    ON_WM_MOUSEMOVE()
    ON_WM_CHAR()
    ON_WM_TIMER()
END_MESSAGE_MAP()

MxGLPane::MxGLPane()
{
    gl_context = NULL;
    pixel_format = 0;
    active_dc = NULL;

    background_color[0] = 0.3f;
    background_color[1] = 0.3f;
    background_color[2] = 1.0f;
}

void MxGLPane::create(char *name)
{
    Create(NULL, name);
}

bool MxGLPane::create_glcontext(HDC hDC)
{
    gl_context = wglCreateContext(hDC);
    if( !gl_context )
	return false;

    if( !wglMakeCurrent(hDC, gl_context) )
	return false;

    return true;
}

bool MxGLPane::set_pixel_format(HDC hDC)
{
    PIXELFORMATDESCRIPTOR pixelDesc;

    //
    // These are the important fields of the PFD
    //
    pixelDesc.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pixelDesc.nVersion = 1;

    pixelDesc.dwFlags =
	PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
	PFD_DOUBLEBUFFER | PFD_STEREO_DONTCARE;  
    pixelDesc.iPixelType = PFD_TYPE_RGBA;
    pixelDesc.cColorBits = 24;
    pixelDesc.iLayerType = PFD_MAIN_PLANE;


    //
    // According to the docs, these can be/are ignored.
    //
    pixelDesc.cRedBits = 8;
    pixelDesc.cRedShift = 16;
    pixelDesc.cGreenBits = 8;
    pixelDesc.cGreenShift = 8;
    pixelDesc.cBlueBits = 8;
    pixelDesc.cBlueShift = 0;
    pixelDesc.cAlphaBits = 0;
    pixelDesc.cAlphaShift = 0;
    pixelDesc.cAccumBits = 0;	
    pixelDesc.cAccumRedBits = 0;
    pixelDesc.cAccumGreenBits = 0;
    pixelDesc.cAccumBlueBits = 0;
    pixelDesc.cAccumAlphaBits = 0;
    pixelDesc.cDepthBits = 32;
    pixelDesc.cStencilBits = 0;
    pixelDesc.cAuxBuffers = 0;
    pixelDesc.bReserved = 0;
    pixelDesc.dwLayerMask = 0;
    pixelDesc.dwVisibleMask = 0;
    pixelDesc.dwDamageMask = 0;

    pixel_format = ChoosePixelFormat(hDC, &pixelDesc);
    if( !pixel_format )
    {
	// Try and guess a decent default pixel format
	pixel_format = 1;
	if( !DescribePixelFormat(hDC, pixel_format,
				 sizeof(PIXELFORMATDESCRIPTOR), &pixelDesc) )
	    return false;
   }

    if( !SetPixelFormat(hDC, pixel_format, &pixelDesc) )
	return false;

    return true;
}

BOOL MxGLPane::PreCreateWindow(CREATESTRUCT& cs)
{
    // Need to set special style requirements for OpenGL windows
    cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    return CFrameWnd::PreCreateWindow(cs);
}

int MxGLPane::OnCreate(LPCREATESTRUCT cs)
{
    if( CFrameWnd::OnCreate(cs) == -1 )
	return -1;

    HWND wnd = GetSafeHwnd();
    HDC dc = ::GetDC(wnd);

    if( !set_pixel_format(dc) )
	MessageBox("Failed to set up proper pixel format.");
    if( !create_glcontext(dc) )
	MessageBox("Failed to create GL context.");

    gl_init();

    return 0;
}

void MxGLPane::OnDestroy()
{
    if( wglGetCurrentContext() )
	wglMakeCurrent(NULL, NULL);

    if( gl_context )
    {
	wglDeleteContext(gl_context);
	gl_context = NULL;
    }

    CFrameWnd::OnDestroy();
}

void MxGLPane::OnSize(UINT /*type*/, int cx, int cy)
{
    size(cx, cy);
}

BOOL MxGLPane::OnEraseBkgnd(CDC *)
{
    return FALSE;   // Don't want background to be erased thank you
}

void MxGLPane::do_redraw(CDC& dc)
{
    make_current(dc.GetSafeHdc());
    active_dc = &dc;
    begin_redraw();
    redraw();
    end_redraw();
    finish(dc.GetSafeHdc());
    dc.SetBkMode(TRANSPARENT); // Draw over the scene image 
    illustrate();
    active_dc = NULL;
}

void MxGLPane::OnPaint()
{
    CPaintDC dc(this);
    do_redraw(dc);
}

void MxGLPane::get_canvas_geometry(int *x, int *y, int *w, int *h)
{
    CRect rect;    

    GetClientRect(rect);
    if( x )  *x = rect.TopLeft().x;
    if( y )  *y = rect.TopLeft().y;
    if( w )  *w = rect.Width();
    if( h )  *h = rect.Height();
}

void MxGLPane::draw_string(int x, int y, const char *str)
{
    if( active_dc )
    {
	active_dc->SetTextColor(fRGB(0.9f, 0.9f, 0.2f));
	active_dc->TextOut(x, y, str, strlen(str));
    }
}

void MxGLPane::require_redraw()
{
    CDC *dc = GetDC();
    if( !dc ) return;  // Failed to get valid DC!

    do_redraw(*dc);
    ReleaseDC(dc);
    
    // Alternative implementation: just SendMessage(WM_PAINT)
    // However, this doesn't quite turn out the way I'd like.
    // For instance, the text drawn in illustrate() by QVis
    // is not (always?) visible.
}

void MxGLPane::post_redraw()
{
    InvalidateRect(NULL, FALSE);
}

void MxGLPane::handle_mouse(int kind, UINT flags, CPoint point)
{
    int which = 0;

    if( flags&MK_MBUTTON || (flags&MK_LBUTTON && flags&MK_RBUTTON) )
	which = 2;
    else if( flags&MK_LBUTTON )
	which = 1;
    else if( flags&MK_RBUTTON )
	which = 3;

    if( flags&MK_SHIFT )  kind |= MX_SHIFTKEY;
    if( flags&MK_CONTROL) kind |= MX_CTRLKEY;
    // Assume that the ALT modifier key is used by the system.

    if( kind==MX_BUTTONDOWN )
	SetCapture();
    else if( kind==MX_BUTTONUP )
	ReleaseCapture();
    
    button(kind, which, point.x, point.y);
}

void MxGLPane::OnLButtonDown(UINT flags, CPoint point)
    { handle_mouse(MX_BUTTONDOWN, flags, point); }
void MxGLPane::OnLButtonUp(UINT flags, CPoint point)
    { handle_mouse(MX_BUTTONUP, flags, point); }
void MxGLPane::OnRButtonDown(UINT flags, CPoint point)
    { handle_mouse(MX_BUTTONDOWN, flags, point); }
void MxGLPane::OnRButtonUp(UINT flags, CPoint point)
    { handle_mouse(MX_BUTTONUP, flags, point); }
void MxGLPane::OnMButtonDown(UINT flags, CPoint point)
    { handle_mouse(MX_BUTTONDOWN, flags, point); }
void MxGLPane::OnMButtonUp(UINT flags, CPoint point)
    { handle_mouse(MX_BUTTONUP, flags, point); }
void MxGLPane::OnMouseMove(UINT flags, CPoint point)
    { handle_mouse(MX_POINTERMOTION, flags, point); }

void MxGLPane::OnChar(UINT ch, UINT repcount, UINT flags)
    { key(ch); }

void MxGLPane::OnTimer(UINT timer_id)
{
    timer(timer_id);
}

unsigned int MxGLPane::add_timer(unsigned int id, unsigned int millisecs)
{
    return SetTimer(id, millisecs, NULL);
}

bool MxGLPane::remove_timer(unsigned int id)
{
    return KillTimer(id)>0;
}

#else

static int GLXF_CFG_DOUBLEBUFFER[] =
{
    GLX_DOUBLEBUFFER,
    GLX_RGBA,
    GLX_RED_SIZE, 1,
    GLX_GREEN_SIZE, 1,
    GLX_BLUE_SIZE, 1,
    GLX_DEPTH_SIZE, 1,
    None
};

MxGLApp *MxGLApp::application = NULL;
bool (*MxGLApp::init_hook)(MxGLApp *) = NULL;

void MxGLApp::bind_pane(MxGLPane *pane, char *name)
{
    pane->create(name);
    pane->make_current();
}

const char *MxGLApp::query_filename(const char *prompt,
				    const char *cwd,
				    const char *filter,
				    const char *default_name)
{
    return fl_show_fselector(prompt, cwd, filter, default_name);
}

main(int argc, char **argv)
{
    if( !MxGLApp::application )
	fatal_error("Fatal Error: No application instance defined.");

    // I've disabled XForms command line parsing.  It was interfering
    // with my own command line parsing, for some reason that I'm not
    // entirely sure of.  It was screwing up the '-r' flag, when in fact,
    // that isn't even one of the XForms command line arguments.  Go figure.
    //
    int fl_argc = 1;
    char *fl_argv[2];  fl_argv[0]=argv[0];  fl_argv[1]=NULL;
    fl_initialize(&fl_argc, fl_argv, "MxGLApp", 0, 0);
    //fl_initialize(&argc, argv, "QVis", 0, 0);
    fl_set_glcanvas_defaults(GLXF_CFG_DOUBLEBUFFER);

    bool result;

    MxGLApp::application->argc = argc;
    MxGLApp::application->argv = argv;

    if( MxGLApp::init_hook )
	result = (*MxGLApp::init_hook)(MxGLApp::application);
    else
	result = MxGLApp::application->init_application();

    if( !result )
	fatal_error("Error during initialization ... Quitting.");

    while( true )
#if FL_INCLUDE_VERSION > 81
	fl_check_forms();
#else
	fl_do_forms();
#endif
}


MxGLPane::MxGLPane()
{
    gc = NULL;

    background_color[0] = 0.3f;
    background_color[1] = 0.3f;
    background_color[2] = 1.0f;
}

MxGLPane::~MxGLPane()
{
    if( gc )
	XFreeGC(fl_display, gc);
}

void MxGLPane::create(char *name, int *x, int *y, int *w, int *h)
{
    int W = mxgl_default_pane_width;
    int H = mxgl_default_pane_height;

    glpane = fl_bgn_form(FL_NO_BOX, W + 20, H + 20);
    fl_add_box(FL_UP_BOX, 0, 0, W + 20, H + 20, "");
    canvas = fl_add_glcanvas(FL_NORMAL_CANVAS, 10, 10, W, H, "");
    fl_end_form();

    //
    // Pass back the canvas geometry we've set up
    if( x ) *x = 10;   if( y ) *y = 10;
    if( w ) *w = W;  if( h ) *h = H;

    fl_add_canvas_handler(canvas, Expose, cb_expose, NULL);
    fl_add_canvas_handler(canvas, MapNotify, cb_mapnotify, NULL);
    fl_add_canvas_handler(canvas, ButtonPress, cb_button, NULL);
    fl_add_canvas_handler(canvas, ButtonRelease, cb_button, NULL);
    fl_add_canvas_handler(canvas, MotionNotify, cb_button, NULL);
    fl_add_canvas_handler(canvas, KeyPress, cb_key, NULL);
    fl_add_canvas_handler(canvas, ConfigureNotify, cb_configure, NULL);

    glpane_for_form(glpane, this);
    fl_set_form_position(glpane, 0, 0);
    fl_show_form(glpane, FL_PLACE_CENTER|FL_FREE_SIZE, FL_FULLBORDER, name);
}

void MxGLPane::get_canvas_geometry(int *x, int *y, int *w, int *h)
{
    int junk;
    fl_get_object_geometry(canvas, x?x:&junk, y?y:&junk, w?w:&junk, h?h:&junk);
}

void MxGLPane::glpane_for_form(FL_FORM *form, MxGLPane *pane)
{
    form->u_vdata = pane;
}

MxGLPane *MxGLPane::glpane_for_form(FL_FORM *form)
{
    return (MxGLPane *)form->u_vdata;
}

MxGLPane *MxGLPane::glpane_for_object(FL_OBJECT *obj)
{
    return glpane_for_form(obj->form);
}

static
inline XColor *fRGB(XColor& c, float r, float g, float b)
{
    c.red   = (unsigned short)(r * 65535);
    c.green = (unsigned short)(g * 65535);
    c.blue  = (unsigned short)(b * 65535);
    c.flags = DoRed|DoGreen|DoBlue;
    return &c;
}

int MxGLPane::cb_mapnotify(FL_OBJECT *canvas, Window win, int, int,
			   XEvent *, void *)
{
    MxGLPane *pane = glpane_for_object(canvas);


    XWindowAttributes attribs;
    XGetWindowAttributes(fl_display, win, &attribs);

    XGCValues gcv;
    gcv.font = XLoadFont(fl_display, "8x13");

    XColor fg;
    XAllocColor(fl_display, attribs.colormap, fRGB(fg, 0.9, 0.9, 0.2));
    gcv.foreground = fg.pixel;


    pane->gc = XCreateGC(fl_display, win, GCFont|GCForeground, &gcv);

    pane->make_current();
    pane->gl_init();
    return 0;
}

int MxGLPane::cb_configure(FL_OBJECT *canvas, Window,
			   int width , int height,
			   XEvent *, void *)
{
    MxGLPane *pane = glpane_for_object(canvas);
    pane->make_current();
    pane->size(width, height);
    return 0;
}
			   

int MxGLPane::cb_expose(FL_OBJECT *canvas, Window,
			int width, int height,
			XEvent *, void *)
{
    MxGLPane *pane = glpane_for_object(canvas);

    if( canvas->visible )
    {
	glViewport(0, 0, width, height);
	pane->post_redraw();
    }

    return 0;
}

int MxGLPane::cb_button(FL_OBJECT *canvas, Window, int, int,
			XEvent *event, void *)
{
    MxGLPane *pane = glpane_for_object(canvas);

    int kind;
    if( event->type == ButtonPress )
	kind = MX_BUTTONDOWN;
    else if( event->type == ButtonRelease )
	kind = MX_BUTTONUP;
    else
	kind = MX_POINTERMOTION;
    
    if( event->xbutton.state & ShiftMask )  kind |= MX_SHIFTKEY;
    if( event->xbutton.state & ControlMask )  kind |= MX_CTRLKEY;

    pane->button(kind, event->xbutton.button,
		 event->xbutton.x, event->xbutton.y);

    return 0;
}


int MxGLPane::cb_key(FL_OBJECT *canvas, Window, int, int,
		     XEvent *event, void *)
{
    MxGLPane *pane = glpane_for_object(canvas);

    char c;
    XLookupString(&event->xkey, &c, 1, NULL, NULL);
    pane->key(c);

    return 0;
}

void MxGLPane::post_redraw()
{
    make_current();

    begin_redraw();
    redraw();
    end_redraw();

    finish();

    illustrate();

    XFlush(fl_display);		// Cuts down a lot on flicker (not completely)
    //XSync(fl_display, False); // No flicker, but a good bit slower
}

void MxGLPane::require_redraw()
{
    // Under X, we always do the redraw right away anyway
    post_redraw();
}

#endif

MxRaster *MxGLPane::grab_snapshot()
{
    // ??BUG: Is this necessary?  The problem is that make_current() does
    //        not currently have a common cross-platform interface.
    //make_current();

    int x, y, w ,h; glGetViewport(&x, &y, &w, &h);

    MxRaster *img = new MxRaster(w-x, h-y, 3);
    glReadBuffer(GL_FRONT);
    glReadPixels(x, y, w, h, GL_RGB, GL_UNSIGNED_BYTE, (unsigned char*)(*img));
    img->vflip();

    return img;
}

void MxGLPane::snapshot_ppm(const char *name)
{
    MxRaster *img = grab_snapshot();

    const char *filename = 
	MxGLApp::application->query_filename("Save PPM Snapshot", ".",
					     "*.ppm", name?name:"snap.ppm");

    if( filename )
    {
        ofstream out(filename);
        pnm_write(out, *img);
        out.close();
    }

    delete img;
}

void MxGLPane::snapshot_tiff(const char *name)
{
    MxRaster *img = grab_snapshot();

    const char *filename = 
	MxGLApp::application->query_filename("Save TIFF Snapshot", ".",
					     "*.tif", name?name:"snap.tif");

    if( filename )
	if( !tiff_write(filename, *img) )
	    mxmsg_signal(MXMSG_WARN,
			 "Failed to write TIFF file.\n"
			 "TIFF may not be supported.");

    delete img;
}

////////////////////////////////////////////////////////////////////////
//
// Default handlers.  Note that these are platform independent.
//

void MxGLPane::gl_init()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);


    glMatrixMode(GL_PROJECTION);
    // load anything?
    glMatrixMode(GL_MODELVIEW);


    GLfloat mat_ambient[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat mat_diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
    GLfloat mat_specular[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 40);

    GLfloat light_pos[] = {0.0f, 0.0f, 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glEnable(GL_LIGHT0);

    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glColor3f(0.5f, 0.5f, 0.5f);
    glClearColor(background_color[0], background_color[1],
		 background_color[2], 0.0f);
}

void MxGLPane::begin_redraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glColor3f(0.5f, 0.5f, 0.5f); // Enforce the default surface color
}

void MxGLPane::redraw()
{

}

void MxGLPane::end_redraw()
{

}

void MxGLPane::illustrate()
{

}

void MxGLPane::key(char)
{

}

void MxGLPane::button(int, int ,int, int)
{

}

void MxGLPane::size(int width, int height)
{
    glViewport(0, 0, width, height);
}

void MxGLPane::timer(unsigned int) { }
