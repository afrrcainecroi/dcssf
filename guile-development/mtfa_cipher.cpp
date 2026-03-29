#include <iostream>
#include <string>

#include <stdlib.h>
#include <libguile.h>

#include <list>

#include <mtfa_utils.h>



/**************INTERFACCIA GUILE*****************/

static SCM mtfa_ecipher_type;  //il tipo del cipher

static void finalize_mtfa_ecipher ( SCM mtfa_ecipher )
{
     mtfa_mat_cypher_ext * pt = ( mtfa_mat_cypher_ext * ) scm_foreign_object_signed_ref ( mtfa_ecipher, 0 );

     if ( pt ) {
          scm_foreign_object_signed_set_x ( mtfa_ecipher, 0, 0 );
          delete pt;
     }
}

#define C_STRING_TO_SYMBOL(str) scm_string_to_symbol(ScmFromString(str))

// SCM scm_c_array_to_aliased_f64_vector ( int count, double* array );
// SCM scm_c_array_to_aliased_f64_vector ( int count, double* array )
// {
// 
// // Include an if clause maybe.
// #ifdef IFCLAUSE
//      SCM x = SCM_BOOL_F;
//      if ( scm_is_false ( x ) ) {
//           x = SCM_BOOL_T;
//      }
// #endif
// 
//      return scm_pointer_to_bytevector ( scm_from_pointer (
//                                              // Include an error maybe.
// #ifdef ERROR
//                                              xreal
// #else
//                                              array
// #endif
//                                              , NULL ), scm_from_int ( count ), scm_from_int ( 0 ), C_STRING_TO_SYMBOL ( "s8" ) );
// }

static SCM make_mtfa_ecipher ( SCM scm_par )
{
     if ( scm_is_number ( scm_par ) ) {
          unsigned long long seed = scm_to_ulong_long ( scm_par );
          mtfa_mat_cypher_ext * pt = new mtfa_mat_cypher_ext ( seed );
          return scm_make_foreign_object_1 ( mtfa_ecipher_type, ( void* ) pt );
     } else if ( scm_is_string ( scm_par ) ) {
          size_t len;
          char * p = ScmToStringn( scm_par, &len );
          mtfa_mat_cypher_ext * pt = new mtfa_mat_cypher_ext ( p, len );
          free ( p );
          return scm_make_foreign_object_1 ( mtfa_ecipher_type, ( void* ) pt );
     } else if ( scm_is_bytevector ( scm_par ) ) {
          size_t len = SCM_BYTEVECTOR_LENGTH ( scm_par );
          char *p = ( char* ) SCM_BYTEVECTOR_CONTENTS ( scm_par );
          mtfa_mat_cypher_ext * pt = new mtfa_mat_cypher_ext ( p, len );
          return scm_make_foreign_object_1 ( mtfa_ecipher_type, ( void* ) pt );
     } else {
          return SCM_EOL;
     }
}

static SCM mtfa_ecipher_mask ( SCM scm_ec, SCM scm_par )
{
     mtfa_mat_cypher_ext * pt = ( mtfa_mat_cypher_ext* ) scm_foreign_object_signed_ref ( scm_ec, 0 );

     if ( scm_is_number ( scm_par ) ) {
          unsigned long long seed = scm_to_ulong_long ( scm_par );
          pt->SetMask ( seed );
          return SCM_BOOL_T;
     } else if ( scm_is_string ( scm_par ) ) {
          size_t len;
          char * p = ScmToStringn( scm_par, &len );
          pt->SetMask ( p, len );
          free ( p );
          return SCM_BOOL_T;
     } else if ( scm_is_bytevector ( scm_par ) ) {
          size_t len = SCM_BYTEVECTOR_LENGTH ( scm_par );
          char * p = ( char* ) SCM_BYTEVECTOR_CONTENTS ( scm_par );
          pt->SetMask ( p, len );
          return SCM_BOOL_T;
     } else {
          return SCM_EOL;
     }
}

//per semplicità torna sempre un bytevector
static SCM mtfa_ecipher_cipher ( SCM scm_ec, SCM scm_par )
{
     mtfa_mat_cypher_ext * pt = ( mtfa_mat_cypher_ext* ) scm_foreign_object_signed_ref ( scm_ec, 0 );

     if ( scm_is_number ( scm_par ) ) {
          unsigned long long seed = scm_to_ulong_long ( scm_par );
          char * p = pt->cypher ( ( char* ) &seed, sizeof ( seed ) );
          SCM scmret = scm_pointer_to_bytevector (
                             scm_from_pointer ( ( void* ) p, free ),
                             scm_from_int ( sizeof ( seed ) ),
                             scm_from_int ( 0 ),
                                                  scm_string_to_symbol ( ScmFromString( "u8" ) ) );
          return scmret;
     } else if ( scm_is_string ( scm_par ) ) {
          size_t len;
          char * s = ScmToStringn( scm_par, &len );
          char * p = pt->cypher ( s, len );
          free ( s );
          SCM scmret = scm_pointer_to_bytevector (
                             scm_from_pointer ( ( void* ) p, free ),
                             scm_from_int ( len ),
                             scm_from_int ( 0 ),
                                                  scm_string_to_symbol ( ScmFromString( "u8" ) ) );
          return scmret;
     } else if ( scm_is_bytevector ( scm_par ) ) {
          int len = SCM_BYTEVECTOR_LENGTH ( scm_par );
          char *s = ( char* ) SCM_BYTEVECTOR_CONTENTS ( scm_par );
          char * p = pt->cypher ( s, len );
          SCM scmret = scm_pointer_to_bytevector (
                             scm_from_pointer ( ( void* ) p, free ),
                             scm_from_int ( len ),
                             scm_from_int ( 0 ),
                                                  scm_string_to_symbol ( ScmFromString( "u8" ) ) );
          return scmret;
     } else {
          return SCM_EOL;
     }
}

static SCM mtfa_ecipher_crc64 ( SCM scm_val )
{
     //mtfa_mat_cypher_ext * pt = ( mtfa_mat_cypher_ext* ) scm_foreign_object_signed_ref ( scm_ec, 0 );
     unsigned long long ullret;

     if ( scm_is_number ( scm_val ) ) {
          unsigned long long seed = scm_to_ulong_long ( scm_val );

          ullret = fastcrc64::crc64speed_native(0, ( char* ) &seed, sizeof ( seed ) );
          return scm_from_ulong_long(ullret);
     } else if ( scm_is_string ( scm_val ) ) {
          size_t len;
          char * s = ScmToStringn( scm_val, &len );
          ullret = fastcrc64::crc64speed_native(0, s, len);
          free ( s );
          return scm_from_ulong_long(ullret);
     } else if ( scm_is_bytevector ( scm_val ) ) {
          int len = SCM_BYTEVECTOR_LENGTH ( scm_val );
          char *s = ( char* ) SCM_BYTEVECTOR_CONTENTS ( scm_val );
          ullret = fastcrc64::crc64speed_native(0, s, len);
          return scm_from_ulong_long(ullret);
     } else {
          return SCM_EOL;
     }
}

static SCM mtfa_ecipher_decipher_s ( SCM scm_ec, SCM scm_par )
{
     mtfa_mat_cypher_ext * pt = ( mtfa_mat_cypher_ext* ) scm_foreign_object_signed_ref ( scm_ec, 0 );

     if ( scm_is_bytevector ( scm_par ) ) {
          int len = SCM_BYTEVECTOR_LENGTH ( scm_par );
          char *s = ( char* ) SCM_BYTEVECTOR_CONTENTS ( scm_par );
          char * p = pt->cypher ( s, len );

//           if ( len > 0 && p[len-1]==0 ) {
//                len--;
//           }
          SCM scmret =  ScmFromStringn( p, len );
          free ( p );
          return scmret;
     } else {
          return SCM_EOL;
     }
}

static SCM mtfa_ecipher_decipher_bv ( SCM scm_ec, SCM scm_par )
{
     mtfa_mat_cypher_ext * pt = ( mtfa_mat_cypher_ext* ) scm_foreign_object_signed_ref ( scm_ec, 0 );

     if ( scm_is_bytevector ( scm_par ) ) {
          int len = SCM_BYTEVECTOR_LENGTH ( scm_par );
          char *s = ( char* ) SCM_BYTEVECTOR_CONTENTS ( scm_par );
          char * p = pt->cypher ( s, len );
          SCM scmret = scm_pointer_to_bytevector (
                             scm_from_pointer ( ( void* ) p, free ),
                             scm_from_int ( len ),
                             scm_from_int ( 0 ),
                                                  scm_string_to_symbol ( ScmFromString( "u8" ) ) );

//           if ( len > 0 && p[len-1]==0 ) {
//                len--;
//           }
          return scmret;
     } else {
          return SCM_EOL;
     }
}

static SCM mtfa_ecipher_decipher_number ( SCM scm_ec, SCM scm_par )
{
    mtfa_mat_cypher_ext * pt = ( mtfa_mat_cypher_ext* ) scm_foreign_object_signed_ref ( scm_ec, 0 );

    if ( scm_is_bytevector ( scm_par ) ) {
          int len = SCM_BYTEVECTOR_LENGTH ( scm_par );
          char *s = ( char* ) SCM_BYTEVECTOR_CONTENTS ( scm_par );
          char * p = pt->cypher ( s, len );
          
          //ora p punta a un unsigned long long
        
          unsigned long long ullret = *(unsigned long long*)p;
          SCM scmret = scm_from_ulong_long(ullret);
          free(p);
          
          return scmret;
     } else {
          return SCM_EOL;
     }
}

static SCM mtfa_ecipher_p ( SCM mtfa_ecipher )
{
//      auto const body ( cify<SCM ( * ) ( void * ) >
//                        (
//      [&] ( void * data ) {
//           scm_assert_foreign_object_type ( mtfa_ecipher_type, mtfa_ecipher );
//           return SCM_BOOL_T;
//      }
//                        ) );
//      //SCM body (void *data);
// 
//      auto const handler ( cify<SCM ( * ) ( void *, SCM, SCM ) >
//                           (
//      [&] ( void *data, SCM key, SCM args ) {
//           return SCM_BOOL_F;
//      }
//                           ) );

    auto const body = [] (void * data) {
        scm_assert_foreign_object_type ( mtfa_ecipher_type, *(SCM*)data);
        return SCM_BOOL_T;
    };
    auto const handler = [] ( void *data, SCM key, SCM args ) {
        return SCM_BOOL_F;
    };
    //SCM handler (void *data, SCM key, SCM args);

     SCM ret = scm_internal_catch ( SCM_BOOL_T, body, &mtfa_ecipher, handler, nullptr );

     return ret;
}

extern "C" void init_mtfa_ecipher();
extern "C" void init_mtfa_ecipher()
{
     SCM name, slots;
     scm_t_struct_finalize finalizer;
     name = scm_from_latin1_symbol ( "mtfa-ecipher" );
     slots = scm_list_1 ( scm_from_latin1_symbol ( "decip" ) );
     finalizer = finalize_mtfa_ecipher;
     mtfa_ecipher_type = scm_make_foreign_object_type ( name, slots, finalizer );

     scm_c_define_gsubr ( "mtfa-ecipher-make", 1, 0, 0, ( void* ) make_mtfa_ecipher );
     scm_c_define_gsubr ( "mtfa-ecipher?", 1, 0, 0, ( void* ) mtfa_ecipher_p );
     scm_c_define_gsubr ( "mtfa-ecipher-mask", 2, 0, 0, ( void* ) mtfa_ecipher_mask );
     scm_c_define_gsubr ( "mtfa-ecipher-cipher", 2, 0, 0, ( void* ) mtfa_ecipher_cipher );
     scm_c_define_gsubr ( "mtfa-ecipher-decipher-s", 2, 0, 0, ( void* ) mtfa_ecipher_decipher_s );
     scm_c_define_gsubr ( "mtfa-ecipher-decipher-bv", 2, 0, 0, ( void* ) mtfa_ecipher_decipher_bv );
     scm_c_define_gsubr ( "mtfa-ecipher-decipher-number", 2, 0, 0, ( void* ) mtfa_ecipher_decipher_number );
     scm_c_define_gsubr ( "mtfa-ecipher-crc64", 1, 0, 0, ( void* ) mtfa_ecipher_crc64 );
}


