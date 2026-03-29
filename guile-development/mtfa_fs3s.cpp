//è chiaro!

#include <stdlib.h>
#include <libguile.h>

#include <iostream>

#include <string>
#include <list>

#include <mtfa_fs3.h>

/**************INTERFACCIA GUILE*****************/

static SCM mtfa_fs3s_type;  //il tipo del fs3s

//static SCM fErrorHandlerProc;

static void finalize_mtfa_fs3s ( SCM mtfa_fs3s )
{
     ImplicitDsString * pt = ( ImplicitDsString * ) scm_foreign_object_signed_ref ( mtfa_fs3s, 0 );
     if ( pt ) {
          scm_foreign_object_signed_set_x ( mtfa_fs3s, 0, 0 );
          delete pt;
     }
}

static SCM make_mtfa_fs3s ()
{
     ImplicitDsString * pt = new ImplicitDsString();
     return scm_make_foreign_object_1 ( mtfa_fs3s_type, ( void* ) pt );

}

static SCM add_mtfa_fs3s ( SCM mtfa_fs3s, SCM key, SCM value )
{
     ImplicitDsString * pt = ( ImplicitDsString* ) scm_foreign_object_signed_ref ( mtfa_fs3s, 0 );

     size_t len = 0;
     char * s = ScmToStringn ( key, &len );

     size_t len1 = 0;
     char * s1 = ScmToStringn ( value, &len1 );

     bool bret = pt->Insert ( s, len, s1, len1 );
     free ( s );
     free ( s1 );

     return bret?SCM_BOOL_T:SCM_BOOL_F;
}

static SCM update_mtfa_fs3s ( SCM mtfa_fs3s, SCM key, SCM value )
{
     int pos=-1;

     ImplicitDsString * pt = ( ImplicitDsString* ) scm_foreign_object_signed_ref ( mtfa_fs3s, 0 );

     size_t len = 0;
     char * s = ScmToStringn ( key, &len );

     size_t len1 = 0;
     char * s1 = ScmToStringn ( value, &len1 );

     bool bret = pt->Update ( s, len, s1, len1 );
     free ( s );
     free ( s1 );

     return bret?SCM_BOOL_T:SCM_BOOL_F;
}

static SCM get_mtfa_fs3s ( SCM mtfa_fs3s, SCM key )
{
     ImplicitDsString * pt = ( ImplicitDsString* ) scm_foreign_object_signed_ref ( mtfa_fs3s, 0 );

     size_t len = 0;
     char * s = ScmToStringn( key, &len );

     //Cerco!
     char* pos;
     int lenpos;
     bool bret = pt->Find ( s, len, &pos, &lenpos );
     free ( s );

     SCM v = ScmFromStringn ( pos, lenpos );

     if ( bret ) {
          return ( SCM ) v;
     } else {
          return SCM_EOL;
     }
}

static SCM mtfa_fs3s_get_all ( SCM mtfa_fs3s )
{
     ImplicitDsString * pt = ( ImplicitDsString* ) scm_foreign_object_signed_ref ( mtfa_fs3s, 0 );

     set<pair<string, string>> st = pt->GetAll();
     SCM list = SCM_EOL;
     for ( auto it: st ) {
         SCM k = ScmFromStringn( it.first.c_str(), it.first.size() );
         SCM v = ScmFromStringn( it.second.c_str(), it.second.size() );
          list = scm_cons ( scm_cons ( k, v ), list );
     }
     return scm_reverse ( list );
}

//static SCM mtfa_fs3s_put_all (SCM mtfa_fs3s, SCM list)
//{
//  ImplicitDsString * pt = (ImplicitDsString*)scm_foreign_object_signed_ref (mtfa_fs3s, 0);
//
//  int pos = 0;
//  while (list != SCM_EOL)
//  {
//    pt->vs[pos++] = (void*)scm_car(list);
//    list = scm_cdr(list);
//  }
//
//  return SCM_EOL;
//}

static SCM serialize_mtfa_fs3s ( SCM mtfa_fs3s )
{
     ImplicitDsString * pt = ( ImplicitDsString* ) scm_foreign_object_signed_ref ( mtfa_fs3s, 0 );

     mtfa_binary_stream_save * bs = new mtfa_binary_stream_save ( "" );
     pt->Serialize ( bs );

     std::string ser = bs->GetStream();
     delete bs;

     char * pser = ( char * ) malloc ( ser.size() );
     memcpy ( pser, ser.data(), ser.size() );
     SCM scm_ret = scm_pointer_to_bytevector (
                        scm_from_pointer ( ( void* ) pser, free ),
                        scm_from_int ( ser.size() ),
                        scm_from_int ( 0 ),
                                              scm_string_to_symbol ( ScmFromString( "u8" ) ) );
     return scm_ret;
}

static SCM deserialize_mtfa_fs3s ( SCM mtfa_fs3s, SCM bytevector_in )
{
     ImplicitDsString * pt = ( ImplicitDsString* ) scm_foreign_object_signed_ref ( mtfa_fs3s, 0 );

     mtfa_binary_stream_load * bs = new mtfa_binary_stream_load ( "" );

     int len = SCM_BYTEVECTOR_LENGTH ( bytevector_in );
     char * p = ( char* ) SCM_BYTEVECTOR_CONTENTS ( bytevector_in );

     bs->mtfa_binary_stream_set ( std::string ( p, len ) );
     pt->Deserialize ( bs );

     delete bs;

     return SCM_BOOL_T;
}


static SCM mtfa_fs3s_p ( SCM mtfa_fs3s )
{
//      auto const body ( cify<SCM ( * ) ( void * ) >
//                        (
//      [&] ( void * data ) {
//           //se va tutto bene, torna true
//           scm_assert_foreign_object_type ( mtfa_fs3s_type, mtfa_fs3s );
//           return SCM_BOOL_T;
//      }
//                        ) );
//      //SCM body (void *data);
// 
//      auto const handler ( cify<SCM ( * ) ( void *, SCM, SCM ) >
//                           (
//      [&] ( void *data, SCM key, SCM args ) {
//           //Se scatta l'handler, torma false
//           return SCM_BOOL_F;
//      }
//                           ) );
     //SCM handler (void *data, SCM key, SCM args);

    auto const body = [] (void * data) {
        scm_assert_foreign_object_type ( mtfa_fs3s_type, *(SCM*)data);
        return SCM_BOOL_T;
    };
    auto const handler = [] ( void *data, SCM key, SCM args ) {
        return SCM_BOOL_F;
    };
    
    SCM ret = scm_internal_catch ( SCM_BOOL_T, body, &mtfa_fs3s, handler, nullptr );

     return ret;
}

//static int print_box (SCM b, SCM port, scm_print_state *pstate)
//{
////  SCM value = SCM_CELL_OBJECT_1 (b);
////
//  scm_puts ("#<box111111 ", port);
//  //scm_write (value, port);
//  scm_puts (">", port);
//
//  /* Non-zero means success.  */
//  return 1;
//}

extern "C" void init_mtfa_fs3s();

extern "C" void init_mtfa_fs3s()
{
     SCM name, slots;
     scm_t_struct_finalize finalizer;
     name = scm_from_latin1_symbol ( "mtfa-fs3s" );
     slots = scm_list_1 ( scm_from_latin1_symbol ( "dss" ) );
     finalizer = finalize_mtfa_fs3s;
     mtfa_fs3s_type = scm_make_foreign_object_type ( name, slots, finalizer );

//  scm_tc16_box = scm_make_smob_type ("box", 0);
//  scm_set_smob_mark (scm_tc16_box, mark_box);
//  scm_set_smob_print (scm_tc16_box, print_box);



     scm_c_define_gsubr ( "mtfa-fs3s-make", 0, 0, 0, ( void* ) make_mtfa_fs3s );
     scm_c_define_gsubr ( "mtfa-fs3s-add", 3, 0, 0, ( void* ) add_mtfa_fs3s );
     scm_c_define_gsubr ( "mtfa-fs3s-get", 2, 0, 0, ( void* ) get_mtfa_fs3s );
     scm_c_define_gsubr ( "mtfa-fs3s-update", 3, 0, 0, ( void* ) update_mtfa_fs3s );
     scm_c_define_gsubr ( "mtfa-fs3s-serialize", 1, 0, 0, ( void* ) serialize_mtfa_fs3s );
     scm_c_define_gsubr ( "mtfa-fs3s-deserialize", 2, 0, 0, ( void* ) deserialize_mtfa_fs3s );
     scm_c_define_gsubr ( "mtfa-fs3s?", 1, 0, 0, ( void* ) mtfa_fs3s_p );
     scm_c_define_gsubr ( "mtfa-fs3s-get-all", 1, 0, 0, ( void* ) mtfa_fs3s_get_all );
     //scm_c_define_gsubr("mtfa-fs3s-put-all", 2, 0, 0,  (void*)mtfa_fs3s_put_all);
     //scm_c_define_gsubr("mtfa-fs3s-write", 1, 0, 0,  (void*)mtfa_fs3s_get_all);

//  scm_set_smob_print(SCM_UNPACK(mtfa_fs3s_type), print_box);
//  fErrorHandlerProc = scm_c_eval_string(
//    "(lambda (key . args) (display (list \"Error in proc:\" key \" args: \" args)) (newline))"
//    );
//  scm_gc_protect_object(fErrorHandlerProc);

}


