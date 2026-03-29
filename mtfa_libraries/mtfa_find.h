#pragma once

#include <iostream>
#include <sstream>
#include <iterator>
#include <list>
#include <string>
//#include <pcrecpp.h>

#if 1
#define PCRE2_CODE_UNIT_WIDTH 8

#include <pcre2.h>

using namespace std;


pcre2_code * mtfa_compile_pattern ( const string & pattern, bool usejit=true );
void mtfa_free_pattern ( pcre2_code * re );


bool mtfa_find_pattern ( const string & data, const string & pattern );
bool mtfa_find_pattern ( const string & data, pcre2_code * pattern );

bool mtfa_get_specific_submatching_pattern ( const string & data, const string & pattern, int subp, string & result );
bool mtfa_get_specific_submatching_pattern ( const string & data, pcre2_code * pattern, int subp, string & result );

std::list<string> mtfa_get_all_submatching_patterns ( const string & data, const string & pattern );
std::list<string> mtfa_get_all_submatching_patterns ( const string & data, pcre2_code * pattern );

string mtfa_replace_matching_pattern ( const string & data, const string & pattern, const string & replace, bool all );
string mtfa_replace_matching_pattern ( const string &data, pcre2_code * pattern, const string &replace, bool all );

std::list<pair<int, int>> mtfa_get_all_submatching_positions ( const string & data, const string & pattern );
std::list<pair<int, int>> mtfa_get_all_submatching_positions ( const string & data, pcre2_code * pattern );

std::list<pair<int, int>> mtfa_get_all_matching_positions( const string& subject, const string& pattern );
std::list<pair<int, int>> mtfa_get_all_matching_positions( const string& subject, const pcre2_code * pattern );

#else
#include <hs/hs.h>
using namespace std;


hs_database_t* mtfa_compile_pattern ( const string& pattern, unsigned int flags = HS_FLAG_DOTALL );
void mtfa_free_pattern ( hs_database_t * re );


bool mtfa_find_pattern ( const string & data, const string & pattern );
bool mtfa_find_pattern ( const string & data, pcre2_code * pattern );

bool mtfa_get_specific_submatching_pattern ( const string & data, const string & pattern, int subp, string & result );
bool mtfa_get_specific_submatching_pattern ( const string & data, pcre2_code * pattern, int subp, string & result );

std::list<string> mtfa_get_all_submatching_patterns ( const string & data, const string & pattern );
std::list<string> mtfa_get_all_submatching_patterns ( const string & data, pcre2_code * pattern );

string mtfa_replace_matching_pattern ( const string & data, const string & pattern, const string & replace, bool all );
string mtfa_replace_matching_pattern ( const string &data, pcre2_code * pattern, const string &replace, bool all );

std::list<pair<int, int>> mtfa_get_all_submatching_positions ( const string & data, const string & pattern );
std::list<pair<int, int>> mtfa_get_all_submatching_positions ( const string & data, pcre2_code * pattern );

std::list<pair<int, int>> mtfa_get_all_matching_positions( const string& subject, const string& pattern );
std::list<pair<int, int>> mtfa_get_all_matching_positions( const string& subject, const pcre2_code * pattern );


#endif
