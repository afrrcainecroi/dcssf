//è chiaro!

#include <stdlib.h>
#include <libguile.h>

#include <iostream>

#include <string>
#include <list>

#include <mtfa_fs3.h>

/**************INTERFACCIA GUILE*****************/

static SCM mtfa_fs3_type;  //il tipo del fs3

//static SCM fErrorHandlerProc;

static void finalize_mtfa_fs3 ( SCM mtfa_fs3 )
{
     ImplicitDs * pt = ( ImplicitDs * ) scm_foreign_object_signed_ref ( mtfa_fs3, 0 );

     //std::cout << "elimina ds" << std::endl;

     if ( pt ) {
          scm_foreign_object_signed_set_x ( mtfa_fs3, 0, 0 );
          std::set<std::pair<std::string, void*>> ls = pt->GetAll();
          //std::cout << "Sblocca " << ls.size() << " elementi\n";
          for ( auto it: ls ) {
               scm_gc_unprotect_object ( ( SCM ) it.second );
          }
          delete pt;
     }
     //scm_gc_unprotect_object(fErrorHandlerProc);
}

static SCM make_mtfa_fs3 ()
{
     ImplicitDs * pt = new ImplicitDs();
     return scm_make_foreign_object_1 ( mtfa_fs3_type, ( void* ) pt );

}

static SCM add_mtfa_fs3 ( SCM mtfa_fs3, SCM key, SCM value )
{
//  scm_gc_protect_object(key);
     scm_gc_protect_object ( value );

     ImplicitDs * pt = ( ImplicitDs* ) scm_foreign_object_signed_ref ( mtfa_fs3, 0 );

     size_t len = 0;
     char * s = scm_to_locale_stringn ( key, &len );
     bool bret = pt->Insert ( s, len, ( void* ) value );
     free ( s );


//  scm_gc_unprotect_object(key);

     return bret?SCM_BOOL_T:SCM_BOOL_F;
}

static SCM update_mtfa_fs3 ( SCM mtfa_fs3, SCM key, SCM value )
{
     //scm_gc_protect_object(key);
     scm_gc_protect_object ( value );

     int pos=-1;

     ImplicitDs * pt = ( ImplicitDs* ) scm_foreign_object_signed_ref ( mtfa_fs3, 0 );

     size_t len = 0;
     char * s = scm_to_locale_stringn ( key, &len );

     //Cerco!
     void * prec;
     bool bret = pt->Find ( s, len, &prec );

     //std::cout << "Cerca: " << std::string(s, s+len) << " => " << bret << std::endl;

     if ( !bret ) {
          //non c'era nulla, inserisco
          //std::cout << "Inserisce\n";
          bret = pt->Insert ( s, len, ( void* ) value );
     } else {
          //C'era, libero precedente e inserisco nuovo
          //std::cout << "Sprotegge e poi Inserisce\n";
          scm_gc_unprotect_object ( ( SCM ) prec );
          bret = pt->Update ( s, len, ( void* ) value );
     }
     free ( s );


     //scm_gc_unprotect_object(key);

     return bret?SCM_BOOL_T:SCM_BOOL_F;
}

static SCM get_mtfa_fs3 ( SCM mtfa_fs3, SCM key )
{
     //scm_gc_protect_object(mtfa_fs3);
     //scm_gc_protect_object(key);

     ImplicitDs * pt = ( ImplicitDs* ) scm_foreign_object_signed_ref ( mtfa_fs3, 0 );

     //Converto value in stringa
     size_t len = 0;
     char * s = scm_to_locale_stringn ( key, &len );

     //Cerco!
     void * pos;
     bool bret = pt->Find ( s, len, &pos );
     free ( s );

     //scm_gc_unprotect_object(key);

     if ( bret ) {
          return ( SCM ) pos;
     } else {
          return SCM_EOL;
     }
}

static SCM mtfa_fs3_get_all ( SCM mtfa_fs3 )
{
     ImplicitDs * pt = ( ImplicitDs* ) scm_foreign_object_signed_ref ( mtfa_fs3, 0 );

     //Torna il vettore degli elementi, indicizzati per posizione
     //In modo che possa essere salvato dall'esterno!
     /*
     SCM list = SCM_EOL;
     for (int i=0; i<pt->vs.righe; ++i)
     {
         list = scm_cons ((SCM)pt->vs[i], list);
     }
     */

     set<pair<string, void*>> st = pt->GetAll();
     SCM list = SCM_EOL;
     for ( auto it: st ) {
          SCM k = scm_from_locale_string ( it.first.c_str() );
          SCM v = ( SCM ) it.second;
          list = scm_cons ( scm_cons ( k, v ), list );
     }
     return scm_reverse ( list );
}

static SCM mtfa_fs3_put_all ( SCM mtfa_fs3, SCM list )
{
     ImplicitDs * pt = ( ImplicitDs* ) scm_foreign_object_signed_ref ( mtfa_fs3, 0 );

     int pos = 0;
     while ( list != SCM_EOL ) {
          pt->vs[pos++] = ( void* ) scm_car ( list );
          list = scm_cdr ( list );
     }

     return SCM_EOL;
}

static SCM serialize_mtfa_fs3 ( SCM mtfa_fs3 )
{
     ImplicitDs * pt = ( ImplicitDs* ) scm_foreign_object_signed_ref ( mtfa_fs3, 0 );

     mtfa_binary_stream_save * bs = new mtfa_binary_stream_save ( "" );
     pt->Serialize ( bs );

     //std::cout << "Ci sono " << pt->vs.righe << " righe.\n";
//  bs->Append(pt->vs.righe); //quanti elementi ci sono.
//  for (int i=0; i<pt->vs.righe; ++i)
//  {
//    //  std::cout << "Serializzo elemento: " << i << "-esimo\n";
//    SCM str = scm_object_to_string((SCM)pt->vs[i], SCM_UNDEFINED);
//    size_t len = 0;
//    char * s = scm_to_locale_stringn(str, &len);
//    bs->Append((unsigned char *)s, len);
//    free(s);
//  }

     std::string ser = bs->GetStream();
     delete bs;

     SCM scm_ret = scm_pointer_to_bytevector (
                        scm_from_pointer ( ( void* ) ser.data(), nullptr ),
                        scm_from_int ( ser.size() ),
                        scm_from_int ( 0 ),
                        scm_string_to_symbol ( scm_from_locale_string ( "u8" ) ) );
     return scm_ret;
}

static SCM deserialize_mtfa_fs3 ( SCM mtfa_fs3, SCM bytevector_in )
{
     ImplicitDs * pt = ( ImplicitDs* ) scm_foreign_object_signed_ref ( mtfa_fs3, 0 );

     mtfa_binary_stream_load * bs = new mtfa_binary_stream_load ( "" );
     int len = SCM_BYTEVECTOR_LENGTH ( bytevector_in );
     char * p = ( char* ) SCM_BYTEVECTOR_CONTENTS ( bytevector_in );

     bs->mtfa_binary_stream_set ( std::string ( p, p+len ) );
     pt->Deserialize ( bs );

//    int righe = bs->ReadUI(); //quanti elementi ci sono.
//    for (int i=0; i<righe; ++i)
//    {
//        unsigned int len;
//        unsigned char * s = bs->ReadVUC(len);
//
//        for (int k=0; k<len; ++k)
//        {
//            if (s[k] >=32 && s[k]<127)
//                putchar(s[k]);
//            else if (s[k]=='\n')
//                std::cout << std::endl;
//            else
//                putchar('.');
//        }
//        std::cout << std::endl;
//        std::cout << "In effetti ho ricevuto: <" << std::string (s, s+len) << ">\n";
//        SCM str = scm_from_locale_stringn((const char*)s, len);
//
//        //ma ho convertito bene?
//        size_t len1;
//        char * s1 = scm_to_locale_stringn (str, &len1);
//        std::cout << "La doppia conversione: <" << std::string (s1, s1+len1) << ">\n";
//
//        //ora devo chiamare la procedura che ricostruisce l'oggetto SCM!
//        SCM it = scm_call_with_input_string(str, (SCM)scm_read);
//
//        pt->vs[i] = (void*)it;
//        free(s);
//    }


     delete bs;

     return SCM_BOOL_T;
}


static SCM mtfa_fs3_p ( SCM mtfa_fs3 )
{
     auto const body ( cify<SCM ( * ) ( void * ) >
                       (
     [&] ( void * data ) {
          //se va tutto bene, torna true
          scm_assert_foreign_object_type ( mtfa_fs3_type, mtfa_fs3 );
          return SCM_BOOL_T;
     }
                       ) );
     //SCM body (void *data);

     auto const handler ( cify<SCM ( * ) ( void *, SCM, SCM ) >
                          (
     [&] ( void *data, SCM key, SCM args ) {
          //Se scatta l'handler, torma false
          return SCM_BOOL_F;
     }
                          ) );
     //SCM handler (void *data, SCM key, SCM args);

     SCM ret = scm_internal_catch ( SCM_BOOL_T, body, &mtfa_fs3, handler, nullptr );

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

extern "C" void init_mtfa_fs3();

extern "C" void init_mtfa_fs3()
{
     SCM name, slots;
     scm_t_struct_finalize finalizer;
     name = scm_from_utf8_symbol ( "mtfa-fs3" );
     slots = scm_list_1 ( scm_from_utf8_symbol ( "ds" ) );
     finalizer = finalize_mtfa_fs3;
     mtfa_fs3_type = scm_make_foreign_object_type ( name, slots, finalizer );

//  scm_tc16_box = scm_make_smob_type ("box", 0);
//  scm_set_smob_mark (scm_tc16_box, mark_box);
//  scm_set_smob_print (scm_tc16_box, print_box);



     scm_c_define_gsubr ( "mtfa-fs3-make", 0, 0, 0, ( void* ) make_mtfa_fs3 );
     scm_c_define_gsubr ( "mtfa-fs3-add", 3, 0, 0, ( void* ) add_mtfa_fs3 );
     scm_c_define_gsubr ( "mtfa-fs3-get", 2, 0, 0, ( void* ) get_mtfa_fs3 );
     scm_c_define_gsubr ( "mtfa-fs3-update", 3, 0, 0, ( void* ) update_mtfa_fs3 );
     scm_c_define_gsubr ( "mtfa-fs3-serialize", 1, 0, 0, ( void* ) serialize_mtfa_fs3 );
     scm_c_define_gsubr ( "mtfa-fs3-deserialize", 2, 0, 0, ( void* ) deserialize_mtfa_fs3 );
     scm_c_define_gsubr ( "mtfa-fs3?", 1, 0, 0, ( void* ) mtfa_fs3_p );
     scm_c_define_gsubr ( "mtfa-fs3-get-all", 1, 0, 0, ( void* ) mtfa_fs3_get_all );
     scm_c_define_gsubr ( "mtfa-fs3-put-all", 2, 0, 0, ( void* ) mtfa_fs3_put_all );
     //scm_c_define_gsubr("mtfa-fs3-write", 1, 0, 0,  (void*)mtfa_fs3_get_all);

//  scm_set_smob_print(SCM_UNPACK(mtfa_fs3_type), print_box);
//  fErrorHandlerProc = scm_c_eval_string(
//    "(lambda (key . args) (display (list \"Error in proc:\" key \" args: \" args)) (newline))"
//    );
//  scm_gc_protect_object(fErrorHandlerProc);

}


