#include <iostream>
#include <string>

#include <stdlib.h>
#include <libguile.h>

#include <list>

#include <mtfa_utils.h>
#include <mtfabrg.h>



/**************INTERFACCIA GUILE*****************/

static SCM mtfa_brg_type;

static void finalize_mtfa_brg ( SCM mtfa_brg )
{
    BRG * pt = ( BRG* ) scm_foreign_object_signed_ref ( mtfa_brg, 0 );

    if ( pt )
    {
        scm_foreign_object_signed_set_x ( mtfa_brg, 0, 0 );
        delete pt;
    }
}

#define C_STRING_TO_SYMBOL(str) scm_string_to_symbol(ScmFromString(str))



// BRG brg("uno", 512);
// for (int i=0; i<1; ++i)
// 	brg.Next();
// auto v = brg.GetBufAsVByte();
// for (auto it: v)
// {
// 	printf("%u ", it);
// }


static SCM brg_p ( SCM mtfa_brg )
{
	auto const body = [] ( void* data )
	{
		scm_assert_foreign_object_type ( mtfa_brg_type, * ( SCM* ) data );
		return SCM_BOOL_T;
	};
	auto const handler = [] ( void* data, SCM key, SCM args )
	{
		return SCM_BOOL_F;
	};
	SCM ret = scm_internal_catch ( SCM_BOOL_T, body, &mtfa_brg, handler, nullptr );
	return ret;
}


static SCM make_brg (SCM scm_id, SCM scm_seed)
{
	if (scm_is_string(scm_id) == 0)
		scm_wrong_type_arg_msg("mtfa-brg-make" , 0, scm_id, "string");  //E qui esce

    if (scm_is_integer(scm_seed))
    {
        if (scm_to_int(scm_integer_length(scm_seed))<62 || scm_to_int(scm_integer_length(scm_seed))>64 )
            scm_wrong_type_arg_msg("mtfa-brg-make" , 1, scm_seed, "64 bit number or bytevector longer than 31 bytes.");  //E qui esce
        
        char * id = ScmToString(scm_id);
        unsigned long long seed = scm_to_uint64(scm_seed);
        BRG * pt = new BRG(id, seed);
        free(id);
        return scm_make_foreign_object_1 ( mtfa_brg_type, ( void* ) pt );
    } else if (scm_is_bytevector(scm_seed)) {
        int len = SCM_BYTEVECTOR_LENGTH ( scm_seed );
        if (len <= 31)
            scm_wrong_type_arg_msg("mtfa-brg-make" , 1, scm_seed, "64 bit number or bytevector longer than 31 bytes.");  //E qui esce
            
        char * id = ScmToString(scm_id);
        char * p = ( char* ) SCM_BYTEVECTOR_CONTENTS ( scm_seed );
        vector<char> v(p, p+len);
        BRG * pt = new BRG(id, v);
        free(id);
        return scm_make_foreign_object_1 ( mtfa_brg_type, ( void* ) pt );
    } else
        scm_wrong_type_arg_msg("mtfa-brg-make" , 1, scm_seed, "64 bit number or bytevector longer than 31 bytes.");  //E qui esce
}

static SCM brg_next(SCM scm_brg)
{
	if (brg_p(scm_brg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa-brg-next" , 0, scm_brg, "mtfa-brg type");
	
	BRG * pt = ( BRG* ) scm_foreign_object_signed_ref ( scm_brg, 0 );
	pt->Next();
	return SCM_BOOL_T;
}

static SCM brg_get(SCM scm_brg)
{
	if (brg_p(scm_brg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa-brg-get" , 0, scm_brg, "mtfa-brg type");
	
	BRG * pt = ( BRG* ) scm_foreign_object_signed_ref ( scm_brg, 0 );
	uint8_t * ptr = (uint8_t*)malloc(pt->bufOriginal.size());
	copy(pt->bufOriginal.begin(), pt->bufOriginal.end(), ptr);
	SCM scm_ret = scm_pointer_to_bytevector (
		scm_from_pointer ( ( void* ) ptr, free ),
											 scm_from_int ( pt->bufOriginal.size() ),
											 scm_from_int ( 0 ),
											 scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
	return scm_ret;
}

static SCM brg_id(SCM scm_brg)
{
	if (brg_p(scm_brg) == SCM_BOOL_F)
		scm_wrong_type_arg_msg("mtfa-brg-get" , 0, scm_brg, "mtfa-brg type");
	
	BRG * pt = ( BRG* ) scm_foreign_object_signed_ref ( scm_brg, 0 );
	
	string id = pt->GetId();
	
	return ScmFromString(id.c_str());
}


extern "C" void init_mtfa_brg();
extern "C" void init_mtfa_brg()
{
    SCM name, slots;
    scm_t_struct_finalize finalizer;
    name = scm_from_latin1_symbol ( "mtfa-brg" );
    slots = scm_list_1 ( scm_from_latin1_symbol ( "brg" ) );
    finalizer = finalize_mtfa_brg;
    mtfa_brg_type = scm_make_foreign_object_type ( name, slots, finalizer );

    scm_c_define_gsubr ( "mtfa-brg-make", 2, 0, 0, ( void* ) make_brg);
	scm_c_define_gsubr ( "mtfa-brg?", 1, 0, 0, ( void* ) brg_p );
	scm_c_define_gsubr ( "mtfa-brg-next", 1, 0, 0, ( void* ) brg_next );
	scm_c_define_gsubr ( "mtfa-brg-get", 1, 0, 0, ( void* ) brg_get );
	scm_c_define_gsubr ( "mtfa-brg-id", 1, 0, 0, ( void* ) brg_id );
}
