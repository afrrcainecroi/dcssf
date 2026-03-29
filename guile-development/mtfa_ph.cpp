#include <iostream>
#include <string>

#include <stdlib.h>
#include <libguile.h>

#include <list>

#include <mtfa_ph.h>
#include <mtfafs.h>
#include <libguile/bytevectors.h>
#include <libguile/bytevectors.h>
#include <libguile/bytevectors.h>

/**************INTERFACCIA GUILE*****************/

static SCM mtfa_ph_type;  //il tipo del perfect-hash

static void finalize_mtfa_ph ( SCM mtfa_ph )
{
    PerfectHash* pt = ( PerfectHash* ) scm_foreign_object_signed_ref ( mtfa_ph, 0 );

    if ( pt )
    {
        scm_foreign_object_signed_set_x ( mtfa_ph, 0, 0 );
        delete pt;
    }
}

#define C_STRING_TO_SYMBOL(str) scm_string_to_symbol(ScmFromString(str))

static SCM mtfa_ph_p ( SCM mtfa_ph )
{
    auto const body = [] ( void* data )
    {
        scm_assert_foreign_object_type ( mtfa_ph_type, * ( SCM* ) data );
        return SCM_BOOL_T;
    };
    auto const handler = [] ( void* data, SCM key, SCM args )
    {
        return SCM_BOOL_F;
    };
    SCM ret = scm_internal_catch ( SCM_BOOL_T, body, &mtfa_ph, handler, nullptr );
    return ret;
}


static SCM make_mtfa_ph ()
{
    PerfectHash* pt = new PerfectHash();
    return scm_make_foreign_object_1 ( mtfa_ph_type, ( void* ) pt );
}

static SCM add_mtfa_ph ( SCM mtfa_ph, SCM key )
{
    if ( SCM_BOOL_F == mtfa_ph_p ( mtfa_ph ) )
        return SCM_EOL;

    PerfectHash* pt = ( PerfectHash* ) scm_foreign_object_signed_ref ( mtfa_ph, 0 );
	
	if (scm_is_string(key))
	{
		int pos = -1;
		size_t len;
		char* s = ScmToStringn ( key, &len );
		
		pos = pt->Insert ( s, len );
		free ( s );
		
		return scm_from_int ( pos );
	}
	if (scm_is_symbol(key))
	{
		auto skey = scm_symbol_to_string(key);
		int pos = -1;
		size_t len;
		char* s = ScmToStringn ( skey, &len );
		
		pos = pt->Insert ( s, len );
		free ( s );
		
		return scm_from_int ( pos );
	}
}

static SCM get_mtfa_ph ( SCM mtfa_ph, SCM key )
{
    if ( SCM_BOOL_F == mtfa_ph_p ( mtfa_ph ) )
        return SCM_EOL;

    PerfectHash* pt = ( PerfectHash* ) scm_foreign_object_signed_ref ( mtfa_ph, 0 );

	if (scm_is_string(key))
	{
		size_t len;
		char* s = ScmToStringn ( key, &len );

		//Cerco!
		int pos = pt->Find ( s, len );
		free ( s );

		return scm_from_int ( pos );
	}
	if (scm_is_symbol(key))
	{
		auto skey = scm_symbol_to_string(key);
		size_t len;
		char* s = ScmToStringn ( skey, &len );
		
		//Cerco!
		int pos = pt->Find ( s, len );
		free ( s );
		
		return scm_from_int ( pos );
	}
}

static SCM get_val_mtfa_ph ( SCM mtfa_ph, SCM pos )
{
    if ( SCM_BOOL_F == mtfa_ph_p ( mtfa_ph ) )
        return SCM_EOL;

    PerfectHash* pt = ( PerfectHash* ) scm_foreign_object_signed_ref ( mtfa_ph, 0 );

    //prendo posizione
    int ipos = scm_to_int ( pos );

    //presente?
    std::string s;
    bool bret = pt->GetVal ( ipos, s ); //s vuota se non esiste!

    return ScmFromStringn ( s.c_str(), s.size() );
}

static SCM for_each_mtfa_ph ( SCM mtfa_ph, SCM function )
{
    if ( SCM_BOOL_F == mtfa_ph_p ( mtfa_ph ) )
        return SCM_EOL;

    PerfectHash* pt = ( PerfectHash* ) scm_foreign_object_signed_ref ( mtfa_ph, 0 );

    for ( int i = 0; i < pt->p->v_dati->righe; ++i )
    {
        //Chiama la funzione con due parametri
        scm_call_2 ( function, scm_from_int32 ( i ), ScmFromStringn ( ( const char* ) & ( *pt->p->v_dati ) [i][0], pt->p->v_dati->v_len[i] ) );
    }

    return SCM_BOOL_T;
}


static SCM get_all_mtfa_ph ( SCM mtfa_ph )
{
    if ( SCM_BOOL_F == mtfa_ph_p ( mtfa_ph ) )
        return SCM_EOL;

    PerfectHash* pt = ( PerfectHash* ) scm_foreign_object_signed_ref ( mtfa_ph, 0 );

    auto all = pt->GetAll();

    SCM list = SCM_EOL;

    for ( int i = 0; i < pt->p->v_dati->righe; ++i )
    {
        SCM v = scm_from_int32 ( i );
        SCM k = ScmFromStringn ( ( const char* ) & ( *pt->p->v_dati ) [i][0], pt->p->v_dati->v_len[i] );
        list = scm_cons ( scm_cons ( k, v ), list );
    }

    return scm_reverse ( list );
}


static SCM serialize_mtfa_ph ( SCM mtfa_ph )
{
    if ( SCM_BOOL_F == mtfa_ph_p ( mtfa_ph ) )
        return SCM_EOL;

    PerfectHash* pt = ( PerfectHash* ) scm_foreign_object_signed_ref ( mtfa_ph, 0 );

    mtfa_binary_stream_save* bs = new mtfa_binary_stream_save ( "" );
    pt->Serialize ( bs );
    std::string ser = bs->GetStream();

    delete bs;

    string cmpser;
    bool bret = mtfa_compress(ser, cmpser, 6);

    char* pser = ( char* ) malloc ( cmpser.size() );
    memcpy ( pser, cmpser.data(), cmpser.size() );

    return scm_pointer_to_bytevector (
               scm_from_pointer ( pser, free ),
               scm_from_int ( cmpser.size() ),
               scm_from_int ( 0 ),
               scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
}

static SCM deserialize_mtfa_ph ( SCM mtfa_ph, SCM bytevector_in )
{
    if ( SCM_BOOL_F == mtfa_ph_p ( mtfa_ph ) )
        return SCM_EOL;

    PerfectHash* pt = ( PerfectHash* ) scm_foreign_object_signed_ref ( mtfa_ph, 0 );

    mtfa_binary_stream_load* bs = new mtfa_binary_stream_load ( "" );
    int len = SCM_BYTEVECTOR_LENGTH ( bytevector_in );
    char* p = ( char* ) SCM_BYTEVECTOR_CONTENTS ( bytevector_in );
    
    string decomp;
    bool bret = mtfa_uncompress(string(p, p+len), decomp);
    
    bs->mtfa_binary_stream_set ( decomp );
    pt->Deserialize ( bs );
    delete bs;

    return SCM_BOOL_T;
}

extern "C" void init_mtfa_ph();
extern "C" void init_mtfa_ph()
{
    SCM name, slots;
    scm_t_struct_finalize finalizer;
    name = scm_from_latin1_symbol ( "mtfa-ph" );
    slots = scm_list_1 ( scm_from_latin1_symbol ( "ds" ) );
    finalizer = finalize_mtfa_ph;
    mtfa_ph_type = scm_make_foreign_object_type ( name, slots, finalizer );

    scm_c_define_gsubr ( "mtfa-ph-make", 0, 0, 0, ( void* ) make_mtfa_ph );
    scm_c_define_gsubr ( "mtfa-ph-add", 2, 0, 0, ( void* ) add_mtfa_ph );
    scm_c_define_gsubr ( "mtfa-ph-get", 2, 0, 0, ( void* ) get_mtfa_ph );
    scm_c_define_gsubr ( "mtfa-ph-get-val", 2, 0, 0, ( void* ) get_val_mtfa_ph );
    scm_c_define_gsubr ( "mtfa-ph-get-all", 1, 0, 0, ( void* ) get_all_mtfa_ph );
    scm_c_define_gsubr ( "mtfa-ph-serialize", 1, 0, 0, ( void* ) serialize_mtfa_ph );
    scm_c_define_gsubr ( "mtfa-ph-deserialize", 2, 0, 0, ( void* ) deserialize_mtfa_ph );
    scm_c_define_gsubr ( "mtfa-ph?", 1, 0, 0, ( void* ) mtfa_ph_p );
    scm_c_define_gsubr ( "mtfa-ph-for-each", 2, 0, 0, ( void* ) for_each_mtfa_ph );

}


