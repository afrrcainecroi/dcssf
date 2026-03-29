#pragma once

#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>

#include <iostream>
#include <fstream>

//il vr_language
#include "va_scanner.h"
#include "va_parser.hpp"
#include "interpreter.h"
#include "command.h"
#include "VAengine.h"

#include <mtfafs.h>
#include <mtfa_fs3.h>
#include <mtfa_utils.h>
#include <mtfa_db.h>

//Per convertire i token in stringhe
string totok ( int i );
string tabs ( int i );

void Stampa ( int l, const Command & cmd );
VA_ItemTypes VRCvt ( int t ) ;

void ParseSetOfStrings ( VAengine & vaeng, const string & name, const list<string> &args_s );
void ParseSetOfIpStar ( VAengine & vaeng, const string & name, const list<string> &args_s );
void ParseSetOfMySql ( VAengine & vaeng, const string & name, const list<string> &args_s );
void ParseSetOfTimings ( VAengine & vaeng, const string & name, const list<string> &args_s );
void ParseSetOfUrlStar ( VAengine & vaeng, const string & name, const list<string> &args_s );
list<VAcriteria> ParseTOKEN_VR_DEF_IS_LISTA_WHEN ( VAengine & vaeng, const Command &cmd );
list<VAwith> ParseTOKEN_VR_DEF_IS_LISTA_WITH ( VAengine & vaeng, const Command &cmd );
VA_NodeTree * ParseTOKEN_VR_DEF_CLAUSES ( VAengine & vaeng, const Command & cmd );
VA_NodeTree * ParseTOKEN_VR_DEF ( VAengine & vaeng, const Command &cmd );
vector<VAcriteria> ParseTOKEN_BASIC_ACCESS_RULES ( VAengine & vaeng, const Command &cmd );
vector<VAcriteria> ParseTOKEN_EXTENDED_ACCESS_RULES ( VAengine & vaeng, const Command &cmd );
pair<vector<VAcriteria>, vector<VAcriteria>>  ParseTOKEN_IF ( VAengine & vaeng, const Command &cmd );
vector<KAMaction> ParseTOKEN_ACCESS_OPERATIONS ( VAengine & vaeng, const Command &cmd );
AccessRule ParseTOKEN_AR_DEF ( VAengine & vaeng, const Command &cmd );
void ParseCommand ( VAengine & vaeng, const Command & cmd );
void AstParse ( VAengine & vaeng, Interpreter & interpreter );
bool DoCompileRules ( string, string, VAengine & );



