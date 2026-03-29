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
//Una unprotect che comunque non emette errore
static SCM Unprotect ( SCM obj )
{
    auto const body = [] ( void* data )
    {
        scm_gc_unprotect_object ( * ( SCM* ) data );
        return SCM_BOOL_T;
    };
    auto const handler = [] ( void* data, SCM key, SCM args )
    {
        return SCM_BOOL_F;
    };
    
    SCM ret = scm_internal_catch ( SCM_BOOL_T, body, &obj, handler, nullptr );
    return ret;
}


static void finalize_mtfa_fs3 ( SCM mtfa_fs3 )
{
    ImplicitDs* pt = ( ImplicitDs* ) scm_foreign_object_signed_ref ( mtfa_fs3, 0 );

    if ( pt )
    {
        scm_foreign_object_signed_set_x ( mtfa_fs3, 0, 0 );

        for ( int i = 0; i < pt->vs.Size(); ++i )
        {
            if ( ( *pt->p->v_dati ) [i] != nullptr )
            {
                Unprotect ( ( SCM ) pt->vs[i] );
            }
        }

        delete pt;
    }
}

static SCM make_mtfa_fs3 ()
{
    ImplicitDs* pt = new ImplicitDs();
    return scm_make_foreign_object_1 ( mtfa_fs3_type, ( void* ) pt );
}

static SCM add_mtfa_fs3 ( SCM mtfa_fs3, SCM key, SCM value )
{
    scm_gc_protect_object ( value );
    ImplicitDs* pt = ( ImplicitDs* ) scm_foreign_object_signed_ref ( mtfa_fs3, 0 );

    size_t len;
    char* s = ScmToStringn ( key, &len );

    bool bret = pt->Insert ( s, len, ( void* ) value );
    free ( s );

    return bret ? SCM_BOOL_T : SCM_BOOL_F;
}

static SCM update_mtfa_fs3 ( SCM mtfa_fs3, SCM key, SCM value )
{
    scm_gc_protect_object ( value );

    int pos = -1;

    ImplicitDs* pt = ( ImplicitDs* ) scm_foreign_object_signed_ref ( mtfa_fs3, 0 );

    size_t len;
    char* s = ScmToStringn ( key, &len );

    //Cerco!
    void* prec;
    bool bret = pt->Find ( s, len, &prec );

    if ( !bret )
    {
        bret = pt->Insert ( s, len, ( void* ) value );
    }
    else
    {
        Unprotect ( ( SCM ) prec );
        bret = pt->Update ( s, len, ( void* ) value );
    }

    free ( s );
    return bret ? SCM_BOOL_T : SCM_BOOL_F;
}

static SCM get_mtfa_fs3 ( SCM mtfa_fs3, SCM key )
{
    ImplicitDs* pt = ( ImplicitDs* ) scm_foreign_object_signed_ref ( mtfa_fs3, 0 );

    size_t len;
    char* s = ScmToStringn ( key, &len );

    //Cerco!
    void* pos;
    bool bret = pt->Find ( s, len, &pos );
    free ( s );

    if ( bret )
    {
        return ( SCM ) pos;
    }
    else
    {
        return SCM_EOL;
    }
}

static SCM mtfa_fs3_get_all ( SCM mtfa_fs3 )
{
    ImplicitDs* pt = ( ImplicitDs* ) scm_foreign_object_signed_ref ( mtfa_fs3, 0 );

    set<pair<string, void*>> st = pt->GetAll();
    SCM list = SCM_EOL;

    for ( int i = 0; i < pt->vs.Size(); ++i )
    {
        if ( ( *pt->p->v_dati ) [i] != nullptr )
        {
            int len;
            unsigned char* val = pt->p->v_dati->Get ( i, &len );
            SCM k = ScmFromStringn ( ( const char* ) val, len );
            SCM v = ( SCM ) pt->vs[i];
            list = scm_cons ( scm_cons ( k, v ), list );
        }
    }

    return scm_reverse ( list );
}

static SCM for_each_mtfa_fs3 ( SCM mtfa_fs3, SCM function )
{
    ImplicitDs* pt = ( ImplicitDs* ) scm_foreign_object_signed_ref ( mtfa_fs3, 0 );

    for ( int i = 0; i < pt->vs.Size(); ++i )
    {
        if ( ( *pt->p->v_dati ) [i] != nullptr )
        {
            int len;
            unsigned char* val = pt->p->v_dati->Get ( i, &len );
            scm_call_2 ( function, ScmFromStringn ( ( const char* ) val, len ), ( SCM ) pt->vs[i] );
        }
    }

    return SCM_BOOL_T;
}

static SCM GetLispSymbol( const string func)
{
    auto const body = [] ( void* data )
    {
        string * func = (string *)data;
        //mtfa_log("Cerca: %s\n", func->c_str());
        SCM scmRet = scm_c_lookup(func->c_str());
        return scm_variable_ref ( scmRet );
    };
    auto const handler = [] ( void* data, SCM key, SCM args )
    {
        return SCM_EOL;
    };
    SCM ret = scm_internal_catch ( SCM_BOOL_T, body, (void*)&func, handler, nullptr );
    return ret;
}

// static SCM mtfa_fs3_put_all ( SCM mtfa_fs3, SCM list )
// {
//      ImplicitDs * pt = ( ImplicitDs* ) scm_foreign_object_signed_ref ( mtfa_fs3, 0 );
//
//      int pos = 0;
//      while ( list != SCM_EOL ) {
//         //proteggo item ma se lo era già lo sproteggo e poi lo riproteggo
//         Unprotect( scm_car ( list ) );
//         scm_gc_protect_object ( scm_car ( list ) );
//         //lo inserisco
//         pt->vs[pos++] = ( void* ) scm_car ( list );
//         list = scm_cdr ( list );
//      }
//
//      return SCM_EOL;
// }

static SCM serialize_mtfa_fs3 ( SCM mtfa_fs3 )
{
    ImplicitDs* pt = ( ImplicitDs* ) scm_foreign_object_signed_ref ( mtfa_fs3, 0 );

    mtfa_binary_stream_save* bs = new mtfa_binary_stream_save ( "" );
    pt->Serialize ( bs );

    SCM mtfa_serializer   = GetLispSymbol ( "mtfa-serializer" );

    //Verifica se ci sono le funzioni, altrimenti nulla!
    if (mtfa_serializer != SCM_EOL)
    {
        //Prova a serializzare un numero
        bs->Append((unsigned int)pt->vs.Size());
        //Chiama la serialize anche per gli elementi contenuti
        for ( int i = 0; i < pt->vs.Size(); ++i )
        {
            SCM scm_string = scm_call ( mtfa_serializer, ( SCM ) pt->vs[i], SCM_UNDEFINED );
            string s = string(ScmToString(scm_string));
            //mtfa_log("Stringa %3d-esima: %s\n", i, s.c_str());
            bs->Append(s);
        }
    }
    else
    {
        mtfa_log("mtfa-serializer/mtfa-deserializer not found. Unable to serialize/deserialize mtfa-fs3\n");
    }

    std::string ser = bs->GetStream();
    delete bs;

    string cmpser;
    bool bret = mtfa_compress(ser, cmpser, 6);

    void* pser = malloc ( cmpser.size() );
    memcpy ( pser, cmpser.data(), cmpser.size() );
    SCM scm_ret = scm_pointer_to_bytevector (
                      scm_from_pointer ( pser, free ),
                      scm_from_int ( cmpser.size() ),
                      scm_from_int ( 0 ),
                      scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
    return scm_ret;
}

static SCM deserialize_mtfa_fs3 ( SCM mtfa_fs3, SCM bytevector_in )
{
    ImplicitDs* pt = ( ImplicitDs* ) scm_foreign_object_signed_ref ( mtfa_fs3, 0 );

    mtfa_binary_stream_load* bs = new mtfa_binary_stream_load ( "" );
    int len = SCM_BYTEVECTOR_LENGTH ( bytevector_in );
    char* p = ( char* ) SCM_BYTEVECTOR_CONTENTS ( bytevector_in );

//     mtfa_log("Da deserializzare: %d\n", len);
//     for (int i=0; i<len; ++i)
//         printf("%02X ", (unsigned char)p[i]);
//     cout << endl << endl;
    
    string decomp;
    bool bret = mtfa_uncompress(string(p, p+len), decomp);

    bs->mtfa_binary_stream_set ( decomp );
    pt->Deserialize ( bs );

    //ora, se possibile, deserializza il resto
    SCM mtfa_deserializer = GetLispSymbol ( "mtfa-deserializer" );
    if (mtfa_deserializer != SCM_EOL)
    {
        unsigned int vsize = bs->Read<unsigned int>();
        //mtfa_log("Dimensioni vettore: %d\n", vsize);
        //Chiama la deserialize anche per gli elementi contenuti
        for ( int i = 0; i < vsize; ++i )
        {
            string s = bs->Read();
            //mtfa_log("Stringa %3d-esima: <%s>\n", i, s.c_str());
            SCM scm_string = ScmFromString(s.c_str());
            SCM scm_item = scm_call_1 ( mtfa_deserializer, scm_string );
            scm_gc_protect_object ( scm_item );
            pt->vs[i] = (void*)scm_item;
        }
    }
    else
    {
        mtfa_log("mtfa-serializer/mtfa-deserializer not found. Unable to serialize/deserialize mtfa-fs3\n");
    }

    delete bs;

    return SCM_BOOL_T;
}


static SCM mtfa_fs3_p ( SCM mtfa_fs3 )
{
    auto const body = [] ( void* data )
    {
        scm_assert_foreign_object_type ( mtfa_fs3_type, * ( SCM* ) data );
        return SCM_BOOL_T;
    };
    auto const handler = [] ( void* data, SCM key, SCM args )
    {
        return SCM_BOOL_F;
    };

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
    name = scm_from_latin1_symbol ( "mtfa-fs3" );
    slots = scm_list_1 ( scm_from_latin1_symbol ( "ds" ) );
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
//      scm_c_define_gsubr ( "mtfa-fs3-put-all", 2, 0, 0, ( void* ) mtfa_fs3_put_all );
    scm_c_define_gsubr ( "mtfa-fs3-for-each", 2, 0, 0, ( void* ) for_each_mtfa_fs3 );

    //scm_c_define_gsubr("mtfa-fs3-write", 1, 0, 0,  (void*)mtfa_fs3_get_all);

//  scm_set_smob_print(SCM_UNPACK(mtfa_fs3_type), print_box);
//  fErrorHandlerProc = scm_c_eval_string(
//    "(lambda (key . args) (display (list \"Error in proc:\" key \" args: \" args)) (newline))"
//    );
//  scm_gc_protect_object(fErrorHandlerProc);

}


