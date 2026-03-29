#pragma once

#include <stdlib.h>
#include <list>
#include <string>

#include<iostream>

#ifdef BOOL
#undef BOOL
#endif

#ifdef DWORD
#undef DWORD
#endif

#ifdef LPSTR
#undef LPSTR
#endif

#ifdef LPDWORD
#undef LPDWORD
#endif

#ifdef HWND
#undef HWND
#endif

//#include <sql.h>
//#include <sqlext.h>
#include <mysql.h>
#include <mysql_com.h>

#ifdef _WIN32

#include <mbstring.h>

#endif

#include <sqlite3.h>

#include <string>
#include <list>

#include "mtfa_db_mysql.h"

using namespace std;

class mtfa_db
{
private:
    static int callback ( void *v_result, int argc, char **argv, char **azColName );

public:
    typedef std::list<std::list<string> > TDynamicTable;    // A list of list of strings
    typedef std::list<std::list<string> >::iterator TDynamicTable_Iterator;    // A list of list of strings


    // static bool Create(string path);
    // se stringa vuota, crea il DB e basta
    // Se inizia per "1", allora contiene multipli comandi, separati da "|"
    static bool Execute ( string db, string statement, TDynamicTable & result );

    static sqlite3 *Open ( string db );
    static bool Execute ( sqlite3 *db, string statement, TDynamicTable & result );
    static void Close ( sqlite3 *db );

//    static std::list<string> ODBC_enum_drivers();
//
//    static std::list<string> ODBC_enum_data_sources();
//
//    static bool ODBC_Execute(string       db,
//                             string       statement,
//                             TDynamicTable & result);    // Questa \ufffd per ODBC
};


#ifdef MAX_OUTPUT
#undef MAX_OUTPUT
#endif

#define MAX_OUTPUT 10000000
/*
class mysql_db_class
{
private:
    string    host_name;
    string    user_name;
    string    password;
    string    db_name;
    bool         already_closed;
    unsigned int port_num;
    MYSQL *      conn;
    MYSQL_RES *  res_set;

    bool process_result_set();

    bool process_query(const char * query);

    long int righe;
    long int colonne;
    char *** result;

public:
    bool do_connect();

    void do_disconnect();

    void freeResult();

    bool Connect(const char *       in_host_name,
                   const char *       in_user_name,
                   const char *       in_password,
                   const char *       in_db_name,
                   const unsigned int in_port_num);

    mysql_db_class();

    mysql_db_class(const char *       in_host_name,
                   const char *       in_user_name,
                   const char *       in_password,
                   const char *       in_db_name,
                   const unsigned int in_port_num);

    mysql_db_class(string          in_host_name,
                   string          in_user_name,
                   string          in_password,
                   string          in_db_name,
                   const unsigned int in_port_num);

    bool doMySqlQuery(std::list<std::list<string> > * result, const string the_query);

    bool StartTransaction();

    bool Commit();

    bool RollBack();

    unsigned long long GetLastId();

    ~mysql_db_class();

    bool do_query(const char * query);

    long int getRows();

    long int getCols();

    char * getResult(int row, int col);
};
*/
