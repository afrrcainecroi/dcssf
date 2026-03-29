#include <stdlib.h>
#include <libguile.h>

#include <iostream>

#include <string>
#include <list>

#include <mtfa_db_mysql.h>
#include <mtfa_utils.h>


/**************INTERFACCIA GUILE*****************/

static SCM mtfa_db_mysql_type;  //il tipo del db mysql

static void finalize_mtfa_db_mysql ( SCM mtfa_db_mysql )
{
     mysql_db_class* pt = ( mysql_db_class* ) scm_foreign_object_signed_ref ( mtfa_db_mysql, 0 );

     if ( pt ) {
          scm_foreign_object_signed_set_x ( mtfa_db_mysql, 0, 0 );
          pt->do_disconnect();
          delete pt;
          pt=nullptr;
     }
}

static SCM mtfa_db_mysql_open ()
{
     mysql_db_class* pt = new mysql_db_class();
     return scm_make_foreign_object_1 ( mtfa_db_mysql_type, ( void* ) pt );
}

static SCM mtfa_db_mysql_connect ( SCM mtfa_db_mysql, SCM ip, SCM user, SCM password, SCM thedb, SCM port )
{
     mysql_db_class* pt = ( mysql_db_class* ) scm_foreign_object_signed_ref ( mtfa_db_mysql, 0 );

     char * s_ip = ScmToString( ip );
     char * s_user = ScmToString( user );
     char * s_password = ScmToString( password );
     char * s_thedb = ScmToString( thedb );
     int i_port = scm_to_int ( port );

     bool bret = pt->Connect ( s_ip, s_user, s_password, s_thedb, i_port );
     free ( s_ip );
     free ( s_user );
     free ( s_password );
     free ( s_thedb );

     return bret?SCM_BOOL_T:SCM_EOL;
}

static SCM mtfa_db_mysql_close ( SCM mtfa_db_mysql )
{
     mysql_db_class* pt = ( mysql_db_class* ) scm_foreign_object_signed_ref ( mtfa_db_mysql, 0 );

     pt->do_disconnect();
     delete pt;
     pt=nullptr;

	 scm_foreign_object_signed_set_x ( mtfa_db_mysql, 0, 0 );
	 return SCM_EOL;
}

static SCM mtfa_db_mysql_disconnect ( SCM mtfa_db_mysql )
{
     mysql_db_class* pt = ( mysql_db_class* ) scm_foreign_object_signed_ref ( mtfa_db_mysql, 0 );

     pt->do_disconnect();
     return SCM_EOL;
}

static SCM mtfa_db_mysql_start_transaction ( SCM mtfa_db_mysql )
{
     mysql_db_class* pt = ( mysql_db_class* ) scm_foreign_object_signed_ref ( mtfa_db_mysql, 0 );

     bool bret = pt->StartTransaction();
     return bret?SCM_BOOL_T:SCM_EOL;
}

static SCM mtfa_db_mysql_rollback ( SCM mtfa_db_mysql )
{
     mysql_db_class* pt = ( mysql_db_class* ) scm_foreign_object_signed_ref ( mtfa_db_mysql, 0 );

     bool bret = pt->RollBack();
     return bret?SCM_BOOL_T:SCM_EOL;
}

static SCM mtfa_db_mysql_commit ( SCM mtfa_db_mysql )
{
     mysql_db_class* pt = ( mysql_db_class* ) scm_foreign_object_signed_ref ( mtfa_db_mysql, 0 );

     bool bret = pt->Commit();
     return bret?SCM_BOOL_T:SCM_EOL;
}

static SCM mtfa_db_mysql_do_sql ( SCM mtfa_db_mysql, SCM sql )
{
     mysql_db_class* pt = ( mysql_db_class* ) scm_foreign_object_signed_ref ( mtfa_db_mysql, 0 );

     size_t st_len;
     char * st = ScmToStringn( sql, &st_len );

     std::string the_query;
     the_query.assign ( st, st_len );
     free(st);

     std::list<std::list<string> > result;
     bool bret = pt->doMySqlQuery ( &result, the_query );

     if ( !bret ) {
          return SCM_BOOL_F;
     }

     SCM list = SCM_EOL;
     for ( auto row: result ) {
          SCM attr = SCM_EOL;
          for ( auto col: row ) {
              attr = scm_cons ( ScmFromString( col.c_str() ), attr );
          }
          list = scm_cons ( scm_reverse ( attr ), list );
     }
     return scm_reverse ( list );
}

static SCM mtfa_db_mysql_last_id ( SCM mtfa_db_mysql )
{
     mysql_db_class* pt = ( mysql_db_class* ) scm_foreign_object_signed_ref ( mtfa_db_mysql, 0 );
     unsigned long long lastid = pt->GetLastId();
     return scm_from_ulong_long ( lastid );
}

static SCM mtfa_mysql_escape_string(SCM mtfa_db_mysql, SCM from)
{
    mysql_db_class* pt = ( mysql_db_class* ) scm_foreign_object_signed_ref ( mtfa_db_mysql, 0 );
    size_t st_len;
    char * st = ScmToStringn( from, &st_len );
    char * to = (char*)malloc(st_len*2+1);
    mysql_real_escape_string(pt->conn, to, (const char*)from, st_len);
    SCM result = ScmFromString(to);
    free(from);
    free(to);
    return result;
}

static SCM mtfa_db_mysql_p ( SCM mtfa_db_mysql )
{
//      auto const body ( cify<SCM ( * ) ( void * ) >
//                        (
//      [&] ( void * data ) {
//           //se va tutto bene, torna true
//           scm_assert_foreign_object_type ( mtfa_db_mysql_type, mtfa_db_mysql );
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
     auto const body = [] (void * data) {
         scm_assert_foreign_object_type ( mtfa_db_mysql_type, *(SCM*)data);
         return SCM_BOOL_T;
     };
     auto const handler = [] ( void *data, SCM key, SCM args ) {
         return SCM_BOOL_F;
     };
     
     SCM ret = scm_internal_catch ( SCM_BOOL_T, body, &mtfa_db_mysql, handler, nullptr );

     return ret;
}

extern "C" void init_mtfa_db_mysql();

extern "C" void init_mtfa_db_mysql()
{
     SCM name, slots;
     scm_t_struct_finalize finalizer;
     name = scm_from_latin1_symbol ( "mtfa-db-mysql" );
     slots = scm_list_1 ( scm_from_latin1_symbol ( "mysql_class" ) );
     finalizer = finalize_mtfa_db_mysql;
     mtfa_db_mysql_type = scm_make_foreign_object_type ( name, slots, finalizer );

     scm_c_define_gsubr ( "mtfa-db-mysql-open", 0, 0, 0, ( void* ) mtfa_db_mysql_open );
     scm_c_define_gsubr ( "mtfa-db-mysql-connect", 6, 0, 0, ( void* ) mtfa_db_mysql_connect );
     scm_c_define_gsubr ( "mtfa-db-mysql-close", 1, 0, 0, ( void* ) mtfa_db_mysql_close );
     scm_c_define_gsubr ( "mtfa-db-mysql-disconnect", 1, 0, 0, ( void* ) mtfa_db_mysql_disconnect );
     scm_c_define_gsubr ( "mtfa-db-mysql-start-transaction", 1, 0, 0, ( void* ) mtfa_db_mysql_start_transaction );
     scm_c_define_gsubr ( "mtfa-db-mysql-rollback", 1, 0, 0, ( void* ) mtfa_db_mysql_rollback );
     scm_c_define_gsubr ( "mtfa-db-mysql-commit", 1, 0, 0, ( void* ) mtfa_db_mysql_commit );
     scm_c_define_gsubr ( "mtfa-db-mysql-do-sql", 2, 0, 0, ( void* ) mtfa_db_mysql_do_sql );
     scm_c_define_gsubr ( "mtfa-db-mysql-last-id", 1, 0, 0, ( void* ) mtfa_db_mysql_last_id );
     scm_c_define_gsubr ( "mtfa-db-mysql-p", 1, 0, 0, ( void* ) mtfa_db_mysql_p );
     scm_c_define_gsubr ( "mtfa-db-mysql-escape-string", 2, 0, 0, ( void* ) mtfa_mysql_escape_string );
}


