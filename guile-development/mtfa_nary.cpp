#include <iostream>
#include <string>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <thread>
#include <future>
#include <atomic>
#include <mutex>         // std::mutex, std::unique_lock
#include <cmath>
#include <unistd.h>
#include <stdlib.h>
#include <libguile.h>
#include <csignal>
#include <unordered_set>
#include <fmt/format.h>
#include <boost/range/combine.hpp>

#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <list>
#include <glib.h>
using namespace std;

/**************INTERFACCIA GUILE*****************/

static SCM mtfa_nary_type;  //il tipo del su

#define C_STRING_TO_SYMBOL(str) scm_string_to_symbol(ScmFromString(str))

// static void finalize_mtfa_nary( SCM mtfa_nary)
// {
//     GNode* pt = ( GNode* ) scm_foreign_object_signed_ref ( mtfa_nary, 0 );
// 
//     if ( pt ) {
//         scm_foreign_object_signed_set_x ( mtfa_nary, 0, 0 );
//         g_node_destroy(pt);
//     }
// }
// 
// static SCM mtfa_nary_p ( SCM mtfa_nary )
// {
//     auto const body = [] ( void* data ) {
//         scm_assert_foreign_object_type ( mtfa_nary_type, * ( SCM* ) data );
//         return SCM_BOOL_T;
//     };
//     auto const handler = [] ( void* data, SCM key, SCM args ) {
//         return SCM_BOOL_F;
//     };
//     SCM ret = scm_internal_catch ( SCM_BOOL_T, body, &mtfa_nary, handler, nullptr );
//     return ret;
// }

static void destroy_gnode(void *p)
{
    g_node_destroy((GNode*)p);
}
static SCM mtfa_nary_make ( SCM data )
{
    gpointer p = (gpointer)scm_to_pointer(data);
    GNode * gn = g_node_new(p);
    return scm_from_pointer(gn, destroy_gnode);
}

// static SCM scm_g_node_copy(SCM scm_gnode)
// {
//     GNode * p = (GNode *)scm_to_pointer(scm_gnode);
//     GNode * p1 = g_node_copy(p);
//     return scm_from_pointer(p1, destroy_gnode);
// }
// static SCM scm_g_node_copy_deep(SCM scm_gnode, SCM scm_copy_func, SCM scm_data)
//     {
//     GNode * p = (GNode *)scm_to_pointer(scm_gnode);
//     GNode * p1 = g_node_copy(p);
//     return scm_from_pointer(p1, destroy_gnode);
// }
static SCM mtfa_g_node_insert( SCM scm_parent, SCM scm_position, SCM scm_gnode)
{
    GNode * parent = (GNode *)scm_to_pointer(scm_parent);
    GNode * gnode = (GNode *)scm_to_pointer(scm_gnode);
    int position = scm_to_int(scm_position);
    g_node_insert(parent, position, gnode);
    return SCM_BOOL_T;
}
static SCM mtfa_g_node_insert_before( SCM scm_parent, SCM scm_sibling, SCM scm_gnode)
{
    GNode * parent = (GNode *)scm_to_pointer(scm_parent);
    GNode * gnode = (GNode *)scm_to_pointer(scm_gnode);
    GNode * sibling = nullptr;
    if (!scm_is_null_or_nil(scm_sibling))
        sibling = (GNode *)scm_to_pointer(scm_sibling);
    g_node_insert_before(parent, sibling, gnode);
    return SCM_BOOL_T;
}
static SCM mtfa_g_node_insert_after( SCM scm_parent, SCM scm_sibling, SCM scm_gnode)
{
    GNode * parent = (GNode *)scm_to_pointer(scm_parent);
    GNode * gnode = (GNode *)scm_to_pointer(scm_gnode);
    GNode * sibling = nullptr;
    if (!scm_is_null_or_nil(scm_sibling))
        sibling = (GNode *)scm_to_pointer(scm_sibling);
    g_node_insert_after(parent, sibling, gnode);
    return SCM_BOOL_T;
}
static SCM mtfa_g_node_append( SCM scm_parent, SCM scm_gnode)
{
    GNode * parent = (GNode *)scm_to_pointer(scm_parent);
    GNode * gnode = (GNode *)scm_to_pointer(scm_gnode);
    g_node_append(parent, gnode);
    return SCM_BOOL_T;
}
static SCM mtfa_g_node_prepend( SCM scm_parent, SCM scm_gnode)
{
    GNode * parent = (GNode *)scm_to_pointer(scm_parent);
    GNode * gnode = (GNode *)scm_to_pointer(scm_gnode);
    g_node_prepend(parent, gnode);
    return SCM_BOOL_T;
}
static SCM mtfa_g_node_reverse_children( SCM scm_gnode)
{
    GNode * gnode = (GNode *)scm_to_pointer(scm_gnode);
    g_node_reverse_children(gnode);
    return SCM_BOOL_T;
}

gboolean myGNodeTraverseFunc(GNode *node, gpointer data)
{
    
}

static SCM mtfa_g_node_traverse( SCM scm_gnode, SCM scm_order, SCM scm_flags, SCM scm_max_depth, SCM scm_func, SCM scm_data)
{
    GNode * gnode = (GNode *)scm_to_pointer(scm_gnode);
    GTraverseType order;
    switch (scm_to_int(scm_order))
    {
        case 0: order = G_IN_ORDER;
        break;
        case 1: order = G_PRE_ORDER;
        break;
        case 2: order = G_POST_ORDER;
        break;
        case 3: order = G_LEVEL_ORDER;
        break;
    }
    
    GTraverseFlags flags;
    switch (scm_to_int(scm_flags))
    {
        case 0: flags = G_TRAVERSE_LEAVES; //     = 1 << 0,
        case 1: flags = G_TRAVERSE_NON_LEAVES; // = 1 << 1,
        case 2: flags = G_TRAVERSE_ALL; //        = G_TRAVERSE_LEAVES | G_TRAVERSE_NON_LEAVES,
        case 3: flags = G_TRAVERSE_MASK; //       = 0x03,
        case 4: flags = G_TRAVERSE_LEAFS; //      = G_TRAVERSE_LEAVES,
        case 5: flags = G_TRAVERSE_NON_LEAFS; //  = G_TRAVERSE_NON_LEAVES
    }
    
    int max_depth = scm_to_int(scm_max_depth);
    
    
    return SCM_BOOL_T;
}



extern "C" void init_mtfa_nary();
void init_mtfa_nary()
{
    SCM name, slots;
    scm_t_struct_finalize finalizer;
    name = scm_from_latin1_symbol ( "mtfa-nary" );
    slots = scm_list_1 ( scm_from_latin1_symbol ( "fanary" ) );
    finalizer = finalize_mtfa_nary;
    mtfa_nary_type = scm_make_foreign_object_type ( name, slots, finalizer );

    scm_c_define_gsubr ( "mtfa-nary-p", 1, 0, 0, ( void* ) mtfa_nary_p );
    scm_c_define_gsubr ( "mtfa-nary-make", 1, 0, 0, ( void* ) mtfa_nary_make );
}

