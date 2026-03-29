#include <iostream>
#include <string>

#include <stdlib.h>
#include <libguile.h>

#include <list>

#include <mtfa_utils.h>

#include <../va/VAengine.h>

/**************INTERFACCIA GUILE*****************/

static SCM mtfa_va_type;

static void finalize_mtfa_va( SCM mtfa_va )
{
//      mtfa_mat_cypher_ext * pt = ( mtfa_mat_cypher_ext * ) scm_foreign_object_signed_ref ( mtfa_ecipher, 0 );
// 
//      if ( pt ) {
//           scm_foreign_object_signed_set_x ( mtfa_ecipher, 0, 0 );
//           delete pt;
//      }
}

#define C_STRING_TO_SYMBOL(str) scm_string_to_symbol(ScmFromString(str))

// static string ScmToStringCPP ( SCM s)
// {
// 	char* p = nullptr;
// 	size_t len = 0;
// 	p = ScmToStringn(s, &len);
// 	string sr;
// 	if (len > 0)
// 		sr.assign(p, p+len);
// 	else
// 		sr.clear();
// 	free(p);
// 	return sr;
// }

//extern VAengine * pvaeng;
// static VAengine vaeng;
// static VAengine * pvaeng=&vaeng;
// 
// static SCM InsertIntoSet ( SCM setname, SCM value, SCM duration )
// {
// 	string sn = ScmToStringCPP ( setname );
// 	string sv = ScmToStringCPP ( value );
// 	string sd = ScmToStringCPP ( duration );
// 	pvaeng->vasets.AddItem ( sn, sv, strtoull ( sd.c_str(), 0, 10 ), true, true);
// 	return SCM_EOL;
// }
// 
// static SCM RemoveFromSet ( SCM setname, SCM value, SCM duration )
// {
// 	string sn = ScmToStringCPP ( setname );
// 	string sv = ScmToStringCPP ( value );
// 	string sd = ScmToStringCPP ( duration );
// 	pvaeng->vasets.DelItem ( sn, sv, strtoull ( sd.c_str(), 0, 10 ), true, true );
// 	return SCM_EOL;
// }
// 
// static SCM GetFromSet ( SCM setname, SCM key)
// {
// 	string sn = ScmToStringCPP ( setname );
// 	string sv = ScmToStringCPP ( key );
// 	string value;
// 	bool bret = pvaeng->vasets.CheckItem ( sn, sv, value );
// 	
// 	if ( bret )
// 		return scm_from_latin1_string(value.c_str());
// 	else
// 		return SCM_BOOL_F;
// }


static SCM mtfa_va_p ( SCM mtfa_va )
{
    auto const body = [] (void * data) {
        scm_assert_foreign_object_type ( mtfa_va_type, *(SCM*)data);
        return SCM_BOOL_T;
    };
    auto const handler = [] ( void *data, SCM key, SCM args ) {
        return SCM_BOOL_F;
    };
    //SCM handler (void *data, SCM key, SCM args);

     SCM ret = scm_internal_catch ( SCM_BOOL_T, body, &mtfa_va, handler, nullptr );

     return ret;
}

SCM mtfa_va_nop();
SCM mtfa_va_nop()
{
	return SCM_EOL;
}

extern "C" void init_mtfa_va();

extern "C" void init_mtfa_va()
{
     SCM name, slots;
     scm_t_struct_finalize finalizer;
     name = scm_from_latin1_symbol ( "mtfa-va" );
     slots = scm_list_1 ( scm_from_latin1_symbol ( "va" ) );
     finalizer = finalize_mtfa_va;
     mtfa_va_type = scm_make_foreign_object_type ( name, slots, finalizer );

	 //Pubblico le funzioni di interesse verso lo strato LISP
	 //Add, Del e Search di insiemi
//  	 scm_c_define_gsubr ( "vaeng::AddToSet", 3, 0, 0, ( void* ) InsertIntoSet );
//  	 scm_c_define_gsubr ( "vaeng::DelToSet", 3, 0, 0, ( void* ) RemoveFromSet );
//  	 scm_c_define_gsubr ( "vaeng::GetFromSet", 2, 0, 0, ( void* ) GetFromSet );
	 scm_c_define_gsubr ( "mtfa-va-nop", 0, 0, 0, ( void* ) mtfa_va_nop ); 
}


