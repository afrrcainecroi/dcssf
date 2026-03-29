#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include <list>
#include <tre/regex.h> 

#include "mtfa_fuzzyfind.h"

using namespace std;

int mtfa_fuzzy_find(string pattern, string toSearch, int maxops)
{
	regex_t compiled;
	
	if (regcomp(&compiled, pattern.c_str(), REG_EXTENDED | REG_ICASE | REG_NEWLINE))
		return -1;
	
	regaparams_t params = { 0 };
	params.cost_ins = 1;
	params.cost_del = 1;
	params.cost_subst = 1;
	params.max_cost = maxops; //2;
	params.max_del = maxops; //2;
	params.max_ins = maxops; //2;
	params.max_subst = maxops; //2;
	params.max_err = maxops; //2;
	
	regamatch_t match;
	match.nmatch = 0;
	match.pmatch = 0;
	
	if (regaexec(&compiled, toSearch.c_str(), &match, params, 0))
	{
		regfree(&compiled);
		return -2;
	}
	
	regfree(&compiled);
	return match.cost;
}
