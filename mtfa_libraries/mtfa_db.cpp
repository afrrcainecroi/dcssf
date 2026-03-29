#include <stdlib.h>
#include <string>
#include <string.h>
#include "mtfa_db.h"
//#include "mtfa_db_mysql.h"
#include <vector>
#include "mtfa_utils.h"

using namespace std;


int mtfa_db::callback ( void *v_result, int argc, char **argv, char **azColName )
{
     mtfa_db::TDynamicTable * result = ( mtfa_db::TDynamicTable * ) v_result;

     std::list<string> ls;
//    if (result->size() == 0)
//    {
//        //E' la prima volta! metto il nome degli attributi
//        for(int i=0; i<argc; i++)
//        {
//            ls.push_back(azColName[i]);
//        }
//        result->push_back(ls);
//    }

     //Se sono qui metto i valori...
     ls.clear();

     for ( int i=0; i<argc; i++ ) {
          ls.push_back ( argv[i] ? argv[i] : "NULL" );
     }
     result->push_back ( ls );
     return 0;
}

bool mtfa_db::Execute ( string path, string statement, mtfa_db::TDynamicTable & result )
{
     //#ifdef _WIN32
     sqlite3 *db;
     char *zErrMsg = 0;
     int rc;

     rc = sqlite3_open ( path.c_str(), &db );
     if ( rc ) {
          sqlite3_close ( db );
          return false;
     }

     if ( statement.size() ==0 ) {
          sqlite3_close ( db );
          return true;
     }

     if ( statement.substr ( 0, 1 ).compare ( "1" ) ==0 ) {
          std::vector<string> vs;
          //è uno statement multiplo! separato da \n
          vs = mtfa_string_split ( statement.substr ( 1 ), "|" );

          for ( int i=0; i<vs.size(); ++i ) {
               //Posso eseguire lo statement
               result.clear();
               rc = sqlite3_exec ( db, vs[i].c_str(), callback, &result, &zErrMsg );
               if ( rc != SQLITE_OK ) {
                    fprintf ( stderr, "SQL error: %s\n", zErrMsg );
                    sqlite3_free ( zErrMsg );
                    sqlite3_close ( db );
                    return false;
               }
          }
     } else {
          //Posso eseguire lo statement
          result.clear();
          rc = sqlite3_exec ( db, statement.c_str(), callback, &result, &zErrMsg );
          if ( rc != SQLITE_OK ) {
               fprintf ( stderr, "SQL error: %s\n", zErrMsg );
               sqlite3_free ( zErrMsg );
               sqlite3_close ( db );
               return false;
          }
     }
     sqlite3_close ( db );
     return true;
}
sqlite3 * mtfa_db::Open ( string db_path )
{
     sqlite3 *db;
     char *zErrMsg = 0;
     int rc = sqlite3_open ( db_path.c_str(), &db );
     if ( rc ) {
          sqlite3_close ( db );
          return nullptr;
     }

     //questo per accelerare i risultati
     mtfa_db::TDynamicTable result;
     mtfa_db::Execute ( db, "PRAGMA synchronous = NORMAL", result );
     mtfa_db::Execute ( db, "PRAGMA journal_mode = WAL", result );

     return db;
}

bool mtfa_db::Execute ( sqlite3 *db, string statement, mtfa_db::TDynamicTable & result )
{
     int rc;
     char *zErrMsg = 0;
     if ( statement.substr ( 0, 1 ).compare ( "1" ) ==0 ) {
          std::vector<string> vs;
          //è uno statement multiplo! separato da \n
          vs = mtfa_string_split ( statement.substr ( 1 ), "|" );

          for ( int i=0; i<vs.size(); ++i ) {
               //Posso eseguire lo statement
               result.clear();
               rc = sqlite3_exec ( db, vs[i].c_str(), callback, &result, nullptr );
               if ( rc != SQLITE_OK ) {
                    return false;
               }
          }
     } else {
          //Posso eseguire lo statement
          result.clear();
          rc = sqlite3_exec ( db, statement.c_str(), callback, &result, nullptr );
          if ( rc != SQLITE_OK ) {
               return false;
          }
     }
     return true;
}

void mtfa_db::Close ( sqlite3 *db )
{
     sqlite3_close ( db );
}


/*

std::list<string> mtfa_db::ODBC_enum_drivers()
{
    std::list <string> ls;
#ifdef _WIN32

    SQLHENV hEnv;
    RETCODE rc;

    rc = SQLAllocEnv(&hEnv);

    SQLCHAR drvDescr[1024];
    SQLSMALLINT bl1=1024;
    SQLSMALLINT dl;
    SQLCHAR da[1024];
    SQLSMALLINT bl2=1024;
    SQLSMALLINT al;
    rc = SQLDrivers(hEnv, SQL_FETCH_FIRST, drvDescr, bl1, &dl, da, bl2, &al);
    while (rc == SQL_SUCCESS)
    {
        //cout << __func__   << drvDescr << " <=> " << da << endl;
        ls.push_back(string(&drvDescr[0], &drvDescr[dl]));
        rc = SQLDrivers(hEnv, SQL_FD_FETCH_NEXT, drvDescr, bl1, &dl, da, bl2, &al);
    }

    rc = SQLFreeEnv(hEnv);
    return ls;
#else
#warning to be written
    return ls;
#endif
}

std::list<string> mtfa_db::ODBC_enum_data_sources()
{
    std::list <string> ls;
#ifdef _WIN32

    SQLHENV hEnv;
    RETCODE rc;

    rc = SQLAllocEnv(&hEnv);

    SQLCHAR drvDescr[1024];
    SQLSMALLINT bl1=1024;
    SQLSMALLINT dl;
    SQLCHAR da[1024];
    SQLSMALLINT bl2=1024;
    SQLSMALLINT al;

    rc = SQLDataSources(hEnv, SQL_FETCH_FIRST, drvDescr, bl1, &dl, da, bl2, &al);
    while (rc == SQL_SUCCESS)
    {
        rc = SQLDataSources(hEnv, SQL_FETCH_NEXT, drvDescr, bl1, &dl, da, bl2, &al);
        ls.push_back(string(&drvDescr[0], &drvDescr[dl]));
    }

    rc = SQLFreeEnv(hEnv);
    return ls;
#else
#warning to be written
    return ls;
#endif
}


bool mtfa_db::ODBC_Execute(string db, string statement, TDynamicTable & result)
{
    result.clear();
#ifdef _WIN32

    HENV hEnv;
    HDBC hDbc;
    RETCODE rc;
    int iOut;
    char strOut[512];
    ////char szDSN[256] = "driver={Microsoft Access Driver (*.mdb)};dbq=[c:\\db1.mdb];";
    //char szDSN[256] = "DSN=postazioniconformi_light;";


    //char* szSql = "select * from STATISTICHE_ALBERO";
    rc = SQLAllocEnv(&hEnv);

    rc = SQLAllocConnect(hEnv, &hDbc);

    rc = SQLDriverConnect(
             hDbc,
             NULL,
             (unsigned char*)db.c_str(),
             SQL_NTS,
             (unsigned char*)strOut,
             511,
             (SQLSMALLINT*)&iOut,
             SQL_DRIVER_NOPROMPT);

    if (rc != SQL_SUCCESS)
    {
        rc = SQLDisconnect(hDbc);
        rc = SQLFreeEnv(hEnv);
        return false;
    }

    //int ival;
    //char chval[128];
    HSTMT hStmt;

    rc = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
    if (rc != SQL_SUCCESS)
    {
        rc = SQLDisconnect(hDbc);
        rc = SQLFreeEnv(hEnv);
        return false;
    }

    rc = SQLExecDirect(hStmt, (SQLCHAR*)statement.c_str(), SQL_NTS);
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
    {
        rc = SQLFreeStmt(hStmt, SQL_DROP);
        rc = SQLDisconnect(hDbc);
        rc = SQLFreeEnv(hEnv);
        return false;
    }

    //Ora elenco tutti i valori.
    //Ma quante colonne sono e a quali attributi corrispondono?
    SQLSMALLINT columnCount;
    rc = SQLNumResultCols( hStmt, & columnCount );
    if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
    {
        rc = SQLFreeStmt(hStmt, SQL_DROP);
        rc = SQLDisconnect(hDbc);
        rc = SQLFreeEnv(hEnv);
        return false;
    }

    if (columnCount == 0)
    {
        rc=SQLFreeStmt(hStmt, SQL_DROP);
        rc = SQLDisconnect(hDbc);
        rc = SQLFreeEnv(hEnv);

        return true;
    }

    //std::cout << __func__   << "Sono in totale: " << columnCount << " colonne\n";

    //Metto i nomi e poi metto i valori
    std::list<string, gc_allocator<string>> ls;
    for (int i=1; i <= columnCount; ++i)
    {
        //ora stampo info sugli attributi
        SQLCHAR		   columnName[512];
        SQLSMALLINT    bufferLength=512;
        SQLSMALLINT    nameLength;
        SQLSMALLINT    dataType;
        SQLULEN        columnSize;
        SQLSMALLINT    decimalDigits;
        SQLSMALLINT    nullable;
        rc = SQLDescribeCol(
                 hStmt,
                 i,
                 columnName,
                 bufferLength,
                 &nameLength,
                 &dataType,
                 &columnSize,
                 &decimalDigits,
                 &nullable);
        if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
        {
            unsigned char szSQLSTATE[10];
            SQLINTEGER nErr;
            unsigned char msg[SQL_MAX_MESSAGE_LENGTH + 1];
            SWORD cbmsg;

            SQLRETURN     rc2;

            int i=1;
            SQLSMALLINT msglen=0;
            while ((rc2 = SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, i, szSQLSTATE, &nErr, msg, sizeof(msg), &msglen)) != SQL_NO_DATA)
            {
                //DisplayError(szSQLSTATE,nErr,msg,msglen);
                i++;
            }
            rc = SQLFreeStmt(hStmt, SQL_DROP);
            rc = SQLDisconnect(hDbc);
            rc = SQLFreeEnv(hEnv);
            return false;
        }
        //std::cout << __func__   << "ColName: " << columnName << std::endl;
        ls.push_front(string(columnName, &columnName[nameLength]));
    }

    result.push_back(ls);
    ls.clear();

    //Ora i valori associati
    while (true)
    {
        rc = SQLFetch(hStmt);
        if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
        {
            unsigned char szSQLSTATE[10];
            SQLINTEGER nErr;
            unsigned char msg[SQL_MAX_MESSAGE_LENGTH + 1];
            SWORD cbmsg;

            SQLRETURN     rc2;

            int i=1;
            SQLSMALLINT msglen=0;
            while ((rc2 = SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, i, szSQLSTATE, &nErr, msg, sizeof(msg), &msglen)) != SQL_NO_DATA)
            {
                //DisplayError(szSQLSTATE,nErr,msg,msglen);
                i++;
            }
            break;
        }

        for (int i=1; i <= columnCount; ++i)
        {
            char * targetValuePtr = new   char[8192];
            SQLLEN len;
            rc = SQLGetData(
                     hStmt,
                     i,
                     SQL_C_CHAR,
                     targetValuePtr,
                     8192,
                     &len);

            if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO)
            {
                unsigned char szSQLSTATE[10];
                SQLINTEGER nErr;
                unsigned char msg[SQL_MAX_MESSAGE_LENGTH + 1];
                SWORD cbmsg;

                SQLRETURN     rc2;

                int i=1;
                SQLSMALLINT msglen=0;
                while ((rc2 = SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, i, szSQLSTATE, &nErr, msg, sizeof(msg), &msglen)) != SQL_NO_DATA)
                {
                    //DisplayError(szSQLSTATE,nErr,msg,msglen);
                    i++;
                }
                rc = SQLFreeStmt(hStmt, SQL_DROP);
                rc = SQLDisconnect(hDbc);
                rc = SQLFreeEnv(hEnv);
                return 0;
            }

            ls.push_front(targetValuePtr);
            //std::cout << __func__   << "ColVal: " << targetValuePtr << std::endl;
            //		attributes.push_front(string(columnName, &columnName[nameLength]));
        }
        result.push_back(ls);
        ls.clear();
    }

    rc=SQLFreeStmt(hStmt, SQL_DROP);

    rc = SQLDisconnect(hDbc);
    rc = SQLFreeEnv(hEnv);
    return true;
#else
#warning to be written
    return false;
#endif
}

*/

#include <mysql.h>
#include <mysql_com.h>

using std::cout;
using std::cerr;
using std::endl;

//Per gestire la prepare statement: https://gist.github.com/hoterran/6365915
mysql_db_class::mysql_db_class()
{
     res_set=nullptr;
     conn=nullptr;
     host_name.clear();
     user_name.clear();
     password.clear();
     db_name.clear();
     port_num = 0;
     righe = 0;
     colonne = 0;
     result = NULL;
     already_closed = true;
     res_set=nullptr;
     conn=nullptr;
//      stmt = nullptr;
//      bind_len=0;
//      memset(msql_bind, 0, sizeof(msql_bind));
}

mysql_db_class::mysql_db_class ( const char * in_host_name, const char * in_user_name, const char * in_password, const char * in_db_name, const unsigned int in_port_num )
{
     //L'allocazione la facciamo direttamente qui!
     res_set=nullptr;
     conn=nullptr;
     host_name =  in_host_name;
     user_name = in_user_name;
     password = in_password;
     db_name = in_db_name;
     port_num = in_port_num;
     righe = 0;
     colonne = 0;
     result = NULL;
     already_closed = true;
//      stmt = nullptr;
//      bind_len=0;
//      memset(msql_bind, 0, sizeof(msql_bind));
}

bool mysql_db_class::Connect ( const char * in_host_name, const char * in_user_name, const char * in_password, const char * in_db_name, const unsigned int in_port_num )
{
     res_set=nullptr;
     conn=nullptr;
     host_name = in_host_name;
     user_name = in_user_name;
     password = in_password;
     db_name = in_db_name;
     port_num = in_port_num;
     righe = 0;
     colonne = 0;
     result = NULL;
//      stmt = nullptr;
//      bind_len=0;
//      memset(msql_bind, 0, sizeof(msql_bind));
     already_closed = true;
     return do_connect();
}

mysql_db_class::mysql_db_class ( string in_host_name, string in_user_name, string in_password, string in_db_name, const unsigned int in_port_num )
{
     //L'allocazione la facciamo direttamente qui!
     res_set=nullptr;
     conn=nullptr;
     host_name = in_host_name;
     user_name = in_user_name;
     password = in_password;
     db_name = in_db_name;
     port_num = in_port_num;
     righe = 0;
     colonne = 0;
     result = NULL;
//      stmt = nullptr;
//      bind_len=0;
//      memset(msql_bind, 0, sizeof(msql_bind));
     already_closed = true;
}

mysql_db_class::~mysql_db_class()
{
    //Nulla da deallocare in quanto le stringhe si dovrebbero deallocare da sole!
//     if (stmt)
//     {
//         mysql_stmt_close(stmt);
//         stmt = nullptr;
//     }
    
    //Ma il risultato, se c'e' stato, no!
    freeResult();
    if ( !already_closed && conn ) {
        mysql_close ( conn );
    }
    conn = nullptr;
     already_closed = true;
}

bool mysql_db_class::StartTransaction()
{
    int dummy;
     return do_query ( "START TRANSACTION;", dummy );
}

bool mysql_db_class::Commit()
{
    int dummy;
    return do_query ( "COMMIT;", dummy );
}

bool mysql_db_class::RollBack()
{
    int dummy;
    return do_query ( "ROLLBACK;" , dummy);
}


bool mysql_db_class::do_query ( const char * query, int & result_size )
{
     if ( process_query ( query, result_size ) == false ) {
          freeResult();   //Se per caso ci sono stati risultati!
          return false;
     }
     return true;
}

long int mysql_db_class::getRows()
{
     return righe;
}

long int mysql_db_class::getCols()
{
     return colonne;
}

char * mysql_db_class::getResult ( int row, int col )
{
     if ( ( result ) && ( righe>row ) && ( colonne>col ) && ( result[row] ) ) {
          return result[row][col];
     } else {
          return NULL;
     }
}

void mysql_db_class::freeResult()
{
     if ( result ) {
          for ( int i=0; i<righe; i++ ) {
               if ( result[i] ) {
                    for ( int j=0; j<colonne; j++ )
                         if ( result[i][j] ) {
                              free ( result[i][j] );
                         }
                    delete [] result[i];
               }
          }
          delete [] result;
     }
     result=NULL;
//    righe=0;
//    colonne=0;
}

bool mysql_db_class::do_connect ()
{
     conn = mysql_init ( NULL ); /* allocate, initialize connection handler */
     if ( conn == NULL ) {
          //print_error (NULL, "mysql_init() failed (probably out of memory)");
          return false;
     }

#if defined(MYSQL_VERSION_ID) && MYSQL_VERSION_ID >= 32200 /* 3.22 and up */
     if ( mysql_real_connect ( conn, host_name.c_str(), user_name.c_str(), password.c_str(), db_name.c_str(), port_num, NULL, 0 ) == NULL ) {
          //print_error (conn, "mysql_real_connect() failed");
          return false;
     }
#else              /* pre-3.22 */
     if ( mysql_real_connect ( conn, host_name.c_str(), user_name.c_str(), password.c_str(), port_num, NULL, 0 ) == NULL ) {
          //print_error (conn, "mysql_real_connect() failed");
          return false;
     }
     if ( db_name != "" ) { /* simulate effect of db_name parameter */
          if ( mysql_select_db ( conn, db_name.c_str() ) != 0 ) {
               //print_error (conn, "mysql_select_db() failed");
               mysql_close ( conn );
               return false;
          }
     }
#endif

     already_closed = false;
     return true;     /* connection is established */
}


unsigned long long mysql_db_class::GetLastId()
{
     return mysql_insert_id ( conn );
}


void mysql_db_class::do_disconnect ()
{
     if ( !already_closed ) {
          mysql_close ( conn );
     }
     already_closed = true;
}

#if !defined(MYSQL_VERSION_ID) || MYSQL_VERSION_ID<32224
#define mysql_field_count mysql_num_fields
#endif

bool mysql_db_class::process_result_set ()
{
     //{
     //  //Facciamo una provetta. Stampo alcune info...
     //  my_print_result_metadata(res_set);
     //  my_process_result_set(res_set);
     //  return true;
     //}
     MYSQL_ROW    row;
     MYSQL_FIELD  *field;
     //unsigned int  i;

     righe = mysql_num_rows ( res_set );
     colonne = mysql_num_fields ( res_set );

     //Inizializzo il risultato
     result = new  char ** [righe];
     for ( int i=0; i<righe; i++ ) {
          //inizializzo la riga i-esima
          result[i] = new  char * [colonne];
          for ( int j=0; j<colonne; j++ ) {
               result[i][j] = NULL;
          }
     }

     int riga = 0;
     char * tmp;
     while ( ( row = mysql_fetch_row ( res_set ) ) != NULL ) {
          //unsigned long *lengths;
          //lengths = mysql_fetch_lengths(res_set);
          mysql_field_seek ( res_set, 0 );
          for ( int i = 0; i < colonne; i++ ) {
               //Devo gestire i pochi tipi che sono vettori binari, il resto è stringa!!!
               field = mysql_fetch_field ( res_set );
               //printf("FIELD TYPE: %d\n", field->type);
               if ( field->type == 16 ) { //enum_field_types::MYSQL_TYPE_BITs
                    tmp = new char[field->max_length+1];
                    //fprintf(stdout, " %*u |", (int) field->max_length, (unsigned int)(row[i][0]));
                    sprintf ( tmp, "%u", ( unsigned int ) ( row[i][0] ) );
                    result[riga][i] = strdup ( tmp );
                    delete [] tmp;
               }
               //else if (field->type == enum_field_types::MYSQL_TYPE_BLOB)
               //{
               //  fprintf(stdout, " %-*s |", (int) field->max_length, row[i]);
               //}
               else {
                    tmp = new char[field->max_length+1];
                    sprintf ( tmp, "%s", row[i] ? row[i] : "NULL" );
                    result[riga][i] = strdup ( tmp );
                    delete [] tmp;
               }
          }

          //mysql_field_seek(res_set, 0);
          //for (int i = 0; i < mysql_num_fields (res_set); i++)
          //         {
          //  field = mysql_fetch_field(res_set);
          //             //result[riga][i] = new char [strlen((row[i]!=NULL)?row[i]:"NULL")+1];
          //  if (row[i] == NULL)
          //  {
          //              result[riga][i] = new char [strlen("NULL")+1];
          //           strcpy(result[riga][i], "NULL");
          //  }
          //  else if (field->type == enum_field_types::MYSQL_TYPE_BIT)
          //  {
          //    //fprintf(stdout, " %*u |", (int) field->max_length, (unsigned int)(row[i][0]));
          //    sprintf(tmp, "%u", (unsigned int)(row[i][0]));
          //    result[riga][i] = strdup(tmp);
          //  }
          //  else if (field->type == enum_field_types::MYSQL_TYPE_INT24)
          //  {
          //    //fprintf(stdout, " %*d |", (int) field->max_length, (int)(row[i]));
          //    sprintf(tmp, "%d", (int)(row[i]));
          //    result[riga][i] = strdup(tmp);
          //  }
          //  else if (field->type == enum_field_types::MYSQL_TYPE_LONG)
          //  {
          //    sprintf(tmp, "%ld", (long)(row[i]));
          //    result[riga][i] = strdup(tmp);
          //    //fprintf(stdout, " %*ld |", (int) field->max_length, (int)(row[i]));
          //  }
          //  else if (field->type == enum_field_types::MYSQL_TYPE_LONGLONG)
          //  {
          //    sprintf(tmp, "%lld", (long long)(row[i]));
          //    result[riga][i] = strdup(tmp);
          //    //fprintf(stdout, " %*Int64 |", (long long) field->max_length, (long long)(row[i]));
          //  }
          //  else if (field->type == enum_field_types::MYSQL_TYPE_NULL)
          //  {
          //    //fprintf(stdout, " %*s |", (int) field->max_length, "NULL");
          //    sprintf(tmp, "%s", "NULL");
          //    result[riga][i] = strdup(tmp);
          //  }
          //  else if (field->type == enum_field_types::MYSQL_TYPE_SHORT)
          //  {
          //    sprintf(tmp, "%hd", (short int)(row[i]));
          //    result[riga][i] = strdup(tmp);
          //    //fprintf(stdout, " %*hd |", (short int) field->max_length, (short int)(row[i]));
          //  }
          //  else if (field->type == enum_field_types::MYSQL_TYPE_TINY)
          //  {
          //    sprintf(tmp, "%d", (int)(row[i]));
          //    result[riga][i] = strdup(tmp);
          //    //fprintf(stdout, " %*d |", (int) field->max_length, (int)(row[i]));
          //  }
          //  else if (IS_NUM(field->type))
          //  {
          //    sprintf(tmp, "%d", (int)(row[i]));
          //    result[riga][i] = strdup(tmp);
          //  }
          //  else
          //  {
          //    //sprintf(tmp, "%s", row[i]);
          //    result[riga][i] = strdup(row[i]);
          //    //fprintf(stdout, " %-*s |", (int) field->max_length, row[i]);
          //  }

          //             //result[riga][i] = new char [field->max_length+1];
          //             //strcpy(result[riga][i], (row[i]!=NULL)?row[i]:"NULL");

          //      }
          riga++;
     }

     if ( mysql_errno ( conn ) != 0 ) {
          //print_error (conn, "mysql_fetch_row() failed");
          freeResult();
          return false;
     }
     //      else
     //              printf ("%lu rows returned\n", (unsigned long) mysql_num_rows (res_set));
     return true;
}

bool mysql_db_class::process_query ( const char *query, int & result_size )
{
     //unsigned int field_count;

     if ( mysql_query ( conn, query ) != 0 ) { /* the query failed */
          //ap_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r, "BCV: process_query() failed in mysql_query()!");
          return false;
     }
     //ap_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r, "BCV: FATTA LA QUERY  !");

     /* the query succeeded; determine whether or not it returns data */
     res_set = mysql_store_result ( conn );
     
     if ( res_set == NULL ) { /* no result set was returned */
         result_size=0;
          /*
          * does the lack of a result set mean that an error
          * occurred or that no result set was returned?
          */
          if ( mysql_field_count ( conn ) > 0 ) {
               /*
               * a result set was expected, but mysql_store_result()
               * did not return one; this means an error occurred
               */
               //ap_log_rerror(APLOG_MARK, APLOG_WARNING, 0, r, "BCV: process_query() Problem processing result set!");
               return false;
          } else {
               /*
               * no result set was returned; query returned no data
               * (it was not a SELECT, SHOW, DESCRIBE, or EXPLAIN),
               * so just report number of rows affected by query
               */
               //printf ("%lu rows affected\n", (unsigned long) mysql_affected_rows (conn));
               return true;    //Era una query che non torna oggetti (tipo update, delete, ...)
          }
     } else { /* a result set was returned */
         result_size=1;
         /* process rows, then free the result set */
          bool nret;
          nret = process_result_set ();
          mysql_free_result ( res_set );
          return nret;    //Questo e' l'unico punto in cui puo' tornare true!
     }
     return false;
}

bool mysql_db_class::doMySqlQuery ( std::list<std::list<string> > * result, const string &the_query )
{
     result->clear();
    int result_available=0;
     if ( do_query ( the_query.c_str(), result_available ) ==false ) {
          return false;
     }
     
    if (result_available)
    {
     for ( int r=0; r<getRows(); r++ ) {
          std::list<string> line;
          for ( int i = 0; i<getCols(); ++i ) {
               char * val = getResult ( r, i );
               if (val)
                    line.push_back ( val );
          }
          if (line.size() > 0)
              result->push_back ( line );
     }
    }
     freeResult ();
     return true;
}

