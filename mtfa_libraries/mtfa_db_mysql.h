#pragma once

#include <stdlib.h>
#include <list>
#include <string>
#include <functional>
#include<iostream>

#include <mysql.h>
//#include <mysql_com.h>

#include <string>
#include <list>

using namespace std;

class mysql_db_class
{
private:
    string    host_name;
    string    user_name;
    string    password;
    string    db_name;
    bool         already_closed;
    unsigned int port_num;
    MYSQL_RES *  res_set;
//     MYSQL_STMT    *stmt;
//     MYSQL_BIND    msql_bind[32];
//     MYSQL_BIND    msql_result[32];
//     int           bind_len;

    bool process_result_set();

    bool process_query ( const char * query, int & result_size );

    long int righe;
    long int colonne;
    char *** result;

public:
    MYSQL *      conn;
    bool do_connect();

    void do_disconnect();

    void freeResult();

    bool Connect ( const char *       in_host_name,
                   const char *       in_user_name,
                   const char *       in_password,
                   const char *       in_db_name,
                   const unsigned int in_port_num );

    mysql_db_class();

    mysql_db_class ( const char *       in_host_name,
                     const char *       in_user_name,
                     const char *       in_password,
                     const char *       in_db_name,
                     const unsigned int in_port_num );

    mysql_db_class ( string          in_host_name,
                     string          in_user_name,
                     string          in_password,
                     string          in_db_name,
                     const unsigned int in_port_num );

    bool doMySqlQuery ( std::list<std::list<string> > * result, const string &the_query );

//     bool doMySqlPrepareStatement ( const string &stmtSql )
//     {
//         if (stmt)
//         {
//             mysql_stmt_close(stmt);
//             stmt = nullptr;
//         }
//         stmt = mysql_stmt_init(conn);
//         mysql_stmt_prepare(stmt, stmtSql.c_str(), stmtSql.size());
//         mysql_stmt_bind_param(stmt, msql_bind);
// 
//         bind_len=0;     //la prossima volta riparto da 0
//         memset(msql_bind, 0, sizeof(msql_bind));  //Ripristino la bind per riusarla
//     }
// 
//     bool doMySqlPrepareResult ( )
//     {
//         mysql_stmt_bind_result(stmt, msql_bind);
// 
//         bind_len=0;     //la prossima volta riparto da 0
//         memset(msql_bind, 0, sizeof(msql_bind));  //Ripristino la bind per riusarla
//     }
//     
//     //i valori al posto dei ? vanno assegnati fuori
//     bool doMysqlExecutePreparedStatement()
//     {
//         
//     }
// 
//     static void floc (MYSQL_BIND *thebind, int thelen, char p) 
//     {
//         thebind[thelen].buffer_type = enum_field_types::MYSQL_TYPE_TINY;
//         thebind[thelen].buffer = (char*)&p;
//     };
//     static void floc (MYSQL_BIND *thebind, int thelen, unsigned char p) 
//     {   
//         thebind[thelen].buffer_type = enum_field_types::MYSQL_TYPE_TINY;
//         thebind[thelen].buffer = (char*)&p;
//     };
//     static void floc (MYSQL_BIND *thebind, int thelen, const short p) 
//     {
//         thebind[thelen].buffer_type = enum_field_types::MYSQL_TYPE_SHORT;
//         thebind[thelen].buffer = (char*)&p;
//     };
//     static void floc (MYSQL_BIND *thebind, int thelen, const unsigned short p) 
//     {
//         thebind[thelen].buffer_type = enum_field_types::MYSQL_TYPE_SHORT;
//         thebind[thelen].buffer = (char*)&p;
//     };
//     static void floc (MYSQL_BIND *thebind, int thelen, const int p) 
//     {
//         thebind[thelen].buffer_type = enum_field_types::MYSQL_TYPE_LONG;
//         thebind[thelen].buffer = (char*)&p;
//     };
//     static void floc (MYSQL_BIND *thebind, int thelen, const unsigned int p) 
//     {
//         thebind[thelen].buffer_type = enum_field_types::MYSQL_TYPE_LONG;
//         thebind[thelen].buffer = (char*)&p;
//     };
//     static void floc (MYSQL_BIND *thebind, int thelen, const long p) 
//     {
//         thebind[thelen].buffer_type = enum_field_types::MYSQL_TYPE_LONG;
//         thebind[thelen].buffer = (char*)&p;
//     };
//     static void floc (MYSQL_BIND *thebind, int thelen, const unsigned long p) 
//     {
//         thebind[thelen].buffer_type = enum_field_types::MYSQL_TYPE_LONG;
//         thebind[thelen].buffer = (char*)&p;
//     };
//     static void floc (MYSQL_BIND *thebind, int thelen, const long long p) 
//     {
//         thebind[thelen].buffer_type = enum_field_types::MYSQL_TYPE_LONGLONG;
//         thebind[thelen].buffer = (char*)&p;
//     };
//     static void floc (MYSQL_BIND *thebind, int thelen, const unsigned long long p) 
//     {
//         thebind[thelen].buffer_type = enum_field_types::MYSQL_TYPE_LONGLONG;
//         thebind[thelen].buffer = (char*)&p;
//     };
//     static void floc (MYSQL_BIND *thebind, int thelen, const char * p) 
//     {
//         thebind[thelen].buffer_type = enum_field_types::MYSQL_TYPE_LONG_BLOB;
//         thebind[thelen].buffer = (char*)p;
//     };
//     static void floc (MYSQL_BIND *thebind, int thelen, const unsigned char * p) 
//     {
//         thebind[thelen].buffer_type = enum_field_types::MYSQL_TYPE_LONG_BLOB;
//         thebind[thelen].buffer = (char*)p;
//     };
// 
//     template <typename H, typename... T>
//     bool doMySqlPrepareStatement ( const string &stmtSql, H p, T... t)
//     {
//         floc(msql_bind, bind_len++, p);
//         doMySqlPrepareStatement ( stmtSql, t...);
//     }
//     
//     template <typename H, typename... T>
//     bool doMySqlPrepareResult (H p, T... t)
//     {
//         floc(msql_bind, bind_len++, p);
//         doMySqlPrepareResult ( t...);
//     }

    bool StartTransaction();

    bool Commit();

    bool RollBack();

    unsigned long long GetLastId();

    ~mysql_db_class();

    bool do_query ( const char * query, int & result_size );

    long int getRows();

    long int getCols();

    char * getResult ( int row, int col );
};

