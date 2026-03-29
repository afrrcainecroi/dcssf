#include <iostream>
#include <string>

#include <stdlib.h>
#include <libguile.h>

#include <list>

#include <mtfa_utils.h>

#include <netfilter_queue.h>


/**************INTERFACCIA GUILE*****************/

static SCM mtfa_netfilter_queue_type;

static void finalize_mtfa_netfilter_queue ( SCM mtfa_netfilter_queue )
{
     queue_manager * qm = ( queue_manager * ) scm_foreign_object_signed_ref ( mtfa_netfilter_queue, 0 );

     if ( qm ) {
          scm_foreign_object_signed_set_x ( mtfa_netfilter_queue, 0, 0 );
          /* Non ci sono oggetti guile qui depositati!
          for (auto it: *pt)
          {
           scm_gc_unprotect_object((SCM)it);
          }
          */
          delete qm;   //le stop, nel caso, le gestiamo qui dentro!
     }
}

#define C_STRING_TO_SYMBOL(str) scm_string_to_symbol(ScmFromString(str))

// SCM scm_c_array_to_aliased_f64_vector ( int count, double* array );
// 
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
// 
//      return scm_pointer_to_bytevector ( scm_from_pointer (
// 
// // Include an error maybe.
// #ifdef ERROR
//                                              xreal
// #else
//                                              array
// #endif
// 
//                                              , NULL ), scm_from_int ( count ), scm_from_int ( 0 ), C_STRING_TO_SYMBOL ( "s8" ) );
// }

static SCM mtfa_netfilter_queue_start (
     //list<int> queues, int queue_len, int bufsize
     SCM scm_list_of_queues, SCM scm_queue_len, SCM scm_bufsize
)
{
     std::list<int> queue_list;
     while ( scm_list_of_queues != SCM_EOL ) {
          queue_list.push_back ( scm_to_int ( SCM_CAR ( scm_list_of_queues ) ) );
          scm_list_of_queues = SCM_CDR ( scm_list_of_queues );
     }
     int queue_len = scm_to_int ( scm_queue_len );
     int bufsize = scm_to_int ( scm_bufsize );
     queue_manager * qm = new queue_manager ( queue_list, queue_len, bufsize );

     return scm_make_foreign_object_1 ( mtfa_netfilter_queue_type, ( void* ) qm );
}

static SCM mtfa_netfilter_queue_stop ( SCM mtfa_netfilter_queue )
{
     queue_manager * qm = ( queue_manager* ) scm_foreign_object_signed_ref ( mtfa_netfilter_queue, 0 );
     qm->stop_queue_manager();
     return SCM_BOOL_T;
}

static SCM mtfa_netfilter_queue_add ( SCM mtfa_netfilter_queue, SCM _src_ip, SCM _dst_ip, SCM _dst_port )
{
     queue_manager * qm = ( queue_manager* ) scm_foreign_object_signed_ref ( mtfa_netfilter_queue, 0 );
     size_t l;
     char * s = ScmToStringn ( _src_ip, &l );
     string src_ip ( s, l );
     free ( s );
     s = ScmToStringn ( _dst_ip, &l );
     string dst_ip ( s, l );
     free ( s );
     s = ScmToStringn ( _dst_port, &l );
     string dst_port ( s, l );
     free ( s );
     if ( qm->add_rule ( src_ip, dst_ip, dst_port ) ) {
          return SCM_BOOL_T;
     } else {
          return SCM_BOOL_F;
     }
}

static SCM mtfa_netfilter_queue_del ( SCM mtfa_netfilter_queue, SCM _src_ip, SCM _dst_ip, SCM _dst_port )
{
     queue_manager * qm = ( queue_manager* ) scm_foreign_object_signed_ref ( mtfa_netfilter_queue, 0 );
     size_t l;
     char * s = ScmToStringn ( _src_ip, &l );
     string src_ip ( s, l );
     free ( s );
     s = ScmToStringn ( _dst_ip, &l );
     string dst_ip ( s, l );
     free ( s );
     s = ScmToStringn ( _dst_port, &l );
     string dst_port ( s, l );
     free ( s );
     if ( qm->del_rule ( src_ip, dst_ip, dst_port ) ) {
          return SCM_BOOL_T;
     } else {
          return SCM_BOOL_F;
     }
}

static SCM mtfa_netfilter_queue_list ( SCM mtfa_netfilter_queue )
{
     queue_manager * qm = ( queue_manager* ) scm_foreign_object_signed_ref ( mtfa_netfilter_queue, 0 );
     string s = qm->list_rules();
     return ScmFromString ( s.c_str() );
}

static SCM mtfa_netfilter_queue_p ( SCM mtfa_netfilter_queue )
{
//      auto const body ( cify<SCM ( * ) ( void * ) >
//                        (
//      [&] ( void * data ) {
//           scm_assert_foreign_object_type ( mtfa_netfilter_queue_type, mtfa_netfilter_queue );
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
     //SCM handler (void *data, SCM key, SCM args);
    
    auto const body = [] (void * data) {
        scm_assert_foreign_object_type ( mtfa_netfilter_queue_type, *(SCM*)data);
        return SCM_BOOL_T;
    };
    auto const handler = [] ( void *data, SCM key, SCM args ) {
        return SCM_BOOL_F;
    };

    SCM ret = scm_internal_catch ( SCM_BOOL_T, body, &mtfa_netfilter_queue, handler, nullptr );

     return ret;
}

extern "C" void init_mtfa_netfilter_queue();

extern "C" void init_mtfa_netfilter_queue()
{
     SCM name, slots;
     scm_t_struct_finalize finalizer;
     name = scm_from_latin1_symbol ( "mtfa-netfilter-queue" );
     slots = scm_list_1 ( scm_from_latin1_symbol ( "nq" ) );
     finalizer = finalize_mtfa_netfilter_queue;
     mtfa_netfilter_queue_type = scm_make_foreign_object_type ( name, slots, finalizer );

     scm_c_define_gsubr ( "mtfa-netfilter-queue-start", 3, 0, 0, ( void* ) mtfa_netfilter_queue_start );
     scm_c_define_gsubr ( "mtfa-netfilter-queue-stop", 1, 0, 0, ( void* ) mtfa_netfilter_queue_stop );
     scm_c_define_gsubr ( "mtfa-netfilter-queue?", 1, 0, 0, ( void* ) mtfa_netfilter_queue_p );
     scm_c_define_gsubr ( "mtfa-netfilter-queue-add", 4, 0, 0, ( void* ) mtfa_netfilter_queue_add );
     scm_c_define_gsubr ( "mtfa-netfilter-queue-del", 4, 0, 0, ( void* ) mtfa_netfilter_queue_del );
     scm_c_define_gsubr ( "mtfa-netfilter-queue-list", 1, 0, 0, ( void* ) mtfa_netfilter_queue_list );
}


