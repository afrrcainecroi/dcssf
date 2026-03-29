#include <iostream>
#include <string>

#include <stdlib.h>
#include <libguile.h>

#include <list>

#include <mtfa_utils.h>
#include <mtfa_fs3.h>

#include <plotter.h>


/**************INTERFACCIA GUILE*****************/

static SCM mtfa_cimg_type;  //il tipo del su

static void finalize_mtfa_cimg( SCM mtfa_cimg )
{
    XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( mtfa_cimg, 0 );

    if ( pt )
    {
        scm_foreign_object_signed_set_x ( mtfa_cimg, 0, 0 );
        pt->closepl();
        delete pt;
    }
}

#define C_STRING_TO_SYMBOL(str) scm_string_to_symbol(ScmFromString(str))

static SCM mtfa_make_cimg (SCM scm_params) //una lista di coppie di stringhe
{
    PlotterParams params;
    while (SCM_EOL != scm_params)
    {
        char * key   = scm_to_locale_string(scm_car(scm_car(scm_params)));
        char * value = scm_to_locale_string(scm_cdr(scm_car(scm_params)));
        params.setplparam(key, value);
//         free(key);
//         free(value);
        scm_params = scm_cdr(scm_params);
    }
    //params.setplparam("BITMAPSIZE", (char *)"2048x2048");

    XPlotter* pt = new XPlotter(params);
    pt->openpl();
    
//     pt->space(0, 0, 1000, 1000);
//     pt->flinewidth (1);       // line thickness in user coordinates
//     pt->erase ();                // erase Plotter's graphics display
//     pt->pencolorname ("red");    // path will be drawn in red
//     pt->fillcolorname ("red");    // path will be drawn in red
//     pt->box (10, 100, 100, 110);

//     XPlotter plotter(params); // declare Plotter
//     plotter.space (0, 0, 10000, 10000); // specify user coor system
//     plotter.flinewidth (1);       // line thickness in user coordinates
//     plotter.erase ();                // erase Plotter's graphics display
//     plotter.pencolorname ("red");    // path will be drawn in red
//     plotter.fillcolorname ("red");    // path will be drawn in red
//     plotter.box (10, 100, 100, 110);
//     plotter.flushpl();
//     plotter.closepl();
    return scm_make_foreign_object_1 ( mtfa_cimg_type, ( void* ) pt );
}

static SCM mtfa_cimg_p ( SCM mtfa_cimg )
{
    auto const body = [] ( void* data )
    {
        scm_assert_foreign_object_type ( mtfa_cimg_type, * ( SCM* ) data );
        return SCM_BOOL_T;
    };
    auto const handler = [] ( void* data, SCM key, SCM args )
    {
        return SCM_BOOL_F;
    };
    
    SCM ret = scm_internal_catch ( SCM_BOOL_T, body, &mtfa_cimg, handler, nullptr );
    
    return ret;
}


static SCM mtfa_cimg_set_space(SCM cimg, SCM sx0, SCM sy0, SCM sx1, SCM sy1)
{
	if (mtfa_cimg_p(cimg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa-cimg-set-space" , 0, cimg, "mtfa-cimg type");
	
	XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( cimg, 0 );
    int x0 = scm_to_int(sx0);
    int x1 = scm_to_int(sx1);
    int y0 = scm_to_int(sy0);
    int y1 = scm_to_int(sy1);
    pt->space (x0, y0, x1, y1);
	return SCM_BOOL_T;
}

static SCM mtfa_cimg_set_linewidth(SCM cimg, SCM sw)
{
	if (mtfa_cimg_p(cimg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("Needed" , 0, cimg, "mtfa-cimg type");
	
	XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( cimg, 0 );
    pt->linewidth(scm_to_int(sw));
	return SCM_BOOL_T;
}

static SCM mtfa_cimg_erase(SCM cimg)
{
	if (mtfa_cimg_p(cimg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("Needed" , 0, cimg, "mtfa-cimg type");
	
	XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( cimg, 0 );
    pt->erase();
	return SCM_BOOL_T;
}

static SCM mtfa_cimg_flush(SCM cimg)
{
	if (mtfa_cimg_p(cimg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("Needed" , 0, cimg, "mtfa-cimg type");
	
	XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( cimg, 0 );
    pt->flushpl();
	return SCM_BOOL_T;
}

static SCM mtfa_cimg_close(SCM cimg)
{
	if (mtfa_cimg_p(cimg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("Needed" , 0, cimg, "mtfa-cimg type");
	
	XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( cimg, 0 );
    pt->closepl();
    return SCM_BOOL_T;
}

static SCM mtfa_cimg_open(SCM cimg)
{
	if (mtfa_cimg_p(cimg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("Needed" , 0, cimg, "mtfa-cimg type");
	
	XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( cimg, 0 );
    pt->openpl();
    return SCM_BOOL_T;
}

static SCM mtfa_cimg_set_linecolor_string(SCM cimg, SCM sc)
{
	if (mtfa_cimg_p(cimg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("Needed" , 0, cimg, "mtfa-cimg type");
	
	XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( cimg, 0 );
    char * color = scm_to_locale_string(sc);
    pt->pencolorname(color);
    free(color);
	return SCM_BOOL_T;
}

static SCM mtfa_cimg_set_linecolor(SCM cimg, SCM sr, SCM sg, SCM sb)
{
	if (mtfa_cimg_p(cimg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("Needed" , 0, cimg, "mtfa-cimg type");
	
	XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( cimg, 0 );
    pt->pencolor(scm_to_int(sr), scm_to_int(sg), scm_to_int(sb));
	return SCM_BOOL_T;
}

static SCM mtfa_cimg_set_fillcolor_string(SCM cimg, SCM sc)
{
	if (mtfa_cimg_p(cimg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("Needed" , 0, cimg, "mtfa-cimg type");
	
	XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( cimg, 0 );
    char * color = scm_to_locale_string(sc);
    pt->fillcolorname(color);
    free(color);
	return SCM_BOOL_T;
}

static SCM mtfa_cimg_set_fillcolor(SCM cimg, SCM sr, SCM sg, SCM sb)
{
	if (mtfa_cimg_p(cimg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("Needed" , 0, cimg, "mtfa-cimg type");
	
	XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( cimg, 0 );
    pt->fillcolor(scm_to_int(sr), scm_to_int(sg), scm_to_int(sb));
	return SCM_BOOL_T;
}

static SCM mtfa_cimg_set_filltype(SCM cimg, SCM sft)
{
	if (mtfa_cimg_p(cimg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("Needed" , 0, cimg, "mtfa-cimg type");
	
	XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( cimg, 0 );
    pt->filltype(scm_to_int(sft));
	return SCM_BOOL_T;
}

static SCM mtfa_cimg_set_bg_string(SCM cimg, SCM sc)
{
	if (mtfa_cimg_p(cimg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("Needed" , 0, cimg, "mtfa-cimg type");
	
	XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( cimg, 0 );
    char * color = scm_to_locale_string(sc);
    pt->bgcolorname(color);
    free(color);
	return SCM_BOOL_T;
}
static SCM mtfa_cimg_set_bg(SCM cimg, SCM sr, SCM sg, SCM sb)
{
	if (mtfa_cimg_p(cimg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("Needed" , 0, cimg, "mtfa-cimg type");
	
	XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( cimg, 0 );
    pt->bgcolor(scm_to_int(sr), scm_to_int(sg), scm_to_int(sb));
	return SCM_BOOL_T;
}


static SCM mtfa_cimg_draw_box(SCM cimg, SCM sx0, SCM sy0, SCM sx1, SCM sy1)
{
	if (mtfa_cimg_p(cimg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa-cimg-set-space" , 0, cimg, "mtfa-cimg type");
	
	XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( cimg, 0 );
    int x0 = scm_to_int(sx0);
    int x1 = scm_to_int(sx1);
    int y0 = scm_to_int(sy0);
    int y1 = scm_to_int(sy1);
    pt->box(x0, y0, x1, y1);
    
	return SCM_BOOL_T;
}


static SCM mtfa_cimg_draw_label(SCM cimg, SCM shj, SCM svj, SCM st)
{
	if (mtfa_cimg_p(cimg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa-cimg-set-space" , 0, cimg, "mtfa-cimg type");
	
	XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( cimg, 0 );
    char hj = scm_to_char(shj);
    char vj = scm_to_char(svj);
    char * text = scm_to_locale_string(st);
    pt->alabel(hj, vj, text);
    free(text);
    
	return SCM_BOOL_T;
}

static SCM mtfa_cimg_move(SCM cimg, SCM sx, SCM sy)
{
	if (mtfa_cimg_p(cimg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("Needed" , 0, cimg, "mtfa-cimg type");
	
	XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( cimg, 0 );
    pt->move(scm_to_int(sx), scm_to_int(sy));
	return SCM_BOOL_T;
}

static SCM mtfa_cimg_draw_circle(SCM cimg, SCM sx, SCM sy, SCM sr)
{
	if (mtfa_cimg_p(cimg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("Needed" , 0, cimg, "mtfa-cimg type");
	
	XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( cimg, 0 );
    pt->circle(scm_to_int(sx), scm_to_int(sy), scm_to_int(sr));
	return SCM_BOOL_T;
}

static SCM mtfa_cimg_draw_line(SCM cimg, SCM sx0, SCM sy0, SCM sx1, SCM sy1)
{
	if (mtfa_cimg_p(cimg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa-cimg-set-space" , 0, cimg, "mtfa-cimg type");
	
	XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( cimg, 0 );
    int x0 = scm_to_int(sx0);
    int x1 = scm_to_int(sx1);
    int y0 = scm_to_int(sy0);
    int y1 = scm_to_int(sy1);
    pt->line(x0, y0, x1, y1);
    
	return SCM_BOOL_T;
}

static SCM mtfa_cimg_draw_point(SCM cimg, SCM sx, SCM sy)
{
	if (mtfa_cimg_p(cimg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("Needed" , 0, cimg, "mtfa-cimg type");
	
	XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( cimg, 0 );
    pt->point(scm_to_int(sx), scm_to_int(sy));
	return SCM_BOOL_T;
}

static SCM mtfa_cimg_font_name(SCM cimg, SCM sn)
{
	if (mtfa_cimg_p(cimg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("Needed" , 0, cimg, "mtfa-cimg type");
	
	XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( cimg, 0 );
    char * name = scm_to_locale_string(sn);
    pt->fontname(name);
    free(name);
	return SCM_BOOL_T;
}

static SCM mtfa_cimg_font_size(SCM cimg, SCM ss)
{
	if (mtfa_cimg_p(cimg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("Needed" , 0, cimg, "mtfa-cimg type");
	
	XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( cimg, 0 );
    pt->fontsize(scm_to_int(ss));
	return SCM_BOOL_T;
}

static SCM mtfa_cimg_text_angle(SCM cimg, SCM sta)
{
	if (mtfa_cimg_p(cimg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("Needed" , 0, cimg, "mtfa-cimg type");
	
	XPlotter * pt = ( XPlotter* ) scm_foreign_object_signed_ref ( cimg, 0 );
    pt->textangle(scm_to_int(sta));
	return SCM_BOOL_T;
}






extern "C" void init_mtfa_cimg()
{
    SCM name, slots;
    scm_t_struct_finalize finalizer;
    name = scm_from_latin1_symbol ( "mtfa-cimg" );
    slots = scm_list_1 ( scm_from_latin1_symbol ( "cimg" ) );
    finalizer = finalize_mtfa_cimg;
    mtfa_cimg_type = scm_make_foreign_object_type ( name, slots, finalizer );

    scm_c_define_gsubr ( "mtfa-make-cimg", 1, 0, 0, ( void* ) mtfa_make_cimg);
    scm_c_define_gsubr ( "mtfa-cimg-p", 1, 0, 0, ( void* ) mtfa_cimg_p);
    scm_c_define_gsubr ( "mtfa-cimg-set-space", 5, 0, 0, ( void* ) mtfa_cimg_set_space);
    scm_c_define_gsubr ( "mtfa-cimg-set-linewidth", 2, 0, 0, ( void* ) mtfa_cimg_set_linewidth);
    scm_c_define_gsubr ( "mtfa-cimg-erase", 1, 0, 0, ( void* ) mtfa_cimg_erase);
    scm_c_define_gsubr ( "mtfa-cimg-flush", 1, 0, 0, ( void* ) mtfa_cimg_flush);
    scm_c_define_gsubr ( "mtfa-cimg-close", 1, 0, 0, ( void* ) mtfa_cimg_close);
    scm_c_define_gsubr ( "mtfa-cimg-open", 1, 0, 0, ( void* ) mtfa_cimg_open);
    scm_c_define_gsubr ( "mtfa-cimg-set-linecolor-string", 2, 0, 0, ( void* ) mtfa_cimg_set_linecolor_string);
    scm_c_define_gsubr ( "mtfa-cimg-set-linecolor", 4, 0, 0, ( void* ) mtfa_cimg_set_linecolor);
    scm_c_define_gsubr ( "mtfa-cimg-set-fillcolor-string", 2, 0, 0, ( void* ) mtfa_cimg_set_fillcolor_string);
    scm_c_define_gsubr ( "mtfa-cimg-set-fillcolor", 4, 0, 0, ( void* ) mtfa_cimg_set_fillcolor);
    scm_c_define_gsubr ( "mtfa-cimg-set-filltype", 2, 0, 0, ( void* ) mtfa_cimg_set_filltype);
    scm_c_define_gsubr ( "mtfa-cimg-set-bg-string", 2, 0, 0, ( void* ) mtfa_cimg_set_bg_string);
    scm_c_define_gsubr ( "mtfa-cimg-set-bg", 4, 0, 0, ( void* ) mtfa_cimg_set_bg);
    scm_c_define_gsubr ( "mtfa-cimg-draw-box", 5, 0, 0, ( void* ) mtfa_cimg_draw_box);
    scm_c_define_gsubr ( "mtfa-cimg-draw-label", 4, 0, 0, ( void* ) mtfa_cimg_draw_label);
    scm_c_define_gsubr ( "mtfa-cimg-move", 3, 0, 0, ( void* ) mtfa_cimg_move);
    scm_c_define_gsubr ( "mtfa-cimg-draw-circle", 4, 0, 0, ( void* ) mtfa_cimg_draw_circle);
    scm_c_define_gsubr ( "mtfa-cimg-draw-line", 5, 0, 0, ( void* ) mtfa_cimg_draw_line);
    scm_c_define_gsubr ( "mtfa-cimg-draw-point", 3, 0, 0, ( void* ) mtfa_cimg_draw_point);
    scm_c_define_gsubr ( "mtfa-cimg-font-name", 2, 0, 0, ( void* ) mtfa_cimg_font_name);
    scm_c_define_gsubr ( "mtfa-cimg-font-size", 2, 0, 0, ( void* ) mtfa_cimg_font_size);
    scm_c_define_gsubr ( "mtfa-cimg-text-angle", 2, 0, 0, ( void* ) mtfa_cimg_text_angle);
}



















