#include <iostream>
#include <string>

#include <stdlib.h>
#include <libguile.h>

#include <list>

#include <mtfa_utils.h>
#include <bitset>


/**************INTERFACCIA GUILE*****************/

static SCM mtfa_su_type;  //il tipo del su

static void finalize_mtfa_su ( SCM mtfa_su )
{
    mtfa_extended_bitset* pt = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su, 0 );

    if ( pt )
    {
        scm_foreign_object_signed_set_x ( mtfa_su, 0, 0 );
        delete pt;
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
//
static SCM make_mtfa_su ()
{
    mtfa_extended_bitset* pt = new mtfa_extended_bitset();
    return scm_make_foreign_object_1 ( mtfa_su_type, ( void* ) pt );
}

static SCM reset_mtfa_su ( SCM mtfa_su )
{
    mtfa_extended_bitset* pt = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su, 0 );
    pt->clear();
    return SCM_BOOL_T;
}

static SCM clone_mtfa_su (SCM mtfa_su_in)
{
	mtfa_extended_bitset* ptin = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su_in, 0 );

	mtfa_extended_bitset* ptout = new mtfa_extended_bitset(*ptin);
	return scm_make_foreign_object_1 ( mtfa_su_type, ( void* ) ptout );
}

static SCM size_mtfa_su ( SCM mtfa_su )
{
    mtfa_extended_bitset* pt = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su, 0 );
    int dim = pt->size();
    return scm_from_int ( dim );
}

static SCM count_mtfa_su ( SCM mtfa_su )
{
    mtfa_extended_bitset* pt = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su, 0 );
    int dim = pt->count();
    return scm_from_int ( dim );
}

static SCM set_all_mtfa_su ( SCM mtfa_su )
{
    mtfa_extended_bitset* pt = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su, 0 );
    pt->set();
    return SCM_BOOL_T;
}

static SCM flip_all_mtfa_su ( SCM mtfa_su )
{
    mtfa_extended_bitset* pt = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su, 0 );
    pt->flip();
    return SCM_BOOL_T;
}

static SCM resize_mtfa_su(SCM mtfa_su, SCM _newSize, SCM _fillWithTrue)
{
    mtfa_extended_bitset* pt = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su, 0 );
    uint newSize = scm_to_uint(_newSize);
    bool fillWithTrue = scm_to_bool(_fillWithTrue);
    pt->resize(newSize, fillWithTrue);
    return SCM_BOOL_T;
}

static SCM set_mtfa_su ( SCM mtfa_su, SCM scm_pos, SCM scm_bool )
{
    mtfa_extended_bitset* pt = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su, 0 );
    int pos = scm_to_int ( scm_pos );
    bool value = scm_to_bool ( scm_bool ) == 1;
    pt->set ( pos, value );
    return SCM_BOOL_T;
}

static SCM flip_mtfa_su ( SCM mtfa_su, SCM scm_pos )
{
    mtfa_extended_bitset* pt = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su, 0 );
    int pos = scm_to_int ( scm_pos );
    pt->flip ( pos );
    return SCM_BOOL_T;
}

static SCM get_mtfa_su ( SCM mtfa_su, SCM scm_pos )
{
    mtfa_extended_bitset* pt = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su, 0 );
    int pos = scm_to_int ( scm_pos );

    if ( ( *pt ) [pos] )
    {
        return SCM_BOOL_T;
    }
    else
    {
        return SCM_BOOL_F;
    }
}

static SCM assign_mtfa_su ( SCM mtfa_su1, SCM mtfa_su2 )
{
    mtfa_extended_bitset* pt1 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su1, 0 );
    mtfa_extended_bitset* pt2 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su2, 0 );

    *pt1 = *pt2;
    return SCM_BOOL_T;
}

static SCM compare_mtfa_su ( SCM mtfa_su1, SCM mtfa_su2 )
{
    mtfa_extended_bitset* pt1 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su1, 0 );
    mtfa_extended_bitset* pt2 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su2, 0 );

    bool bret = pt1->equal( *pt2 );

    return bret ? SCM_BOOL_T : SCM_BOOL_F;
}

static SCM or_mtfa_su ( SCM mtfa_su1, SCM mtfa_su2, SCM mtfa_su3 )
{
    mtfa_extended_bitset* pt1 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su1, 0 );
    mtfa_extended_bitset* pt2 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su2, 0 );
    mtfa_extended_bitset* pt3 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su3, 0 );

    *pt1 = ( *pt2 ) | ( *pt3 );
    return SCM_BOOL_T;
}

static SCM or_equal_mtfa_su ( SCM mtfa_su1, SCM mtfa_su2 )
{
    mtfa_extended_bitset* pt1 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su1, 0 );
    mtfa_extended_bitset* pt2 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su2, 0 );

    *pt1 |= ( *pt2 );
    return SCM_BOOL_T;
}

static SCM and_mtfa_su ( SCM mtfa_su1, SCM mtfa_su2, SCM mtfa_su3 )
{
    mtfa_extended_bitset* pt1 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su1, 0 );
    mtfa_extended_bitset* pt2 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su2, 0 );
    mtfa_extended_bitset* pt3 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su3, 0 );

    *pt1 = ( *pt2 );
    *pt1 &= ( *pt3 );
    return SCM_BOOL_T;
}

static SCM and_equal_mtfa_su ( SCM mtfa_su1, SCM mtfa_su2 )
{
    mtfa_extended_bitset* pt1 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su1, 0 );
    mtfa_extended_bitset* pt2 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su2, 0 );

    *pt1 &= ( *pt2 );
    return SCM_BOOL_T;
}

static SCM xor_mtfa_su ( SCM mtfa_su1, SCM mtfa_su2, SCM mtfa_su3 )
{
    mtfa_extended_bitset* pt1 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su1, 0 );
    mtfa_extended_bitset* pt2 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su2, 0 );
    mtfa_extended_bitset* pt3 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su3, 0 );

    *pt1 = ( *pt2 );
    *pt1 ^= ( *pt3 );
    return SCM_BOOL_T;
}

static SCM xor_equal_mtfa_su ( SCM mtfa_su1, SCM mtfa_su2 )
{
    mtfa_extended_bitset* pt1 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su1, 0 );
    mtfa_extended_bitset* pt2 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su2, 0 );

    *pt1 ^= ( *pt2 );
    return SCM_BOOL_T;
}

static SCM diff_mtfa_su ( SCM mtfa_su1, SCM mtfa_su2, SCM mtfa_su3 )
{
    mtfa_extended_bitset* pt1 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su1, 0 );
    mtfa_extended_bitset* pt2 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su2, 0 );
    mtfa_extended_bitset* pt3 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su3, 0 );

    *pt1 = ( *pt2 );
    *pt1 -= ( *pt3 );
    return SCM_BOOL_T;
}

static SCM diff_equal_mtfa_su ( SCM mtfa_su1, SCM mtfa_su2 )
{
    mtfa_extended_bitset* pt1 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su1, 0 );
    mtfa_extended_bitset* pt2 = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su2, 0 );

    *pt1 -= ( *pt2 );
    return SCM_BOOL_T;
}

static SCM get_all_mtfa_su ( SCM mtfa_su )
{
    mtfa_extended_bitset* pt = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su, 0 );

    list<int> p = pt->GiveOnes();

    if ( p.size() == 0)
    {
        return SCM_EOL;
    }

    SCM lista = SCM_EOL;

    for(auto pos: p)
    {
        lista = scm_cons ( scm_from_int ( pos ), lista );
        pos++;
    }

    return lista;
}

static SCM to_string_mtfa_su ( SCM mtfa_su )
{
    mtfa_extended_bitset* pt = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su, 0 );
    string s = pt->toString();

    SCM k = ScmFromString ( s.c_str() );

    return k;
}

static SCM serialize_mtfa_su ( SCM mtfa_su )
{
    mtfa_extended_bitset* pt = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su, 0 );

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

static SCM deserialize_mtfa_su ( SCM mtfa_su, SCM bytevector_in )
{
    mtfa_extended_bitset* pt = ( mtfa_extended_bitset* ) scm_foreign_object_signed_ref ( mtfa_su, 0 );

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

static SCM mtfa_su_p ( SCM mtfa_su )
{
//      auto const body ( cify<SCM ( * ) ( void * ) >
//                        (
//      [&] ( void * data ) {
//           scm_assert_foreign_object_type ( mtfa_su_type, mtfa_su );
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

    auto const body = [] ( void* data )
    {
        scm_assert_foreign_object_type ( mtfa_su_type, * ( SCM* ) data );
        return SCM_BOOL_T;
    };
    auto const handler = [] ( void* data, SCM key, SCM args )
    {
        return SCM_BOOL_F;
    };

    SCM ret = scm_internal_catch ( SCM_BOOL_T, body, &mtfa_su, handler, nullptr );

    return ret;
}

extern "C" void init_mtfa_su();
extern "C" void init_mtfa_su()
{
    SCM name, slots;
    scm_t_struct_finalize finalizer;
    name = scm_from_latin1_symbol ( "mtfa-su" );
    slots = scm_list_1 ( scm_from_latin1_symbol ( "dsu" ) );
    finalizer = finalize_mtfa_su;
    mtfa_su_type = scm_make_foreign_object_type ( name, slots, finalizer );

    scm_c_define_gsubr ( "mtfa-su-make", 0, 0, 0, ( void* ) make_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-reset", 1, 0, 0, ( void* ) reset_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-size", 1, 0, 0, ( void* ) size_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-count", 1, 0, 0, ( void* ) count_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-set-all", 1, 0, 0, ( void* ) set_all_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-flip-all", 1, 0, 0, ( void* ) flip_all_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-set", 3, 0, 0, ( void* ) set_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-flip", 2, 0, 0, ( void* ) flip_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-get", 2, 0, 0, ( void* ) get_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-assign", 2, 0, 0, ( void* ) assign_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-compare", 2, 0, 0, ( void* ) compare_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-or", 3, 0, 0, ( void* ) or_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-or-equal", 2, 0, 0, ( void* ) or_equal_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-and", 3, 0, 0, ( void* ) and_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-and-equal", 2, 0, 0, ( void* ) and_equal_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-xor", 3, 0, 0, ( void* ) xor_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-xor-equal", 2, 0, 0, ( void* ) xor_equal_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-diff", 3, 0, 0, ( void* ) diff_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-diff-equal", 2, 0, 0, ( void* ) diff_equal_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-get-all", 1, 0, 0, ( void* ) get_all_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-to-string", 1, 0, 0, ( void* ) to_string_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-serialize", 1, 0, 0, ( void* ) serialize_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-deserialize", 2, 0, 0, ( void* ) deserialize_mtfa_su );
    scm_c_define_gsubr ( "mtfa-su-resize", 3, 0, 0, ( void* ) resize_mtfa_su );
	scm_c_define_gsubr ( "mtfa-su?", 1, 0, 0, ( void* ) mtfa_su_p );
	scm_c_define_gsubr ( "mtfa-su-clone", 1, 0, 0, ( void* ) clone_mtfa_su);
}


