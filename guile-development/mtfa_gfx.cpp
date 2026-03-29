#include <iostream>
#include <string>

#include <stdlib.h>
#include <libguile.h>

#include <list>

#include <X11/Xlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <mtfa_utils.h>

/*
A simple graphics library for CSE 20211 by Douglas Thain

This work is licensed under a Creative Commons Attribution 4.0 International License.  https://creativecommons.org/licenses/by/4.0/

For course assignments, you should not change this file.
For complete documentation, see:
http://www.nd.edu/~dthain/courses/cse20211/fall2013/gfx
Version 3, 11/07/2012 - Now much faster at changing colors rapidly.
Version 2, 9/23/2011 - Fixes a bug that could result in jerky animation.
*/

#include <X11/Xlib.h>
#include <X11/X.h>

class gfx {
private:
public:
    bool closed;
    Display *gfx_display;
    Window  gfx_window;
    GC      gfx_gc;
    Colormap gfx_colormap;
    int      gfx_fast_color_mode;
    int saved_xpos;
    int saved_ypos;
    
    double Sx;
    double Sy;

    gfx() {
        gfx_display=0;
        gfx_fast_color_mode=0;
        saved_xpos=0;
        saved_ypos=0;
        closed=false;
        Sx = 1;
        Sy = 1;
    };
    ~gfx();
    
    bool gfx_open( int xpos, int ypos, int width, int height, int border_width, unsigned long border, unsigned long background, const char *title);
    void gfx_move_window( int x, int y);
//     void gfx_set_background( int x);
//     void gfx_set_foreground( int x);
    void gfx_point( int x, int y);
    void gfx_line( int x1, int y1, int x2, int y2);
    void gfx_fill_arc( int x, int y, unsigned int width, unsigned int heigth, int angle1, int angle2);
    void gfx_arc( int x, int y, unsigned int width, unsigned int heigth, int angle1, int angle2);
    void gfx_rect( int x, int y, unsigned int width, unsigned int heigth);
    void gfx_fill_rect( int x, int y, unsigned int width, unsigned int heigth);
    void gfx_string( int x, int y, _Xconst char* s, int length);
    void gfx_color( int r, int g, int b);
    void gfx_clear();
    void gfx_clear_color( int r, int g, int b);
    int gfx_event_waiting();
    char gfx_wait();
    char gfx_idle();
    int gfx_xpos();
    int gfx_ypos();
    void gfx_flush();
    inline int X(int x){return ceil(x/Sx);}
    inline int Y(int y){return ceil(y/Sy);}
};


/*
gfx_open creates several X11 objects, and stores them in globals
for use by the other functions in the library.
*/

/* Open a new graphics window. */
bool gfx::gfx_open( int xpos, int ypos, int width, int height, int border_width, unsigned long border, unsigned long background, const char *title )
{
    //Dato che xpos e yypos non so ma non funzionano, li uso per le dimensioni della viewport
    //quindi per una finestra di width e height ho una viewport di xpos e ypos
    //entro con viewport e ottengo window
    
// (defun ManageVP (Xvm Yvm Xwm Ywm)
//   (let ((Sx (/ Xvm Xwm))
// 	(Sy (/ Yvm Ywm)))
//     (lambda
//       (Xw Yw)
//        (values (/ Xw Sx) (/ Yw Sy)))))
    Sx = (double)xpos / (double)width;
    Sy = (double)ypos / (double)height;

	gfx_display = XOpenDisplay(0);
	if(!gfx_display) {
		//fprintf(stderr,"gfx_open: unable to open the graphics window.\n");
		return false;
	}

	Visual *visual = DefaultVisual(gfx_display,0);
	if(visual && visual->c_class == TrueColor) {
		gfx_fast_color_mode = 1;
	} else {
		gfx_fast_color_mode = 0;
	}

	//int blackColor = BlackPixel(gfx_display, DefaultScreen(gfx_display));
	int whiteColor = WhitePixel(gfx_display, DefaultScreen(gfx_display));

	gfx_window = XCreateSimpleWindow(gfx_display, DefaultRootWindow(gfx_display), 0, 0, width, height, border_width, border, background);

	XSetWindowAttributes attr;
	attr.backing_store = Always;

	XChangeWindowAttributes(gfx_display,gfx_window,CWBackingStore,&attr);

	XStoreName(gfx_display,gfx_window,title);

	XSelectInput(gfx_display, gfx_window, StructureNotifyMask|KeyPressMask|ButtonPressMask);

	XMapWindow(gfx_display,gfx_window);

	gfx_gc = XCreateGC(gfx_display, gfx_window, 0, 0);

	gfx_colormap = DefaultColormap(gfx_display,0);

	XSetForeground(gfx_display, gfx_gc, whiteColor);

	// Wait for the MapNotify event

	for(;;) {
		XEvent e;
		XNextEvent(gfx_display, &e);
		if (e.type == MapNotify)
			break;
	}
    return true;
}

//Move window
void gfx::gfx_move_window( int x, int y)
{
	XMoveWindow(gfx_display, gfx_window, x, y);
}
    
// //Cambia background
// void gfx::gfx_set_background( int x)
// {
//     XSetBackground(gfx_display, gfx_window, x);
// }
// 
// //Cambia foreground
// void gfx::gfx_set_foreground( int x)
// {
//     XSetForeground(gfx_display, gfx_window, x);
// }

/* Draw a single point at (x,y) */
void gfx::gfx_point( int x, int y)
{
	XDrawPoint(gfx_display,gfx_window,gfx_gc,X(x), Y(y));
}

/* Draw a line from (x1,y1) to (x2,y2) */

void gfx::gfx_line( int x1, int y1, int x2, int y2)
{
	XDrawLine(gfx_display,gfx_window,gfx_gc,X(x1),Y(y1),X(x2),Y(y2));
}

void gfx::gfx_arc( int x, int y, unsigned int width, unsigned int heigth, int angle1, int angle2)
{
    XDrawArc(gfx_display,gfx_window,gfx_gc,X(x), Y(y), X(width), Y(heigth), angle1, angle2);
}
void gfx::gfx_fill_arc( int x, int y, unsigned int width, unsigned int heigth, int angle1, int angle2)
{
    XFillArc(gfx_display,gfx_window,gfx_gc,X(x), Y(y), X(width), Y(heigth), angle1, angle2);
}
void gfx::gfx_rect( int x, int y, unsigned int width, unsigned int heigth)
{
    XDrawRectangle(gfx_display,gfx_window,gfx_gc,X(x), Y(y), X(width), Y(heigth));
}

void gfx::gfx_fill_rect( int x, int y, unsigned int width, unsigned int heigth)
{
    XFillRectangle(gfx_display,gfx_window,gfx_gc,X(x), Y(y), X(width), Y(heigth));
}

void gfx::gfx_string( int x, int y, _Xconst char* s, int length)
{
    XDrawString(gfx_display,gfx_window,gfx_gc,X(x), Y(y), s, X(length));
}


/* Change the current drawing color. */

void gfx::gfx_color( int r, int g, int b)
{

    XColor color;

	if(gfx_fast_color_mode) {
		/* If this is a truecolor display, we can just pick the color directly. */
		color.pixel = ((b&0xff) | ((g&0xff)<<8) | ((r&0xff)<<16) );
	} else {
		/* Otherwise, we have to allocate it from the colormap of the display. */
		color.pixel = 0;
		color.red = r<<8;
		color.green = g<<8;
		color.blue = b<<8;
		XAllocColor(gfx_display,gfx_colormap,&color);
	}

	XSetForeground(gfx_display, gfx_gc, color.pixel);
}

/* Clear the graphics window to the background color. */

void gfx::gfx_clear()
{
	XClearWindow(gfx_display,gfx_window);
}

gfx::~gfx()
{
    if (!closed)
    {
        XFreeGC(gfx_display, gfx_gc);
        XDestroyWindow(gfx_display,gfx_window);
        XCloseDisplay(gfx_display);	
    }
    closed=true;
}

/* Change the current background color. */
void gfx::gfx_clear_color( int r, int g, int b)
{
	XColor color;
	if(gfx_fast_color_mode) {
		/* If this is a truecolor display, we can just pick the color directly. */
		color.pixel = ((b&0xff) | ((g&0xff)<<8) | ((r&0xff)<<16) );
	} else {
		/* Otherwise, we have to allocate it from the colormap of the display. */
		color.pixel = 0;
		color.red = r<<8;
		color.green = g<<8;
		color.blue = b<<8;
		XAllocColor(gfx_display,gfx_colormap,&color);
	}

	XSetWindowAttributes attr;
	attr.background_pixel = color.pixel;
	XChangeWindowAttributes(gfx_display,gfx_window,CWBackPixel,&attr);
}

int gfx::gfx_event_waiting()
{
       XEvent event;

       gfx_flush();

       while (1) {
               if(XCheckMaskEvent(gfx_display,-1,&event)) {
                       if(event.type==KeyPress) {
                               XPutBackEvent(gfx_display,&event);
                               return 1;
                       } else if (event.type==ButtonPress) {
                               XPutBackEvent(gfx_display,&event);
                               return 1;
                       } else {
                               return 0;
                       }
               } else {
                       return 0;
               }
       }
}

/* Wait for the user to press a key or mouse button. */

char gfx::gfx_wait()
{
	XEvent event;

	gfx_flush();

	while(1) {
		XNextEvent(gfx_display,&event);

		if(event.type==KeyPress) {
			saved_xpos = event.xkey.x;
			saved_ypos = event.xkey.y;
			return XLookupKeysym(&event.xkey,0);
		} else if(event.type==ButtonPress) {
			saved_xpos = event.xkey.x;
			saved_ypos = event.xkey.y;
			return event.xbutton.button;
		}
	}
}

//Catches events of the window and exit
char gfx::gfx_idle()
{
	XEvent event;

	gfx_flush();

    XNextEvent(gfx_display,&event);

    if(event.type==KeyPress) {
        saved_xpos = event.xkey.x;
        saved_ypos = event.xkey.y;
        return XLookupKeysym(&event.xkey,0);
    } else if(event.type==ButtonPress) {
        saved_xpos = event.xkey.x;
        saved_ypos = event.xkey.y;
        return event.xbutton.button;
    } else
        return 0;
}


/* Return the X and Y coordinates of the last event. */

int gfx::gfx_xpos()
{
	return saved_xpos*Sx;
}

int gfx::gfx_ypos()
{
	return saved_ypos*Sy;
}

/* Flush all previous output to the window. */

void gfx::gfx_flush()
{
	XFlush(gfx_display);
}


/**************INTERFACCIA GUILE*****************/

static SCM mtfa_gfx_type;  //il tipo del su

static void finalize_mtfa_gfx( SCM mtfa_gfx)
{
    gfx * pt = ( gfx* ) scm_foreign_object_signed_ref ( mtfa_gfx, 0 );

    if ( pt )
    {
        scm_foreign_object_signed_set_x ( mtfa_gfx, 0, 0 );
        delete pt;
    }
}

#define C_STRING_TO_SYMBOL(str) scm_string_to_symbol(ScmFromString(str))

static SCM mtfa_make_gfx (SCM xpos, SCM ypos, SCM width, SCM height, SCM border_width, SCM border, SCM background, SCM title)
{
    gfx * pt = new gfx();
    bool ok = pt->gfx_open(scm_to_int(xpos),scm_to_int(ypos),scm_to_int(width),scm_to_int(height),scm_to_int(border_width), scm_to_ulong(border),scm_to_ulong(background),(const char*)scm_to_locale_string(title));
    if (ok)
        return scm_make_foreign_object_1 ( mtfa_gfx_type, ( void* ) pt );
    else
    {
        delete pt;
        return SCM_BOOL_F;
    }
}

static SCM mtfa_gfx_p ( SCM mtfa_gfx )
{
    auto const body = [] ( void* data )
    {
        scm_assert_foreign_object_type ( mtfa_gfx_type, * ( SCM* ) data );
        return SCM_BOOL_T;
    };
    auto const handler = [] ( void* data, SCM key, SCM args )
    {
        return SCM_BOOL_F;
    };
    
    SCM ret = scm_internal_catch ( SCM_BOOL_T, body, &mtfa_gfx, handler, nullptr );
    
    return ret;
}

static SCM mtfa_gfx_point(SCM mtfa_gfx, SCM x, SCM y)
{
	if (mtfa_gfx_p(mtfa_gfx) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa_gfx_point" , 0, mtfa_gfx, "mtfa-gfx type");
	gfx * pt = ( gfx* ) scm_foreign_object_signed_ref ( mtfa_gfx, 0 );
    
    pt->gfx_point(scm_to_int(x),scm_to_int(y));
	return SCM_BOOL_T;
}

static SCM mtfa_gfx_line(SCM mtfa_gfx, SCM x, SCM y, SCM x1, SCM y1)
{
	if (mtfa_gfx_p(mtfa_gfx) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa_gfx_line" , 0, mtfa_gfx, "mtfa-gfx type");
	
	gfx * pt = ( gfx* ) scm_foreign_object_signed_ref ( mtfa_gfx, 0 );
    
    pt->gfx_line(scm_to_int(x),scm_to_int(y), scm_to_int(x1),scm_to_int(y1));
	return SCM_BOOL_T;
}
static SCM mtfa_gfx_fill_arc(SCM mtfa_gfx, SCM x, SCM y, SCM width, SCM heigth, SCM angle1, SCM angle2)
{
	if (mtfa_gfx_p(mtfa_gfx) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa_gfx_fill_arc" , 0, mtfa_gfx, "mtfa-gfx type");
		gfx * pt = ( gfx* ) scm_foreign_object_signed_ref ( mtfa_gfx, 0 );
    
    pt->gfx_fill_arc(scm_to_int(x), scm_to_int(y), scm_to_uint(width), scm_to_uint(heigth), scm_to_int(angle1), scm_to_int(angle2));
	return SCM_BOOL_T;
}
static SCM mtfa_gfx_arc(SCM mtfa_gfx, SCM x, SCM y, SCM width, SCM heigth, SCM angle1, SCM angle2)
{
	if (mtfa_gfx_p(mtfa_gfx) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa_gfx_arc" , 0, mtfa_gfx, "mtfa-gfx type");
		gfx * pt = ( gfx* ) scm_foreign_object_signed_ref ( mtfa_gfx, 0 );
    
    pt->gfx_arc(scm_to_int(x), scm_to_int(y), scm_to_uint(width), scm_to_uint(heigth), scm_to_int(angle1), scm_to_int(angle2));
	return SCM_BOOL_T;
}
static SCM mtfa_gfx_rect(SCM mtfa_gfx, SCM x, SCM y, SCM width, SCM heigth)
{
	if (mtfa_gfx_p(mtfa_gfx) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa_gfx_rect" , 0, mtfa_gfx, "mtfa-gfx type");
		gfx * pt = ( gfx* ) scm_foreign_object_signed_ref ( mtfa_gfx, 0 );
    
    pt->gfx_rect(scm_to_int(x), scm_to_int(y), scm_to_uint(width), scm_to_uint(heigth));
	return SCM_BOOL_T;
}
static SCM mtfa_gfx_fill_rect(SCM mtfa_gfx, SCM x, SCM y, SCM width, SCM heigth)
{
	if (mtfa_gfx_p(mtfa_gfx) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa_gfx_fill_rect" , 0, mtfa_gfx, "mtfa-gfx type");
		gfx * pt = ( gfx* ) scm_foreign_object_signed_ref ( mtfa_gfx, 0 );
    
    pt->gfx_fill_rect(scm_to_int(x), scm_to_int(y), scm_to_uint(width), scm_to_uint(heigth));
	return SCM_BOOL_T;
}

static SCM mtfa_gfx_move_window(SCM mtfa_gfx, SCM x, SCM y)
{
	if (mtfa_gfx_p(mtfa_gfx) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa_gfx_string" , 0, mtfa_gfx, "mtfa-gfx type");
		gfx * pt = ( gfx* ) scm_foreign_object_signed_ref ( mtfa_gfx, 0 );
    
    pt->gfx_move_window(scm_to_int(x), scm_to_int(y));
	return SCM_BOOL_T;
}

static SCM mtfa_gfx_string(SCM mtfa_gfx, SCM x, SCM y, SCM s, SCM length)
{
	if (mtfa_gfx_p(mtfa_gfx) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa_gfx_string" , 0, mtfa_gfx, "mtfa-gfx type");
		gfx * pt = ( gfx* ) scm_foreign_object_signed_ref ( mtfa_gfx, 0 );
    
    pt->gfx_string(scm_to_int(x), scm_to_int(y), scm_to_locale_string(s), scm_to_int(length));
	return SCM_BOOL_T;
}

static SCM mtfa_gfx_color(SCM mtfa_gfx, SCM r, SCM g, SCM b)
{
	if (mtfa_gfx_p(mtfa_gfx) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa_gfx_color" , 0, mtfa_gfx, "mtfa-gfx type");
		gfx * pt = ( gfx* ) scm_foreign_object_signed_ref ( mtfa_gfx, 0 );
    
    pt->gfx_color(scm_to_int(r), scm_to_int(g), scm_to_int(b));
	return SCM_BOOL_T;
}
static SCM mtfa_gfx_clear_color(SCM mtfa_gfx, SCM r, SCM g, SCM b)
{
	if (mtfa_gfx_p(mtfa_gfx) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa_gfx_clear_color" , 0, mtfa_gfx, "mtfa-gfx type");
		gfx * pt = ( gfx* ) scm_foreign_object_signed_ref ( mtfa_gfx, 0 );
    
    pt->gfx_clear_color(scm_to_int(r), scm_to_int(g), scm_to_int(b));
	return SCM_BOOL_T;
}
static SCM mtfa_gfx_clear(SCM mtfa_gfx)
{
	if (mtfa_gfx_p(mtfa_gfx) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa_gfx_clear" , 0, mtfa_gfx, "mtfa-gfx type");
		gfx * pt = ( gfx* ) scm_foreign_object_signed_ref ( mtfa_gfx, 0 );
    
    pt->gfx_clear();
	return SCM_BOOL_T;
}
static SCM mtfa_gfx_flush(SCM mtfa_gfx)
{
	if (mtfa_gfx_p(mtfa_gfx) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa_gfx_flush" , 0, mtfa_gfx, "mtfa-gfx type");
		gfx * pt = ( gfx* ) scm_foreign_object_signed_ref ( mtfa_gfx, 0 );
    
    pt->gfx_flush();
	return SCM_BOOL_T;
}

static SCM mtfa_gfx_event_waiting(SCM mtfa_gfx)
{
	if (mtfa_gfx_p(mtfa_gfx) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa_gfx_event_waiting" , 0, mtfa_gfx, "mtfa-gfx type");
		gfx * pt = ( gfx* ) scm_foreign_object_signed_ref ( mtfa_gfx, 0 );
    
    int ret = pt->gfx_event_waiting();
	return scm_from_int(ret);
}

static SCM mtfa_gfx_wait(SCM mtfa_gfx)
{
	if (mtfa_gfx_p(mtfa_gfx) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa_gfx_wait" , 0, mtfa_gfx, "mtfa-gfx type");
		gfx * pt = ( gfx* ) scm_foreign_object_signed_ref ( mtfa_gfx, 0 );
    
    char ret = pt->gfx_wait();
	return scm_from_char(ret);
}
static SCM mtfa_gfx_idle(SCM mtfa_gfx)
{
	if (mtfa_gfx_p(mtfa_gfx) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa_gfx_idle" , 0, mtfa_gfx, "mtfa-gfx type");
		gfx * pt = ( gfx* ) scm_foreign_object_signed_ref ( mtfa_gfx, 0 );
    
    char ret = pt->gfx_idle();
	return scm_from_char(ret);
}
static SCM mtfa_gfx_xpos(SCM mtfa_gfx)
{
	if (mtfa_gfx_p(mtfa_gfx) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa_gfx_xpos" , 0, mtfa_gfx, "mtfa-gfx type");
		gfx * pt = ( gfx* ) scm_foreign_object_signed_ref ( mtfa_gfx, 0 );
    
    int ret = pt->gfx_xpos();
	return scm_from_int(ret);
}
static SCM mtfa_gfx_ypos(SCM mtfa_gfx)
{
	if (mtfa_gfx_p(mtfa_gfx) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa_gfx_ypos" , 0, mtfa_gfx, "mtfa-gfx type");
		gfx * pt = ( gfx* ) scm_foreign_object_signed_ref ( mtfa_gfx, 0 );
    
    int ret = pt->gfx_ypos();
	return scm_from_int(ret);
}

static SCM mtfa_gfx_close(SCM mtfa_gfx)
{
	if (mtfa_gfx_p(mtfa_gfx) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa_gfx_ypos" , 0, mtfa_gfx, "mtfa-gfx type");
		gfx * pt = ( gfx* ) scm_foreign_object_signed_ref ( mtfa_gfx, 0 );
    
    if (pt && !pt->closed)
    {
        XFreeGC(pt->gfx_display, pt->gfx_gc);
        XDestroyWindow(pt->gfx_display,pt->gfx_window);
        XCloseDisplay(pt->gfx_display);	
    }
    pt->closed=true;
	return SCM_BOOL_T;
}


extern "C" void init_mtfa_gfx()
{
    SCM name, slots;
    scm_t_struct_finalize finalizer;
    name = scm_from_latin1_symbol ( "mtfa-gfx" );
    slots = scm_list_1 ( scm_from_latin1_symbol ( "gfx" ) );
    finalizer = finalize_mtfa_gfx;
    mtfa_gfx_type = scm_make_foreign_object_type ( name, slots, finalizer );

    scm_c_define_gsubr("mtfa-gfx-close", 1,0,0, (void*)mtfa_gfx_close);
    scm_c_define_gsubr("mtfa-make-gfx", 8,0,0, (void*)mtfa_make_gfx);
    scm_c_define_gsubr("mtfa-gfx-p", 1,0,0, (void*)mtfa_gfx_p);
    scm_c_define_gsubr("mtfa-gfx-point",		3, 0, 0, (void*)mtfa_gfx_point);
    scm_c_define_gsubr("mtfa-gfx-line",		5, 0, 0, (void*)mtfa_gfx_line);
    scm_c_define_gsubr("mtfa-gfx-fill-arc",		7, 0, 0, (void*)mtfa_gfx_fill_arc);
    scm_c_define_gsubr("mtfa-gfx-arc",		7, 0, 0, (void*)mtfa_gfx_arc);
    scm_c_define_gsubr("mtfa-gfx-rect",		5, 0, 0, (void*)mtfa_gfx_rect);
    scm_c_define_gsubr("mtfa-gfx-fill-rect",	5, 0, 0, (void*)mtfa_gfx_fill_rect);
    scm_c_define_gsubr("mtfa-gfx-string",		5, 0, 0, (void*)mtfa_gfx_string);
    scm_c_define_gsubr("mtfa-gfx-color",		4, 0, 0, (void*)mtfa_gfx_color);
    scm_c_define_gsubr("mtfa-gfx-clear-color",	4, 0, 0, (void*)mtfa_gfx_clear_color);
    scm_c_define_gsubr("mtfa-gfx-clear",		1, 0, 0, (void*)mtfa_gfx_clear);
    scm_c_define_gsubr("mtfa-gfx-flush",		1, 0, 0, (void*)mtfa_gfx_flush);
    scm_c_define_gsubr("mtfa-gfx-event-waiting",	1, 0, 0, (void*)mtfa_gfx_event_waiting);
    scm_c_define_gsubr("mtfa-gfx-wait",		1, 0, 0, (void*)mtfa_gfx_wait);
    scm_c_define_gsubr("mtfa-gfx-idle",		1, 0, 0, (void*)mtfa_gfx_idle);
    scm_c_define_gsubr("mtfa-gfx-xpos",		1, 0, 0, (void*)mtfa_gfx_xpos);
    scm_c_define_gsubr("mtfa-gfx-ypos",		1, 0, 0, (void*)mtfa_gfx_ypos);
    scm_c_define_gsubr("mtfa-gfx-move-window",		3, 0, 0, (void*)mtfa_gfx_move_window);
}
