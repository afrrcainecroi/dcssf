#include <iostream>
#include <string>

#include <stdlib.h>
#include <libguile.h>

#include <list>

#include <mtfa_utils.h>
#include <mtfa_fs3.h>
#include <unordered_map>

/**************INTERFACCIA GUILE*****************/

static SCM mtfa_wss_type;  //il tipo del wss

static void finalize_wss( SCM mtfa_wss)
{
    MyWebSocket* pt = ( MyWebSocket* ) scm_foreign_object_signed_ref ( mtfa_wss, 0 );

    if ( pt ) {
        scm_foreign_object_signed_set_x ( mtfa_wss, 0, 0 );
        if (!pt->isClosed())
            pt->close();
        delete pt;
    }
}

#define C_STRING_TO_SYMBOL(str) scm_string_to_symbol(ScmFromString(str))

static SCM mtfa_wss_p ( SCM mtfa_wss)
{
    auto const body = [] ( void* data )
    {
        scm_assert_foreign_object_type ( mtfa_wss_type, * ( SCM* ) data );
        return SCM_BOOL_T;
    };
    auto const handler = [] ( void* data, SCM key, SCM args )
    {
        return SCM_BOOL_F;
    };
    
    SCM ret = scm_internal_catch ( SCM_BOOL_T, body, &mtfa_wss, handler, nullptr );
    
    return ret;
}

/*
 * Funzionamento: lo trovi in /volume1/sources/NEW_DEVS/DEPLOYED/SCM_TST/gtest/cpp-test
 * - 
 * */
static SCM mtfa_wss_make(SCM scm_cb)
{
    //function<pair<string, bool>(pair<string, bool> & data)> 
//    auto cb = [] (string & data, bool & data_type, void * user_data) -> pair<string, bool> {
    auto cb = [](const string & data, bool & data_type, void * user_data) -> pair<string, bool>  {
        SCM recvd;
        if (data_type) {
            //formato text
            recvd = scm_from_utf8_string(data.c_str());
        } else {
            //formato binary
            char * p = (char*)malloc(data.size());
            std::copy(data.begin(), data.end(), p);
            recvd = scm_pointer_to_bytevector (
                scm_from_pointer ( ( void* ) p, free ),
                scm_from_int ( data.size() ),
                scm_from_int ( 0 ),
                scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
        }
        SCM scm_cb = (SCM)user_data;
        SCM tosend = scm_call_1(scm_cb, recvd);
        if (scm_is_bytevector(tosend)) {
            auto len = SCM_BYTEVECTOR_LENGTH ( tosend );
            auto p = ( char* ) SCM_BYTEVECTOR_CONTENTS ( tosend );
            return make_pair(string(p, len), false);
        } else {
            char * p = scm_to_utf8_string(tosend);
            string s(p, p+strlen(p));
            free(p);
            return make_pair(s, true);
        }
    };
    
//     //string & data, bool & data_type, server* s, websocketpp::connection_hdl hdl, void * user_data
//     MyWebSocket *pt = new MyWebSocket(cb, scm_cb);
// 
//     auto cb = [](const string & data, bool & data_type) -> pair<string, bool>  {
//         return make_pair(data, data_type);
//     };
    MyWebSocket *pt = new MyWebSocket(cb, (void*)scm_cb);
    return scm_make_foreign_object_1 ( mtfa_wss_type, ( void* ) pt );
}

static SCM mtfa_wss_close(SCM scm_mws)
{
    MyWebSocket * pt = ( MyWebSocket * ) scm_foreign_object_signed_ref ( scm_mws, 0 );
//     string data = pt->close();
//     //torna, se ci sono, i dati ancora da inviare sul socket
//     char * p = (char*)malloc(data.size());
//     std::copy(data.begin(), data.end(), p);
//     auto tosend = scm_pointer_to_bytevector (
//         scm_from_pointer ( ( void* ) p, free ),
//         scm_from_int ( data.size() ),
//         scm_from_int ( 0 ),
//         scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
//     return tosend;
    delete pt;
    return SCM_BOOL_T;
}

static SCM mtfa_wss_prepare_to_send(SCM scm_mws, SCM data)
{
    MyWebSocket* mws = ( MyWebSocket* ) scm_foreign_object_signed_ref ( scm_mws, 0 );
    string tosend;
    if (scm_is_bytevector(data)) {
        auto len = SCM_BYTEVECTOR_LENGTH ( data );
        auto p = ( char* ) SCM_BYTEVECTOR_CONTENTS ( data );
        string s(p, p+len);
        mws->prepare_to_send(make_pair(s, false));
    } else {
        char * p = scm_to_utf8_string(data);
        string s(p, p+strlen(p));
        free(p);
        mws->prepare_to_send(make_pair(s, true));
    }
    return SCM_BOOL_T;
}

//Quando mi arrivano dati. Sono in formato bytevector!
//torna un bytevector da trasmettere sul canale, altrimenti false!
static SCM mtfa_wss_push(SCM scm_mws, SCM scm_data)
{
    MyWebSocket * pt = ( MyWebSocket * ) scm_foreign_object_signed_ref ( scm_mws, 0 );
    string sret;
    
    auto len = SCM_BYTEVECTOR_LENGTH ( scm_data );
    auto p = ( char* ) SCM_BYTEVECTOR_CONTENTS ( scm_data );
    
    string work(p, p+len);
    pt->q_in.push(work);

    if (pt->gotAnError())
        return SCM_BOOL_F;
    else
        return SCM_BOOL_T;
}
static SCM mtfa_wss_pop(SCM scm_mws)
{
    MyWebSocket * pt = ( MyWebSocket * ) scm_foreign_object_signed_ref ( scm_mws, 0 );
    
    if (!pt->q_out.empty()) {
        auto data = pt->q_out.front();
        char * p = (char*)malloc(data.size());
        std::copy(data.begin(), data.end(), p);
        auto tosend = scm_pointer_to_bytevector (
            scm_from_pointer ( ( void* ) p, free ),
            scm_from_int ( data.size() ),
            scm_from_int ( 0 ),
            scm_string_to_symbol ( ScmFromString ( "u8" ) ) );
        return tosend;
    } else
        return SCM_BOOL_F;
}

extern "C" void init_mtfa_wss();
extern "C" void init_mtfa_wss()
{
    //Il finalizzatore per i web socket
    SCM name, slots;
    scm_t_struct_finalize finalizer;
    name = scm_from_latin1_symbol ( "mtfa-wss" );
    slots = scm_list_1 ( scm_from_latin1_symbol ( "wss" ) );
    finalizer = finalize_wss;
    mtfa_wss_type = scm_make_foreign_object_type ( name, slots, finalizer );
    
    scm_c_define_gsubr ( "mtfa_wss_make", 1, 0, 0, (void*)mtfa_wss_make);
    scm_c_define_gsubr ( "mtfa_wss_p", 1, 0, 0, (void*)mtfa_wss_p);
    scm_c_define_gsubr ( "mtfa_wss_close", 1, 0, 0, (void*)mtfa_wss_close);
    scm_c_define_gsubr ( "mtfa_wss_push", 2, 0, 0, (void*)mtfa_wss_push);
    scm_c_define_gsubr ( "mtfa_wss_pop", 1, 0, 0, (void*)mtfa_wss_pop);
    scm_c_define_gsubr ( "mtfa_wss_prepare_to_send", 2, 0, 0, (void*)mtfa_wss_prepare_to_send);
}
