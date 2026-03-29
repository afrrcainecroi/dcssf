#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include <list>
#include "mtfa_find.h"

using namespace std;

#if 1

pcre2_code* mtfa_compile_pattern ( const string& pattern, bool usejit )
{
    pcre2_code* re;
    PCRE2_SIZE erroffset;
    int errorcode;
    int rc;
    re = pcre2_compile ( (unsigned char*)pattern.c_str(), PCRE2_ZERO_TERMINATED, 0, &errorcode, &erroffset, NULL );

    if ( re == nullptr )
    {
        PCRE2_UCHAR buffer[256];
        pcre2_get_error_message ( errorcode, buffer, sizeof ( buffer ) );
        cerr << "PCRE2 compilation failed at offset " << erroffset << ": " << buffer << endl;
        return nullptr;
    }

    //ne faccio anche il just in time!
    if ( usejit )
        pcre2_jit_compile ( re, PCRE2_JIT_COMPLETE );

    return re;
}

void mtfa_free_pattern ( pcre2_code* re )
{
    if ( re )
    {
        pcre2_code_free ( re );
    }
}

bool mtfa_find_pattern ( const string& subject, const string& pattern )
{
    pcre2_match_data* match_data = nullptr;
    int rc;

    pcre2_code* re = mtfa_compile_pattern ( pattern, false );

    if ( !re )
        return false;

    match_data = pcre2_match_data_create_from_pattern ( re, NULL );
    rc = pcre2_match (
             re,                   /* the compiled pattern */
             (unsigned char*)subject.data(),       /* the subject string */
             subject.size(),       /* the length of the subject */
             0,                    /* start at offset 0 in the subject */
             0,                    /* default options */
             match_data,           /* block for storing the result */
             NULL );               /* use default match context */

    if ( match_data )
        pcre2_match_data_free ( match_data ); /* Release memory used for the match */

    pcre2_code_free ( re );              /* data and the compiled pattern. */

    if ( rc < 0 )
        return false;
    else
        return true;
}

bool mtfa_find_pattern ( const string& subject, pcre2_code* re )
{
    pcre2_match_data* match_data = nullptr;
    int rc;

    if ( !re )
        return false;

    match_data = pcre2_match_data_create_from_pattern ( re, NULL );
    rc = pcre2_jit_match (
             re,                   /* the compiled pattern */
             (unsigned char*)subject.data(),       /* the subject string */
             subject.size(),       /* the length of the subject */
             0,                    /* start at offset 0 in the subject */
             0,                    /* default options */
             match_data,           /* block for storing the result */
             NULL );               /* use default match context */

    if ( match_data )
        pcre2_match_data_free ( match_data ); /* Release memory used for the match */

    if ( rc < 0 )
        return false;
    else
        return true;
}

bool mtfa_get_specific_submatching_pattern (
    const string&    subject,
    const string&    pattern,
    int         subp,
    string& result )
{
    int rc;
    pcre2_match_data* match_data = nullptr;
    PCRE2_SIZE *ovector = nullptr;
    pcre2_code* re = mtfa_compile_pattern ( pattern, false );

    bool bret=false;

    result.clear();

    if ( !re )
        return false;

    match_data = pcre2_match_data_create_from_pattern ( re, NULL );
    rc = pcre2_match (
             re,                   /* the compiled pattern */
             (unsigned char*)subject.data(),       /* the subject string */
             subject.size(),       /* the length of the subject */
             0,                    /* start at offset 0 in the subject */
             0,                    /* default options */
             match_data,           /* block for storing the result */
             NULL );               /* use default match context */

    pcre2_code_free ( re );              /* data and the compiled pattern. */

    if ( rc >= 0 && rc >= subp)
    {
        bret = true;
        ovector = pcre2_get_ovector_pointer(match_data);
        if (ovector[2*subp+1] == ovector[2*subp])
            result.clear();
        else
            result = subject.substr(ovector[2*subp], ovector[2*subp+1] - ovector[2*subp]);
//         for (i = 0; i < rc; i++)
//         {
//             PCRE2_SPTR substring_start = subject + ovector[2*i];
//             size_t substring_length = ovector[2*i+1] - ovector[2*i];
//             printf("%2d: %.*s\n", i, (int)substring_length, (char *)substring_start);
//         }
    }
    if ( match_data )
        pcre2_match_data_free ( match_data ); /* Release memory used for the match */
    return bret;
}

bool mtfa_get_specific_submatching_pattern ( 
        const string& subject,
        pcre2_code* re,
        int         subp,
        string& result )
{
    int rc;
    pcre2_match_data* match_data = nullptr;
    PCRE2_SIZE *ovector = nullptr;

    bool bret=false;

    result.clear();

    if ( !re )
        return false;

    match_data = pcre2_match_data_create_from_pattern ( re, NULL );
    rc = pcre2_jit_match (
             re,                   /* the compiled pattern */
             (unsigned char*)subject.data(),       /* the subject string */
             subject.size(),       /* the length of the subject */
             0,                    /* start at offset 0 in the subject */
             0,                    /* default options */
             match_data,           /* block for storing the result */
             NULL );               /* use default match context */

    if ( rc >= 0 && rc >= subp)
    {
        bret = true;
        ovector = pcre2_get_ovector_pointer(match_data);
        if (ovector[2*subp+1] == ovector[2*subp])
            result.clear();
        else
            result = subject.substr(ovector[2*subp], ovector[2*subp+1] - ovector[2*subp]);
//         for (i = 0; i < rc; i++)
//         {
//             PCRE2_SPTR substring_start = subject + ovector[2*i];
//             size_t substring_length = ovector[2*i+1] - ovector[2*i];
//             printf("%2d: %.*s\n", i, (int)substring_length, (char *)substring_start);
//         }
    }
    if ( match_data )
        pcre2_match_data_free ( match_data ); /* Release memory used for the match */
    return bret;
}

std::list<string> mtfa_get_all_submatching_patterns ( const string& subject, const string& pattern )
{
    int rc;
    pcre2_match_data* match_data = nullptr;
    PCRE2_SIZE *ovector = nullptr;
    pcre2_code* re = mtfa_compile_pattern ( pattern, false );

    bool bret=false;

    std::list<string> result;

    if ( !re )
        return result;

    match_data = pcre2_match_data_create_from_pattern ( re, NULL );
    rc = pcre2_match (
             re,                   /* the compiled pattern */
             (unsigned char*)subject.data(),       /* the subject string */
             subject.size(),       /* the length of the subject */
             0,                    /* start at offset 0 in the subject */
             0,                    /* default options */
             match_data,           /* block for storing the result */
             NULL );               /* use default match context */

    pcre2_code_free ( re );              /* data and the compiled pattern. */

    if ( rc >= 0 )
    {
        bret = true;
        ovector = pcre2_get_ovector_pointer(match_data);
        for (int i = 0; i < rc; i++)
        {
            if (ovector[2*i+1] > ovector[2*i])
                result.push_back(subject.substr(ovector[2*i], ovector[2*i+1] - ovector[2*i]));
        }
    }
    if ( match_data )
        pcre2_match_data_free ( match_data ); /* Release memory used for the match */

    return result;
}

std::list<string> mtfa_get_all_submatching_patterns ( const string& subject, pcre2_code* re )
{
    int rc;
    pcre2_match_data* match_data = nullptr;
    PCRE2_SIZE *ovector = nullptr;

    bool bret=false;

    std::list<string> result;

    if ( !re )
        return result;

    match_data = pcre2_match_data_create_from_pattern ( re, NULL );
    rc = pcre2_jit_match (
             re,                   /* the compiled pattern */
             (unsigned char*)subject.data(),       /* the subject string */
             subject.size(),       /* the length of the subject */
             0,                    /* start at offset 0 in the subject */
             0,                    /* default options */
             match_data,           /* block for storing the result */
             NULL );               /* use default match context */

    if ( rc >= 0 )
    {
        bret = true;
        ovector = pcre2_get_ovector_pointer(match_data);
        for (int i = 0; i < rc; i++)
        {
            if (ovector[2*i+1] > ovector[2*i])
                result.push_back(subject.substr(ovector[2*i], ovector[2*i+1] - ovector[2*i]));
        }
    }
    if ( match_data )
        pcre2_match_data_free ( match_data ); /* Release memory used for the match */

    return result;
}

string mtfa_replace_matching_pattern ( const string& subject,
                                       const string& pattern,
                                       const string& replace,
                                       bool      all )
{
    int rc;
    PCRE2_SIZE *ovector = nullptr;
    pcre2_code* re = mtfa_compile_pattern ( pattern, false );

    string result;

    if ( !re )
        return result;

    
    PCRE2_SIZE outlength=0;
    PCRE2_UCHAR* outputbuffer=nullptr;
    int tries = 2;

    while (true)
    {
        outlength = subject.size()*tries;
        outputbuffer = (PCRE2_UCHAR*)malloc(outlength);
        rc = pcre2_substitute(
            re,
            (unsigned char*)subject.data(),
            subject.size(),
            0, 
			(all?PCRE2_SUBSTITUTE_GLOBAL:0)|PCRE2_SUBSTITUTE_EXTENDED, 
            nullptr, 
            nullptr, 
            (unsigned char*)replace.data(), 
            replace.size(), 
            outputbuffer, 
            &outlength
        );
        if (PCRE2_ERROR_NOMEMORY == rc)
        {
            tries++;
            free(outputbuffer);
            continue;
        }
        else
            break;
    }
    
    pcre2_code_free ( re );              /* data and the compiled pattern. */

    result.assign(outputbuffer, outputbuffer+outlength);
    free(outputbuffer);
    
    return result;
}

string mtfa_replace_matching_pattern ( const string& subject,
                                       pcre2_code* re,
                                       const string& replace,
                                       bool      all )
{
    int rc;
    PCRE2_SIZE *ovector = nullptr;

    string result;

    if ( !re )
        return result;

    
    PCRE2_SIZE outlength=0;
    PCRE2_UCHAR* outputbuffer=nullptr;
    int tries = 2;

    while (true)
    {
        outlength = subject.size()*tries;
        outputbuffer = (PCRE2_UCHAR*)malloc(outlength);
        rc = pcre2_substitute(
            re,
            (unsigned char*)subject.data(),
            subject.size(),
            0, 
			(all?PCRE2_SUBSTITUTE_GLOBAL:0)|PCRE2_SUBSTITUTE_EXTENDED, 
			nullptr, 
            nullptr, 
            (unsigned char*)replace.data(), 
            replace.size(), 
            outputbuffer, 
            &outlength
        );
        if (PCRE2_ERROR_NOMEMORY == rc)
        {
            tries++;
            free(outputbuffer);
            continue;
        }
        else
            break;
    }
    
    if (outputbuffer)
    {
        result.assign(outputbuffer, outputbuffer+outlength);
        free(outputbuffer);
    }
    
    return result;
}

std::list<pair<int, int>> mtfa_get_all_submatching_positions ( const string& subject, const string& pattern )
{
    int rc;
    pcre2_match_data* match_data = nullptr;
    PCRE2_SIZE *ovector = nullptr;

    pcre2_code* re = mtfa_compile_pattern ( pattern, false );

    bool bret=false;

    std::list<pair<int, int>> result;

    if ( !re )
        return result;

    match_data = pcre2_match_data_create_from_pattern ( re, NULL );
    rc = pcre2_match (
             re,                   /* the compiled pattern */
             (unsigned char*)subject.data(),       /* the subject string */
             subject.size(),       /* the length of the subject */
             0,                    /* start at offset 0 in the subject */
             0,                    /* default options */
             match_data,           /* block for storing the result */
             NULL );               /* use default match context */
    pcre2_code_free ( re );              /* data and the compiled pattern. */

    if ( rc >= 0 )
    {
        bret = true;
        ovector = pcre2_get_ovector_pointer(match_data);
        for (int i = 0; i < rc; i++)
        {
            if (ovector[2*i+1] > ovector[2*i])
            {
                result.push_back(make_pair(ovector[2*i], ovector[2*i+1]));
            }
        }
    }
    if ( match_data )
        pcre2_match_data_free ( match_data ); /* Release memory used for the match */

    return result;
}

std::list<pair<int, int>> mtfa_get_all_submatching_positions ( const string& subject, pcre2_code* re )
{
    int rc;
    pcre2_match_data* match_data = nullptr;
    PCRE2_SIZE *ovector = nullptr;

    bool bret=false;

    std::list<pair<int, int>> result;

    if ( !re )
        return result;

    match_data = pcre2_match_data_create_from_pattern ( re, NULL );
    rc = pcre2_jit_match (
             re,                   /* the compiled pattern */
             (unsigned char*)subject.data(),       /* the subject string */
             subject.size(),       /* the length of the subject */
             0,                    /* start at offset 0 in the subject */
             0,                    /* default options */
             match_data,           /* block for storing the result */
             NULL );               /* use default match context */

    if ( rc >= 0 )
    {
        bret = true;
        ovector = pcre2_get_ovector_pointer(match_data);
        for (int i = 0; i < rc; i++)
        {
            if (ovector[2*i+1] > ovector[2*i])
                result.push_back(make_pair(ovector[2*i], ovector[2*i+1]));
        }
    }
    if ( match_data )
        pcre2_match_data_free ( match_data ); /* Release memory used for the match */

    return result;
}



static int callout_handler(pcre2_callout_block *c, void *data) {
	++*((int*)data);  //solo per contare
	return 0;
}

std::list<pair<int, int>> mtfa_get_all_matching_positions( const string& subject, const string& pattern )
{
	pcre2_code *re;
	int error;
	PCRE2_SIZE erroffset;
	pcre2_match_context *match_context;
	pcre2_match_data *match_data;
	int rc, startOffset = 0;
	int calloutCount = 0;
	PCRE2_SIZE *ovector;

	std::list<pair<int, int>> result;
	
	re = mtfa_compile_pattern ( pattern, false );
	if (re == 0)
		return result;
	
	match_context = pcre2_match_context_create(0);
	pcre2_set_callout(match_context, callout_handler, &calloutCount);
	
	match_data = pcre2_match_data_create_from_pattern(re, 0);
	ovector = pcre2_get_ovector_pointer(match_data);
	
	startOffset = 0;
	for (;;) {
		rc = pcre2_match(re, (unsigned char*)subject.c_str(), PCRE2_ZERO_TERMINATED, startOffset, 0, match_data, match_context);
		if (rc < 0)
			break;
		startOffset = ovector[1];
		
		for (int i = 0; i < rc; i++)
		{
			if (ovector[2*i+1] > ovector[2*i])
				result.push_back(make_pair(ovector[2*i], ovector[2*i+1] - ovector[2*i]));
		}
	}
	
	pcre2_match_context_free(match_context);
	pcre2_match_data_free(match_data);
	pcre2_code_free(re);
	
	return result;
}

std::list<pair<int, int>> mtfa_get_all_matching_positions( const string& subject, const pcre2_code * re )
{
	int error;
	PCRE2_SIZE erroffset;
	pcre2_match_context *match_context;
	pcre2_match_data *match_data;
	int rc, startOffset = 0;
	int calloutCount = 0;
	PCRE2_SIZE *ovector;

	std::list<pair<int, int>> result;
	
	match_context = pcre2_match_context_create(0);
	pcre2_set_callout(match_context, callout_handler, &calloutCount);
	
	match_data = pcre2_match_data_create_from_pattern(re, 0);
	ovector = pcre2_get_ovector_pointer(match_data);
	
	startOffset = 0;
	for (;;) {
		rc = pcre2_match(re, (unsigned char*)subject.c_str(), PCRE2_ZERO_TERMINATED, startOffset, 0, match_data, match_context);
		if (rc < 0)
			break;
		startOffset = ovector[1];
		
		for (int i = 0; i < rc; i++)
		{
			if (ovector[2*i+1] > ovector[2*i])
				result.push_back(make_pair(ovector[2*i], ovector[2*i+1] - ovector[2*i]));
		}
	}
	
	pcre2_match_context_free(match_context);
	pcre2_match_data_free(match_data);
	
	return result;
}

// int main(int argc, char **argv) {
// 	char version[256];
// 	pcre2_config(PCRE2_CONFIG_VERSION, &version);
// 	printf("PCRE version: %s\n\n", version);
// 	
// 	test("(Stake: £)(\\d+(?:\\.\\d+)?)");
// 	test("(winnings: £)(\\d+(?:\\.\\d+)?)");
// 	
// 	return 0;
// }

#else

hs_database_t* mtfa_compile_pattern ( const string& pattern, unsigned int flags )
{
    hs_database_t* db = nullptr;
    hs_compile_error_t* compile_err = nullptr;

    // Hyperscan compila sempre alla massima velocità possibile per l'architettura (es. Sapphire Rapids)
    // HS_MODE_BLOCK è l'equivalente della ricerca standard su stringa singola
    hs_error_t err = hs_compile(
        pattern.c_str(),       // Il pattern
                                flags,                 // Flag (es. HS_FLAG_CASELESS, HS_FLAG_DOTALL)
    HS_MODE_BLOCK,         // Modalità: Block, Stream o Vectored
    nullptr,               // Platform info (nullptr usa la CPU corrente)
    &db,                   // Il database risultante
    &compile_err           // Eventuali errori
    );

    if (err != HS_SUCCESS)
    {
        if (compile_err) {
            cerr << "Hyperscan compilation failed: " << compile_err->message << endl;
            // Se l'errore riguarda un pattern specifico (nella versione multi), l'offset è in compile_err->expression
            hs_free_compile_error(compile_err);
        } else {
            cerr << "Hyperscan compilation failed with error code: " << err << endl;
        }
        return nullptr;
    }

    return db;
}

void mtfa_free_pattern ( hs_database_t* re )
{
    if ( re )
    {
        hs_free_database ( re );
    }
}

bool mtfa_find_pattern ( const string& subject, const string& pattern )
{
    pcre2_match_data* match_data = nullptr;
    int rc;

    pcre2_code* re = mtfa_compile_pattern ( pattern, false );

    if ( !re )
        return false;

    match_data = pcre2_match_data_create_from_pattern ( re, NULL );
    rc = pcre2_match (
        re,                   /* the compiled pattern */
        (unsigned char*)subject.data(),       /* the subject string */
                      subject.size(),       /* the length of the subject */
                      0,                    /* start at offset 0 in the subject */
                      0,                    /* default options */
                      match_data,           /* block for storing the result */
                      NULL );               /* use default match context */

    if ( match_data )
        pcre2_match_data_free ( match_data ); /* Release memory used for the match */

        pcre2_code_free ( re );              /* data and the compiled pattern. */

        if ( rc < 0 )
            return false;
    else
        return true;
}

bool mtfa_find_pattern ( const string& subject, pcre2_code* re )
{
    pcre2_match_data* match_data = nullptr;
    int rc;

    if ( !re )
        return false;

    match_data = pcre2_match_data_create_from_pattern ( re, NULL );
    rc = pcre2_jit_match (
        re,                   /* the compiled pattern */
        (unsigned char*)subject.data(),       /* the subject string */
                          subject.size(),       /* the length of the subject */
                          0,                    /* start at offset 0 in the subject */
                          0,                    /* default options */
                          match_data,           /* block for storing the result */
                          NULL );               /* use default match context */

    if ( match_data )
        pcre2_match_data_free ( match_data ); /* Release memory used for the match */

        if ( rc < 0 )
            return false;
    else
        return true;
}

bool mtfa_get_specific_submatching_pattern (
    const string&    subject,
    const string&    pattern,
    int         subp,
    string& result )
{
    int rc;
    pcre2_match_data* match_data = nullptr;
    PCRE2_SIZE *ovector = nullptr;
    pcre2_code* re = mtfa_compile_pattern ( pattern, false );

    bool bret=false;

    result.clear();

    if ( !re )
        return false;

    match_data = pcre2_match_data_create_from_pattern ( re, NULL );
    rc = pcre2_match (
        re,                   /* the compiled pattern */
        (unsigned char*)subject.data(),       /* the subject string */
                      subject.size(),       /* the length of the subject */
                      0,                    /* start at offset 0 in the subject */
                      0,                    /* default options */
                      match_data,           /* block for storing the result */
                      NULL );               /* use default match context */

    pcre2_code_free ( re );              /* data and the compiled pattern. */

    if ( rc >= 0 && rc >= subp)
    {
        bret = true;
        ovector = pcre2_get_ovector_pointer(match_data);
        if (ovector[2*subp+1] == ovector[2*subp])
            result.clear();
        else
            result = subject.substr(ovector[2*subp], ovector[2*subp+1] - ovector[2*subp]);
        //         for (i = 0; i < rc; i++)
        //         {
        //             PCRE2_SPTR substring_start = subject + ovector[2*i];
        //             size_t substring_length = ovector[2*i+1] - ovector[2*i];
        //             printf("%2d: %.*s\n", i, (int)substring_length, (char *)substring_start);
        //         }
    }
    if ( match_data )
        pcre2_match_data_free ( match_data ); /* Release memory used for the match */
        return bret;
}

bool mtfa_get_specific_submatching_pattern (
const string& subject,
pcre2_code* re,
int         subp,
string& result )
{
    int rc;
    pcre2_match_data* match_data = nullptr;
    PCRE2_SIZE *ovector = nullptr;

    bool bret=false;

    result.clear();

    if ( !re )
        return false;

    match_data = pcre2_match_data_create_from_pattern ( re, NULL );
    rc = pcre2_jit_match (
        re,                   /* the compiled pattern */
        (unsigned char*)subject.data(),       /* the subject string */
                          subject.size(),       /* the length of the subject */
                          0,                    /* start at offset 0 in the subject */
                          0,                    /* default options */
                          match_data,           /* block for storing the result */
                          NULL );               /* use default match context */

    if ( rc >= 0 && rc >= subp)
    {
        bret = true;
        ovector = pcre2_get_ovector_pointer(match_data);
        if (ovector[2*subp+1] == ovector[2*subp])
            result.clear();
        else
            result = subject.substr(ovector[2*subp], ovector[2*subp+1] - ovector[2*subp]);
        //         for (i = 0; i < rc; i++)
        //         {
        //             PCRE2_SPTR substring_start = subject + ovector[2*i];
        //             size_t substring_length = ovector[2*i+1] - ovector[2*i];
        //             printf("%2d: %.*s\n", i, (int)substring_length, (char *)substring_start);
        //         }
    }
    if ( match_data )
        pcre2_match_data_free ( match_data ); /* Release memory used for the match */
        return bret;
}

std::list<string> mtfa_get_all_submatching_patterns ( const string& subject, const string& pattern )
{
    int rc;
    pcre2_match_data* match_data = nullptr;
    PCRE2_SIZE *ovector = nullptr;
    pcre2_code* re = mtfa_compile_pattern ( pattern, false );

    bool bret=false;

    std::list<string> result;

    if ( !re )
        return result;

    match_data = pcre2_match_data_create_from_pattern ( re, NULL );
    rc = pcre2_match (
        re,                   /* the compiled pattern */
        (unsigned char*)subject.data(),       /* the subject string */
                      subject.size(),       /* the length of the subject */
                      0,                    /* start at offset 0 in the subject */
                      0,                    /* default options */
                      match_data,           /* block for storing the result */
                      NULL );               /* use default match context */

    pcre2_code_free ( re );              /* data and the compiled pattern. */

    if ( rc >= 0 )
    {
        bret = true;
        ovector = pcre2_get_ovector_pointer(match_data);
        for (int i = 0; i < rc; i++)
        {
            if (ovector[2*i+1] > ovector[2*i])
                result.push_back(subject.substr(ovector[2*i], ovector[2*i+1] - ovector[2*i]));
        }
    }
    if ( match_data )
        pcre2_match_data_free ( match_data ); /* Release memory used for the match */

        return result;
}

std::list<string> mtfa_get_all_submatching_patterns ( const string& subject, pcre2_code* re )
{
    int rc;
    pcre2_match_data* match_data = nullptr;
    PCRE2_SIZE *ovector = nullptr;

    bool bret=false;

    std::list<string> result;

    if ( !re )
        return result;

    match_data = pcre2_match_data_create_from_pattern ( re, NULL );
    rc = pcre2_jit_match (
        re,                   /* the compiled pattern */
        (unsigned char*)subject.data(),       /* the subject string */
                          subject.size(),       /* the length of the subject */
                          0,                    /* start at offset 0 in the subject */
                          0,                    /* default options */
                          match_data,           /* block for storing the result */
                          NULL );               /* use default match context */

    if ( rc >= 0 )
    {
        bret = true;
        ovector = pcre2_get_ovector_pointer(match_data);
        for (int i = 0; i < rc; i++)
        {
            if (ovector[2*i+1] > ovector[2*i])
                result.push_back(subject.substr(ovector[2*i], ovector[2*i+1] - ovector[2*i]));
        }
    }
    if ( match_data )
        pcre2_match_data_free ( match_data ); /* Release memory used for the match */

        return result;
}

string mtfa_replace_matching_pattern ( const string& subject,
                                       const string& pattern,
                                       const string& replace,
                                       bool      all )
{
    int rc;
    PCRE2_SIZE *ovector = nullptr;
    pcre2_code* re = mtfa_compile_pattern ( pattern, false );

    string result;

    if ( !re )
        return result;


    PCRE2_SIZE outlength=0;
    PCRE2_UCHAR* outputbuffer=nullptr;
    int tries = 2;

    while (true)
    {
        outlength = subject.size()*tries;
        outputbuffer = (PCRE2_UCHAR*)malloc(outlength);
        rc = pcre2_substitute(
            re,
            (unsigned char*)subject.data(),
                              subject.size(),
                              0,
                              (all?PCRE2_SUBSTITUTE_GLOBAL:0)|PCRE2_SUBSTITUTE_EXTENDED,
                              nullptr,
                              nullptr,
                              (unsigned char*)replace.data(),
                              replace.size(),
                              outputbuffer,
                              &outlength
        );
        if (PCRE2_ERROR_NOMEMORY == rc)
        {
            tries++;
            free(outputbuffer);
            continue;
        }
        else
            break;
    }

    pcre2_code_free ( re );              /* data and the compiled pattern. */

    result.assign(outputbuffer, outputbuffer+outlength);
    free(outputbuffer);

    return result;
}

string mtfa_replace_matching_pattern ( const string& subject,
                                       pcre2_code* re,
                                       const string& replace,
                                       bool      all )
{
    int rc;
    PCRE2_SIZE *ovector = nullptr;

    string result;

    if ( !re )
        return result;


    PCRE2_SIZE outlength=0;
    PCRE2_UCHAR* outputbuffer=nullptr;
    int tries = 2;

    while (true)
    {
        outlength = subject.size()*tries;
        outputbuffer = (PCRE2_UCHAR*)malloc(outlength);
        rc = pcre2_substitute(
            re,
            (unsigned char*)subject.data(),
                              subject.size(),
                              0,
                              (all?PCRE2_SUBSTITUTE_GLOBAL:0)|PCRE2_SUBSTITUTE_EXTENDED,
                              nullptr,
                              nullptr,
                              (unsigned char*)replace.data(),
                              replace.size(),
                              outputbuffer,
                              &outlength
        );
        if (PCRE2_ERROR_NOMEMORY == rc)
        {
            tries++;
            free(outputbuffer);
            continue;
        }
        else
            break;
    }

    if (outputbuffer)
    {
        result.assign(outputbuffer, outputbuffer+outlength);
        free(outputbuffer);
    }

    return result;
}

std::list<pair<int, int>> mtfa_get_all_submatching_positions ( const string& subject, const string& pattern )
{
    int rc;
    pcre2_match_data* match_data = nullptr;
    PCRE2_SIZE *ovector = nullptr;

    pcre2_code* re = mtfa_compile_pattern ( pattern, false );

    bool bret=false;

    std::list<pair<int, int>> result;

    if ( !re )
        return result;

    match_data = pcre2_match_data_create_from_pattern ( re, NULL );
    rc = pcre2_match (
        re,                   /* the compiled pattern */
        (unsigned char*)subject.data(),       /* the subject string */
                      subject.size(),       /* the length of the subject */
                      0,                    /* start at offset 0 in the subject */
                      0,                    /* default options */
                      match_data,           /* block for storing the result */
                      NULL );               /* use default match context */
    pcre2_code_free ( re );              /* data and the compiled pattern. */

    if ( rc >= 0 )
    {
        bret = true;
        ovector = pcre2_get_ovector_pointer(match_data);
        for (int i = 0; i < rc; i++)
        {
            if (ovector[2*i+1] > ovector[2*i])
            {
                result.push_back(make_pair(ovector[2*i], ovector[2*i+1]));
            }
        }
    }
    if ( match_data )
        pcre2_match_data_free ( match_data ); /* Release memory used for the match */

        return result;
}

std::list<pair<int, int>> mtfa_get_all_submatching_positions ( const string& subject, pcre2_code* re )
{
    int rc;
    pcre2_match_data* match_data = nullptr;
    PCRE2_SIZE *ovector = nullptr;

    bool bret=false;

    std::list<pair<int, int>> result;

    if ( !re )
        return result;

    match_data = pcre2_match_data_create_from_pattern ( re, NULL );
    rc = pcre2_jit_match (
        re,                   /* the compiled pattern */
        (unsigned char*)subject.data(),       /* the subject string */
                          subject.size(),       /* the length of the subject */
                          0,                    /* start at offset 0 in the subject */
                          0,                    /* default options */
                          match_data,           /* block for storing the result */
                          NULL );               /* use default match context */

    if ( rc >= 0 )
    {
        bret = true;
        ovector = pcre2_get_ovector_pointer(match_data);
        for (int i = 0; i < rc; i++)
        {
            if (ovector[2*i+1] > ovector[2*i])
                result.push_back(make_pair(ovector[2*i], ovector[2*i+1]));
        }
    }
    if ( match_data )
        pcre2_match_data_free ( match_data ); /* Release memory used for the match */

        return result;
}



static int callout_handler(pcre2_callout_block *c, void *data) {
    ++*((int*)data);  //solo per contare
    return 0;
}

std::list<pair<int, int>> mtfa_get_all_matching_positions( const string& subject, const string& pattern )
{
    pcre2_code *re;
    int error;
    PCRE2_SIZE erroffset;
    pcre2_match_context *match_context;
    pcre2_match_data *match_data;
    int rc, startOffset = 0;
    int calloutCount = 0;
    PCRE2_SIZE *ovector;

    std::list<pair<int, int>> result;

    re = mtfa_compile_pattern ( pattern, false );
    if (re == 0)
        return result;

    match_context = pcre2_match_context_create(0);
    pcre2_set_callout(match_context, callout_handler, &calloutCount);

    match_data = pcre2_match_data_create_from_pattern(re, 0);
    ovector = pcre2_get_ovector_pointer(match_data);

    startOffset = 0;
    for (;;) {
        rc = pcre2_match(re, (unsigned char*)subject.c_str(), PCRE2_ZERO_TERMINATED, startOffset, 0, match_data, match_context);
        if (rc < 0)
            break;
        startOffset = ovector[1];

        for (int i = 0; i < rc; i++)
        {
            if (ovector[2*i+1] > ovector[2*i])
                result.push_back(make_pair(ovector[2*i], ovector[2*i+1] - ovector[2*i]));
        }
    }

    pcre2_match_context_free(match_context);
    pcre2_match_data_free(match_data);
    pcre2_code_free(re);

    return result;
}

std::list<pair<int, int>> mtfa_get_all_matching_positions( const string& subject, const pcre2_code * re )
{
    int error;
    PCRE2_SIZE erroffset;
    pcre2_match_context *match_context;
    pcre2_match_data *match_data;
    int rc, startOffset = 0;
    int calloutCount = 0;
    PCRE2_SIZE *ovector;

    std::list<pair<int, int>> result;

    match_context = pcre2_match_context_create(0);
    pcre2_set_callout(match_context, callout_handler, &calloutCount);

    match_data = pcre2_match_data_create_from_pattern(re, 0);
    ovector = pcre2_get_ovector_pointer(match_data);

    startOffset = 0;
    for (;;) {
        rc = pcre2_match(re, (unsigned char*)subject.c_str(), PCRE2_ZERO_TERMINATED, startOffset, 0, match_data, match_context);
        if (rc < 0)
            break;
        startOffset = ovector[1];

        for (int i = 0; i < rc; i++)
        {
            if (ovector[2*i+1] > ovector[2*i])
                result.push_back(make_pair(ovector[2*i], ovector[2*i+1] - ovector[2*i]));
        }
    }

    pcre2_match_context_free(match_context);
    pcre2_match_data_free(match_data);

    return result;
}

// int main(int argc, char **argv) {
// 	char version[256];
// 	pcre2_config(PCRE2_CONFIG_VERSION, &version);
// 	printf("PCRE version: %s\n\n", version);
//
// 	test("(Stake: £)(\\d+(?:\\.\\d+)?)");
// 	test("(winnings: £)(\\d+(?:\\.\\d+)?)");
//
// 	return 0;
// }


#endif
