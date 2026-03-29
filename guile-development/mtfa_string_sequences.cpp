#include <iostream>
#include <string>

#include <stdlib.h>
#include <libguile.h>

#include <list>

#include <mtfa_utils.h>
#include <mtfa_fs3.h>


/**************INTERFACCIA GUILE*****************/
// //La gestione di sequenze di stringhe!
// class mtfa_string_sequences {
// private:
//     mtfa_umph ph;
//     mtfa_sequence_base *seq;
//     unordered_map<int, vector<uint8_t>> itovui8;
//     
// public:
//     mtfa_string_sequences();
//     ~mtfa_string_sequences();
// 
//     int insert(const list<string> & ls, const int len=3);  //len va da 1 a 4 i base alla numerosità delle stringhe di input differenti
//     int insert(const vector <uint8_t> vui8);
//     int search(const list<string> & ls, const int len=3);
//     int search(const vector <uint8_t> vui8);
//     list<string> getval(int idx);
//     vector<uint8_t> getvalvui8(int idx);
// };


static SCM mtfa_sss_type;  //il tipo del su

static void finalize_mtfa_sss ( SCM mtfa_sss)
{
    mtfa_string_sequences* pt = ( mtfa_string_sequences* ) scm_foreign_object_signed_ref ( mtfa_sss, 0 );

    if ( pt )
    {
        scm_foreign_object_signed_set_x ( mtfa_sss, 0, 0 );
        delete pt;
    }
}

#define C_STRING_TO_SYMBOL(str) scm_string_to_symbol(ScmFromString(str))

static SCM make_mtfa_sss ()
{
    mtfa_string_sequences* pt = new mtfa_string_sequences();
    return scm_make_foreign_object_1 ( mtfa_sss_type, ( void* ) pt );
}

static SCM mtfa_sss_p ( SCM mtfa_sss)
{
    auto const body = [] ( void* data )
    {
        scm_assert_foreign_object_type ( mtfa_sss_type, * ( SCM* ) data );
        return SCM_BOOL_T;
    };
    auto const handler = [] ( void* data, SCM key, SCM args )
    {
        return SCM_BOOL_F;
    };
    
    SCM ret = scm_internal_catch ( SCM_BOOL_T, body, &mtfa_sss, handler, nullptr );
    
    return ret;
}

//Solo liste di stringhe
static SCM insert_mtfa_sss (SCM mtfa_sss, SCM scm_to_insert)
{
    if (SCM_BOOL_F == mtfa_sss_p(mtfa_sss))
        return SCM_BOOL_F;
    
    mtfa_string_sequences* pt = ( mtfa_string_sequences* ) scm_foreign_object_signed_ref ( mtfa_sss, 0 );

    //posso inserire solo liste di stringhe
    if (scm_list_p(scm_to_insert)==SCM_BOOL_T)
    {
        list<string> ls;
        while (!scm_is_null(scm_to_insert))
        {
            SCM it = scm_car(scm_to_insert);
            string s = ScmToString(it);
            scm_to_insert = scm_cdr(scm_to_insert);
            ls.push_back(s);
        }
        int iret = pt->insert(ls);
        return scm_from_int32(iret);
    }
    if (scm_is_bytevector(scm_to_insert))
    {
        int len = SCM_BYTEVECTOR_LENGTH ( scm_to_insert );
        char * p = ( char* ) SCM_BYTEVECTOR_CONTENTS ( scm_to_insert );
        vector<uint8_t> vui8;
        vui8.assign(p, p+len);
        int iret = pt->seq->Insert(vui8);
        return scm_from_int32(iret);
    }
    return ScmFromString("Parameters: list of string or bytevector to insert");
}

static SCM to_bytevector_mtfa_sss (SCM mtfa_sss, SCM scm_to_insert)
{
    if (SCM_BOOL_F == mtfa_sss_p(mtfa_sss))
        return SCM_BOOL_F;
    
    mtfa_string_sequences* pt = ( mtfa_string_sequences* ) scm_foreign_object_signed_ref ( mtfa_sss, 0 );

    //posso inserire solo liste di stringhe
    if (scm_list_p(scm_to_insert)==SCM_BOOL_T)
    {
        list<string> ls;
        while (!scm_is_null(scm_to_insert))
        {
            string s = ScmToString(scm_car(scm_to_insert));
            scm_to_insert = scm_cdr(scm_to_insert);
            ls.push_back(s);
        }
        vector<uint8_t> vui8 = mtfa_LsToVui8(pt->ph, ls);
        unsigned char * p = (unsigned char*)malloc(vui8.size());
        std::copy(vui8.begin(), vui8.end(), p);
        SCM scm_ret = scm_pointer_to_bytevector (
                scm_from_pointer ( ( void* ) p, free ),
                scm_from_int ( vui8.size() ),
                scm_from_int ( 0 ),
                scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
        return scm_ret;
    }
    return ScmFromString("Parameters: list of string to convert in bytevector");
}


static SCM search_mtfa_sss (SCM mtfa_sss, SCM scm_to_search)
{
    if (SCM_BOOL_F == mtfa_sss_p(mtfa_sss))
        return SCM_BOOL_F;
    
    mtfa_string_sequences* pt = ( mtfa_string_sequences* ) scm_foreign_object_signed_ref ( mtfa_sss, 0 );

    if (scm_list_p(scm_to_search)==SCM_BOOL_T)
    {
        //devo convertire tutta la lista di stringhe in c++
        list<string> ls;
        while (!scm_is_null(scm_to_search))
        {
            string s = ScmToString(scm_car(scm_to_search));
            scm_to_search = scm_cdr(scm_to_search);
            ls.push_back(s);
        }
        int iret = pt->search(ls);
        return scm_from_int32(iret);
    }
    if (scm_is_bytevector(scm_to_search))
    {
        int len = SCM_BYTEVECTOR_LENGTH ( scm_to_search );
        char * p = ( char* ) SCM_BYTEVECTOR_CONTENTS ( scm_to_search );
        vector<uint8_t> vui8;
        vui8.assign(p, p+len);
        int iret = pt->seq->Search(vui8);
        return scm_from_int32(iret);
    }

    return ScmFromString("Parameters: list of string or bytevector to search");
}

static SCM getall_mtfa_sss (SCM mtfa_sss, SCM scm_to_search)
{
    if (SCM_BOOL_F == mtfa_sss_p(mtfa_sss))
        return SCM_BOOL_F;
    
    mtfa_string_sequences* pt = ( mtfa_string_sequences* ) scm_foreign_object_signed_ref ( mtfa_sss, 0 );

    if (scm_list_p(scm_to_search)==SCM_BOOL_T)
    {
        //devo convertire tutta la lista di stringhe in c++
        list<string> ls;
        while (!scm_is_null(scm_to_search))
        {
            string s = ScmToString(scm_car(scm_to_search));
            scm_to_search = scm_cdr(scm_to_search);
            ls.push_back(s);
        }
        
        vector<uint8_t> vui8 = mtfa_LsToVui8(pt->ph, ls);
        unordered_set<long> us = pt->seq->GetAll( vui8 );
        
        SCM lret = SCM_EOL;
        for (auto id: us)
        {
            auto ls = pt->getval(id);
            SCM l = SCM_EOL;
            for (auto s: ls)
            {
                l = scm_cons(ScmFromString(s.c_str()), l);
            }
            lret = scm_cons(l, lret);
        }
        return lret;
    }
    return ScmFromString("Parameters: list of string to search");
}

static SCM unlink_mtfa_sss (SCM mtfa_sss, SCM scm_to_unlink)
{
    if (SCM_BOOL_F == mtfa_sss_p(mtfa_sss))
        return SCM_BOOL_F;
    
    mtfa_string_sequences* pt = ( mtfa_string_sequences* ) scm_foreign_object_signed_ref ( mtfa_sss, 0 );

    if (scm_list_p(scm_to_unlink)==SCM_BOOL_T)
    {
        //devo convertire tutta la lista di stringhe in c++
        list<string> ls;
        while (!scm_is_null(scm_to_unlink))
        {
            string s = ScmToString(scm_car(scm_to_unlink));
            scm_to_unlink = scm_cdr(scm_to_unlink);
            ls.push_back(s);
        }
        bool iret = pt->unlink(ls);
        return scm_from_bool(iret);
    }
    if (scm_is_bytevector(scm_to_unlink))
    {
        int len = SCM_BYTEVECTOR_LENGTH ( scm_to_unlink );
        char * p = ( char* ) SCM_BYTEVECTOR_CONTENTS ( scm_to_unlink );
        vector<uint8_t> vui8;
        vui8.assign(p, p+len);
        bool iret = pt->seq->Unlink(vui8);
        return scm_from_bool(iret);
    }

    return ScmFromString("Parameters: list of string or bytevector to search");
}

static SCM getallid_mtfa_sss (SCM mtfa_sss, SCM scm_to_search)
{
    if (SCM_BOOL_F == mtfa_sss_p(mtfa_sss))
        return SCM_BOOL_F;
    
    mtfa_string_sequences* pt = ( mtfa_string_sequences* ) scm_foreign_object_signed_ref ( mtfa_sss, 0 );

    if (scm_list_p(scm_to_search)==SCM_BOOL_T)
    {
        //devo convertire tutta la lista di stringhe in c++
        list<string> ls;
        while (!scm_is_null(scm_to_search))
        {
            string s = ScmToString(scm_car(scm_to_search));
            scm_to_search = scm_cdr(scm_to_search);
            ls.push_back(s);
        }
        
        vector<uint8_t> vui8 = mtfa_LsToVui8(pt->ph, ls);
        unordered_set<long> us = pt->seq->GetAll( vui8 );
        
        SCM lret = SCM_EOL;
        for (auto id: us)
        {
            lret = scm_cons(scm_from_uint32(id), lret);
        }
        return lret;
    }
    return ScmFromString("Parameters: list of string to search");
}

static SCM getval_mtfa_sss (SCM mtfa_sss, SCM scm_id)
{
    if (SCM_BOOL_F == mtfa_sss_p(mtfa_sss))
        return SCM_BOOL_F;
    
    mtfa_string_sequences* pt = ( mtfa_string_sequences* ) scm_foreign_object_signed_ref ( mtfa_sss, 0 );

    if (scm_is_number(scm_id))
    {
        int id = scm_to_int32(scm_id);
        auto ls = pt->getval(id);
        SCM lret = SCM_EOL;
        for (auto s: ls)
        {
            lret = scm_cons(ScmFromString(s.c_str()), lret);
        }
        return lret;
    }
    return ScmFromString("Parameters: the id number to search for");
}

static SCM serialize_mtfa_sss ( SCM mtfa_sss )
{
    if ( SCM_BOOL_F == mtfa_sss_p ( mtfa_sss ) )
        return SCM_EOL;

    mtfa_string_sequences* pt = ( mtfa_string_sequences* ) scm_foreign_object_signed_ref ( mtfa_sss, 0 );

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
static SCM deserialize_mtfa_sss ( SCM mtfa_sss, SCM bytevector_in )
{
    if ( SCM_BOOL_F == mtfa_sss_p ( mtfa_sss ) )
        return SCM_EOL;

    mtfa_string_sequences* pt = ( mtfa_string_sequences* ) scm_foreign_object_signed_ref ( mtfa_sss, 0 );

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





extern "C" void init_mtfa_sss();
extern "C" void init_mtfa_sss()
{
    SCM name, slots;
    scm_t_struct_finalize finalizer;
    name = scm_from_latin1_symbol ( "mtfa-sss" );
    slots = scm_list_1 ( scm_from_latin1_symbol ( "dsss" ) );
    finalizer = finalize_mtfa_sss;
    mtfa_sss_type = scm_make_foreign_object_type ( name, slots, finalizer );

    scm_c_define_gsubr ( "mtfa-sss-make", 0, 0, 0, ( void* ) make_mtfa_sss );
    scm_c_define_gsubr ( "mtfa-sss?", 1, 0, 0, ( void* ) mtfa_sss_p );

    scm_c_define_gsubr ( "mtfa-sss-insert", 2, 0, 0, ( void* ) insert_mtfa_sss);
    scm_c_define_gsubr ( "mtfa-sss-search", 2, 0, 0, ( void* ) search_mtfa_sss);
    scm_c_define_gsubr ( "mtfa-sss-unlink", 2, 0, 0, ( void* ) unlink_mtfa_sss);
    scm_c_define_gsubr ( "mtfa-sss-getval", 2, 0, 0, ( void* ) getval_mtfa_sss);
    scm_c_define_gsubr ( "mtfa-sss-getall", 2, 0, 0, ( void* ) getall_mtfa_sss);
    scm_c_define_gsubr ( "mtfa-sss-getallid", 2, 0, 0, ( void* ) getallid_mtfa_sss);
    scm_c_define_gsubr ( "mtfa-sss-ls2bv", 2, 0, 0, ( void* ) to_bytevector_mtfa_sss);
    scm_c_define_gsubr ( "mtfa-sss-serialize", 1, 0, 0, ( void* ) serialize_mtfa_sss);
    scm_c_define_gsubr ( "mtfa-sss-deserialize", 2, 0, 0, ( void* ) deserialize_mtfa_sss);
}
