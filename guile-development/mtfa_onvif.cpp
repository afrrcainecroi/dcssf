#include <iostream>
#include <string>

#include <stdlib.h>
#include <libguile.h>

#include <list>

#include <onvifclass.h>

/**************INTERFACCIA GUILE*****************/

static SCM mtfa_onvif_type;

static void finalize_mtfa_onvif ( SCM mtfa_onvif )
{
    PtzCamera * pt = ( PtzCamera* ) scm_foreign_object_signed_ref ( mtfa_onvif, 0 );

    if ( pt )
    {
        scm_foreign_object_signed_set_x ( mtfa_onvif, 0, 0 );
        delete pt;
    }
}

#define C_STRING_TO_SYMBOL(str) scm_string_to_symbol(ScmFromString(str))

static SCM mtfa_onvif_p ( SCM mtfa_onvif)
{
	auto const body = [] ( void* data )
	{
		scm_assert_foreign_object_type ( mtfa_onvif_type, * ( SCM* ) data );
		return SCM_BOOL_T;
	};
	auto const handler = [] ( void* data, SCM key, SCM args )
	{
		return SCM_BOOL_F;
	};
	SCM ret = scm_internal_catch ( SCM_BOOL_T, body, &mtfa_onvif, handler, nullptr );
	return ret;
}


static SCM make_onvif (SCM scm_uri, SCM scm_username, SCM scm_password)
{
	if (scm_is_string(scm_uri) == 0)
		scm_wrong_type_arg_msg("mtfa-onvif-make" , 0, scm_uri, "string");  //E qui esce
	if (scm_is_string(scm_username) == 0)
		scm_wrong_type_arg_msg("mtfa-onvif-make" , 0, scm_username, "string");  //E qui esce
	if (scm_is_string(scm_password) == 0)
		scm_wrong_type_arg_msg("mtfa-onvif-make" , 0, scm_password, "string");  //E qui esce
        
    char * pcuri = scm_to_locale_string(scm_uri);
    char * pcusername = scm_to_locale_string(scm_username);
    char * pcpassword = scm_to_locale_string(scm_password);
    
    string uri = pcuri; free(pcuri);
    string username = pcusername; free(pcusername);
    string password = pcpassword; free(pcpassword);

    auto ptz = new PtzCamera(uri, username, password);
    if (ptz == nullptr)
        return scm_values_2(SCM_BOOL_F, SCM_EOL);
    else
        return scm_values_2(SCM_BOOL_T, scm_make_foreign_object_1(mtfa_onvif_type, (void*) ptz));
}

static SCM onvif_connect(SCM scm_onvif)
{
	if (mtfa_onvif_p(scm_onvif) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa-onvif-connect" , 0, scm_onvif, "mtfa-onvif type");
	
	auto ptz = ( PtzCamera* ) scm_foreign_object_signed_ref ( scm_onvif, 0 );
    bool bret = ptz->Connect();
    if (!bret)
        ptz->Disconnect();

    return bret?SCM_BOOL_T:SCM_BOOL_F;
}

static SCM onvif_move(SCM scm_onvif, SCM scm_panx, SCM scm_pany, SCM scm_zoom, SCM scm_speed)
{
	if (mtfa_onvif_p(scm_onvif) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa-onvif-move" , 0, scm_onvif, "mtfa-onvif type");

    auto ptz = ( PtzCamera* ) scm_foreign_object_signed_ref ( scm_onvif, 0 );
    
    if (scm_is_number(scm_panx)==0)
		scm_wrong_type_arg_msg("mtfa-onvif-move" , 0, scm_panx, "number (float -1:1) type");
    if (scm_is_number(scm_pany)==0)
		scm_wrong_type_arg_msg("mtfa-onvif-move" , 0, scm_pany, "number (float -1:1) type");
    if (scm_is_number(scm_zoom)==0)
		scm_wrong_type_arg_msg("mtfa-onvif-move" , 0, scm_zoom, "number (float 0:1) type");
    if (scm_is_number(scm_speed)==0)
        scm_wrong_type_arg_msg("mtfa-onvif-move" , 0, scm_speed, "number (float 0:1)");
    
    float panx = scm_to_double(scm_panx);
    float pany = scm_to_double(scm_pany);
    float zoom = scm_to_double(scm_zoom);
    float speed = scm_to_double(scm_speed);
    
    bool bret = ptz->Move(panx, pany, zoom ,speed);
    return bret?SCM_BOOL_T:SCM_BOOL_F;
}

extern "C" void init_mtfa_onvif();
extern "C" void init_mtfa_onvif()
{
    SCM name, slots;
    scm_t_struct_finalize finalizer;
    name = scm_from_latin1_symbol ( "mtfa-onvif" );
    slots = scm_list_1 ( scm_from_latin1_symbol ( "onvif" ) );
    finalizer = finalize_mtfa_onvif;
    mtfa_onvif_type = scm_make_foreign_object_type ( name, slots, finalizer );

    scm_c_define_gsubr ( "mtfa-onvif-make", 3, 0, 0, ( void* ) make_onvif);
	scm_c_define_gsubr ( "mtfa-onvif?", 1, 0, 0, ( void* ) mtfa_onvif_p );
	scm_c_define_gsubr ( "mtfa-onvif-connect", 1, 0, 0, ( void* ) onvif_connect );
	scm_c_define_gsubr ( "mtfa-onvif-move", 5, 0, 0, ( void* ) onvif_move);
}
