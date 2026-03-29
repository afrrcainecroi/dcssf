#include <iostream>
#include <string>

#include <stdlib.h>
#include <libguile.h>

#include <list>

#include <mtfa_utils.h>
#include <mtfa_fs3.h>


/**************INTERFACCIA GUILE*****************/

static SCM mtfa_star_type;  //il tipo del su

static void finalize_mtfa_star ( SCM mtfa_star )
{
    mtfa_sequence_star * pt = ( mtfa_sequence_star* ) scm_foreign_object_signed_ref ( mtfa_star, 0 );

    if ( pt )
    {
        scm_foreign_object_signed_set_x ( mtfa_star, 0, 0 );
        delete pt;
    }
}

#define C_STRING_TO_SYMBOL(str) scm_string_to_symbol(ScmFromString(str))

static SCM make_mtfa_star ()
{
    mtfa_sequence_star* pt = new mtfa_sequence_star();
    return scm_make_foreign_object_1 ( mtfa_star_type, ( void* ) pt );
}

static SCM mtfa_star_p ( SCM mtfa_star )
{
    auto const body = [] ( void* data )
    {
        scm_assert_foreign_object_type ( mtfa_star_type, * ( SCM* ) data );
        return SCM_BOOL_T;
    };
    auto const handler = [] ( void* data, SCM key, SCM args )
    {
        return SCM_BOOL_F;
    };
    
    SCM ret = scm_internal_catch ( SCM_BOOL_T, body, &mtfa_star, handler, nullptr );
    
    return ret;
}

static SCM insert_mtfa_star (SCM mtfa_star, SCM scm_to_insert, SCM scm_id, SCM scm_jolly)
{
    if (SCM_BOOL_F == mtfa_star_p(mtfa_star))
        return SCM_EOL;
    
    mtfa_sequence_star* pt = ( mtfa_sequence_star* ) scm_foreign_object_signed_ref ( mtfa_star, 0 );
    //posso inserire sia stringhe, sie vettori di interi
    if (scm_is_string(scm_to_insert) && scm_is_string(scm_jolly))
    {
        string s = ScmToString(scm_to_insert);
        string jolly = ScmToString(scm_jolly);
        unsigned int id = scm_to_uint(scm_id);
        pt->Insert(s, id, (int)jolly[0]);
        return SCM_BOOL_T;
    }

    if (scm_list_p(scm_to_insert) && scm_is_integer(scm_jolly))
    {
        unsigned short int jolly = scm_to_uint(scm_jolly);
        unsigned int id = scm_to_uint(scm_id);

        //converte la lista in un vector<unsigned short>
        vector<unsigned short> vus;
        SCM p = scm_to_insert;
        while ( p != SCM_EOL)
        {
            unsigned int v = scm_to_uint(scm_car(p));
            if (v == jolly)
                vus.push_back(mtfa_sequence_star::jollyvalue);
            else
                vus.push_back(v);
            p = scm_cdr(p);
        }
        pt->Insert(vus, id);
        return SCM_BOOL_T;
    }
    return ScmFromString("Parameters: string to insert, int id, string jolly char or vector<short>, int id, short jolly");
}

static SCM build_mtfa_star (SCM mtfa_star)
{
    if (SCM_BOOL_F == mtfa_star_p(mtfa_star))
        return SCM_EOL;
    mtfa_sequence_star* pt = ( mtfa_sequence_star* ) scm_foreign_object_signed_ref ( mtfa_star, 0 );
    unsigned int ret = pt->Build();
    return scm_from_uint(ret);
}

/*
 *    bool Search ( const unsigned char * s, int slen, unsigned int & ident);
 *    bool Search ( const vector<unsigned short> s, unsigned int & ident);
 *    void Serialize ( mtfa_binary_stream_save * bs );
 *    void Deserialize ( mtfa_binary_stream_load * bs );
 *    set<int> GetRules ( int );
 * 
 * */

static SCM search_mtfa_star (SCM mtfa_star, SCM scm_to_search)
{
    if (SCM_BOOL_F == mtfa_star_p(mtfa_star))
        return SCM_EOL;
    mtfa_sequence_star* pt = ( mtfa_sequence_star* ) scm_foreign_object_signed_ref ( mtfa_star, 0 );
    //posso cercare sia stringhe, sia vettori di interi
    if (scm_is_string(scm_to_search))
    {
        string s = ScmToString(scm_to_search);
        unsigned int id;
        bool bret = pt->Search((const unsigned char*)s.c_str(), s.size(), id);
        if (bret)
            return scm_from_uint(id);
        else
            return SCM_BOOL_F;
    }

    if (scm_list_p(scm_to_search))
    {
        //converte la lista in un vector<unsigned short>
        vector<unsigned short> vus;
        SCM p = scm_to_search;
        while ( p != SCM_EOL)
        {
            unsigned int v = scm_to_uint(scm_car(p));
            vus.push_back(v);
            p = scm_cdr(p);
        }
        unsigned int id;
        bool bret = pt->Search(vus, id);
        if (bret)
            return scm_from_uint(id);
        else
            return SCM_BOOL_F;
    }
    return ScmFromString("Parameters: string to search or list of short integers to search");
}

static SCM getrules_mtfa_star (SCM mtfa_star, SCM scm_id)
{
    if (SCM_BOOL_F == mtfa_star_p(mtfa_star))
        return SCM_EOL;
    if (!scm_is_integer(scm_id))
        return SCM_EOL;

    mtfa_sequence_star* pt = ( mtfa_sequence_star* ) scm_foreign_object_signed_ref ( mtfa_star, 0 );
    set<int> si = pt->GetRules(scm_to_int(scm_id));
    SCM lret = SCM_EOL;
    for (auto it: si)
    {
        lret = scm_cons(scm_from_int(it), lret);
    }
    return lret;
}

static SCM serialize_mtfa_star ( SCM mtfa_star )
{
    if (SCM_BOOL_F == mtfa_star_p(mtfa_star))
        return SCM_EOL;
    mtfa_sequence_star* pt = ( mtfa_sequence_star* ) scm_foreign_object_signed_ref ( mtfa_star, 0 );

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

static SCM deserialize_mtfa_star ( SCM mtfa_star, SCM bytevector_in )
{
    if (SCM_BOOL_F == mtfa_star_p(mtfa_star))
        return SCM_EOL;
    mtfa_sequence_star* pt = ( mtfa_sequence_star* ) scm_foreign_object_signed_ref ( mtfa_star, 0 );
    
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

extern "C" void init_mtfa_star();
extern "C" void init_mtfa_star()
{
    SCM name, slots;
    scm_t_struct_finalize finalizer;
    name = scm_from_latin1_symbol ( "mtfa-star" );
    slots = scm_list_1 ( scm_from_latin1_symbol ( "dss" ) );
    finalizer = finalize_mtfa_star;
    mtfa_star_type = scm_make_foreign_object_type ( name, slots, finalizer );

    scm_c_define_gsubr ( "mtfa-star-make", 0, 0, 0, ( void* ) make_mtfa_star );
    scm_c_define_gsubr ( "mtfa-star?", 1, 0, 0, ( void* ) mtfa_star_p );

    scm_c_define_gsubr ( "mtfa-star-insert", 4, 0, 0, ( void* ) insert_mtfa_star);
    scm_c_define_gsubr ( "mtfa-star-search", 2, 0, 0, ( void* ) search_mtfa_star);
    scm_c_define_gsubr ( "mtfa-star-get-rules", 2, 0, 0, ( void* ) getrules_mtfa_star);
    scm_c_define_gsubr ( "mtfa-star-build", 1, 0, 0, ( void* ) build_mtfa_star);
    scm_c_define_gsubr ( "mtfa-star-serialize", 1, 0, 0, ( void* ) serialize_mtfa_star);
    scm_c_define_gsubr ( "mtfa-star-deserialize", 2, 0, 0, ( void* ) deserialize_mtfa_star);
}
