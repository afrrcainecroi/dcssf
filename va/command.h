#pragma once
#include <string>
#include <list>
#include <vector>
#include <stdint.h>

using namespace std;

namespace VaParser
{

/**
 * AST node. If you can call it AST at all...
 * It keeps function name and a list of arguments.
 */
class Command
{
public:
    Command ( const int id, const std::string &name, const std::list<Command> &arguments );
    Command ( const int id, const std::string &name, const std::list<string> &arguments );
    Command ( const int id, const std::string &name, const std::list<pair<string, string>> &arguments );
    Command ( const int id, const std::string &name );
    Command();
    ~Command();

    std::string str() const;
    void addArg ( string );
    void addArg ( Command );

    std::string name() const;
    int id() const;
    std::list<Command> args_c() const;
    std::list<string> args_s() const;
    std::list<pair<string, string>> args_spair() const;
    

    int m_id;
    std::string m_name;
    std::list<Command> m_args_c;
    std::list<string> m_args_s;
    std::list<pair<string, string>> m_args_spair;
};


}
