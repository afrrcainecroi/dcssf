#include <mtfa_utils.h>
#include "command.h"
#include "va_parser.hpp"

#include <iostream>
#include <sstream>

using namespace std;

using namespace VaParser;

using std::cout;
using std::endl;

Command::Command ( const int id, const std::string &name, const std::list<Command> &arguments ) :
     m_id ( id ),
     m_name ( name ),
     m_args_c ( arguments ),
     m_args_s()
{
}

void Command::addArg ( string s )
{
     m_args_s.push_back ( s );
}

void Command::addArg ( Command c )
{
     m_args_c.push_back ( c );
}

Command::Command ( const int id, const std::string &name, const std::list<string> &arguments ) :
     m_id ( id ),
     m_name ( name ),
     m_args_c(),
     m_args_s ( arguments )
{
}

Command::Command ( const int id, const std::string &name, const std::list<pair<string, string>> &arguments ) :
m_id ( id ),
m_name ( name ),
m_args_c(),
m_args_s(),
m_args_spair ( arguments )
{
}

Command::Command ( const int id, const std::string &name ) :
     m_id ( id ),
     m_name ( name ),
     m_args_c(),
     m_args_s()
{
}

Command::Command() :
     m_id ( 0 ),
     m_name(),
     m_args_c(),
     m_args_s()
{
}

Command::~Command()
{
}

//std::string Command::str() const
//{
//    std::stringstream ts;
//    ts << "id = [" << m_id << "], ";
//    ts << "name = [" << m_name << "], ";
//
//    if (m_args_s.size() > 0)
//    {
//        ts << "string arguments = [";
//        for (auto it: m_args_s)
//        {
//            ts << it << " ";
//        }
//        ts << "]";
//    }
//
//    if (m_args_c.size() > 0)
//    {
//        ts << "command arguments = [";
//        for(auto it: m_args_c)
//        {
//            ts << it.str() << " ";
//        }
//        ts << "]";
//    }
//    return ts.str();
//}

std::string Command::str() const
{
     std::stringstream ts;

     //Si parte con le strutture iniziali: set, vr, e ar
     switch ( m_id ) {
     case Parser::token::TOKEN_SET_LIST_ID:
     {
          mtfa_log("StringSet: %s\n", m_name.c_str());
          for ( auto it: m_args_s ) {
               cout << "\t" << it << endl;
          }
          cout << endl;
     }
          break;
     case Parser::token::TOKEN_DICTIONARY_LIST_ID:
     {
         mtfa_log("DictionarySet: %s\n", m_name.c_str());
         for ( auto it: m_args_spair ) {
             cout << "\t" << it.first << ": " << it.second << endl;
         }
         cout << endl;
     }   
     break;
     default:
          break;
     }
     ts << "id = [" << m_id << "], ";
     ts << "name = [" << m_name << "], ";

     if ( m_args_s.size() > 0 ) {
          ts << "string arguments = [";
          for ( auto it: m_args_s ) {
               ts << it << " ";
          }
          ts << "]";
     }
     if ( m_args_spair.size() > 0 ) {
         ts << "pair string arguments = [";
         for ( auto it: m_args_spair ) {
             ts << it.first << ":" << it.second << " ";
         }
         ts << "]";
     }
     
     if ( m_args_c.size() > 0 ) {
          ts << "command arguments = [";
          for ( auto it: m_args_c ) {
               ts << it.str() << " ";
          }
          ts << "]";
     }
     return ts.str();
}

int Command::id() const
{
    return m_id;
}


std::string Command::name() const
{
     return m_name;
}

std::list<Command> Command::args_c() const
{
     return m_args_c;
}

std::list<string> Command::args_s() const
{
     return m_args_s;
}

std::list<pair<string, string>> Command::args_spair() const
{
    return m_args_spair;
}
