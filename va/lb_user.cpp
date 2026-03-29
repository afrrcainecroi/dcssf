#include "VA_utils.h"
//#define DUMP_ALL 1
#include <iostream>
#include <sstream>
//bool FRANCO_CHECK = true; //true: i check di franco sono attivi

//#define NEW_RELEASE_4 1

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <hs/hs.h>
#include <cstring>
#include <fstream>
#include <errno.h>
#include <sys/stat.h>


#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/un.h>
#endif

#include <openssl/ssl.h>
#include <openssl/err.h>

#include <event2/bufferevent_ssl.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/dns.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/thread.h>

#include <string>
#include <map>

#include <list>
#include <set>
#include <vector>
#include <algorithm>
#include <tuple>
#include <unordered_set>
#include <unordered_map>
#include <mutex>
//#include <pcrecpp.h>

#include <ConfigFile.h>
#include <ULL_Rng.h>
#include <mtfa_threadpool.h>
#include <mtfa_utils.h>
#include <c_http_parser.h>
#include <base64.h>
#include <mtfa_find.h>
#include <mtfa_db.h>
#include <mtfa_fs3.h>
#include <json.hpp>
#include "listaeventi.h"

#include "network.h"
#include "mtfa_ph.h"
#include "crc.h"

#include "libguile.h"
#include <thread>

#include "version.h"

#include "parserds.h"

#include "lb_user.h"
#include "rulemgr.h"
#include "VA_utils.h"

#include "internalsonda.h"
#include <netinet/tcp.h>
#include <magic.h>

#include <json.hpp>
using json = nlohmann::json;


#ifdef MAX_OUTPUT
#undef MAX_OUTPUT
#endif

//#define MAX_OUTPUT 16000000
#define MAX_OUTPUT 16000000

using namespace std;
using json = nlohmann::json;
list<pair<string, string>> GetkeysFromJson(string sjson); //Dichiarata in internalsonda.cpp

std::mutex mutexSetEventoStructAllocati;
std::unordered_set<void*> SetEventoStructAllocati;

extern int number_of_arguments;

/*********************************************************/
/*********************************************************/
/*************INIZIO  SEZIONE DEDICATA AL KAM*************/
/*********************************************************/
/*********************************************************/

/*********************************************************/
/*********************************************************/
/*************FINE SEZIONE  DEDICATA  AL  KAM*************/
/*********************************************************/
/*********************************************************/

//unordered_map<unsigned long long, pair<unsigned long long, unsigned long long>> aperti_chiusi;

string LbUser_OutIp;

string LbUser_Client_PubKey;
string LbUser_Client_PrivKey;
string LbUser_MyAgentKey;
string LbUser_MyAgentId;
string LbUser_VA_DB_Addr;
string LbUser_VA_DB_Port;
string LbUser_VA_DB_Name;
string LbUser_VA_DB_User;
string LbUser_VA_DB_Pass;

string LbUser_VA_VRules;
string LbUser_VA_VReports;
bool LbUser_VA_GetAllHTTP;      //To be used with software agent. Hardware agent is fixed.
string LbUser_VA_QueryFilters;  //struttura: host key. Elimina dalle osservazioni i campi key nella parte query
unordered_map<string, list<string>> VA_remove_from_query;


string LbUser_Initialize;       //IP e porta sulla quale inviare una richiesta (in chiaro) per gestire l'inizializzazione 
                                //delle strutture di connessione alle VA sorelle e alle VA madri
                                
string LbUser_SecureVa;         //ip:port:url

string LbUser_SVA_Listen;       //La superva con i suoi pochi ma necessari comandi.
                                //Tutti si collegano a lei in modalità https mutua autenticazione
                                //ip e porta vanno messi anche in LbUser_VSSLPort insieme ai certificati...
                                
WhoAmI LbUser_Id_WhoAmI;        //VA,KAM,IDM
string LbUser_Hy_VA_Sisters;    //le sorelle di questa VA IP:PORT;...
string LbUser_Hy_KAM_Children;  //I KAM figli di questa VA IP:PORT;...

unsigned short int LbUser_VA_KAM_CommandPort;
string LbUser_VA_KAM_CommandIp;
string LbUser_FromVACommands;   //IP:PORT in SSL

unsigned short int LbUser_VA_HwAgentPort;
string LbUser_VA_HwAgentIp;
unsigned short int LbUser_VA_SwAgentPort;
string LbUser_VA_SwAgentIp;

// //per la sonda hardware interna
// //"/volume1/sources/NEW_DEVS/DEPLOYED/GENERAL/sonda_linux \"any\"  0 \"127.0.0.1\" 50000 \"/va/askfor\" \"portrange 8000-9000\"");
// string LbUser_VA_LocalSH_path;
// string LbUser_VA_LocalSH_attach;
// string LbUser_VA_LocalSH_promiscous;
// string LbUser_VA_LocalSH_VA_IP;
// string LbUser_VA_LocalSH_VA_PORT;
// string LbUser_VA_LocalSH_pcapfilter;
bool LbUser_VA_LocalSH_on;
string LbUser_VA_LocalSH_HTTPMethods;

string LbUser_VA_AskForUrl;
string LbUser_VA_TraceUrl;
string LbUser_VA_TraceDomain;
string LbUser_VA_TraceQuery;
string LbUser_VA_mtfa_rnd;

list<string> AS_Rules;

//string LbUser_PassTroughIp;
//string LbUser_PassTroughPort;

bool LbUser_AsDaemon;   //se true allora si lancia come daemon
string LbUser_VAObservedApps;

string LbUser_VListenIpPort;
string LbUser_VSSLPort; //per le porte SSL
string LbUser_VTCPPort; //per le porte TCP

int LbUser_HowManyInstances; //per mettere in listen più thread

set<string> IpsAndPortsToListen;

string LbUser_ListenIp;
string LbUser_ListenPort;

string LbUser_LogFile;

int LbUser_ThPoolSize;

//indica quanti / devo prendere delle url (1: prendo la prima parte /jasjaj)
int LbUser_PartialUrl;

string LbUser_Rules; //block, accept, redirect
string LbUser_BlockMessage;

//se in presenza di content-lenght e se il body non supera queste dimensioni, allora lo prende
int LbUser_CSBodyMaxSize;
int LbUser_SCBodyMaxSize;

string glb_BlockPage; //the complete html answer to send back when blocking pages

#if EIS_LEVEL & EIS_LEVEL_7
string LbUser_LspCode;
SCM LispManagement;
string LbUser_VaDomainServerSocketName; //per thread con il codice LISP che risponde a domain socket
#endif

SCM KamRun;
SCM ThreadGestoreComandi; //il thread per gestire tutte le richieste del LISP


//The engine for Validation Authority and KAM!
VAengine vaeng;
VAengine *pvaeng = &vaeng; //per utilizzarlo dal mondo esterno!

extern ULL_Rng* glb_rng;


//Definisco le funzioni di wrapping per lo strato lisp per accedere ai dati di vaeng
string ScmToStringCPP ( SCM s );

static SCM InsertIntoSet ( SCM setname, SCM value, SCM duration, SCM distribute_to_peers )
{
	string sn = ScmToStringCPP ( setname );
	string sv = ScmToStringCPP ( value );
	string sd = ScmToStringCPP ( duration );
	bool distribute = scm_to_bool(distribute_to_peers);
	pvaeng->vasets.AddItem ( sn, sv, strtoull ( sd.c_str(), 0, 10 ), true, distribute);
	return SCM_EOL;
}

static SCM InsertIntoDict ( SCM setname, SCM key, SCM value, SCM duration, SCM distribute_to_peers )
{
	string sn = ScmToStringCPP ( setname );
	string sk = ScmToStringCPP ( key );
	string sv = ScmToStringCPP ( value );
	string sd = ScmToStringCPP ( duration );
	bool distribute = scm_to_bool(distribute_to_peers);
	pvaeng->vasets.AddItem ( sn, sk, sv, strtoull ( sd.c_str(), 0, 10 ), true, distribute);
	return SCM_EOL;
}

static SCM RemoveFromSet ( SCM setname, SCM value, SCM duration, SCM distribute_to_peers )
{
	string sn = ScmToStringCPP ( setname );
	string sv = ScmToStringCPP ( value );
	string sd = ScmToStringCPP ( duration );
	bool distribute = scm_to_bool(distribute_to_peers);
	pvaeng->vasets.DelItem ( sn, sv, strtoull ( sd.c_str(), 0, 10 ), true, distribute );
	return SCM_EOL;
}

static SCM GetFromSet ( SCM setname, SCM key)
{
	string sn = ScmToStringCPP ( setname );
	string sv = ScmToStringCPP ( key );
	string value;
	bool bret = pvaeng->vasets.CheckItem ( sn, sv, value );
	
	if ( bret )
		return scm_from_latin1_string(value.c_str());
	else
		return SCM_BOOL_F;
}

static SCM CheckIfDeallocated(SCM pointer) {
    
    mutexSetEventoStructAllocati.lock();

    void *p = scm_to_pointer(pointer);
    if (SetEventoStructAllocati.find(p) == SetEventoStructAllocati.end()) {
        mutexSetEventoStructAllocati.unlock();
        return SCM_BOOL_T;
    } else {
        mutexSetEventoStructAllocati.unlock();
        return SCM_BOOL_F;
    }
}
/*********************************** Le strutture per Hyperscan ******************************/


// Struttura generica per mantenere lo stato di un database
struct ScannerInstance {
    hs_database_t *db = nullptr;
    hs_scratch_t *scratch = nullptr;
    time_t last_mtime = 0; // Usato solo per i db esterni
};

// Mappe per supportare N database esterni ed N database interni
static std::unordered_map<std::string, ScannerInstance> external_scanners;
static std::unordered_map<std::string, ScannerInstance> internal_scanners;

// Funzione di pulizia memoria per l'istanza
static void clear_instance(ScannerInstance& inst) {
    if (inst.scratch) hs_free_scratch(inst.scratch);
    if (inst.db) hs_free_database(inst.db);
    inst.db = nullptr;
    inst.scratch = nullptr;
    inst.last_mtime = 0;
}

// Callback invocata da Hyperscan quando trova una corrispondenza
static int match_handler(unsigned int id, unsigned long long from,
                         unsigned long long to, unsigned int flags, void *context) {
    bool* found = static_cast<bool*>(context);
    *found = true;
    std::cout << "[Hyperscan] MATCH! Regola ID: " << id
    << " all'offset da " << from << " a " << to << std::endl;
    return 1; // Ritorna 1 per fermare la scansione al primo match
}

std::string FromBV2String(SCM bv) {
    size_t len = scm_c_bytevector_length(bv);

    // 2. Ottieni il puntatore ai dati (ritorna void*)
    void* raw_data = SCM_BYTEVECTOR_CONTENTS(bv);

    // 3. Converti in std::string (Copia i byte)
    std::string str(reinterpret_cast<const char*>(raw_data), len);

    // Ora puoi usare la tua stringa C++
    return str;
}

// --------------------------------------------------------------------------
// 1. SCANNER ESTERNO (Basato su file .hdb e MTIME)
// Guile call: (hs-scan-external "key" "path/file.hdb" "input-string")
// --------------------------------------------------------------------------

/*
 913xxx - Scanner Detection (Rilevamento Bot/Scanner):
    Cosa fa: Cerca pattern negli User-Agent o negli header tipici di tool automatici (Nmap, Nikto, Nessus, sqlmap, curl, wg*et).
    Utilità: Ottimo per bloccare il rumore di fondo di internet.

 920xxx e 921xxx - Protocol Anomalies (Anomalie di Protocollo HTTP):
    Cosa fa: Controlla che la sintassi HTTP sia perfetta (header mancanti, Content-Length sballati, HTTP Request Smuggling, ecc.).
    Attenzione: Molto prona ai falsi positivi se hai client custom o API vecchie.

 930xxx e 931xxx - LFI & RFI (Local/Remote File Inclusion):
    Cosa fa: Blocca i tentativi di Path Traversal (il tuo test di ieri!). Cerca ../, ..\, /etc/passwd, C:\Windows, o tentativi di far scaricare file da URL esterni (http://... in parametri sospetti).

 932xxx e 933xxx - RCE (Remote Command Execution):
    Cosa fa: Cerca tentativi di iniettare comandi di sistema Unix/Windows o codice PHP nel server.

 941xxx - XSS (Cross-Site Scripting):
    Cosa fa: Cerca tag HTML malevoli (<script>, onerror=, javascript:), vettori di attacco DOM e offuscamenti vari. Indispensabile se la tua app accetta input che poi viene renderizzato a schermo ad altri utenti.

 942xxx - SQL Injection (SQLi):
    Cosa fa: Il cuore pulsante della sicurezza database. Cerca keyword SQL (UNION, SELECT, DROP, SLEEP()), tautologie (1=1, 'a'='a'), commenti SQL (--, #), ed evasioni varie.

 944xxx - Java/Magecart/NodeJS Injection:
    Cosa fa: Cerca vulnerabilità specifiche per stack moderni e Java (incluso il famigerato attacco Log4Shell ${jndi:...}).

 * */

static SCM hs_scan_external(SCM parlist, SCM pbuf=SCM_EOL) {
    cout << "hs_scan_external: " << "len lista: " << scm_to_int(scm_length(parlist)) << endl;
    SCM key_scm = scm_car(parlist);
    SCM path_scm = scm_cadr(parlist);
    SCM input_scm = scm_caddr(parlist);

    // char *c_key = scm_to_utf8_string(key_scm);
    // char *c_path = scm_to_utf8_string(path_scm);
    // char *c_input = scm_to_utf8_string(input_scm);

    std::string s_key = FromBV2String(key_scm); //(c_key);
    std::string s_path = FromBV2String(path_scm);
    std::string s_input = FromBV2String(input_scm);
    cout << "hs_scan_external: " << s_key << ", " << s_path << ", " << s_input <<endl;

    ScannerInstance& inst = external_scanners[s_key];

    struct stat st;
    if (stat(s_path.c_str(), &st) == 0) {
        // Se il file è stato modificato, ricarichiamo il database
        if (st.st_mtime > inst.last_mtime || inst.db == nullptr) {
            clear_instance(inst);

            std::ifstream file(s_path.c_str(), std::ios::binary | std::ios::ate);
            if (file.is_open()) {
                std::streamsize size = file.tellg();
                file.seekg(0, std::ios::beg);
                std::vector<char> buffer(size);

                if (file.read(buffer.data(), size)) {
                    if (hs_deserialize_database(buffer.data(), size, &inst.db) == HS_SUCCESS) {
                        hs_alloc_scratch(inst.db, &inst.scratch);
                        inst.last_mtime = st.st_mtime;
                    } else {
                        std::cerr << "[Hyperscan] Errore deserializzazione file: " << s_path << std::endl;
                    }
                }
            } else {
                std::cerr << "[Hyperscan] Impossibile aprire il file: " << s_path << std::endl;
            }
        }
    }

    bool found = false;
    if (inst.db && inst.scratch) {
        hs_scan(inst.db, s_input.c_str(), s_input.length(), 0, inst.scratch, match_handler, &found);
    }

    // free(c_key);
    // free(c_path);
    // free(c_input);
    if (found) 
    	cout << "hs_scan_external: FOUND!\n";

    return scm_from_bool(!found);
}

// --------------------------------------------------------------------------
// 2. SCANNER INTERNO (In memoria, con ricompilazione guidata da flag)
// Guile call: (hs-scan-internal "key" '("rule1" "rule2") is-regex? "input" force-recompile?)
// --------------------------------------------------------------------------
static SCM hs_scan_internal(SCM parlist, SCM pbuf=SCM_EOL) {
    SCM akey_scm = scm_car(parlist);
    SCM asetname_scm = scm_cadr(parlist);
    SCM ais_regex_scm = scm_caddr(parlist);
    SCM ainput_scm = scm_cadddr(parlist);
    SCM aforce_scm = scm_car(scm_cddddr(parlist));

    string s_key = FromBV2String(akey_scm);
    string s_setname = FromBV2String(asetname_scm);
    string s_isregex = FromBV2String(ais_regex_scm);
    string s_input= FromBV2String(ainput_scm);
    string s_force= FromBV2String(aforce_scm);

    ScannerInstance& inst = internal_scanners[s_key];
    bool force_recompile = s_force.compare("true")==0;

    if (force_recompile || inst.db == nullptr) {
        clear_instance(inst);

        //Il set esiste?
        auto theset = pvaeng->vasets.datasets.find ( s_setname );
        if ( theset == pvaeng->vasets.datasets.end() ) {
            cerr << __func__ << "set " << s_setname << " not found.\n";
            return SCM_BOOL_F;
        }

        //Ok il set è presente
        DO_VASET_LOCK(theset->second->rmtx);

        auto data_set = theset->second;

        if (data_set->stype != VA_SetTypes::SetOfStrings) {
            cerr << __func__ << "set " << s_setname << " is not a set of string.\n";
            return SCM_BOOL_F;
        }

        {
            //std::vector<std::string> strings;
            std::vector<const char*> patterns;
            std::vector<unsigned int> flags;
            std::vector<unsigned int> ids;
            std::vector<size_t> lengths;
            int i=0;

            // Pre-allochiamo la memoria dei vector per pura ottimizzazione (opzionale ma consigliato)
            size_t num_items = data_set->initial_values.size();
            patterns.reserve(num_items);
            flags.reserve(num_items);
            ids.reserve(num_items);
            lengths.reserve(num_items);

            for (const auto &item  : data_set->initial_values) {
                //strings.push_back(item);
                patterns.push_back(item.c_str());
                lengths.push_back(item.size());
                // Includiamo PREFILTER per salvaguardarci da regex complesse
                flags.push_back(HS_FLAG_DOTALL | HS_FLAG_CASELESS | HS_FLAG_PREFILTER);
                ids.push_back(i++);
            }

            hs_compile_error_t *compile_err = nullptr;
            hs_error_t err;

            if (true || s_isregex.compare("true")==0) {
                err = hs_compile_multi(patterns.data(), flags.data(), ids.data(),
                                    patterns.size(), HS_MODE_BLOCK, nullptr, &inst.db, &compile_err);
            } else {
                err = hs_compile_lit_multi(patterns.data(), flags.data(), ids.data(), lengths.data(),
                                        patterns.size(), HS_MODE_BLOCK, nullptr, &inst.db, &compile_err);
            }

            if (err == HS_SUCCESS) {
                hs_alloc_scratch(inst.db, &inst.scratch);
            } else {
                std::cerr << "[Hyperscan] Errore compilazione interna key '" << s_key << "': "
                << (compile_err ? compile_err->message : "Unknown error") << std::endl;
                if (compile_err) hs_free_compile_error(compile_err);
                clear_instance(inst); // Assicuriamoci che sia pulito
            }
        }
    }

    bool found = false;
    if (inst.db && inst.scratch) {
        hs_scan(inst.db, s_input.c_str(), s_input.length(), 0, inst.scratch, match_handler, &found);
    }

    return scm_from_bool(!found);
}



/**************************************************************************************/

list<T_bar_rule*> parsed_rules;

//set<pair<string, string>> cookie_header_set;
set<string> cookie_header_set;
PerfectHash* global_rules;
ImplicitDs* rules_action;
ImplicitDs* rules_change;

string s00 = string ( "\x00", 1 );
string s01 = string ( "\x01", 1 );
string s02 = string ( "\x02", 1 );
string s03 = string ( "\x03", 1 );
string s04 = string ( "\x04", 1 );

inline string nth_item ( const list<string>& ls, int i )
{
    return * ( next ( ls.begin(), i ) );
}

int yyparse();

void CheckAndLoadRules()
{
    //static regex * pat_01 = mtfa_compile_pattern ( "\n\\#.*\n" );

    //Leggo le regole di blocco, accept, manage e redirect!
    int numline = 0;

    if ( LbUser_Rules.size() > 0 && mtfa_FileIo::Exists ( LbUser_Rules ) )
    {

        FILE* file = fopen ( LbUser_Rules.c_str(), "r" );
        extern FILE* yyin;
        yyin = file;
        int iret = yyparse();

        //se iret == 0
        if ( iret != 0 )
        {
            cerr << "\nSyntax error in file: " << LbUser_Rules << endl;
            exit ( -1 );
        }


        //Costruisco la lista ordinata dei nomi dei cookie e degli header
        for ( auto it : parsed_rules )
        {
            for ( auto it1 : it->mapof_chq )
            {
                //cout << __func__   << "Found: " << it1.first << ", " << it1.second << endl;
                cookie_header_set.insert ( it1.first );
            }
        }


        //La lista di come i cookie e gli header devono essere esaminati è la visita ordinata dell'insieme cookie_header_set

        //Ora inserisco le regole nel corretto ordine
        global_rules = new PerfectHash();
        rules_action = new ImplicitDs();
        rules_change = new ImplicitDs();

        int rulenumber = 0;

        for ( auto it : parsed_rules )
        {
            rulenumber++;
            cout << __func__   << "Elaborate rule " << rulenumber << endl;
            //La sequenza è sempre la stessa: ip, host, url e poi header e cookie ma introduco anche la change!
            string ip, host, url;
            int next;

            ip = it->ip;

            if ( ip.size() == 0 )
            {
                ip = s00;     //"\x00" indica nessuna valorizzazione dell'ip: any value is admitted
            }

            next = global_rules->Find ( ( char* ) ip.data(), ip.size() );

            if ( next == -1 )
            {
                next = global_rules->Insert ( ( char* ) ip.data(), ip.size() );
            }

            host = it->host;

            if ( host.size() == 0 )
            {
                host = s00;     //"\x00" indica nessuna valorizzazione dell'host
            }

            host = to_string ( next ) + s01 + host;
            next = global_rules->Find ( ( char* ) host.data(), host.size() );

            if ( next == -1 )
            {
                next = global_rules->Insert ( ( char* ) host.data(), host.size() );
            }

            url = it->url;

            if ( url.size() == 0 )
            {
                url = s00;     //"\x00" indica nessuna valorizzazione della url
            }

            url = to_string ( next ) + s01 + url;
            next = global_rules->Find ( ( char* ) url.data(), url.size() );

            if ( next == -1 )
            {
                next = global_rules->Insert ( ( char* ) url.data(), url.size() );
            }

            //ora vanno aggiunti header e cookie.
            for ( auto corh : cookie_header_set )
            {
                string key, value;
                auto found = it->mapof_chq.find ( corh );

                if ( found != it->mapof_chq.end() )
                {
                    //Inserisco key e valore nella struttura dati. Il kind resta in cookie_header_set
                    //se valore è vuoto, vale "\x00"
                    key = found->first.substr ( 1 );
                    value = found->second;

                    if ( value.size() == 0 )
                    {
                        value = s00;     //key presente con qualsiasi valore possibile
                    }
                }
                else
                {
                    //in questo caso key e value stanno a 0, non c'è il key!
                    key = s00; //key non presente
                    value = s00;
                }

                string key_value = key + s02 + value;

                key_value = to_string ( next ) + s01 + key_value;
                next = global_rules->Find ( ( char* ) key_value.data(), key_value.size() );

                if ( next == -1 )
                {
                    next = global_rules->Insert ( ( char* ) key_value.data(), key_value.size() );
                }
            }

            //al termine, il next di uscita lo utilizzo per determinare se block, accept, manage oppure redirect!
            switch ( it->action )
            {
                case e_block:
                {
                    string* what;
                    bool bret = rules_action->Find ( ( char* ) &next, sizeof ( next ), ( void** ) &what );

                    if ( bret )
                    {
                        cerr << "Warning: the rule: " << rulenumber << " has been previously inserted with action: " << *what << endl;
                    }
                    else
                    {
                        string* s = new string ( "BLOCK" );
                        rules_action->Insert ( ( char* ) &next, sizeof ( next ), s );
                    }
                }
                break;

                case e_accept:
                {
                    string* what;
                    bool bret = rules_action->Find ( ( char* ) &next, sizeof ( next ), ( void** ) &what );

                    if ( bret )
                    {
                        cerr << "Warning: the rule: " << rulenumber << " has been previously inserted with action: " << *what << endl;
                    }
                    else
                    {
                        string* s = new string ( "ACCEPT" );
                        rules_action->Insert ( ( char* ) &next, sizeof ( next ), s );
                    }
                }
                break;

                case e_give:
                {
                    string* what;
                    bool bret = rules_action->Find ( ( char* ) &next, sizeof ( next ), ( void** ) &what );

                    if ( bret )
                    {
                        cerr << "Warning: the rule: " << rulenumber << " has been previously inserted with action: " << *what << endl;
                    }
                    else
                    {
                        string* s = new string ( "GIVE" );

                        for ( auto it1 : it->action_parameters )
                        {
                            ( *s ) += s02 + it1;
                        }

                        rules_action->Insert ( ( char* ) &next, sizeof ( next ), s );
                    }
                }
                break;

                case e_manage:
                {
                    string* what;
                    bool bret = rules_action->Find ( ( char* ) &next, sizeof ( next ), ( void** ) &what );

                    if ( bret )
                    {
                        cerr << "Warning: the rule: " << rulenumber << " has been previously inserted with action: " << *what << endl;
                    }
                    else
                    {
                        string* s = new string ( "MANAGE" );

                        for ( auto it1 : it->action_parameters )
                        {
                            ( *s ) += s02 + it1;
                        }

                        rules_action->Insert ( ( char* ) &next, sizeof ( next ), s );
                    }
                }
                break;

                case e_redirect:
                {
                    string* what;
                    bool bret = rules_action->Find ( ( char* ) &next, sizeof ( next ), ( void** ) &what );

                    if ( bret )
                    {
                        cerr << "Warning: the rule: " << rulenumber << " has been previously inserted with action: " << *what << endl;
                    }
                    else
                    {
                        string* s = new string ( nth_item ( it->action_parameters, 0 ) + s02 + nth_item ( it->action_parameters, 1 ) + s02 + nth_item ( it->action_parameters, 2 ) );
                        rules_action->Insert ( ( char* ) &next, sizeof ( next ), s );
                    }
                }
                break;
            }

//            if (it->is_change) {
//                T_list_rule_change * ltrc = nullptr;
//                bool bret = rules_change->Find((char*) &next, sizeof (next), (void**) &ltrc);
//                if (bret) {
//                    cerr << "Warning: the rule: " << rulenumber << " has been previously inserted with change." << endl;
//                } else {
//                    //string * s = new string("CHANGE");
//                    auto ltrc = new T_list_rule_change();
//                    for (auto it1 : it->change_parameters_cs) {
//                        auto trc = new T_rule_change();
//                        trc->apply_to = it1.first[0]; //H, C, B
//                        pcrecpp::RE * pat_cs = CompilePattern(it1.first.substr(1));
//                        trc->pat_cs = pat_cs;
//                        trc->subst_cs = new string(it1.second);
//                        ltrc->push_back(trc);
//                    }
//                    for (auto it1 : it->change_parameters_sc) {
//                        auto trc = new T_rule_change();
//                        trc->apply_to = it1.first[0]; //H, C, B
//                        pcrecpp::RE * pat_sc = CompilePattern(it1.first.substr(1));
//                        trc->pat_sc = pat_sc;
//                        trc->subst_sc = new string(it1.second);
//                        ltrc->push_back(trc);
//                    }
//                    rules_change->Insert((char*) &next, sizeof (next), ltrc);
//                }
//            }
        }

        //Provo il matching
        /*
        rulenumber = 0;
        for (auto crule : parsed_rules) {
            rulenumber++;
            cout << __func__   << "Verify the rule " << rulenumber << endl;

            string ip = crule->ip;
            if (ip.size() == 0)
                ip = genip();
            int next = global_rules->Find((char*) ip.data(), ip.size());
            if (next == -1) {
                ip = s00; //provo se esiste regola "any" ip
                next = global_rules->Find((char*) ip.data(), ip.size());
                if (next == -1) {
                    cerr << "Rule not matched. 0" << endl;
                    exit(0);
                }
            }

            string host = crule->host;
            if (host.size() == 0)
                host = genhost();
            host = to_string(next) + s01 + host;
            int newnext = global_rules->Find((char*) host.data(), host.size());
            if (newnext == -1) {
                host = to_string(next) + s01 + s00;
                next = global_rules->Find((char*) host.data(), host.size());
                if (next == -1) {
                    cerr << "Rule not matched. 1" << endl;
                    exit(0);
                }
            } else {
                next = newnext;
            }

            string url = crule->url;
            if (url.size() == 0)
                url = genurl();
            url = to_string(next) + s01 + url;
            newnext = global_rules->Find((char*) url.data(), url.size());
            if (newnext == -1) {
                url = to_string(next) + s01 + s00;
                next = global_rules->Find((char*) url.data(), url.size());
                if (next == -1) {
                    cerr << "Rule not matched. 2" << endl;
                    exit(0);
                }
            } else {
                next = newnext;
            }

            //ora vanno cercati header e cookie.
            for (auto corh : cookie_header_set) {
                string key, value;
                auto found = crule->mapof_chq.find(corh); //simulazione della ricerca nell'header http
                if (found != crule->mapof_chq.end()) {
                    //Cerco key e valore nella struttura dati
                    //se valore è vuoto, vale "\x00"
                    key = found->first.substr(1);
                    value = found->second;
                    if (value.size() == 0)
                        value = s03; //questo valore va messo quando il campo è vuoto nell'http e non per indicare qualsiasi valore (campo vuoto nella specifica)
                } else {
                    //in questo caso key non deve esserci necessariamente
                    key = s00;
                    value = s00;
                }
                string key_value = key + s02 + value;

                key_value = to_string(next) + s01 + key_value;
                Dump(key_value);
                next = global_rules->Find((char*) key_value.data(), key_value.size());
                if (next == -1) {
                    cerr << "Rule not matched. 3" << endl;
                    exit(0);
                }
            }

            //Ora da verificare le azioni...
            string * action;
            bool bret = rules_action->Find((char*) &next, sizeof (next), (void**) &action);
            if (!bret) {
                cerr << "Rule not matched. 4" << endl;
                exit(0);
            } else {
                if (action->compare("ACCEPT") == 0 || action->compare("BLOCK") == 0 || action->substr(0, 6).compare("MANAGE") == 0)
                    cerr << "Rule action: " << *action << endl;
                else {
                    vector<string> vs = mtfa_string_split(*action, s02);
                    cerr << "http://" << vs[0] << vs[1] << "?" << vs[2] << endl;
                }
            }
        }
         */
    }

    /*
     * Examples:
    Rule {
      action: manage (RUN /bin/ls /usr/local/ "ls apt");
      ip: ;
      host: 127.0.0.1;
      url: /dothreads;
    }
    //This is a comment. / * multiline comments * / are accepted
    #also this is a comment

    Rule {
      action: redirect (host=127.0.0.1:18002; url=; query=;);
      change: header="search","replace" body="bsea","brepl" cookie="bsea","brepl";
      ip: ;
      host: 127.0.0.1;
      url: /pluto.html;
    }

    Rule {
      action: redirect (host=www.google.it; url=/; query=;);
      ip: ;
      host: 127.0.0.1;
      url: /pippo1.html;
    }

    Rule {
      action: redirect (host=127.0.0.1:18002; url=/index.html; query=;);
      ip: ;
      host: 127.0.0.1;
      url: /pippo2.html;
    }
     */
}

string GetRemoteIpPort ( int s );
string GetLocalIpPort ( int s );

string GetRemoteIpPort ( int fd )
{
    // assume s is a connected socket
    socklen_t len;
    struct sockaddr_storage addr;
    char ipstr[INET6_ADDRSTRLEN];
    int port;

    len = sizeof addr;
    int iret = getpeername ( fd, ( struct sockaddr* ) &addr, &len );

    if ( iret != 0 )
    {
        return "";
    }

    // deal with both IPv4 and IPv6:
    if ( addr.ss_family == AF_INET )
    {
        struct sockaddr_in* s = ( struct sockaddr_in* ) &addr;
        port = ntohs ( s->sin_port );
        inet_ntop ( AF_INET, &s->sin_addr, ipstr, sizeof ipstr );
    }
    else     // AF_INET6
    {
        struct sockaddr_in6* s = ( struct sockaddr_in6* ) &addr;
        port = ntohs ( s->sin6_port );
        inet_ntop ( AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr );
    }

    return string ( ipstr ) + ":" + to_string ( port );
}

string GetLocalIpPort ( int fd )
{
    // assume s is a connected socket
    socklen_t len;
    struct sockaddr_storage addr;
    char ipstr[INET6_ADDRSTRLEN];
    int port;

    socklen_t len_sa = sizeof ( addr );
    memset ( &addr, 0, len_sa );
    int iret = getsockname ( fd, ( struct sockaddr* ) &addr, &len_sa );

    if ( iret != 0 )
    {
        return "";
    }

//    bufs_in_out->listen_ip = inet_ntoa(*(struct in_addr *) &(in_sa.sa_data[2]));
//    bufs_in_out->listen_port = ntohs(*(unsigned short*) &(in_sa.sa_data[0]));

    // deal with both IPv4 and IPv6:
    if ( addr.ss_family == AF_INET )
    {
        struct sockaddr_in* s = ( struct sockaddr_in* ) &addr;
        port = ntohs ( s->sin_port );
        inet_ntop ( AF_INET, &s->sin_addr, ipstr, sizeof ipstr );
    }
    else     // AF_INET6
    {
        struct sockaddr_in6* s = ( struct sockaddr_in6* ) &addr;
        port = ntohs ( s->sin6_port );
        inet_ntop ( AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr );
    }

    return string ( ipstr ) + ":" + to_string ( port );

}

typedef struct __Manage_LB_USER__ARGS__
{
    string in_ip;
    unsigned short in_port;
    set<string> ipsAndPorts;
    int thnum;
} T_ManageLBUserArgs;


time_t last_evaluated = time ( 0 );
time_t last_printed = time ( 0 );
unsigned long long last_connections = 0;
time_t inizio_valutazione = time ( 0 );

// std::map<string, string> GetBodyData ( string parms )
// {
//     std::map<string, string> mres;
//     std::vector<string> vs;
// 
//     //Splitto
//     vs = mtfa_string_split ( parms, "&" );
// 
//     for ( auto it = vs.begin(); it != vs.end(); ++it )
//     {
//         int poseq = ( *it ).find ( "=" );
// 
//         if ( poseq < 0 )
//         {
//             continue;
//         }
//         else
//         {
//             mres[urldecode ( ( *it ).substr ( 0, poseq ) )] = urldecode ( ( *it ).substr ( poseq + 1 ) );
//         }
//     }
// 
//     return mres;
// }

//La funzione che estrae i dati dall'header http (sia domanda, sia risposta)

bool GetHeadersDataCS ( char* headersPtr, int dataLen,
                        std::map<string, string>* current_query,
                        std::map<string, string>* current_headers,
                        std::map<string, string>* current_cookies,
                        string* s_loc_method,
                        string* s_loc_protocol,
                        string* s_loc_host,
                        string* s_loc_full_host,
                        string* s_loc_port,
                        string* s_loc_url,
                        string* s_loc_pars,
                        string* s_loc_tag,
                        string* s_loc_user_passwd,
                        bool need_body,
                        string* s_body
                      )
{
    current_query->clear();
    current_headers->clear();
    current_cookies->clear();

    //Parsing degli headers
    c_http_parser* cp = new c_http_parser();
    int iret = cp->Parse ( ( char* ) headersPtr, dataLen );

    if ( iret < 0 || !cp->headers_complete || ( need_body && !cp->message_complete ) )
    {
        delete cp;
        return false;
    }

    *s_loc_method = http_method_str ( ( enum http_method ) cp->parser->method );
    /*Parto con surl e poi proseguo con headers. le posizioni sono:
      http www.host.it 1920 /1/2/3/4/5/downloadpub/zfad%3Alx011 cat=702&op=0&referer=http%3A%2F%2Fwww.sian.it%2Fportale-sian%2Fsottosezione.jsp%3Fpid%3D6 111213141516 username:password
     * protocollo (http) - 0
     * host (l'host nella url) - 1
     * porta (nella url) - 2
     * url - 3
     * parametri - 4
     * campo tag (#) - 5
     * username e password - 6
     */
    int i = 0;
    *s_loc_protocol = string ( cp->url_start + cp->parsed_url.field_data[i].off, cp->parsed_url.field_data[i].len );
    ++i;
    *s_loc_host = /*urldecode*/ ( string ( cp->url_start + cp->parsed_url.field_data[i].off, cp->parsed_url.field_data[i].len ) );
    ++i;
    *s_loc_port = string ( cp->url_start + cp->parsed_url.field_data[i].off, cp->parsed_url.field_data[i].len );
    ++i;
    *s_loc_url = urldecode ( string ( cp->url_start + cp->parsed_url.field_data[i].off, cp->parsed_url.field_data[i].len ) );
    ++i;
    *s_loc_pars = string ( cp->url_start + cp->parsed_url.field_data[i].off, cp->parsed_url.field_data[i].len );
    ++i;
    *s_loc_tag = urldecode ( string ( cp->url_start + cp->parsed_url.field_data[i].off, cp->parsed_url.field_data[i].len ) );
    ++i;
    *s_loc_user_passwd = /*urldecode*/ ( string ( cp->url_start + cp->parsed_url.field_data[i].off, cp->parsed_url.field_data[i].len ) );
    ++i;

    //Se il messaggio è completo, lo carico in bosy!
    if ( cp->message_complete )
        s_body->assign ( cp->body_from, cp->body_len );
    else
        s_body->clear();

//     if ( cp->body_from )
//         free ( cp->body_from );
// 
//     cp->body_len = 0;
//     cp->body_from = nullptr;

    //Facciamoci la query
    if ( s_loc_pars->size() <= 0 )
    {
        current_query->clear();
    }
    else
    {
        //Provo con il parsing del body
        list<pair<string, string>> lkv = GetkeysFromJson(*s_loc_pars);
        if (lkv.size() > 0)
        {
            //è un json
            for (auto it: lkv)
            {
                (*current_query)[it.first] = it.second;
            }
        } else {

            T_V_Split vs = mtfa_string_split ( *s_loc_pars, "&" );

            for ( auto& it : vs )
            {
                T_V_Split vs1 = mtfa_string_split ( it, "=" );

                int l = vs1.size();

                if ( l > 0 )
                    ( *current_query ) [/*urldecode*/ ( vs1[0] )] = ( l == 2 ) ? urldecode ( vs1[1] ) : "";
            }
        }
        (*current_query)["0_FULL_QUERY"]=*s_loc_pars;
    }

    //E ora il body
    if ( s_body->size() >= 0 )
    {
        //Provo con il parsing del body
        list<pair<string, string>> lkv = GetkeysFromJson(*s_body);
        if (lkv.size() > 0)
        {
            //è un json
            for (auto it: lkv)
            {
                (*current_query)[it.first] = it.second;
            }
        } else {

            T_V_Split vs = mtfa_string_split ( *s_body, "&" );
            for ( auto& it : vs )
            {
                T_V_Split vs1 = mtfa_string_split ( it, "=" );
                int l = vs1.size();
                if ( l > 0 )
                    ( *current_query ) [/*urldecode*/ ( vs1[0] )] = ( l == 2 ) ? urldecode ( vs1[1] ) : "";
            }
        }
    }

    //ora gli headers
    for ( auto it = cp->headers.begin(); it != cp->headers.end(); ++it )
    {
        string chiave = string ( std::get<0> ( *it ), std::get<1> ( *it ) );
        std::transform ( chiave.begin(), chiave.end(), chiave.begin(), ::tolower );
        string valore = string ( std::get<2> ( *it ), std::get<3> ( *it ) );

        if ( chiave.compare ( "cookie" ) == 0 )
        {
            std::vector<string> vs2 = mtfa_string_split ( valore, "; " );

            for ( auto i = 0; i < vs2.size(); ++i )
            {
                int pos = vs2[i].find_first_of ( "=" );

                if ( pos == string::npos )
                {
                }
                else
                {
                    string name = /*urldecode*/ ( vs2[i].substr ( 0, pos ) );
                    string value = urldecode ( vs2[i].substr ( pos + 1 ) );
                    ( *current_cookies ) [name] = value;
                }
            }
        }
        else
        {
            ( *current_headers ) [/*urldecode*/ ( chiave )] = /*urldecode*/ ( valore );
        }
    }

    //Calcolo hostname
    string hostname;
    string portnum;
    auto found = current_headers->find ( "host" );

    if ( found != current_headers->end() )
    {
        hostname = ( *found ).second;
        int pos = hostname.find ( ":" );
        *s_loc_full_host = hostname;

        if ( pos == string::npos )
        {
            string tmp = *s_loc_protocol;
            for_each(tmp.begin(), tmp.end(), [](char &c){ c=tolower(c); });
            if (s_loc_protocol->compare("http")==0)
                portnum = "80";
            else
                portnum = "443";
        }
        else
        {
            portnum = hostname.substr ( pos + 1 );
            hostname = hostname.substr ( 0, pos );
        }

        *s_loc_host = hostname;
        *s_loc_port = portnum;
    }
    else
    {
        //prendo quello che ho gi\u00e0, i dati della url
    }

    delete cp;
    return true;
}

bool GetHeadersDataSC ( char* headersPtr, int headersLen,
                        std::map<string, string>* current_headers,
                        int* status_code
                      )
{
    current_headers->clear();

    //Parsing degli headers
    c_http_parser* cp = new c_http_parser();
    int iret = cp->Parse ( ( char* ) headersPtr, headersLen );

    if ( iret < 0 || !cp->headers_complete )
    {
        delete cp;
        return false;
    }

    //ora gli headers
    for ( auto it = cp->headers.begin(); it != cp->headers.end(); ++it )
    {
        string chiave = string ( std::get<0> ( *it ), std::get<1> ( *it ) );
        string valore = string ( std::get<2> ( *it ), std::get<3> ( *it ) );
        ( *current_headers ) [/*urldecode*/ ( chiave )] = /*urldecode*/ ( valore );
    }

    *status_code = cp->parser->status_code;

    delete cp;
    return true;
}

string RunLispCommand ( string command )
{
    SCM ret_val = scm_c_eval_string ( command.c_str() );
    char* p = scm_to_latin1_string ( ret_val );
    string s = string ( p );
    free ( p );
    return s;
}

string RunLispCommand ( std::list<string> commands )
{
    string cmd;

    for ( auto i : commands )
    {
        cmd += i;
    }

    SCM ret_val = scm_c_eval_string ( cmd.c_str() );
    char* p = scm_to_latin1_string ( ret_val );
    string s = string ( p );
    free ( p );
    return s;
}

string MySqlResultToString ( std::list<std::list<std::string> >& res_query )
{
    //Converto in base64 e concateno
    string dump_res;
    string dump_rec;

    for ( auto rec : res_query )
    {
        dump_rec.clear();

        for ( auto fld : rec )
        {
            dump_rec += Base64::encode ( fld.c_str() ) + ";";
        }

        dump_rec = dump_rec.substr ( 0, dump_rec.size() - 1 );
        dump_res += dump_rec + ":";
    }

    dump_res = dump_res.substr ( 0, dump_res.size() - 1 );
    return dump_res;
}


bool SendAll ( int sockfd, char* dati, int len )
{
    int sent = 0;

    while ( sent < len )
    {
        int l = send ( sockfd, &dati[sent], len - sent, 0 );

        if ( l < 0 )
        {
            return false;
        }

        if ( l == 0 && sent == len )
        {
            return true;
        }

        sent += l;
    }

    return true;
}

void AnswerJsonData ( string answer, string headers, const string & data, T_EventoStruct * bufs_in_out, bool do_close )
{
    if ( headers.size() > 0 && headers[headers.size() - 1] != '\n' )
        headers += "\r\n";
    
    if ( answer[answer.size() - 1] != '\n' )
        answer += "\r\n";
    
    string out_body = data;
    
    if ( do_close )
    {
        string out_header = answer + headers +
        "Content-Type: application/json\r\n"
        "Connection: close\r\n"
        "Content-Length: " + to_string ( out_body.size() ) +
        +"\r\n\r\n";
        string risposta = out_header + out_body;
        SendAnswerAndCloseAll ( bufs_in_out, risposta.c_str(), ( int ) risposta.size() );
    }
    else
    {
        string out_header = answer + headers +
        "Content-Type: application/json\r\n"
        "Connection: Keep-Alive\r\n"
        "Content-Length: " + to_string ( out_body.size() ) +
        +"\r\n\r\n";
        string risposta = out_header + out_body;
        SendAnswerAndNotClose ( bufs_in_out, risposta.c_str(), ( int ) risposta.size() );
    }
}

void AnswerData ( string answer, string headers, const string& data, T_EventoStruct* bufs_in_out, bool do_close )
{
    if ( headers.size() > 0 && headers[headers.size() - 1] != '\n' )
    {
        headers += "\r\n";
    }

    if ( answer[answer.size() - 1] != '\n' )
    {
        answer += "\r\n";
    }

    string out_body = data;

    //std::cout << __func__   << "RISPOSTA: " << risposta << "\n";

    if ( do_close )
    {
        string out_header = answer + headers +
                            "Content-Type: text/plain; charset=UTF-8\r\n"
                            "Connection: close\r\n"
                            "Content-Length: " + to_string ( out_body.size() ) +
                            +"\r\n\r\n";
        string risposta = out_header + out_body;
        SendAnswerAndCloseAll ( bufs_in_out, risposta.c_str(), ( int ) risposta.size() );
    }
    else
    {
        string out_header = answer + headers +
                            "Content-Type: text/plain; charset=UTF-8\r\n"
                            "Connection: Keep-Alive\r\n"
                            "Content-Length: " + to_string ( out_body.size() ) +
                            +"\r\n\r\n";
        string risposta = out_header + out_body;
        SendAnswerAndNotClose ( bufs_in_out, risposta.c_str(), ( int ) risposta.size() );
    }
}

const char* mtfa_magic_mime (const char * filename)
{
    const char *magic_full;
    static magic_t magic_cookie=nullptr;
    
    /* MAGIC_MIME tells magic to return a mime of the file, 
    *       but you can specify different things	*/
    if (!magic_cookie)
    {
        magic_cookie = magic_open(MAGIC_MIME_TYPE);
        if (magic_cookie == NULL) {
            return "";
        }
        
        if (magic_load(magic_cookie, NULL) != 0) {
            magic_close(magic_cookie);
            return "";
        }
    }
    
    magic_full = magic_file(magic_cookie, filename);
    //cout << magic_full << endl;
    return magic_full;
}

void AnswerFile ( string http_answer, string filename, string headers, T_EventoStruct* bufs_in_out, bool do_close )
{
    //    cout << __func__   << "Enter AnswerFile" << endl;
    //Ma qui potri farmi una cache dei file che sono stati dati finora, con la loro expiring
    //     10.3.5 304 Not Modified
    // If the client has performed a conditional GET request and access is allowed, but the document has not been modified, the server SHOULD respond with this status code. The 304 response MUST NOT contain a message-body, and thus is always terminated by the first empty line after the header fields.
    // The response MUST include the following header fields:
    //       - Date, unless its omission is required by section 14.18.1
    // If a clockless origin server obeys these rules, and proxies and clients add their own Date to any response received without one (as already specified by [RFC 2068], section 14.19), caches will operate correctly.
    //       - ETag and/or Content-Location, if the header would have been sent
    //         in a 200 response to the same request
    //       - Expires, Cache-Control, and/or Vary, if the field-value might
    //         differ from that sent in any previous response for the same
    //         variant
    // If the conditional GET used a strong cache validator (see section 13.3.3), the response SHOULD NOT include other entity-headers. Otherwise (i.e., the conditional GET used a weak validator), the response MUST NOT include other entity-headers; this prevents inconsistencies between cached entity-bodies and updated headers.
    // If a 304 response indicates an entity not currently cached, then the cache MUST disregard the response and repeat the request without the conditional.
    // If a cache uses a received 304 response to update a cache entry, the cache MUST update the entry to reflect any new field values given in the response.
    //
    // 3.11 Entity Tags
    // Entity tags are used for comparing two or more entities from the same requested resource. HTTP/1.1 uses entity tags in the ETag (section 14.19), If-Match (section 14.24), If-None-Match (section 14.26), and If-Range (section 14.27) header fields. The definition of how they are used and compared as cache validators is in section 13.3.3. An entity tag consists of an opaque quoted string, possibly prefixed by a weakness indicator.
    //       entity-tag = [ weak ] opaque-tag
    //       weak       = "W/"
    //       opaque-tag = quoted-string
    // A "strong entity tag" MAY be shared by two entities of a resource only if they are equivalent by octet equality.
    // A "weak entity tag," indicated by the "W/" prefix, MAY be shared by two entities of a resource only if the entities are equivalent and could be substituted for each other with no significant change in semantics. A weak entity tag can only be used for weak comparison.
    // An entity tag MUST be unique across all versions of all entities associated with a particular resource. A given entity tag value MAY be used for entities obtained by requests on different URIs. The use of the same entity tag value in conjunction with entities obtained by requests on different URIs does not imply the equivalence of those entities.
    //

    if ( headers.size() > 0 && headers[headers.size() - 1] != '\n' )
    {
        headers += "\r\n";
    }

    if ( http_answer[http_answer.size() - 1] != '\n' )
    {
        http_answer += "\r\n";
    }

    //prende il tipo del file (utilizzando l'estensione)
    //size_t pos = bufs_in_out->s_loc_url.find_last_of(".");
    size_t pos = filename.find_last_of ( "." );
    string content_type = "";

    try
    {
        if ( pos != string::npos )
        {
            string ext = filename.substr ( pos ); //ivi compreso il .
            content_type = glb_mimetypes[ext];
        }
        else
        {
            //tipo generico, non posso calcolare la sua estensione
            content_type = "application/octet-stream";
        }
    }
    catch ( exception& ex )
    {
        mtfa_log("AnswerFile: %s\n", ex.what());
    }
    
    //se il tipo non è stato assegnato, metto il generico
    if (content_type.size() == 0)
    {
        //uso il magic mime
        content_type = mtfa_magic_mime(filename.c_str());
    }
    if (content_type.size() == 0)
    {
        content_type = "text/plain";
    }

    //Apre il file as input e lo aggiunge a quanto già presente
    int fnum = open ( filename.c_str(), O_RDONLY );

    if ( fnum < 0 )
    {
        perror ( ( string ( "Error file: " ) + filename ).c_str() );

        string out_header = http_answer +
                            headers +
                            "Connection: close\r\n"
                            "Content-Length: 0"
                            "\r\n\r\n";
        SendAnswerAndCloseAll ( bufs_in_out, out_header.data(), out_header.size() );
    }
    else
    {
        struct stat stat_buf;
        fstat ( fnum, &stat_buf );

        if ( do_close )
        {
            string out_header = http_answer +
                                headers +
                                "Content-Type: " + content_type + "\r\n"
                                "Connection: close\r\n"
                                "Content-Length: " + to_string ( stat_buf.st_size ) +
                                "\r\n\r\n";
            SendFileAndCloseAll ( bufs_in_out, out_header.data(), out_header.size(), fnum, 0, stat_buf.st_size );
        }
        else
        {
//R"a(HTTP/1.1 200 OK
//Date: Sun, 10 Jul 2016 09:59:06 GMT
//Server: Apache/2.4.16 (Unix) OpenSSL/1.0.1e-fips PHP/5.6.17
//Last-Modified: Fri, 22 Apr 2016 09:25:13 GMT
//ETag: "123a-5310f6777fc3e"
//Accept-Ranges: bytes
//Content-Length: 4666
//Keep-Alive: timeout=5, max=100
//Connection: Keep-Alive
//Content-Type: text/html
//HTTP/1.1 200 OK
//Content-Type: text/html
//Connection: Keep-Alive
//Content-Length: 58
//)a";
            string out_header = http_answer +
                                headers +
                                "Content-Type: " + content_type + "\r\n"
                                "Date: " + MakeHttpDateTime ( time ( nullptr ) ) + "\r\n"
                                "Last-Modified: " + MakeHttpDateTime ( stat_buf.st_mtim.tv_sec ) + "\r\n"
                                "ETag: \"123a-5310f6777fc3e\"\r\n"
                                "Keep-Alive: timeout=5, max=100\r\n"
                                "Connection: Keep-Alive\r\n"
                                "Accept-Ranges: none\r\n"
                                "Content-Length: " + to_string ( stat_buf.st_size ) +
                                "\r\n\r\n";
            //cout << __func__   << "GiveFile: " << filename << ": HTTP: " << out_header << endl;
            SendFileAndNotClose ( bufs_in_out, out_header.data(), out_header.size(), fnum, 0, stat_buf.st_size );
        }
    }
}


string BuildXmlAnswer ( const string& code, const string& subcode, const string& resto )
{
    //  cout << __func__   << "Enter BuildXmlAnswer" << endl;
    //  cout << __func__   << "Code: " << code << endl;
    //  cout << __func__   << "Subcode: " << subcode << endl;
    //  cout << __func__   << "Resto: " << resto << endl;
    if ( resto.size() <= 0 )
        return "<?xml version = \"1.0\" encoding=\"UTF-8\"?>\r\n<root><answer_code>" +
               code +
               "</answer_code><answer_subcode>" + subcode +
               "</answer_subcode></root>";
    else
        return "<?xml version = \"1.0\" encoding=\"UTF-8\"?>\r\n<root><answer_code>" +
               code +
               "</answer_code><answer_subcode>" + subcode +
               "</answer_subcode>" + resto + "</root>";
}

void AnswerHtml ( string answer, string headers, const string& html, T_EventoStruct* bufs_in_out, bool do_close )
{
    if ( headers.size() > 0 && headers[headers.size() - 1] != '\n' )
    {
        headers += "\r\n";
    }

    if ( answer[answer.size() - 1] != '\n' )
    {
        answer += "\r\n";
    }

    string out_body = html;

    if ( do_close )
    {
        string out_header = answer + headers +
                            "Content-Type: text/html\r\n"
                            "Connection: close\r\n"
                            "Content-Length: " + to_string ( out_body.size() ) +
                            +"\r\n\r\n";
        string risposta = out_header + out_body;
        SendAnswerAndCloseAll ( bufs_in_out, risposta.c_str(), ( int ) risposta.size() );
    }
    else
    {
        string out_header = answer + headers +
                            "Content-Type: text/html\r\n"
                            "Connection: Keep-Alive\r\n"
                            "Content-Length: " + to_string ( out_body.size() ) +
                            +"\r\n\r\n";
        string risposta = out_header + out_body;
        SendAnswerAndNotClose ( bufs_in_out, risposta.c_str(), ( int ) risposta.size() );
    }
}

void AnswerRedirect ( const std::string& where, std::string headers, T_EventoStruct* bufs_in_out )
{
    if ( headers.size() > 0 && headers[headers.size() - 1] != '\n' )
    {
        headers += "\r\n";
    }

    std::string out_body = "<html><head><title>Moved</title></head><body><h1>Moved</h1><p>This page has moved to "
                           "<a href=\"" + where + "\">"
                           + where +
                           "</a>.</p></body></html>";
    std::string out_header = std::string ( "HTTP/1.1 302 Found\r\n" +
                                           headers +
                                           "Location: " + where + "\r\n"
                                           "Connection: close\r\n"
                                           "Content-Type: text/html\r\n"
                                           "Content-Length: " ) + std::to_string ( out_body.size() ) + "\r\n\r\n";
    std::string risposta = out_header + out_body;
    //std::cout << __func__   << "RISPOSTA: " << risposta << "\n";
    SendAnswerAndCloseAll ( bufs_in_out, risposta.c_str(), ( int ) risposta.size() );
}



//void ConnectSendAndClose(
//        std::string dati,
//        T_EventoStruct * bufs_in_out,
//        void (*f)(struct bufferevent *b_in, void *ctx),
//        struct bufferevent * b_in
//        ) {
//    std::vector<string> vs;
//    vs = mtfa_string_split("ip_dst;port_dst;ip_src;data_in_base64;regex per capire se comunicazione corretta", ";");
//    if (vs.size() != 5) {
//errore:
//        //i dati sono stati costruiti dal livello III, se sono errati, allora non contano e quindi di deve tornare al punto di origine!
//        //ripristino l'event buffer!
//        if (b_in)
//        {
//            bufferevent_enable(b_in, EV_READ | EV_WRITE);
//            f(b_in, bufs_in_out);
//        }
//        else
//        {
//            cerr << "ConnectSendAndClose: Unable to recover. Closing all.\n";
//            CloseOne(false, bufs_in_out);
//            CloseOne(true, bufs_in_out);
//        }
//        return;
//    }
//
//    std::string ip_dst = vs[0];
//    std::string port_dst = vs[1];
//    std::string ip_src = vs[2];
//    std::string data = Base64::decode(vs[3]);
//    string regex_expression = Base64::decode(vs[4]);
//
//    int sockfd = 0;
//    int n = 0;
//    char recvBuff[1024];
//    struct sockaddr_in serv_addr;
//    memset(recvBuff, 0, sizeof (recvBuff));
//    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
//        goto errore;
//
//    memset(&serv_addr, 0, sizeof (serv_addr));
//
//    serv_addr.sin_family = AF_INET;
//    serv_addr.sin_port = htons(atoi(port_dst.c_str()));
//
//    sockaddr_in service;
//    std::string ipToBind;
//    service.sin_family = AF_INET;
//    ipToBind = ip_src;
//    service.sin_addr.s_addr = inet_addr(ipToBind.c_str());
//    service.sin_port = htons(0);
//
//    // Bind the socket.
//    //printf("faccio bind\n");
//    if (::bind(sockfd, (struct sockaddr*) &service, sizeof (service)) < 0) {
//        close(sockfd);
//        goto errore;
//    }
//
//    //printf("converto ip in numero\n");
//    if (inet_pton(AF_INET, ip_dst.c_str(), &serv_addr.sin_addr) < 0) {
//        close(sockfd);
//        goto errore;
//    }
//
//    //printf("faccio connect\n");
//    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
//        close(sockfd);
//        goto errore;
//    }
//
//    //printf("faccio send all\n");
//    if (!SendAll(sockfd, (char*) data.data(), data.size())) {
//        close(sockfd);
//        goto errore;
//    }
//
//    //printf("faccio recv\n");
//    int recvd = recv(sockfd, recvBuff, 1024, 0);
//    close(sockfd);
//    if (recvd <= 0) {
//        goto errore;
//    }
//
//    string srecvd;
//    srecvd.assign(recvBuff, recvBuff + recvd);
//    //printf("Ricevuti: %d <%s>\n", recvd, srecvd.c_str());
//    bool bret = FindPattern(srecvd, regex_expression);
//    if (bret) {
//        //printf("Presa risposta soddisfacente. Riprendo la comunicazione.\n");
//        if (b_in)
//        {
//            bufferevent_enable(b_in, EV_READ | EV_WRITE);
//            f(b_in, bufs_in_out);
//        }
//        else
//        {
//            cerr << "ConnectSendAndClose: Unable to recover. Closing all.\n";
//            CloseOne(false, bufs_in_out);
//            CloseOne(true, bufs_in_out);
//        }
//    } else {
//        //Non trovato quello che aspettavo
//        //blocco la comunicazione!
//        //printf("Non presa risposta soddisfacente. Blocco\n");
//        if (b_in)
//        {
//            bufferevent_enable(b_in, EV_READ | EV_WRITE);
//        }
//        else
//        {
//            cerr << "ConnectSendAndClose: Unable to recover. Closing all.\n";
//            CloseOne(false, bufs_in_out);
//            CloseOne(true, bufs_in_out);
//        }
//        std::string ans = "HTTP/1.1 401 Unauthorized\r\nConnection: close\r\nContent-Length: 0\r\n\r\n";
//        SendAnswerAndCloseAll(bufs_in_out, ans.c_str(), ans.size());
//    }
//    return;
//}

std::string GetCookieId ( std::string cvt_cookie )
{
    //Esempio di lavoro
    std::string sa = cvt_cookie;

    int pos = sa.find ( "=" );

    if ( pos >= 0 )
    {
        sa = sa.substr ( pos + 1 );

        //ora cerco inizio id (il campo _
        pos = sa.find ( "_" );

        if ( pos > 0 )
        {
            sa = sa.substr ( pos + 3 );
            pos = sa.find ( "_" );

            if ( pos > 0 )
            {
                sa = sa.substr ( 0, pos );
                return sa;
                //std::cout << __func__   <<StrTime() << "IDENTITA': " << sa << endl;
            }
        }
    }

    return "";
}

//typedef std::list<std::string> T_lstring;

//Per gestire la lista degli elementi in read/write...
C_ListaEventi* listaEventi;

//per gestire gli ip su cui fare bind in uscita
mutex singleAccessToVsOutIp;
mutex singleAccessToVsOutPort;
mutex singleAccessToLast; //per ricordare l'ultimo degli applet forniti
std::vector<string> vs_out_ip;

string GetNextOutIp()
{
    string sRet;
    std::lock_guard<std::mutex> lock ( singleAccessToVsOutIp );

    static int p = 0;

    if ( p >= vs_out_ip.size() )
    {
        p = 0;
    }

    sRet = vs_out_ip[p];
    ++p;
    return ( sRet );
}


//Stringhe di utilizzo comune negli headers
static std::string sEndHeaders = "\r\n\r\n";
//static std::string sEndHeaders2 = "\n\n";


//void InviaRisposta(T_EventoStruct * bufs_in_out, string okko, string dati, std::list<string> headers = std::list<string>({})) {
//    stringstream ConcatenateStrings;
//    ConcatenateStrings << okko << "\r\n";
//    ConcatenateStrings << "Content-Length: " << to_string(dati.size()) << "\r\n";
//    if (headers.size() > 0)
//        for (auto i = headers.begin(); i != headers.end(); ++i) {
//            ConcatenateStrings << (*i) << "\r\n";
//        }
//    ConcatenateStrings << "Connection: Close\r\n";
//    ConcatenateStrings << "\r\n" << dati;
//    string risposta = ConcatenateStrings.str();
//    ConcatenateStrings.str("");
//    SendAnswerAndCloseAll(bufs_in_out, risposta.c_str(), risposta.size());
//}

void CloseOne ( bool isIn, T_EventoStruct* bufs_in_out )
{
    if ( isIn )
    {
        if ( !bufs_in_out->buf_in_freed )
        {
            if ( bufs_in_out->p_buf_in )
            {
//                int fd = bufferevent_getfd(bufs_in_out->p_buf_in);
//                {
//                    open_sockets_mutex.lock();
//                    if (open_sockets.find(fd)!=open_sockets.end())
//                        open_sockets.erase(fd);
//                    open_sockets_mutex.unlock();
//                }
                bufferevent_free ( bufs_in_out->p_buf_in );
                bufs_in_out->p_buf_in = 0;
            }

            bufs_in_out->buf_in_freed = true;
        }

        //Inoltre, se out non è mai stato associato, allora lo elimina
        if ( bufs_in_out->p_buf_out && bufferevent_getfd ( bufs_in_out->p_buf_out ) == -1 )
        {
            bufferevent_free ( bufs_in_out->p_buf_out );
            bufs_in_out->p_buf_out = 0;
            bufs_in_out->buf_out_freed = true;
        }

        if ( !bufs_in_out->closed )
        {
            bufs_in_out->closed = GetULLTime();
        }
    }
    else
    {
        if ( !bufs_in_out->buf_out_freed )
        {
            if ( bufs_in_out->p_buf_out )
            {
//                int fd = bufferevent_getfd(bufs_in_out->p_buf_out);
//                {
//                    open_sockets_mutex.lock();
//                    if (open_sockets.find(fd)!=open_sockets.end())
//                        open_sockets.erase(fd);
//                    open_sockets_mutex.unlock();
//                }
                bufferevent_free ( bufs_in_out->p_buf_out );
                bufs_in_out->p_buf_out = 0;
            }

            bufs_in_out->buf_out_freed = true;
        }

        if ( !bufs_in_out->closed )
        {
            bufs_in_out->closed = GetULLTime();
        }
    }

    return;
}

//In tutti i casi in cui ho una risposta, dovrei voler prefiggere un elemento alla parte header dei dati.
//Qui faccio quest'operazione
void UpdateEvBufferAnswer ( struct evbuffer* b_out_read, T_EventoStruct* bufs_in_out )
{
    if ( !bufs_in_out->b_add_header_to_answer && !bufs_in_out->b_replace_header_in_answer)
        return;

    if ( bufs_in_out->b_add_header_to_answer)
    {
        size_t len = 0;
        char* linea = evbuffer_readln ( b_out_read, &len, evbuffer_eol_style::EVBUFFER_EOL_ANY );

        if ( linea )
        {

            //Aggiunge i dati indicati nel bufs_in_out e poi rimette a posto la prima riga.
            string tobeadded = "\r\n";

            for ( auto& it : bufs_in_out->add_header_to_answer )
                tobeadded += it + "\r\n";

            int iret = evbuffer_prepend ( b_out_read, tobeadded.data(), tobeadded.size() );;

            if ( iret == -1 )
                perror ( "prepend: 1" );

            iret = evbuffer_prepend ( b_out_read, linea, len );

            if ( iret == -1 )
                perror ( "prepend: 2" );

            free ( linea );
        }
    }
    if ( bufs_in_out->b_replace_header_in_answer)
    {
        //devo estrarre da evbuffer tutta la prima parte, fino a doppio fine linea
        //Cerco il doppio fine linea
        struct evbuffer_ptr ptrFineHeader = evbuffer_search ( b_out_read, sEndHeaders.c_str(), sEndHeaders.size(), NULL );
        if ( ptrFineHeader.pos == -1 )
        {
            //Nothing to do, end of headers not found!
            return;
        }

        //ptrFineHeader punta alla posizione del primo elemento di \r\n\r\n: se fosse 10, significa che al posto 10 c'è un \r
        char * data = new char[ptrFineHeader.pos+4];
        size_t dataLen = ptrFineHeader.pos+4;
        int iret = evbuffer_remove(b_out_read, data, dataLen);
        if (iret > 0)
        {
            string sData(data, data+dataLen);
            sData = mtfa_replace_matching_pattern(sData, bufs_in_out->replace_header_in_answer.first, bufs_in_out->replace_header_in_answer.second, true);

            iret = evbuffer_prepend ( b_out_read, sData.c_str(), sData.size());
            if ( iret == -1 )
                mtfa_log( "prepend: 3\n" );

            delete[] data;
        }
    }
}

void UpdateEvBufferAnswer ( string& p, T_EventoStruct* bufs_in_out )
{
    if ( !bufs_in_out->b_add_header_to_answer && !bufs_in_out->b_replace_header_in_answer)
        return;

    if ( bufs_in_out->b_add_header_to_answer)
    {
        //prendo fino a eoln e subito dopo ci metto il add_header_to_request
        size_t pos = p.find ( "\n" );

        if ( pos != string::npos )
        {
            string tobeadded;

            for ( auto& it : bufs_in_out->add_header_to_answer )
                tobeadded += it + "\r\n";

            p.insert ( pos+1, tobeadded );
        }
    }

    if ( bufs_in_out->b_replace_header_in_answer)
    {
        p = mtfa_replace_matching_pattern(p, bufs_in_out->replace_header_in_answer.first, bufs_in_out->replace_header_in_answer.second, true);
    }
    return;
}

void UpdateEvBufferRequest ( string& p, T_EventoStruct* bufs_in_out )
{
    if ( !bufs_in_out->b_add_header_to_request && !bufs_in_out->b_replace_header_in_request)
        return;

    if ( bufs_in_out->b_add_header_to_request )
    {
        //prendo fino a eoln e subito dopo ci metto il add_header_to_request
        size_t pos = p.find ( "\n" );

        if ( pos == string::npos )
            return;

        string tobeadded;

        for ( auto& it : bufs_in_out->add_header_to_request )
            tobeadded += it + "\r\n";

        p.insert ( pos+1, tobeadded );
    }

    if (bufs_in_out->b_replace_header_in_request)
    {
        p = mtfa_replace_matching_pattern(p, bufs_in_out->replace_header_in_request.first, bufs_in_out->replace_header_in_request.second, true);
    }
}


//Modifica il buffer di input, prima di trasferirlo in oputput
void UpdateEvBufferRequest ( struct evbuffer* b_in_read, T_EventoStruct* bufs_in_out )
{
    if ( !bufs_in_out->b_add_header_to_request && !bufs_in_out->b_replace_header_in_request)
        return;

    if ( bufs_in_out->b_add_header_to_request )
    {
        mtfa_log("Add headers to request: "); for_each(bufs_in_out->add_header_to_request.begin(), bufs_in_out->add_header_to_request.end(), [&] (string & it) {cout << it << " ";});
        cout << endl;
        size_t len = 0;
        char* linea = evbuffer_readln ( b_in_read, &len, evbuffer_eol_style::EVBUFFER_EOL_ANY );

        if ( !linea )
            return; //non saprei cosa fare altrimenti!

        //Aggiunge i dati indicati nel bufs_in_out e poi rimette a posto la prima riga.
        string tobeadded = "\r\n";

        for ( auto& it : bufs_in_out->add_header_to_request )
            tobeadded += it + "\r\n";

        int iret = evbuffer_prepend ( b_in_read, tobeadded.data(), tobeadded.size() );;

        if ( iret == -1 )
            perror ( "prepend: 1" );

        iret = evbuffer_prepend ( b_in_read, linea, len );

        if ( iret == -1 )
            perror ( "prepend: 2" );

        free ( linea );
    }

    if ( bufs_in_out->b_replace_header_in_request)
    {
        mtfa_log("Replace headers in request: %s\n", bufs_in_out->replace_header_in_request.second.c_str());
        //devo estrarre da evbuffer tutta la prima parte, fino a doppio fine linea
        //Cerco il doppio fine linea
        struct evbuffer_ptr ptrFineHeader = evbuffer_search ( b_in_read, sEndHeaders.c_str(), sEndHeaders.size(), NULL );
        if ( ptrFineHeader.pos == -1 )
        {
            //Nothing to do, end of headers not found!
            return;
        }

        //ptrFineHeader punta alla posizione del primo elemento di \r\n\r\n: se fosse 10, significa che al posto 10 c'è un \r
        char * data = new char[ptrFineHeader.pos+4];
        size_t dataLen = ptrFineHeader.pos+4;
        int iret = evbuffer_remove(b_in_read, data, dataLen);
        if (iret > 0)
        {
            string sData(data, data+dataLen);
            sData = mtfa_replace_matching_pattern(sData, bufs_in_out->replace_header_in_request.first, bufs_in_out->replace_header_in_request.second, true);

            iret = evbuffer_prepend ( b_in_read, sData.c_str(), sData.size());
            if ( iret == -1 )
                mtfa_log( "prepend: 4\n" );

            delete[] data;
        }
    }
}

void SendAnswerAndNotClose ( T_EventoStruct* bufs_in_out, const char* dati, int len )
{
    //Qui aggiungo i dati per la SondaInternal
    if (LbUser_VA_LocalSH_on)
    {
//         bufs_in_out->answer_is_needed=true;
        bufs_in_out->data_given_sh_cs=0;
        SondaHwSimulator_Producer(bufs_in_out, false, (char*)dati, len);
    }

    try
    {
        struct evbuffer* b_in_write = bufferevent_get_output ( bufs_in_out->p_buf_in );
//        struct evbuffer *b_in_read = bufferevent_get_input(bufs_in_out->p_buf_in);

//        //Svuota tutti i buffer
//        evbuffer_drain(b_in_write, evbuffer_get_length(b_in_write));
//        evbuffer_drain(b_in_read, evbuffer_get_length(b_in_read));

        //Aggiunge i dati correnti
        string data;
        data.assign ( dati, dati + len );
        UpdateEvBufferAnswer ( data, bufs_in_out );

        evbuffer_add ( b_in_write, data.data(), data.size() );
        //Riabilito gli eventi
        if ( bufs_in_out->p_buf_in )
            bufferevent_enable ( bufs_in_out->p_buf_in, EV_READ | EV_WRITE | EV_TIMEOUT);
        if ( bufs_in_out->p_buf_out )
            bufferevent_enable ( bufs_in_out->p_buf_out, EV_READ | EV_WRITE | EV_TIMEOUT);


        ////Chiude i canali che non servono e si predispone alla chiusura totale
        //bufferevent_setcb(bufs_in_out->p_buf_in, NULL, close_on_finished_writecb_b_in, eventcb_b_in, bufs_in_out);

        ////Chiudo il b_out!!
        //CloseOne(false, bufs_in_out);
    }
    catch ( std::exception& ex )
    {
	mtfa_log("%s\n", ex.what());
    }

    return;
}

void SendAnswerAndCloseAll ( T_EventoStruct* bufs_in_out, const char* dati, int len )
{
    //Qui aggiungo i dati per la SondaInternal
    if (LbUser_VA_LocalSH_on && len>0)
    {
//         bufs_in_out->answer_is_needed=true;
        bufs_in_out->data_given_sh_cs=0;
        SondaHwSimulator_Producer(bufs_in_out, false, (char*)dati, len);
    }


    try
    {
        if ( bufs_in_out && bufs_in_out->p_buf_in )
        {
            struct evbuffer* b_in_write = bufferevent_get_output ( bufs_in_out->p_buf_in );

            //Aggiunge i dati correnti
            if (len > 0) {
                string data;
                data.assign ( dati, dati + len );
                UpdateEvBufferAnswer ( data, bufs_in_out );

                int iret = evbuffer_add ( b_in_write, data.data(), data.size() );

    /*
                int len = evbuffer_get_length(b_in_write);
                cout << "Scrivo: " << len << " bytes\n";
                unsigned  char * p = evbuffer_pullup(b_in_write, -1);
                cout << "Stampo: <"  << string(p, p+len) << ">\n";*/

                if ( iret == -1 )
                {
                    cout << __func__   << "Errore aggiungendo i dati: " << strerror ( errno ) << endl;
                }
            }
            //Chiude i canali che non servono e si predispone alla chiusura totale
//             if ( bufs_in_out && bufs_in_out->p_buf_in )
//             {
            bufferevent_setcb ( bufs_in_out->p_buf_in, NULL, close_on_finished_writecb_b_in, eventcb_b_in, bufs_in_out );
            bufferevent_enable ( bufs_in_out->p_buf_in, EV_WRITE ); // | EV_TIMEOUT);
//             }
        }
    }
    catch ( std::exception& ex )
    {
	mtfa_log("%s\n", ex.what());
    }

    return;
}

void SendFileAndNotClose ( T_EventoStruct* bufs_in_out, const char* data, int datalen, int filenum, int offset, int length )
{
    //Qui aggiungo i dati per la SondaInternal!! Assente il file!
    if (LbUser_VA_LocalSH_on)
    {
//         bufs_in_out->answer_is_needed=true;
        bufs_in_out->data_given_sh_cs=0;
        SondaHwSimulator_Producer(bufs_in_out, false, (char*)data, datalen);
    }

    try
    {
        if ( bufs_in_out && bufs_in_out->p_buf_in )
        {
            struct evbuffer* b_in_write = bufferevent_get_output ( bufs_in_out->p_buf_in );

            //Aggiunge i dati
            //Aggiunge i dati correnti
            string dati;
            dati.assign ( data, data + datalen );
            UpdateEvBufferAnswer ( dati, bufs_in_out );

            evbuffer_add ( b_in_write, dati.data(), dati.size() );

            //Aggiunge il file
            evbuffer_add_file ( b_in_write, filenum, offset, length );

            //Chiude i canali che non servono e si predispone alla chiusura totale
            if ( bufs_in_out->p_buf_out )
            {
                bufferevent_disable ( bufs_in_out->p_buf_in, EV_READ );
                bufferevent_enable ( bufs_in_out->p_buf_in, EV_WRITE ); // | EV_TIMEOUT);
                bufferevent_disable ( bufs_in_out->p_buf_out, EV_READ | EV_WRITE ); // | EV_TIMEOUT);
                bufferevent_setcb ( bufs_in_out->p_buf_in, NULL, close_on_finished_writecb_b_in, eventcb_b_in, bufs_in_out );
            }
        }
    }
    catch ( std::exception& ex )
    {
	mtfa_log("%s\n", ex.what());
    }
}

void SendFileAndCloseAll ( T_EventoStruct* bufs_in_out, const char* data, int datalen, int filenum, int offset, int length )
{
    //Qui aggiungo i dati per la SondaInternal!! Assente il file!
    if (LbUser_VA_LocalSH_on)
    {
//         bufs_in_out->answer_is_needed=true;
        bufs_in_out->data_given_sh_cs=0;
        SondaHwSimulator_Producer(bufs_in_out, false, (char*)data, datalen);
    }

    try
    {
        if ( bufs_in_out && bufs_in_out->p_buf_in )
        {
            struct evbuffer* b_in_write = bufferevent_get_output ( bufs_in_out->p_buf_in );

            //Aggiunge i dati
            string dati;
            dati.assign ( data, data + datalen );
            UpdateEvBufferAnswer ( dati, bufs_in_out );

            evbuffer_add ( b_in_write, dati.data(), dati.size() );

            //Aggiunge il file
            evbuffer_add_file ( b_in_write, filenum, offset, length );

            //Chiude i canali che non servono e si predispone alla chiusura totale
            if ( bufs_in_out->p_buf_out )
            {
                bufferevent_disable ( bufs_in_out->p_buf_in, EV_READ );
                bufferevent_enable ( bufs_in_out->p_buf_in, EV_WRITE ); // | EV_TIMEOUT);
                bufferevent_disable ( bufs_in_out->p_buf_out, EV_READ | EV_WRITE ); // | EV_TIMEOUT);
                bufferevent_setcb ( bufs_in_out->p_buf_in, NULL, close_on_finished_writecb_b_in, eventcb_b_in, bufs_in_out );
            }
        }

        //Chiudo il b_out!!
        CloseOne ( false, bufs_in_out );
    }
    catch ( std::exception& ex )
    {
	mtfa_log("%s\n", ex.what());
    }

    return;
}

bool CloseOneIfEmpty ( bool isIn, T_EventoStruct* bufs_in_out )
{
    try
    {
        struct evbuffer* b_write;
        struct evbuffer* b_read;

        if ( isIn )
        {
            b_write = bufferevent_get_output ( bufs_in_out->p_buf_in );
            b_read = bufferevent_get_input ( bufs_in_out->p_buf_in );
        }
        else
        {
            b_write = bufferevent_get_output ( bufs_in_out->p_buf_out );
            b_read = bufferevent_get_input ( bufs_in_out->p_buf_out );
        }

        if ( ( evbuffer_get_length ( b_write ) > 0 ) || ( evbuffer_get_length ( b_read ) > 0 ) )
        {
            return false;     //non posso chiudere poich? ? ancora da svuotare!
        }

        CloseOne ( isIn, bufs_in_out );
        return true;
    }
    catch ( std::exception& ex )
    {
	mtfa_log("%s\n", ex.what());
        return false;
    }
}

void GoHome
(
    T_EventoStruct* bufs_in_out,
    const string& ip_dest,
    const string& port_dest,
    const string& s_loc_host,     //!Usato nel SNI nel caso di HTTPS
	const string& s_loc_method,   //!non utilizzato
	const string& s_loc_url,      //!non utilizzato
	const string& sheader,        //!Completo di metodo e uri!, se presente sostituisce l'attuale
	const string& sbody,          //!se presente, sostituisce l'attuale
    int src_data_to_pass,        //quando > 0 indica la parte di dati da passare (se non ci sono alterazioni di buffer)
    bool isSSL,
    const string c_server_cert,
    const string c_host_cert,
    const string c_client_cert,
    const string c_client_key,
    bool verify_server_cert,
    const string c_client_method, //="tls",
    const string c_client_algo    //"ALL:!aNULL:RC4+RSA:+HIGH:+MEDIUM:+LOW:+EXP:+eNULL" //"HIGH:!aNULL:!kRSA:!PSK:!SRP:!MD5:!RC4"
)
{
    //cout << "Entra correttamente\n";
    //Se indirizzo ip_dest non è ip ma è hostname, allora lo converto in hostname
    void* ctx = ( void* ) bufs_in_out;  //Rimette a posto il ctx
    
    /*
    if (sheader.size()==0)
        mtfa_log("Pass to server: %s:%s. URL: %s. Secure: %s.\n", ip_dest.c_str(), port_dest.c_str(), bufs_in_out->s_loc_uri.c_str(), (isSSL?"HTTPS":"HTTP"));
    else
        mtfa_log("Pass to server: %s:%s. URL: %s. Secure: %s.\n", ip_dest.c_str(), port_dest.c_str(), sheader.c_str(), (bufs_in_out->ServerIsSSL?"HTTPS":"HTTP"));
    */

    //riprende il src e dst
    struct evbuffer* src;
    src = bufferevent_get_input ( bufs_in_out->p_buf_in );
    string s_dest = ip_dest;

    if ( !isdigit ( ip_dest[0] ) )
    {
        struct addrinfo aiHints;
        struct addrinfo* aiList = NULL;
        int retVal;

        memset ( &aiHints, 0, sizeof ( aiHints ) );
        aiHints.ai_family = AF_INET;
        aiHints.ai_socktype = SOCK_STREAM;
        aiHints.ai_protocol = IPPROTO_TCP;

        if ( ( retVal = getaddrinfo ( ip_dest.c_str(), port_dest.c_str(), &aiHints, &aiList ) ) != 0 )
        {
            mtfa_log ( "getaddrinfo() failed. Comunication to <%s:%s> blocked.\n", ip_dest.c_str(), port_dest.c_str() );
            AnswerHtml ( "HTTP/1.1 200 OK\r\n", "", glb_BlockPage, bufs_in_out );
            return;
        }
        else
        {
            char* localIP = inet_ntoa ( * ( struct in_addr* ) & ( aiList->ai_addr->sa_data[2] ) );
            s_dest.assign ( localIP );
            freeaddrinfo ( aiList );
        }
    }

    unsigned short u_port = atoi ( port_dest.c_str() );
    string s_port = port_dest;

    //Ma se è connesso, dove è connesso?
    int server_socket = bufferevent_getfd ( bufs_in_out->p_buf_out );
    string connessoa = GetRemoteIpPort ( server_socket );

    if ( connessoa.size() == 0 )
    {
        //non è connesso
        bufs_in_out->toBeConnected = true;
    }
    else
    {
        //E' già connesso. Se allo stesso allora inutile crearne uno nuovo
        if ( connessoa.compare ( s_dest + ":" + to_string ( u_port ) ) == 0 )
        {
            bufs_in_out->server_ip = s_dest;
            bufs_in_out->server_port = u_port;
            bufs_in_out->toBeConnected = false;
        }
    }

    if ( isSSL != bufs_in_out->ServerIsSSL ||
            bufs_in_out->toBeConnected ||
            bufs_in_out->server_ip.compare ( s_dest ) != 0 ||
            bufs_in_out->server_port != u_port ||
            bufs_in_out->client_cert.compare ( c_client_cert ) != 0 ||
            bufs_in_out->server_cert.compare ( c_server_cert ) != 0
       )
    {
        //Visto che mi devo connettere...
        if ( bufs_in_out->ssl_ctx )
        {
            SSL_CTX_free ( bufs_in_out->ssl_ctx );
            bufs_in_out->ssl_ctx = nullptr;
        }

        struct sockaddr_storage connect_to_addr;
        int connect_to_addrlen;

        string s_ip_port = ( s_dest ) + ":" + ( s_port );

        bufs_in_out->server_ip = ( s_dest );
        bufs_in_out->server_port = u_port;

        memset ( &connect_to_addr, 0, sizeof ( connect_to_addr ) );
        connect_to_addrlen = sizeof ( connect_to_addr );

        if ( evutil_parse_sockaddr_port ( s_ip_port.c_str(), ( struct sockaddr* ) &connect_to_addr, &connect_to_addrlen ) < 0 )
        {
            mtfa_log ( "Error parsing the output address: %s\n", s_ip_port.c_str() );

            CloseOne ( true, bufs_in_out );
            CloseOne ( false, bufs_in_out );
            return;
        }

        //Prima di fare la connect, devo creare il socket, bindarlo all'ip e poi posso fare connect!
        evutil_socket_t fd_connect;
        fd_connect = socket ( AF_INET, SOCK_STREAM, 0 );

        if ( fd_connect < 0 )
        {
            int errore = evutil_socket_geterror ( fd_connect );
            const char* s_errore = evutil_socket_error_to_string ( errore );
            mtfa_log("Error creating socket: %d - %s\n", errore, s_errore);

            CloseOne ( true, bufs_in_out );
            CloseOne ( false, bufs_in_out );
            return;
        }

        //Lo bindo al prossimo ip di uscita.
        {
            sockaddr_in service;
            string ipToBind;
            service.sin_family = AF_INET;
            ipToBind = GetNextOutIp();
            service.sin_addr.s_addr = inet_addr ( ipToBind.c_str() );
            service.sin_port = htons ( 0 );

            // Bind the socket.
            if ( ::bind ( fd_connect, ( struct sockaddr* ) &service, sizeof ( service ) ) == -1 )
            {
                int errore = evutil_socket_geterror ( fd_connect );
                const char* s_errore = evutil_socket_error_to_string ( errore );
                mtfa_log("Error binding socket: %d - %s\n", errore, s_errore);
            }

            bufs_in_out->bind_ip = ipToBind;
        }

        //associo il socket al bufferevent
        //Faccio il socket non bloccante e senza ritardi!
        SetSocketNoDelay ( fd_connect );
        SetSocketUnblocking ( fd_connect );

        int iRet;

        //Se la comunicazione deve essere SSL, allora il bufferevent lo devo creare come ssl!
        if ( isSSL )
        {
            bufs_in_out->ServerIsSSL = isSSL;
            bufs_in_out->server_cert = c_server_cert;
            bufs_in_out->client_cert = c_client_cert;
            bufs_in_out->client_key = c_client_key;
            
            //ora, se il certificato server contiene "|" allora sono stati specificati client method e client algorithm

            bufs_in_out->client_method = c_client_method;
            bufs_in_out->client_algo = c_client_algo;

            T_V_Split vs = mtfa_string_split(c_server_cert, "|");
            if (vs.size() > 1)
            {
                bufs_in_out->server_cert = vs[0];
                bufs_in_out->client_method = vs[1];
                if (vs.size() > 2)
                {
                    bufs_in_out->client_algo = mtfa_replace_matching_pattern(vs[2], ",", ":", true);
                }
            }

            bufs_in_out->server_host = c_host_cert;
            bufs_in_out->verify_server_cert = verify_server_cert;

            SSL_CTX* ssl_ctx = NULL;
            SSL* ssl = NULL;
            static pcre2_code * ssl_pattern = mtfa_compile_pattern("[sS][sS][lL]");
            static pcre2_code * tls_pattern = mtfa_compile_pattern("[tT][lL][sS]");
            static pcre2_code * dtls_pattern = mtfa_compile_pattern("[dD][tT][lL][sS]");
            if (mtfa_find_pattern(bufs_in_out->client_method, dtls_pattern))
                ssl_ctx = SSL_CTX_new ( DTLS_client_method());
            else if (mtfa_find_pattern(bufs_in_out->client_method, tls_pattern))
                ssl_ctx = SSL_CTX_new ( TLS_client_method());
            else if (mtfa_find_pattern(bufs_in_out->client_method, ssl_pattern))
                ssl_ctx = SSL_CTX_new ( SSLv23_client_method());
            else
                ssl_ctx = SSL_CTX_new ( DTLS_client_method());

            if ( !ssl_ctx )
            {
                mtfa_log("error on SSL_CTX_new\n");
                close ( fd_connect );
                //SSL_CTX_free ( ssl_ctx );
                CloseOne ( true, bufs_in_out );
                CloseOne ( false, bufs_in_out );
                return;
            }
            SSL_CTX_set_cipher_list(ssl_ctx, bufs_in_out->client_algo.c_str());

            if ( 1 != SSL_CTX_load_verify_locations ( ssl_ctx, NULL, bufs_in_out->server_cert.c_str() ) )
            {
                mtfa_log("error on SSL_CTX_load_verify_locations\n");
                close ( fd_connect );
                SSL_CTX_free ( ssl_ctx );
                CloseOne ( true, bufs_in_out );
                CloseOne ( false, bufs_in_out );
                return;
            }

            if (verify_server_cert)
                SSL_CTX_set_verify ( ssl_ctx, SSL_VERIFY_PEER, NULL );
            else
                SSL_CTX_set_verify ( ssl_ctx, SSL_VERIFY_NONE, NULL );

            if ( bufs_in_out->client_cert.size() > 0 )
            {
                SSL_CTX_use_certificate_file ( ssl_ctx, bufs_in_out->client_cert.c_str(), SSL_FILETYPE_PEM );
                SSL_CTX_use_PrivateKey_file ( ssl_ctx, bufs_in_out->client_cert.c_str(), SSL_FILETYPE_PEM );
            }

            ssl = SSL_new ( ssl_ctx );

            if ( ssl == NULL )
            {
                mtfa_log("error on SSL_new\n");
                close ( fd_connect );
                SSL_CTX_free ( ssl_ctx );
                CloseOne ( true, bufs_in_out );
                CloseOne ( false, bufs_in_out );
                return;
            }

            // Set hostname for SNI extension
            if (s_loc_host.size() > 0)
				SSL_set_tlsext_host_name ( ssl, s_loc_host.c_str() );
			else if ( bufs_in_out->server_host.size() > 0 )
                SSL_set_tlsext_host_name ( ssl, bufs_in_out->server_host.c_str() );

            //Ora creo il bufferevent per ssl
            if ( !bufs_in_out->toBeConnected )
            {
                mtfa_log("Chiude il socket precedente\n");
                bufferevent_disable ( bufs_in_out->p_buf_out, EV_READ | EV_WRITE ); // | EV_TIMEOUT);
                bufferevent_setcb ( bufs_in_out->p_buf_out, NULL, NULL, NULL, NULL );
                bufferevent_free ( bufs_in_out->p_buf_out );
            }

            bufs_in_out->p_buf_out = bufferevent_openssl_socket_new ( bufferevent_get_base ( bufs_in_out->p_buf_in ), fd_connect, ssl, BUFFEREVENT_SSL_CONNECTING, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS );

            bufferevent_openssl_set_allow_dirty_shutdown ( bufs_in_out->p_buf_out, 1 );

            //Ora mi collego al server indicato e approvato!
            iRet = bufferevent_socket_connect ( bufs_in_out->p_buf_out, ( struct sockaddr* ) &connect_to_addr, connect_to_addrlen );

            bufs_in_out->ssl_ctx = ssl_ctx;
        }
        else
        {
            bufs_in_out->server_cert.clear();
            bufs_in_out->client_cert.clear();
            bufs_in_out->client_key.clear();
            bufs_in_out->server_host.clear();

            //Assegno il nuovo socket al bufferevent
            if ( !bufs_in_out->toBeConnected )
            {
                mtfa_log("Chiude il socket precedente\n");
                bufferevent_disable ( bufs_in_out->p_buf_out, EV_READ | EV_WRITE ); // | EV_TIMEOUT);
                bufferevent_setcb ( bufs_in_out->p_buf_out, NULL, NULL, NULL, NULL );
                bufferevent_free ( bufs_in_out->p_buf_out );
                bufs_in_out->p_buf_out = 0;
            }

            bufs_in_out->p_buf_out = bufferevent_socket_new(bufferevent_get_base ( bufs_in_out->p_buf_in ), fd_connect, ( bufferevent_options ) ( BEV_OPT_THREADSAFE | BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS ) );

            //bufferevent_setfd ( bufs_in_out->p_buf_out, fd_connect );

            //Ora mi collego al server indicato e approvato!
            iRet = bufferevent_socket_connect ( bufs_in_out->p_buf_out, ( struct sockaddr* ) &connect_to_addr, connect_to_addrlen );
        }

        if ( iRet < 0 )
        {
            perror ( "in bufferevent_socket_connect" );

            cout << __FILE__ << ":" << __LINE__ << ": "
                 "CONN?: " << ( ( bufs_in_out->toBeConnected ) ? ( "NO" ) : ( "YES" ) ) << endl <<
                 "Server: " << bufs_in_out->server_ip << ":" << bufs_in_out->server_port << endl <<
                 "Dest: " << s_dest <<  ":" << u_port << endl;
            //Svuoto!!! NON SO COSA FARE, DEVO CHIUDERE TUTTO!
            CloseOne ( true, bufs_in_out );
            CloseOne ( false, bufs_in_out );
            return;
        }

        bufs_in_out->toBeConnected = false;
        bufs_in_out->connected_to_server = GetULLTime();
    }

    bufs_in_out->connected_to_server = GetULLTime();

    struct evbuffer* dst = bufferevent_get_output ( bufs_in_out->p_buf_out );

    //Questa comunicazione è da client a server (GET/POST/HEAD...))
    //se sheader o sbody sono pieni, li devo sostituire agli originali. Poi potro' fare altre operazioni!
    //it is more simple. se sheader è pieno, lo utilizzo.
    //se sbody è pieno. lo utilizzo,
    //altrimenti utilizzo gli altri!

    int original_length = bufs_in_out->header_cs.size() + bufs_in_out->content_length;

    string new_sbody = sbody;
    string new_sheader = sheader;
	
	//cout << "NEWSBODY  : <" << new_sbody << ">" << endl;
	//cout << "NEWSHEADER: <" << new_sheader << ">" << endl;
	
//     if ( bufs_in_out->protocollo == Protocols::eProtSSL )   //questo mi dice che il client parla ssl
//     {
//         //cout << "URL: " << bufs_in_out->s_loc_url << endl;
//         new_sheader += bufs_in_out->header_cs.insert ( bufs_in_out->header_cs.size() - 2, "X-Forwarded-Proto: https\r\n" );
//     }

    //se siamo il tutto manage, allora non ci sono changes.
    if ( new_sbody.size() > 0 )
    {
        if ( new_sheader.size() > 0 )
        {
            bufs_in_out->new_src = new_sheader + new_sbody;
        }
        else
        {
            bufs_in_out->new_src = bufs_in_out->header_cs + new_sbody;
        }
    }
    else
    {
        if ( new_sheader.size() > 0 )
        {
            bufs_in_out->new_src = new_sheader + bufs_in_out->s_body;
        }
    }

    if ( bufs_in_out->new_src.size() == 0 )
    {
//         mtfa_log("Send data not modified: %d\n", src_data_to_pass);
        if ( src_data_to_pass > 0 )
        {
            evbuffer_remove_buffer ( src, dst, src_data_to_pass );
            bufs_in_out->data_in += src_data_to_pass;
        }
        else
        {
            //Aggiungo alla richiesta gli eventuali headers indicati nella struttura bufs_in_out
            UpdateEvBufferRequest ( src, bufs_in_out );
            bufs_in_out->data_in += evbuffer_get_length ( src );
            
//             mtfa_log("ADD BUFFER. SRC=%d, DST=%d, ", evbuffer_get_length ( src ), evbuffer_get_length ( dst ));

            evbuffer_add_buffer ( dst, src );
//             mtfa_log("NEW-DST=%d\n", evbuffer_get_length ( dst ));
        }
    }
    else
    {
        //Svuoto src
        //mtfa_log("Send data modified\n");
        evbuffer_drain ( src, original_length );

        if ( new_sheader.size() > 0 )
        {
            bufs_in_out->header_cs = new_sheader;
        }

        if ( new_sbody.size() > 0 )
        {
            bufs_in_out->s_body = new_sbody;
        }

        //Aggiungo alla richiesta gli eventuali headers indicati nella struttura bufs_in_out
        UpdateEvBufferRequest ( bufs_in_out->new_src, bufs_in_out );

        evbuffer_add ( dst, bufs_in_out->new_src.data(), bufs_in_out->new_src.size() );
        bufs_in_out->data_in += bufs_in_out->new_src.size();

        //per ora, per sicurezza di inviare tutto ma deve essere gestito l'invio da client a server!
        //se ci sono più dati del necessario, questi vanno gestiti.
        evbuffer_add_buffer ( dst, src );
        bufs_in_out->data_in += evbuffer_get_length ( src );
    }

//     mtfa_log("original_length: %d\n", original_length);
//     mtfa_log("Sending %d bytes to server.\n", evbuffer_get_length ( dst ));
//     mtfa_log("Headers were: %s\n", bufs_in_out->header_cs.c_str());
//     mtfa_log("Sending really %.*s\n", evbuffer_get_length ( dst ), evbuffer_pullup(dst, evbuffer_get_length ( dst )));

    
    if ( evbuffer_get_length ( dst ) >= MAX_OUTPUT )
    {
        //    cout << __func__   << "GoHome. flooding, rallenta\n";
        // We're giving the other side data faster than it can
        //pass it on.  Stop reading here until we have drained the
        //other side to MAX_OUTPUT/2 bytes.
        //std::cerr <<StrTime() << "Set watermark per OUT, non riesco a inviare dati da in a out\n";
        if ( bufs_in_out && bufs_in_out->p_buf_out && bufs_in_out->p_buf_in )
        {
            bufferevent_disable ( bufs_in_out->p_buf_in, EV_READ );
            bufferevent_setcb ( bufs_in_out->p_buf_out, readcb_b_out, drained_writecb_b_out, eventcb_b_out, bufs_in_out );
            bufferevent_setwatermark ( bufs_in_out->p_buf_out, EV_WRITE, MAX_OUTPUT / 32 + 128, MAX_OUTPUT );
            bufferevent_enable ( bufs_in_out->p_buf_out, EV_READ | EV_WRITE );
//             bufferevent_setcb ( bufs_in_out->p_buf_in, readcb_b_in, writecb_b_in, eventcb_b_in, ctx );
//             bufferevent_enable ( bufs_in_out->p_buf_in, EV_READ | EV_WRITE ); // | EV_TIMEOUT);
        }
    }
    else
    {
        //Abilito gli eventi per il nuovo buffer
        bufferevent_setcb ( bufs_in_out->p_buf_out, readcb_b_out, writecb_b_out, eventcb_b_out, ctx );
        bufferevent_enable ( bufs_in_out->p_buf_out, EV_READ | EV_WRITE );

        bufferevent_setcb ( bufs_in_out->p_buf_in, readcb_b_in, writecb_b_in, eventcb_b_in, ctx );
        bufferevent_enable ( bufs_in_out->p_buf_in, EV_READ | EV_WRITE ); // | EV_TIMEOUT);
    }

    if ( bufs_in_out->first_sent_to_server == 0 )
    {
        bufs_in_out->first_sent_to_server = GetULLTime();
    }

    return;
}

bool VerificaValiditaBufferOut ( bool isRead, struct bufferevent* b_out, T_EventoStruct* bufs_in_out )
{
    return bufs_in_out && bufs_in_out->p_buf_out;
    //    if (!bufs_in_out)
    //    {
    //        if (b_out)
    //        {
    //            std::cerr << StrTime() << "Close without bufs_in_out" << std::endl;
    //            bufferevent_disable(b_out, EV_READ | EV_WRITE | EV_TIMEOUT);
    //            if (!b_out)
    //                bufferevent_free(b_out);
    //        }
    //        return false; //Non so cos'altro fare!
    //    }
    //
    //    if (!b_out)
    //    {
    //        bufs_in_out->closed = GetULLTime();
    //
    //        if (bufs_in_out->p_buf_out) {
    //            CloseOne(false, bufs_in_out);
    //        }
    //
    //        if (!bufs_in_out->buf_in_freed)
    //        {
    //            //Bout e' stato eliminato ma bin no!
    //            //devo terminare di scrivere su bin
    //            //e devo svuotare l'ingresso di bin
    //            if (bufs_in_out->p_buf_in)
    //            {
    //                struct evbuffer *evbin_in = bufferevent_get_input(bufs_in_out->p_buf_in);
    //                int evbin_in_len = evbuffer_get_length(evbin_in);
    //                evbuffer_drain(evbin_in, evbin_in_len);
    //
    //                bufferevent_setcb(bufs_in_out->p_buf_in, NULL, close_on_finished_writecb_b_in, eventcb_b_in, bufs_in_out);
    //                return false;
    //            }
    //            else
    //            {
    //                CloseOne(true, bufs_in_out);
    //                return false;
    //            }
    //        }
    //    }
    //    else
    //    {
    //        if (bufs_in_out->buf_in_freed) {
    //            bufs_in_out->closed = GetULLTime();
    //
    //            //Bin e' stato eliminato. Scarico tutto quello che ho letto da b_out ma non posso scaricare quello che dovrei scrivere su out
    //            struct evbuffer *src = bufferevent_get_output(bufs_in_out->p_buf_out);
    //            int src_len = evbuffer_get_length(src);
    //            evbuffer_drain(src, src_len);
    //
    //            bufferevent_setcb(bufs_in_out->p_buf_out, NULL, close_on_finished_writecb_b_out, eventcb_b_out, bufs_in_out);
    //            return false;
    //        }
    //    }
    //    return true;
}

bool VerificaValiditaBufferIn ( bool isRead, struct bufferevent* b_in, T_EventoStruct* bufs_in_out )
{
    return bufs_in_out && bufs_in_out->p_buf_in;
    //    return true;
    //    try {
    //        //Molto molto difensiva!
    //        if (bufs_in_out == NULL) {
    //            if (b_in != NULL) {
    //                try {
    //                    std::cerr << StrTime() << "Close without bufs_in_out" << std::endl;
    //                    bufferevent_disable(b_in, EV_READ | EV_WRITE | EV_TIMEOUT);
    //                    bufferevent_free(b_in);
    //                }                catch (std::exception ex) {
    //                    std::cout << __func__   << StrTime() << ex.what() << std::endl;
    //                }
    //            }
    //            return false; //Non so cos'altro fare!
    //        }
    //
    //        return true;
    //
    //        if (b_in == NULL) {
    //            bufs_in_out->closed = GetULLTime();
    //
    //            if (bufs_in_out->p_buf_in) {
    //                CloseOne(true, bufs_in_out);
    //            }
    //
    //            if (!bufs_in_out->buf_out_freed) {
    //                //Bin e' stato eliminato ma bout no!
    //                //devo terminare di scrivere su bout
    //                //e devo svuotare l'ingresso di bout
    //                struct evbuffer *evbout_in = bufferevent_get_input(bufs_in_out->p_buf_out);
    //                int evbout_in_len = evbuffer_get_length(evbout_in);
    //                evbuffer_drain(evbout_in, evbout_in_len);
    //
    //                bufferevent_setcb(bufs_in_out->p_buf_out, NULL, close_on_finished_writecb_b_out, eventcb_b_out, bufs_in_out);
    //                return false;
    //            }
    //        } else {
    //            if (bufs_in_out->buf_out_freed) {
    //                bufs_in_out->closed = GetULLTime();
    //
    //                //Bout e' stato eliminato. Scarico tutto quello che ho letto da b_in ma non posso scaricare quello che dovrei scrivere su bin
    //                struct evbuffer *src = bufferevent_get_output(bufs_in_out->p_buf_in);
    //                int src_len = evbuffer_get_length(src);
    //                evbuffer_drain(src, src_len);
    //
    //                bufferevent_setcb(bufs_in_out->p_buf_in, NULL, close_on_finished_writecb_b_in, eventcb_b_in, bufs_in_out);
    //                return false;
    //            }
    //        }
    //        return true;
    //    }    catch (std::exception ex) {
    //        std::cout << __func__   << StrTime() << ex.what() << std::endl;
    //        return false;
    //    }
}

void CloseOneAndWaitWriteOther ( bool isIn, T_EventoStruct* bufs_in_out )
{
    try
    {
        struct evbuffer* b_write;
        struct evbuffer* b_read;
        struct evbuffer* b2_read;

        if ( isIn )
        {
            b_write = bufferevent_get_output ( bufs_in_out->p_buf_out );
            b_read = bufferevent_get_input ( bufs_in_out->p_buf_out );
            b2_read = bufferevent_get_input ( bufs_in_out->p_buf_in );
        }
        else
        {
            b_write = bufferevent_get_output ( bufs_in_out->p_buf_in );
            b_read = bufferevent_get_input ( bufs_in_out->p_buf_in );
            b2_read = bufferevent_get_input ( bufs_in_out->p_buf_out );
        }

        //Svuota tutti i buffer
        //uno in lettura scrittura
        evbuffer_drain ( b_write, evbuffer_get_length ( b_write ) );
        evbuffer_drain ( b_read, evbuffer_get_length ( b_read ) );

        //l'altro in lettura
        evbuffer_drain ( b2_read, evbuffer_get_length ( b2_read ) );

        //Chiude i canali che non servono e si predispone alla chiusura totale
        if ( isIn )
        {
            CloseOne ( true, bufs_in_out );

            if ( bufs_in_out && bufs_in_out->p_buf_out && !CloseOneIfEmpty ( false, bufs_in_out ) )
            {
                bufferevent_setcb ( bufs_in_out->p_buf_out, NULL, close_on_finished_writecb_b_out, eventcb_b_out, bufs_in_out );
            }
        }
        else
        {
            CloseOne ( false, bufs_in_out );

            if ( bufs_in_out && bufs_in_out->p_buf_in && !CloseOneIfEmpty ( true, bufs_in_out ) )
            {
                bufferevent_setcb ( bufs_in_out->p_buf_in, NULL, close_on_finished_writecb_b_in, eventcb_b_in, bufs_in_out );
            }
        }

    }
    catch ( std::exception& ex )
    {
	mtfa_log("%s\n", ex.what());
    }

    return;
}

void drained_writecb_b_in ( struct bufferevent* b_in, void* ctx )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) ctx;

    // We were choking the other side until we drained our outbuf a bit.
    // Now it seems drained.
    mtfa_log("In drained writecb_b_in\n");

    if ( b_in )
    {
        bufferevent_setcb ( b_in, readcb_b_in, writecb_b_in, eventcb_b_in, ctx );
        bufferevent_setwatermark ( b_in, EV_WRITE, 0, 0 );

        if ( bufs_in_out->p_buf_out )
        {
            bufferevent_enable ( bufs_in_out->p_buf_out, EV_READ );
        }
    }
}

void drained_writecb_b_out ( struct bufferevent* b_out, void* ctx )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) ctx;
    mtfa_log("In drained writecb_b_out\n");

    // We were choking the other side until we drained our outbuf a bit.
    //Now it seems drained.
    if ( b_out )
    {
        bufferevent_setcb ( b_out, readcb_b_out, writecb_b_out, eventcb_b_out, ctx );
        bufferevent_setwatermark ( b_out, EV_WRITE, 0, 0 );

        if ( bufs_in_out->p_buf_in )
        {
            bufferevent_enable ( bufs_in_out->p_buf_in, EV_READ );
        }
    }
}

//Dal server al client
//Se ci sono i dati, allora metto a true il fatto che last_was_in_0
void readcb_b_out ( struct bufferevent* b_out, void* ctx )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) ctx;

    if ( !VerificaValiditaBufferOut ( true, b_out, bufs_in_out ) )
    {
        return;
    }

    struct evbuffer* src = bufferevent_get_input ( b_out );

    int src_len = evbuffer_get_length ( src );

    if ( src_len <= 0 )
    {
        return; //Inutile proseguire se non ho dati!
    }

    if ( !ctx )
    {
        evbuffer_drain ( src, src_len );
        return;
    }

    unsigned long long processing_time = GetULLTime();

    bufs_in_out->last_received_from_server = GetULLTime();

    if ( bufs_in_out->first_received_from_server == 0 )
    {
        bufs_in_out->first_received_from_server = GetULLTime();
    }

    bufs_in_out->last_was_in_0 = true;

    bool justfoundheaders = false;

    if ( !bufs_in_out->headerFound_sc && bufs_in_out->protocollo != Protocols::eProtTCP )
    {
        //Allora, ho ricevuto tutti gli headers? Non lo so. Proviamo!
        //Prendo tutti gli headers... (ricorda che non ho messo il doppio fine riga finale!)
        int headersLen = 0;
        char* headersPtr = NULL;

        //C'e' il terminatore degli headers?
        struct evbuffer_ptr ptrFineHeader = evbuffer_search ( src, sEndHeaders.c_str(), sEndHeaders.size(), NULL );

        if ( ptrFineHeader.pos == -1 )
        {
            //Ok, non ho ancora preso tutti gli headers, aspetto e proseguo con la bufferizzazione dell'input!
            bufs_in_out->processing_time += GetULLTime() - processing_time;

            return;
        }
        else
        {
            //se c'è un content-length ragionevole, imposto da un parametro di configurazione, e se non ho ancora ricevuto i dati, attendo.
            //Trovata terminazione degli headers. Linearizza fino a questo punto, con il terminatore di fine headers
            headersPtr = ( char* ) evbuffer_pullup ( src, ptrFineHeader.pos + 4 );
            headersLen = ptrFineHeader.pos + 4;
            bufs_in_out->headerFound_sc = true;
            bufs_in_out->header_sc.assign ( headersPtr, headersLen );
            //cout << "RESP: <" << bufs_in_out->header_sc << ">" << endl;
            //mtfa_log("LOGDATA-SC: <%s>\n", Base64::encode(headersPtr, headersLen).c_str());
            justfoundheaders = true; //li ho trovati in questa comunicazione!
        }

#if 0
        //TODO: Devo aggiungere la set del cookie, se non settato. ma andrebbe fato solo se la comunicazione passa attraverso la sonda. Quindi
        //la devo gestire da linguaggio! Questo codice non va bene, deve essere aggiornato!
        if ( false && bufs_in_out->tag_cookie.size() == 0 )
        {
            //Il cookie non c'è, lo inserisco!
            //appende una riga per settare questo cookie
            string new_header = string ( headersPtr, headersLen );
            new_header = new_header.insert ( headersLen - 4, "\r\nSet-Cookie: VAHDR100=0000000000000000000000000000000000000000; Path=/;" );

            //Toglie da src la parte di header e la sostituisce con la nuova
            evbuffer_drain ( src, headersLen );

            if ( !bufs_in_out->p_buf_in )
            {
                return;
            }

            struct evbuffer* dst = bufferevent_get_output ( bufs_in_out->p_buf_in );

            evbuffer_add ( dst, new_header.data(), new_header.size() );

            evbuffer_add_buffer ( dst, src );

            if ( evbuffer_get_length ( dst ) >= MAX_OUTPUT )
            {
                // We're giving the other side data faster than it can
                // pass it on.  Stop reading here until we have drained the
                // other side to MAX_OUTPUT/2 bytes.
                //std::cerr << "Set watermark per IN, non riesco a inviare dati da out a in\n";
                if ( bufs_in_out && bufs_in_out->p_buf_in && bufs_in_out->p_buf_out )
                {
                    bufferevent_setcb ( bufs_in_out->p_buf_in, readcb_b_in_0, drained_writecb_b_in, eventcb_b_in, bufs_in_out );
                    bufferevent_setwatermark ( bufs_in_out->p_buf_in, EV_WRITE, MAX_OUTPUT / 32 + 128, MAX_OUTPUT );
                    bufferevent_disable ( bufs_in_out->p_buf_out, EV_READ );
                }
            }

            bufs_in_out->processing_time += GetULLTime() - processing_time;
            bufs_in_out->last_received_from_server = GetULLTime();
            return;
        }
#endif
        //Inutile prendere cookie e altro se non serve.
        //In queste versioni aggiunge cookie se necessario
    }
    else
    {
        //cout << __func__   << "readcb_b_out: riconosciuto TCP\n";
    }

    bufs_in_out->data_out += evbuffer_get_length ( src );

    if ( bufs_in_out->p_buf_in && !bufs_in_out->buf_in_freed )
    {
        //salva le abilitazioni
        unsigned save_enabling = bufferevent_get_enabled ( bufs_in_out->p_buf_in );
        bufferevent_disable ( bufs_in_out->p_buf_in, EV_WRITE );

        if ( justfoundheaders )
            UpdateEvBufferAnswer ( src, bufs_in_out );

        struct evbuffer* dst = bufferevent_get_output ( bufs_in_out->p_buf_in );
        evbuffer_add_buffer ( dst, src );
        int lendst = evbuffer_get_length ( dst );
        
        if (LbUser_VA_LocalSH_on)
        {
//             bufs_in_out->answer_is_needed=true; // significa che ha risposto la readcb_out
            bufs_in_out->data_given_sh_cs=0;
            unsigned char* p = evbuffer_pullup ( dst, -1 );
            string dati ( p, p + lendst );
            SondaHwSimulator_Producer(bufs_in_out, false, (char*)p, lendst, false);
        }

        //Ripristina abilitazioni
        if ( save_enabling )
            bufferevent_enable ( bufs_in_out->p_buf_in, save_enabling );

        if ( lendst >= MAX_OUTPUT )
        {
            // We're giving the other side data faster than it can
            // pass it on.  Stop reading here until we have drained the
            // other side to MAX_OUTPUT/2 bytes.
            std::cerr << "Set watermark per IN, non riesco a inviare dati da out a in\n";

            if ( bufs_in_out && bufs_in_out->p_buf_in && bufs_in_out->p_buf_out )
            {
                bufferevent_setcb ( bufs_in_out->p_buf_in, readcb_b_in_0, drained_writecb_b_in, eventcb_b_in, bufs_in_out );
                bufferevent_setwatermark ( bufs_in_out->p_buf_in, EV_WRITE, MAX_OUTPUT / 32 + 128, MAX_OUTPUT );
                bufferevent_disable ( bufs_in_out->p_buf_out, EV_READ );
            }
        }
        else 
        {
            //??per quale motivo???
            bufferevent_setcb ( bufs_in_out->p_buf_in, readcb_b_in_0, writecb_b_in, eventcb_b_in, bufs_in_out );
        }
    }
    else
    {
        //Sono arrivati dati ma il canale di ingresso è chiuso! Chiudo anche l'uscita
        CloseOne ( false, bufs_in_out );
    }

    bufs_in_out->processing_time += GetULLTime() - processing_time;
    bufs_in_out->last_received_from_server = GetULLTime();
}

//bool doMySqlQuery(std::list<std::list<std::string> > * result, const std::string the_query, T_EventoStruct * bufs_in_out, unsigned long long * lastid = nullptr) {
//    result->clear();
//
//    mysql_db_class * mydb;
//    mydb = new mysql_db_class(LbUser_DbIp.c_str(), LbUser_DbUser.c_str(), LbUser_DbPasswd.c_str(), LbUser_DbName.c_str(), 0);
//    if (!mydb) {
//        //printf("BCV: ERRORE IN ESECUZIONE QUERY AL DB VA: %s", the_query.c_str());
//        delete mydb;
//        return false;
//    }
//
//    if (!mydb->do_connect()) {
//        //printf("BCV: ERRORE IN ESECUZIONE QUERY AL DB VA: %s", the_query.c_str());
//        delete mydb;
//        return false;
//    }
//
//    if (mydb->do_query(the_query.c_str()) == false) {
//        //printf("BCV: ERRORE IN ESECUZIONE QUERY AL DB VA: %s", the_query.c_str());
//        mydb->do_disconnect();
//        delete mydb; //tanto per ricordarsi di chiudere il DB
//        return false;
//    }
//
//    for (int r = 0; r < mydb->getRows(); r++) {
//        std::list<std::string> line;
//        for (int i = 0; i < mydb->getCols(); ++i) {
//            std::string val = mydb->getResult(r, i);
//            line.push_back(val);
//        }
//        result->push_back(line);
//    }
//
//    if (lastid) {
//        *lastid = mydb->GetLastId();
//    }
//
//    mydb->do_disconnect();
//    delete mydb; //tanto per ricordarsi di chiudere il DB
//    return true;
//}
//
//mysql_db_class * doMySqlOpenDb() {
//    mysql_db_class * mydb;
//    mydb = new mysql_db_class(LbUser_DbIp.c_str(), LbUser_DbUser.c_str(), LbUser_DbPasswd.c_str(), LbUser_DbName.c_str(), 0);
//    if (!mydb) {
//        return nullptr;
//    }
//
//    if (!mydb->do_connect()) {
//        delete mydb;
//        return nullptr;
//    }
//    return mydb;
//}
//
//bool doMySqlQueryDb(mysql_db_class * mydb, std::list<std::list<std::string> > * result, const std::string the_query, T_EventoStruct * bufs_in_out, unsigned long long * lastid = nullptr) {
//    result->clear();
//
//    if (!mydb)
//        return false;
//
//    if (mydb->do_query(the_query.c_str()) == false) {
//        //printf("BCV: ERRORE IN ESECUZIONE QUERY AL DB VA: %s", the_query.c_str());
//        return false;
//    }
//
//    //printf("Dimensione del risultato. Righe: %d, Colonne: %d\n", mydb->getRows(), mydb->getCols());
//    for (int r = 0; r < mydb->getRows(); r++) {
//        std::list<std::string> line;
//        for (int i = 0; i < mydb->getCols(); ++i) {
//            std::string val = mydb->getResult(r, i);
//            line.push_back(val);
//            //std::cout << __func__   << val << ", ";
//        }
//        result->push_back(line);
//    }
//
//    if (lastid) {
//        *lastid = mydb->GetLastId();
//    }
//
//    return true;
//}
//
//void doMySqlRollBack(mysql_db_class * mydb) {
//    std::list<std::list<std::string> > result;
//    doMySqlQueryDb(mydb, &result, "roolback", nullptr);
//}
//
//void doMySqlCommit(mysql_db_class * mydb) {
//    std::list<std::list<std::string> > result;
//    doMySqlQueryDb(mydb, &result, "commit", nullptr);
//}
//
//bool doMySqlCloseDb(mysql_db_class * mydb) {
//    if (!mydb) {
//        return false;
//    }
//
//    mydb->do_disconnect();
//    delete mydb; //tanto per ricordarsi di chiudere il DB
//    return true;
//}

bool local_find_pattern ( const char * p, const int len, pcre2_code* re )
{
    pcre2_match_data* match_data = nullptr;
    int rc;

    if ( !re )
        return false;

    match_data = pcre2_match_data_create_from_pattern ( re, NULL );
    rc = pcre2_jit_match (
             re,                   /* the compiled pattern */
             (unsigned char*)p,    /* the subject string */
             len,                  /* the length of the subject */
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


void readcb_b_in ( struct bufferevent* b_in, void* ctx )
{
    try
    {
        unsigned long long processing_time = GetULLTime();

        T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) ctx;

        if ( !VerificaValiditaBufferIn ( true, b_in, bufs_in_out ) )
        {
            return;
        }

        struct evbuffer* src = bufferevent_get_input ( b_in );

        int src_len = evbuffer_get_length ( src );

        if ( src_len <= 0 )
        {
            goto termina_operazioni; //Tanto per uscire correttamente!
        }

        if (LbUser_VA_LocalSH_on)
        {
            bufs_in_out->data_given_sh_cs=0;
            unsigned char* p = evbuffer_pullup ( src, src_len );
            string dati ( p, p + src_len );
            if (src_len)
                SondaHwSimulator_Producer(bufs_in_out, true, (char*)p, src_len, false);
        }

        if ( !ctx )
        {
            evbuffer_drain ( src, src_len );
            return;
        }

        bufs_in_out->last_was_in_0 = true;

        //Ma che accade se trovo una GET o una POST? riparto con read_cb_in_0 ma solo se non è tcpip
        if ( bufs_in_out->protocollo != Protocols::eProtTCP )   //QUESTO E' PIUTTOSTO ARBITRARIO DA FARE!!! MA ALTRIMENTI CHE COSA FARESTI?
        {
            static pcre2_code * methods_pattern = mtfa_compile_pattern("^ACL |^BASELINE-CONTROL |^BIND |^CHECKIN |^CHECKOUT |^CONNECT |^COPY |^DELETE |^GET |^HEAD |^LABEL |^LINK |^LOCK |^MERGE |^MKACTIVITY |^MKCALENDAR |^MKCOL |^MKREDIRECTREF |^MKWORKSPACE |^MOVE |^OPTIONS |^ORDERPATCH |^PATCH |^POST |^PRI |^PROPFIND |^PROPPATCH |^PUT |^REBIND |^REPORT |^SEARCH |^TRACE |^UNBIND |^UNCHECKOUT |^UNLINK |^UNLOCK |^UPDATE |^UPDATEREDIRECTREF |^VERSION-CONTROL ");

            char* p = ( char* ) evbuffer_pullup ( src, src_len );
            if (local_find_pattern(p, src_len, methods_pattern))
//             if (( memcmp ( p, "GET ", 4 ) == 0 ) || ( memcmp ( p, "POST", 4 ) == 0 ) || ( memcmp ( p, "HEAD", 4 ) == 0 ) || ( memcmp ( p, "PROP", 4 ) == 0 ) || ( memcmp ( p, "OPTI", 4 ) == 0 ) || ( memcmp ( p, "PUT ", 4 ) == 0 ) || ( memcmp ( p, "DELE", 4 ) == 0 ) || ( memcmp ( p, "TRAC", 4 ) == 0 ))
            {
                //In questo modo ripristino le condizioni iniziali della setcb
                if ( b_in )
                {
                    bufferevent_setcb ( b_in, readcb_b_in_0, writecb_b_in, eventcb_b_in, bufs_in_out );
                }

                bufs_in_out->processing_time += GetULLTime() - processing_time;

                mtfa_log("Got a new HTTP request\n");
                bufs_in_out->data_given_sh_cs=0;
                readcb_b_in_0 ( b_in, ctx );
                return;
            }
        }
        else
        {
            //mtfa_log("Riconosciuto TCP. Chiama GHome ma prima elimina i tag\n");

            if ( bufs_in_out->tag_cookie.size() == 0 )
            {
                //non è tagged! faccio passare
                //mtfa_log("NON E' TAGGED!!!\n");
            }
            else
            {
                //In questo modo ripristino le condizioni iniziali della setcb
                if ( b_in )
                {
                    bufferevent_setcb ( b_in, readcb_b_in_0, writecb_b_in, eventcb_b_in, bufs_in_out );
                }

                bufs_in_out->processing_time += GetULLTime() - processing_time;

                mtfa_log("da readcb_b_in vado a readcb_b_in_0\n");
                bufs_in_out->last_was_in_0 = true;
                readcb_b_in_0 ( b_in, ctx );
            }
        }


        {
            struct evbuffer* dst = bufferevent_get_output ( bufs_in_out->p_buf_out );

            evbuffer_add_buffer ( dst, src );

            if ( evbuffer_get_length ( dst ) >= MAX_OUTPUT )
            {
                if ( bufs_in_out && bufs_in_out->p_buf_out && bufs_in_out->p_buf_in )
                {
                    bufferevent_setcb ( bufs_in_out->p_buf_out, readcb_b_out, drained_writecb_b_out, eventcb_b_out, bufs_in_out );
                    bufferevent_setwatermark ( bufs_in_out->p_buf_out, EV_WRITE, MAX_OUTPUT / 32 + 128, MAX_OUTPUT );
                    bufferevent_disable ( bufs_in_out->p_buf_in, EV_READ );
                }
            }
        }

    termina_operazioni:

        if ( bufs_in_out->first_sent_to_server == 0 )
        {
            bufs_in_out->first_sent_to_server = GetULLTime();
        }

        bufs_in_out->processing_time += GetULLTime() - processing_time;
    }
    catch ( std::exception& ex )
    {
        mtfa_log("%s\n", ex.what());
    }
}

void readcb_b_in_0 ( struct bufferevent* b_in, void* ctx )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) ctx;

    if ( !VerificaValiditaBufferIn ( true, b_in, bufs_in_out ) )
    {
        if ( bufs_in_out )
        {
            bufs_in_out->closed = GetULLTime();
        }
        return;
    }

    struct evbuffer* src;

    src = bufferevent_get_input ( b_in );

    int srcLen = evbuffer_get_length ( src );

    if ( srcLen <= 0 )
    {
        return;     //inutile operare, non abbiamo nulla!
    }

    if ( !ctx )
    {
        evbuffer_drain ( src, srcLen );
        return;
    }

    //Il tempo di processing del nostro sw
    unsigned long long processing_time = GetULLTime();

    //per sicurezza azzero alcune variabili globali
    string s_dest;
    unsigned short u_port;
    string s_port;

    string hostName;
    string s_portNum;
    unsigned short portNum;
    string s_url;

    s_dest.clear();
    u_port = 0;
    s_port.clear();

    hostName.clear();
    s_portNum.clear();
    portNum = 0;
    s_url.clear();

    struct evbuffer_ptr ptrFineHeader;

    if ( bufs_in_out->last_was_in_0 )
    {
        //Creo un bio nuovo e lo sostituisco al vecchio che lascio in coda.
        T_EventoStruct* bio = new T_EventoStruct();

        bio->protocollo = bufs_in_out->protocollo;
        bio->accepted_client = bufs_in_out->accepted_client;
//          std::swap ( bio->answer, bufs_in_out->answer );

        bio->bind_ip = bufs_in_out->bind_ip;
        bio->buf_in_freed = bufs_in_out->buf_in_freed;
        bio->buf_out_freed = bufs_in_out->buf_out_freed;

        //La somma dei tempi di processing del ns sw!
        bio->processing_time = bufs_in_out->processing_time;
        bufs_in_out->processing_time = 0;

        bio->client_ip = bufs_in_out->client_ip;
        bio->client_port = bufs_in_out->client_port;

        if ( bio->closed == 0 )
        {
            bio->closed = GetULLTime();     //Marco il momento in cui ho chiuso la transazione (anche se il socket ? rimasto aperto!)
        }

        bio->data_in = bufs_in_out->data_in;
        bio->data_out = bufs_in_out->data_out;

        bio->connected_to_server = bufs_in_out->connected_to_server;
        bio->last_received_from_server = bufs_in_out->last_received_from_server;
        bio->first_received_from_server = bufs_in_out->first_received_from_server;
        bio->first_sent_to_server = bufs_in_out->first_sent_to_server;
        bio->first_received_from_client = bufs_in_out->first_received_from_client;

        bio->headerFound_cs = bufs_in_out->headerFound_cs;
        bio->headerFound_sc = bufs_in_out->headerFound_sc;
//           bio->host = bufs_in_out->host;
        bio->last_was_in_0 = bufs_in_out->last_was_in_0;
        bio->listen_ip = bufs_in_out->listen_ip;
        bio->listen_port = bufs_in_out->listen_port;
        bio->p_buf_in = NULL;
        bio->p_buf_out = NULL;
        bio->server_ip = bufs_in_out->server_ip;
        bio->server_port = bufs_in_out->server_port;
        bio->toBeConnected = bufs_in_out->toBeConnected;
        bio->UniqueId = bufs_in_out->UniqueId;
        bio->tag_cookie = bufs_in_out->tag_cookie;

        bio->b_add_header_to_answer = bufs_in_out->b_add_header_to_answer;
        bio->add_header_to_answer = bufs_in_out->add_header_to_answer;
        bio->b_add_header_to_request = bufs_in_out->b_add_header_to_request;
        bio->add_header_to_request = bufs_in_out->add_header_to_request;
        
        bio->replace_header_in_answer = bufs_in_out->replace_header_in_answer;
        bio->replace_header_in_request = bufs_in_out->replace_header_in_request;
        bio->data_given_sh_cs = bufs_in_out->data_given_sh_cs;
        
        bufs_in_out->tag_cookie.clear();

//           std::swap ( bio->url, bufs_in_out->url );

        T_list_rule_change* ltrc = bufs_in_out->ltrc;
        struct bufferevent* p_buf_in = bufs_in_out->p_buf_in;
        struct bufferevent* p_buf_out = bufs_in_out->p_buf_out;

        bio->ServerIsSSL = bufs_in_out->ServerIsSSL;
        bio->server_cert = bufs_in_out->server_cert;
        bio->server_host = bufs_in_out->server_host;
        bio->client_cert = bufs_in_out->client_cert;
        bio->client_key = bufs_in_out->client_key;
        bio->client_method = bufs_in_out->client_method;
        bio->client_algo = bufs_in_out->client_algo;
        bio->verify_server_cert = bufs_in_out->verify_server_cert;
        bio->ssl_ctx = bufs_in_out->ssl_ctx;
        bio->server_name = bufs_in_out->server_name;

        bufs_in_out->Init();

        //TANTO PER SICUREZZA
        bufs_in_out->accepted_client = GetULLTime();

        bufs_in_out->ltrc = ltrc;
        bufs_in_out->p_buf_in = p_buf_in;
        bufs_in_out->p_buf_out = p_buf_out;
        bufs_in_out->client_ip = bio->client_ip;
        bufs_in_out->client_port = bio->client_port;
        bufs_in_out->connected_to_server = bio->connected_to_server;
        bufs_in_out->listen_ip = bio->listen_ip;
        bufs_in_out->listen_port = bio->listen_port;
        bufs_in_out->mac_src = bio->mac_src;
        bufs_in_out->toBeConnected = bio->toBeConnected;
        bufs_in_out->protocollo = bio->protocollo;

        bufs_in_out->ServerIsSSL = bio->ServerIsSSL;
        bufs_in_out->server_cert = bio->server_cert;
        bufs_in_out->server_host = bio->server_host;
        bufs_in_out->client_cert = bio->client_cert;
        bufs_in_out->client_key = bio->client_key;
        bufs_in_out->client_method = bio->client_method;
        bufs_in_out->client_algo = bio->client_algo;
        bufs_in_out->verify_server_cert = bio->verify_server_cert;
        bufs_in_out->ssl_ctx = bio->ssl_ctx;
        bufs_in_out->server_name = bio->server_name;
        bufs_in_out->data_given_sh_cs = 0;
        

        //se è un'altra comunicazione sullo stesso socket, non è necessario mantenere le modifiche richieste dalle regole di accesso, se dovesse servire lo
        //faranno le regole stesse di nuovo!
//          bufs_in_out->b_add_header_to_answer = false;
//          bufs_in_out->b_add_header_to_request = false;
//          bufs_in_out->add_header_to_answer.clear();
//          bufs_in_out->add_header_to_request.clear();

        //Bio va aggiunto alla lista degli eventi in modo che venga eliminato!
        //listaEventi->AddEvento(bio);
        delete bio;
    }

    if (LbUser_VA_LocalSH_on)
    {
        unsigned char* p = evbuffer_pullup ( src, srcLen );

        if (srcLen > bufs_in_out->data_given_sh_cs)
        {
            SondaHwSimulator_Producer(bufs_in_out, true, (char*)&p[bufs_in_out->data_given_sh_cs], srcLen-bufs_in_out->data_given_sh_cs, false);
            bufs_in_out->data_given_sh_cs = srcLen;
        }
    }

    //OK, e' la prima volta che passo qui, non sono ancora stato usato!
    //Ma essendo io in_0, e' sempre la prima volta quando passi di qui, tranne quando ho richiesto di riprovare

    if ( bufs_in_out->first_received_from_client == 0 )
    {
        bufs_in_out->first_received_from_client = GetULLTime();
    }

    //Se è una comunicazione TCP, chiamo direttamente lo strato LSP
    if ( bufs_in_out->protocollo == Protocols::eProtTCP )
    {
        //Disabilita li gestore eventi su questo socket
        bufferevent_disable( bufs_in_out->p_buf_in, EV_READ | EV_WRITE ); // | EV_TIMEOUT);
        
        //TODO: da correggere per gestire tutti i protocolli tcp, sia taggati che non!
        //mtfa_log("Riconosciuto TCP. Chiama GHome ma prima elimina i tag\n");
#if 0
        //"VAHDR100"  4 byte little endian lunghezza e poi i dati: 12 byte di tag + dati.
        //C'è il TAG?
//        struct evbuffer_ptr ptrTag;
//        ptrTag = evbuffer_search(src, "VAHDR100", 8, NULL);
//        if (ptrFineHeader.pos == -1) {

        const int prefix_len = 32 + 20; //+ 20 per il session id
        unsigned char* p = evbuffer_pullup ( src, prefix_len );

        if ( p == nullptr || memcmp ( "VAHDR100", p, 8 ) != 0 )
        {
            //ricominciamo
            cout << __func__   << "Attenzione protocollo non taggato! Mando lo stesso" << endl;
            GoHome ( bufs_in_out, "192.168.167.10", "32999", "", "", "", "", "", 0 );
            return;
        }

        //il protocollo è taggato, prendo la parte dati
        int len = * ( unsigned int* ) &p[28 + 20];
        cout << __func__   << "Parte dati: " << len << endl;

        //Ora prendo i dati, se non sono tutti, attendo
        if ( srcLen >= ( 32 + 20 + len ) )
        {
            cout << __func__   << "Ok, ho tutti i dati, proseguo indicando quanta parte dei dati deve rimanere in src per la prossima call\n";
            bufs_in_out->tag_cookie.assign ( p, p + 8 );
            bufs_in_out->idsonda.assign ( p + 8, p + 28 + 20 );
            cout << __func__   << "TAG: " <<  bufs_in_out->tag_cookie << endl;
            cout << __func__   << "IDS: " <<  bufs_in_out->idsonda << endl;
            evbuffer_drain ( src, prefix_len );
            GoHome ( bufs_in_out, "192.168.167.10", "32999", "", "", "", "", "", srcLen - prefix_len - len );
            return;
        }
        else
        {
            cout << __func__   << "non ho ancora tutti i dati, aspetto!\n";
            //ricominciamo
            bufs_in_out->last_was_in_0 = false;
            return;
        }

        /*se il protocollo è tcp, allora ho due casi
         * 1) protocollo controllato
         *  - devo scapsulare il protocollo
         * 2) protocollo non controllato
         *  - devo solo gestire
         * Per ora gestiamo solo e quindi ogni volta prendo tutto
        */

        //per prima cosa verifico che sia un protocollo "TAGGATO" oppure no
//        if (tcp_tagged_ip_and_ports.find(bufs_in_out->server_ip + ":" + to_string(bufs_in_out->server_port))!=tcp_tagged_ip_and_ports.end())
//        {
//            //Questo protocollo deve essere tagged!
//            //10 di prefisso
//            //10 di idsonda
//            //12 di OTP
//            const header_len = 32;
//
//            const char prefix[] ="mTfA123ZX!";
//            const int prefix_len = strlen(prefix);
//
//            char * p = evbuffer_pullup(src, prefix_len);
//            if (p == nullptr)
//            {
//                //ricominciamo
//                bufs_in_out->last_was_in_0 = false;
//                return;
//            }
//            if (memcmp(prefix, p, prefix_len) != 0)
//            {
//                //chiude la connessione
//                SendAnswerAndCloseAll(bufs_in_out, nullptr, 0);
//                return;
//            }
//            //Ci siamo, prendiamo il protocollo!
//            if (srcLen < header_len)
//            {
//                //ricominciamo
//                bufs_in_out->last_was_in_0 = false;
//                return;
//            }
//
//            //Estraggo l'header
//            char * header = (char*)malloc(header_len);
//            evbuffer_remove(src, header, header_len);
//
//            //Ho tolto l'header, posso gestire i dati
//            bufs_in_out->idsonda = string(&header[10], &header[20]);
//            bufs_in_out->real_data_size = *(int*)&header[20];
//        }
#endif
        bufs_in_out->mac_src = ""; //GetRemotePeerMac(ip_and_interfaces, the_socket);
        bufs_in_out->content_length = srcLen;
        bufs_in_out->idsonda = "";  //in caso di codifica dei dati, qui prende l'id sonda

        return ThPoolEnqueueLsp (
                   bufs_in_out, //The io pointers
                   0, //0: recvd data from client, 1: recvd data from server
                   src,
                   ctx
               );
    }

    // Prendo tutti gli headers se non li ho ancora presi... (ricorda che non ho messo il doppio fine riga finale!)
    if ( !bufs_in_out->headerFound_cs || bufs_in_out->need_body )
    {
        int headersLen = 0;
        //int real_len_to_parse = 0;
        char* headersPtr = nullptr;

        //C'e' il terminatore degli headers?
        ptrFineHeader = evbuffer_search ( src, sEndHeaders.c_str(), sEndHeaders.size(), NULL );

        if ( ptrFineHeader.pos == -1 )
        {
            bufs_in_out->processing_time += GetULLTime() - processing_time;
            bufs_in_out->headerFound_cs = false;
            bufs_in_out->last_was_in_0 = false; //Ripasso di nuovo poiche' non ho terminato gli headers

            bufferevent_setcb ( bufs_in_out->p_buf_out, NULL, NULL, eventcb_b_out, ctx );
            bufferevent_enable ( bufs_in_out->p_buf_out, EV_READ | EV_WRITE );

            bufferevent_setcb ( bufs_in_out->p_buf_in, readcb_b_in_0, writecb_b_in, eventcb_b_in, ctx);
            bufferevent_enable ( bufs_in_out->p_buf_in, EV_READ | EV_WRITE ); // | EV_TIMEOUT);

            return;
        }
        else
        {
            //Trovata terminazione degli headers. Linearizza fino a questo punto, con il terminatore di fine headers
            //per evitare di dover richiedere ulteriori dati quando questi sono già disponibili, allora preferisco linearizzare tutti i dati qui e poi lavrarli

            headersLen = ptrFineHeader.pos + 4;
            headersPtr = ( char* ) evbuffer_pullup ( src, headersLen );
            bufs_in_out->headerFound_cs = true;
            //mtfa_log("LOGDATA-CS: <%s>\n", Base64::encode(headersPtr, headersLen).c_str());
        }

        //Sono passato per readcb_b_in_0, poiché ho preso gli headers di questa comunicazione.
        bufs_in_out->last_was_in_0 = true;

        bufs_in_out->header_cs.assign ( headersPtr, headersLen );
        bufs_in_out->headersLen = headersLen;
        
        int totalToRead = headersLen;

        if ( headersLen < srcLen || bufs_in_out->need_body )
        {
            headersPtr = ( char* ) evbuffer_pullup ( src, srcLen );
            totalToRead = srcLen;
        }

        try
        {
            GetHeadersDataCS ( ( char* ) headersPtr, totalToRead,
                               &bufs_in_out->current_query,
                               &bufs_in_out->current_headers,
                               &bufs_in_out->current_cookies,
                               &bufs_in_out->s_loc_method,
                               &bufs_in_out->s_loc_protocol,
                               &bufs_in_out->s_loc_host,
                               &bufs_in_out->s_loc_full_host,
                               &bufs_in_out->s_loc_port,
                               &bufs_in_out->s_loc_url,
                               &bufs_in_out->s_loc_pars,
                               &bufs_in_out->s_loc_tag,
                               &bufs_in_out->s_loc_user_passwd,
                               bufs_in_out->need_body,
                               &bufs_in_out->s_body
                             );
        }
        catch ( exception& ex )
        {
            mtfa_log ( "Eccezione in GetHeadersDataCS: %s\n", ex.what() );
        }
        
        //ora, se il metodo è una POST, allora need_body è true
        //Se è una post senza dati, allora prendo i dati
//         static auto ppost = mtfa_compile_pattern("^[pP][oO][sS][tT]$");
//         if ( mtfa_find_pattern(bufs_in_out->s_loc_method, ppost) || bufs_in_out->need_body )
        if ( bufs_in_out->need_body )
        {
            if (bufs_in_out->s_body.size() > 0 )
                bufs_in_out->need_body = false;
            else
            {
                //Ricominciamo... Il body non è ancora stato preso tutto!
                mtfa_log("Still waiting for full body...\n");
                bufs_in_out->processing_time += GetULLTime() - processing_time;
                bufs_in_out->headerFound_cs = false;
                bufs_in_out->last_was_in_0 = false; //Ripasso di nuovo poiche' non ho terminato gli headers
                bufferevent_setcb ( bufs_in_out->p_buf_out, NULL, NULL, eventcb_b_out, ctx );
                bufferevent_enable ( bufs_in_out->p_buf_out, EV_READ | EV_WRITE );

                bufferevent_setcb ( bufs_in_out->p_buf_in, readcb_b_in_0, writecb_b_in, eventcb_b_in, ctx);
                bufferevent_enable ( bufs_in_out->p_buf_in, EV_READ | EV_WRITE ); // | EV_TIMEOUT);
                return;
            }
        }

#if 0 //solo per una vecchia tag dei cookie
        auto tag_cookie_it = bufs_in_out->current_cookies.find ( "VAHDR100" );

        if ( tag_cookie_it != bufs_in_out->current_cookies.end() )
        {
            bufs_in_out->tag_cookie = tag_cookie_it->second;
        }
        else
        {
            bufs_in_out->tag_cookie.clear();
        }
#endif
        //Metto l'uri completa senza decodifiche
        static pcre2_code* reCompiled = mtfa_compile_pattern ( "^([A-Za-z_-]+ )([^ ]+)" );
        bool found = mtfa_get_specific_submatching_pattern ( string ( headersPtr, headersPtr + headersLen ), reCompiled, 2, bufs_in_out->s_loc_uri );
    }

    //ho gli headers in bufs_in_out!!
    //cout << "Chiesto: " << bufs_in_out->s_loc_url << endl;
    s_url = bufs_in_out->s_loc_url; //(hdrMgm.GetUrl());

    hostName = bufs_in_out->s_loc_host;
    s_portNum = bufs_in_out->s_loc_port;

    //Se ho chiesto tutti i dati lo setto oppure se è presente in headers
    if (bufs_in_out->content_length == 0)
        bufs_in_out->content_length = bufs_in_out->s_body.size();
    
    if (bufs_in_out->s_body.size() > 0)
    {
        try {
            //se è un multipart, lo decifro! Ma è necessario decifrarlo qui? Non potrei decifrarlo solo nel caso 
            //in cui sia necessario? Ma così è, poiché il body viene preso solo su richiesta.
            static pcre2_code * multipart_decode = mtfa_compile_pattern ( "multipart/([^;]*); *boundary=(.*)$", true );
            auto mp = bufs_in_out->current_headers.find("content-type");
            //mtfa_log("Analysing multipart data: <%s>\n", bufs_in_out->s_body.c_str());
            if (mp != bufs_in_out->current_headers.end())
            {
                //OK! è multipart. Prendo il codice
                auto intestazione = mtfa_get_all_submatching_patterns(mp->second, multipart_decode);
                //Se ha matchato, allora il primo è il tipo di multipart e il secondo è il boundary!
                if (intestazione.size() == 3)
                {
                    auto it=intestazione.begin(); ++it;
                    string tipo_multipart = *it; ++it;
                    string boundary = "--" + *it;

                    multipart::multipart_parser_settings callbacks;
                    callbacks.lmr.clear();
                    callbacks.on_body_end=nullptr;
                    callbacks.on_header_field=nullptr;
                    callbacks.on_header_value=nullptr;
                    callbacks.on_headers_complete=nullptr;
                    callbacks.on_part_data=nullptr;
                    callbacks.on_part_data_begin=nullptr;
                    callbacks.on_part_data_end=nullptr;

                    callbacks.on_header_field = multipart::read_header_name;
                    callbacks.on_header_value = multipart::read_header_value;
                    callbacks.on_part_data = multipart::read_data;

                    multipart::multipart_parser* parser = multipart::multipart_parser_init(boundary.c_str(), &callbacks);
                    multipart::multipart_parser_execute(parser, bufs_in_out->s_body.data(), bufs_in_out->s_body.size());
                    //Ora posso estrarre, se presenti, i dati multipart
                    bufs_in_out->lmr = move(parser->settings->lmr);
                    multipart::multipart_parser_free(parser);
                    
//                     for (auto it: bufs_in_out->lmr)
//                     {
//                         cout << "name: "  << it.name << endl;
//                         cout << "value: " << it.value << endl;
//                         cout << "data: " << it.data << endl << endl;
//                     }
                }
            }
        } catch (exception ex)
        {
            mtfa_log("Eccezione: %s\n", ex.what());
        }
    }

//     //Prendo tutto se serve, altrimenti no e prendo solo headers
//     size_t real_src_len = bufs_in_out->headersLen + bufs_in_out->content_length;

//     //Se ho il body lo prendo!
//     if ( bufs_in_out->content_length > 0 )
//     {
//         if ( srcLen >= real_src_len )
//         {
//             bufs_in_out->s_body.assign ( ( char* ) evbuffer_pullup ( src, real_src_len ), real_src_len );
//             bufs_in_out->s_body = bufs_in_out->s_body.substr ( bufs_in_out->headersLen ); //ho preso la parte dati
//             //cout << "BODY: " << bufs_in_out->s_body << endl;
//         }
//         else
//         {
//             bufs_in_out->last_was_in_0 = false; //ripasso poiché non ho terminato!
//             //non ho dati sufficienti per prendere il body, rispondo con 100 continue!
//             //string answer = "HTTP/1.1 100 CONTINUE\r\n\r\n";
//             //SendAnswerAndNotClose ( bufs_in_out, answer.data(), answer.size() );
//             return;
//         }
//     }
//     else
//     {
//         //Altrimenti NO!
//         bufs_in_out->s_body.clear();
//
//         if ( bufs_in_out->need_body )
//         {
//             bufs_in_out->last_was_in_0 = false; //Ripasso di nuovo poiche' non ho terminato gli headers
//             return;
//         }
//     }

//    if ((srcLen ) bufs_in_out->need_body && bufs_in_out->s_body.size() != bufs_in_out->content_length)
//    {
//        if (srcLen >= real_src_len) {
//            bufs_in_out->s_body.assign((char*) evbuffer_pullup(src, real_src_len), real_src_len);
//            bufs_in_out->s_body = bufs_in_out->s_body.substr(bufs_in_out->headersLen); //ho preso la parte dati
//        } else {
//            //Non ho dati sufficienti!
//            bufs_in_out->s_body.clear();
//            if (bufs_in_out->need_body) {
//                bufs_in_out->last_was_in_0 = false; //Ripasso di nuovo poiche' non ho terminato gli headers
//                return;
//            }
//        }
//    }

    //Per tutte le comunicazioni, marco il mio cookie, se sono in livello 3
    //prendo mac del sorgente
    //static std::map<string, string> ip_and_interfaces = GetIpAndInterfaces();
    //int the_socket = bufferevent_getfd(b_in);
    string mac_src = ""; //GetRemotePeerMac(ip_and_interfaces, the_socket);
    bufs_in_out->mac_src = mac_src;

//    {
//        cerr << "READCBIN0-1: <" << bufs_in_out->header_cs << ">\n";
//    }

    ThPoolEnqueueLsp (
        bufs_in_out, //The io pointers
        0, //0: recvd data from client, 1: recvd data from server
        src,
        ctx
    );
    return;
}

void close_on_finished_writecb_b_in ( struct bufferevent* b_in, void* ctx )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) ctx;

    struct evbuffer* b = bufferevent_get_output ( b_in );

//     int srclen = evbuffer_get_length ( b );

//     if (LbUser_VA_LocalSH_on)
//     {
//         //se sono server, allora il secondo socket è a null
//         T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) ctx;
//         if (!bufs_in_out->answer_is_needed)
//         {
//             unsigned char* p = evbuffer_pullup ( b, srclen );
//             string dati ( p, p + srclen );
//             //cout << "1-SID: " << bufs_in_out->UniqueId << ". Dati(" << srclen << "): " << dati << endl;
//             if (srclen)
//                 SondaHwSimulator_Producer(bufs_in_out, false, (char*)p, srclen, false);
//         }
//         else
//         {
//             bufs_in_out->answer_is_needed=false;
//             //cout << "il secondo socket non sta a null\n";
//         }
//     }

    if ( evbuffer_get_length ( b ) == 0 )
    {
        CloseOne ( true, bufs_in_out );

        if ( bufs_in_out->closed == 0 )
        {
            bufs_in_out->closed = GetULLTime();
        }
    }
}

void writecb_b_in ( struct bufferevent* b_in, void* ctx )
{
//     T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) ctx;
//     struct evbuffer* b = bufferevent_get_output ( b_in );
//     int srclen = evbuffer_get_length ( b );

//     if (LbUser_VA_LocalSH_on)
//     {
//         //cout << "Sono qui e verifico se il secondo socket sta a null\n";
//         //se sono server, allora il secondo socket è a null
//         if (!bufs_in_out->answer_is_needed)
//         {
//             //cout << "il secondo socket sta a null\n";
//             unsigned char* p = evbuffer_pullup ( b, srclen );
//             string dati ( p, p + srclen );
//             //cout << "2-SID: " << bufs_in_out->UniqueId << ". Dati(" << srclen << "): " << dati << endl;
//             if (srclen)
//                 SondaHwSimulator_Producer(bufs_in_out, false, (char*)p, srclen, false);
//         }
//         else
//         {
//             bufs_in_out->answer_is_needed=false;
//             //cout << "il secondo socket non sta a null\n";
//         }
// 
//     }
}

void writecb_b_out ( struct bufferevent* b_out, void* ctx )
{
}

void close_on_finished_writecb_b_out ( struct bufferevent* b_out, void* ctx )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) ctx;

    struct evbuffer* b = bufferevent_get_output ( b_out );

    if ( evbuffer_get_length ( b ) == 0 )
    {
        CloseOne ( false, bufs_in_out );

        if ( bufs_in_out->closed == 0 )
        {
            bufs_in_out->closed = GetULLTime();
        }
    }

//    else {
//        std::cout << __func__   << " - " << evbuffer_get_length(b) << endl;
//    }
}

void eventcb_b_out ( struct bufferevent* b_out, short what, void* ctx )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) ctx;

//     if ( !VerificaValiditaBufferOut ( false, b_out, bufs_in_out ) )
//     {
//         mtfa_log("Buffer out not valid\n");
//         return;
//     }

    if ( what & ( BEV_EVENT_EOF | BEV_EVENT_ERROR ) )
    {
        if ( what & BEV_EVENT_ERROR )
        {
            mtfa_log ( "connection error" );
            if (bufs_in_out)
                std::cerr << StrTime() << "BIND IP: " << bufs_in_out->bind_ip << "\n\tDEST IP: " << bufs_in_out->server_ip << ":" << bufs_in_out->server_port << "\n\tURI: " << bufs_in_out->s_loc_host << ":/" << bufs_in_out->s_loc_url << std::endl;
        }

        if ( bufs_in_out && bufs_in_out->closed == 0 )
        {
            bufs_in_out->closed = GetULLTime();
        }

        if ( bufs_in_out && bufs_in_out->last_received_from_server == 0 )
        {
            bufs_in_out->last_received_from_server = GetULLTime();
        }

        CloseOne ( false, bufs_in_out );

        //Ci sono dati in fase di scrittura su b_in?
        if ( bufs_in_out && bufs_in_out->p_buf_in )
        {
            struct evbuffer* dst = bufferevent_get_output ( bufs_in_out->p_buf_in );

            if ( dst && evbuffer_get_length ( dst ) )
            {
                bufferevent_setcb ( bufs_in_out->p_buf_in, NULL, close_on_finished_writecb_b_in, eventcb_b_in, bufs_in_out );
            }
            else
            {
                CloseOne ( true, bufs_in_out );
            }
        }
    }
    else if ( what & BEV_EVENT_CONNECTED )
    {
        bufs_in_out->connected_to_server = GetULLTime();
//         bufferevent_setcb ( bufs_in_out->p_buf_out, readcb_b_out, writecb_b_out, eventcb_b_out, bufs_in_out );
//         bufferevent_enable ( bufs_in_out->p_buf_out, EV_READ | EV_WRITE ); // | EV_TIMEOUT);
        bufs_in_out->toBeConnected = false;
    }
    else
    {
        mtfa_log("Error: %s\n", what);
    }
}

void eventcb_b_in ( struct bufferevent* b_in, short what, void* ctx )
{
    T_EventoStruct* bufs_in_out = ( T_EventoStruct* ) ctx;

    if ( !VerificaValiditaBufferIn ( false, b_in, bufs_in_out ) )
    {
        cout << __func__ << " - " << "Buffer in not valid" << endl;
        return;
    }

    if ( what & ( BEV_EVENT_EOF | BEV_EVENT_ERROR ) )
    {
        if (LbUser_VA_LocalSH_on)
        {
            if (b_in == bufs_in_out->p_buf_in)
            {
                bufs_in_out->data_given_sh_cs=0;
                SondaHwSimulator_Producer(bufs_in_out, true, nullptr, 0, true);
            }
            else
            {
                bufs_in_out->data_given_sh_cs=0;
                SondaHwSimulator_Producer(bufs_in_out, false, nullptr, 0, true);
            }

            if ( bufs_in_out->closed == 0 )
            {
                bufs_in_out->closed = GetULLTime();
            }
        }
        if ( bufs_in_out->last_received_from_server == 0 )
        {
            bufs_in_out->last_received_from_server = GetULLTime();
        }

        //Close in
        CloseOne ( true, bufs_in_out );

        if ( !bufs_in_out->buf_out_freed )
        {
            //Ci sono dati in fase di scrittura su b_out?
            if ( bufs_in_out->p_buf_out )
            {
                struct evbuffer* dst = bufferevent_get_output ( bufs_in_out->p_buf_out );

                if ( dst && evbuffer_get_length ( dst ) )
                {
                    bufferevent_setcb ( bufs_in_out->p_buf_out, NULL, close_on_finished_writecb_b_out, eventcb_b_out, bufs_in_out );
                }
                else
                {
                    CloseOne ( false, bufs_in_out );
                }
            }
        }
    }
    else
    {
        if ( what & BEV_EVENT_CONNECTED )
        {
        }
        else
        {
            cout << __func__ << " - " << StrTime() << ": " << what << endl;
        }
    }
}

//Quando arriva la accept, non so ancora (poiche' non ho letto i dati) dove devo andare... L'ultimo string è, se serve, il certificato ca dei client
typedef tuple<Protocols, struct sockaddr_storage, string, short, string, string, SSL_CTX*, event_base*, string, string> T_argument_tuple;

void accept_cb ( struct evconnlistener* listener, evutil_socket_t fd, struct sockaddr* a, int slen, void* v_args )
{
    struct bufferevent* b_out, *b_in;
    T_EventoStruct* bufs_in_out = new T_EventoStruct();  //per poter archiviare il servername dall'SSL

    //Per prendere anche gli argomenti...
    T_argument_tuple* args = ( T_argument_tuple* ) v_args;

//     static unsigned long long ptimes=0;
//     int mtfa_trunc_string( int from, int to);
//     if (ptimes++ > 10000)
//     {
//       mtfa_trunc_string(12, 32);
//       ptimes = 0;
//     }

    //faccio fd SetSocketNoDelay
    SetSocketNoDelay ( fd );
    SetSocketUnblocking ( fd );

    Protocols protocollo = get<0> ( *args );

    event_base* base = get<7> ( *args );

    switch ( protocollo )
    {
        case Protocols::eProtSSL:
        {
            //é un SSL!
            //mtfa_log("ACCEPT SSL\n");
            SSL_CTX* ctx = get<6> ( *args );
            //mtfa_log("Callback modified\n");
            SSL_CTX_set_tlsext_servername_arg(ctx, (void *)bufs_in_out);  //a tempo di challenge SSL il server name potrà essere messo nel bufs_inout
            SSL* ssl = SSL_new ( ctx );

            int val = 1;
            setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&val, sizeof(val));

            b_in = bufferevent_openssl_socket_new (
                       base,
                       fd,
                       ssl,
                       BUFFEREVENT_SSL_ACCEPTING,
                       ( bufferevent_options ) ( BEV_OPT_THREADSAFE | BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS )
                   );
            //bufferevent_enable(b_in, EV_READ | EV_WRITE);

            //bufferevent_openssl_set_allow_dirty_shutdown ( b_in, 1 );

            if ( !b_in )
            {
                delete bufs_in_out;
                perror ( "bufferevent_socket_new: bin" );
                return;
            }
        }
        break;

        case Protocols::eProtTCP:
        {
            //mtfa_log("ACCEPT TCP\n");
            b_in = bufferevent_socket_new ( base, fd, ( bufferevent_options ) ( BEV_OPT_THREADSAFE | BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS ) );

            if ( !b_in )
            {
                delete bufs_in_out;
                perror ( "bufferevent_socket_new: bin" );
                return;
            }
        }
        break;

        case Protocols::eProtHTTP:
        {
            //cout << __func__   << "ACCEPT HTTP\n";
//            {
//                open_sockets_mutex.lock();
//                open_sockets.insert(fd);
//                open_sockets_mutex.unlock();
//            }
            b_in = bufferevent_socket_new ( base, fd, ( bufferevent_options ) ( BEV_OPT_THREADSAFE | BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS ) );

            if ( !b_in )
            {
                delete bufs_in_out;
                perror ( "bufferevent_socket_new: bin" );
                return;
            }
        }
        break;

        default:
            delete bufs_in_out;
            cout << __func__   << "Acceptcb: unknown protocol\n";
            return;
    }

    //non serve poiché nella connect ricreiamo questo bufferevent (se è ssl) ma dovrei guardarmi tutto il codice per togliere questo (eventi, callback, ...)
    b_out = bufferevent_socket_new ( base, -1, ( bufferevent_options ) ( BEV_OPT_THREADSAFE | BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS ) );

    if ( !b_out )
    {
        delete bufs_in_out;
        perror ( "bufferevent_socket_new: bout" );
        bufferevent_free ( b_in );

//        {
//            open_sockets_mutex.lock();
//            if (open_sockets.find(fd)!=open_sockets.end())
//                open_sockets.erase(fd);
//            open_sockets_mutex.unlock();
//        }
        try
        {
            EVUTIL_CLOSESOCKET ( bufferevent_getfd ( b_in ) );
        }
        catch ( ... )
        {
            cout << __func__   << "error closing socket\n";
        }

        return;
    }

    //bufs_in_out = new T_EventoStruct();  ==> Già fatto in precedenza!

    bufs_in_out->p_buf_in = b_in;
    bufs_in_out->p_buf_out = b_out;

//    //Su quale porta e ip sono in listening
//    //ERRORE, UTILIZZO PIù PORTE E IP!!
//    bufs_in_out->listen_ip = get<2>(*args); //->in_ip;
//    bufs_in_out->listen_port = get<3>(*args); //args->in_port;

    bufs_in_out->protocollo = protocollo;

    if ( protocollo == Protocols::eProtTCP )
    {
        bufs_in_out->tcpmanage = get<4> ( *args );     //La funzione di management da chiamare per il TCP
    }

    bufs_in_out->accepted_client = GetULLTime();

    //Chi sono io
    struct sockaddr in_sa;
    socklen_t len_sa = sizeof ( in_sa );
    memset ( &in_sa, 0, len_sa );
    getsockname ( fd, &in_sa, &len_sa );
    bufs_in_out->listen_ip = inet_ntoa ( * ( struct in_addr* ) & ( in_sa.sa_data[2] ) );
    bufs_in_out->listen_port = ntohs ( * ( unsigned short* ) & ( in_sa.sa_data[0] ) );

    //chi e' il client
    len_sa = sizeof ( in_sa );
    memset ( &in_sa, 0, len_sa );

    getpeername ( fd, &in_sa, &len_sa );

    //  struct in_addr
    bufs_in_out->client_ip = inet_ntoa ( * ( struct in_addr* ) & ( in_sa.sa_data[2] ) );
    bufs_in_out->client_port = ntohs ( * ( unsigned short* ) & ( in_sa.sa_data[0] ) );
    
    listaEventi->AddEvento ( bufs_in_out );

    if ( b_in && b_out )
    {
        //Quello che arriva da bin lo invio a bout! Lo schema e' bin invia a bout. bout risponde a bin.
        bufferevent_setcb ( b_in, readcb_b_in_0, writecb_b_in, eventcb_b_in, bufs_in_out );
        bufferevent_setcb ( b_out, NULL, NULL, eventcb_b_out, bufs_in_out );

        bufferevent_enable ( b_in, EV_READ | EV_WRITE ); // | EV_TIMEOUT);
        bufferevent_enable ( b_out, EV_READ | EV_WRITE ); // | EV_TIMEOUT);
        //cout << __func__   << "ACCEPT: callback abilitate\n";
    }

}

SCM U8;
void * InitScmEnvironment(void *)
{
    scm_init_guile();
    mtfa_log("Initializes the LISP environment: %s\n", LbUser_LspCode.c_str());
    scm_c_primitive_load ( LbUser_LspCode.c_str() ); //carica il compilato!
    mtfa_log("LISP environment initialized\n");
    U8 = scm_from_locale_string ( "u8" );
    
    //Questa funzione viene richiamata dal LISP per le comunicazione TCP. Serve a inviare i dati
    //la recv dei dati avviene SEMPRE tramite il canale che poi chiama la Manage!!!
    void SendAnswerAndNotClose( T_EventoStruct* bufs_in_out, const char* dati, int len );
    scm_c_define_gsubr ( "va-c::vaeng::SendRawData", 3, 0, 0, ( void* )SendAnswerAndNotClose); // ( T_EventoStruct* bufs_in_out, const char* dati, int len )

    //Aggiungo le funzioni di gestione dei set da lisp
    scm_c_define_gsubr ( "va-c::vaeng::AddToSet", 4, 0, 0, ( void* ) InsertIntoSet );
	scm_c_define_gsubr ( "va-c::vaeng::AddToDict", 5, 0, 0, ( void* ) InsertIntoDict );
	scm_c_define_gsubr ( "va-c::vaeng::DelFromSet", 4, 0, 0, ( void* ) RemoveFromSet );
	scm_c_define_gsubr ( "va-c::vaeng::GetFromSet", 2, 0, 0, ( void* ) GetFromSet );
    
	scm_c_define_gsubr ( "va-c::vaeng::CheckIfDeallocated", 1, 0, 0, ( void* ) CheckIfDeallocated );


    scm_c_define_gsubr ( "va-c::vaeng::fast-scan-external", 1, 1, 0, (void*)hs_scan_external);
    scm_c_define_gsubr ( "va-c::vaeng::fast-scan-stringset", 1, 1, 0, (void*)hs_scan_internal);

    SCM sScmRunString = scm_c_lookup ( "va::vaeng::run-string" );
	SCM ScmRunString = scm_variable_ref ( sScmRunString ); //tutti posso accedere a questa funzione

    //Inizializzo le chiamate in ambiente lisp, in questo modo non ho simboli non definiti
	scm_call_1(ScmRunString, scm_from_latin1_string("(set! va::vaeng::AddToSet   va-c::vaeng::AddToSet)"));
	scm_call_1(ScmRunString, scm_from_latin1_string("(set! va::vaeng::AddToDict  va-c::vaeng::AddToDict)"));
	scm_call_1(ScmRunString, scm_from_latin1_string("(set! va::vaeng::DelFromSet va-c::vaeng::DelFromSet)"));
	scm_call_1(ScmRunString, scm_from_latin1_string("(set! va::vaeng::GetFromSet va-c::vaeng::GetFromSet)"));

    scm_call_1(ScmRunString, scm_from_latin1_string("(set! va::vaeng::fast-scan-external  va-c::vaeng::fast-scan-external)"));
    scm_call_1(ScmRunString, scm_from_latin1_string("(set! va::vaeng::fast-scan-stringset va-c::vaeng::fast-scan-stringset)"));


    scm_call_1(ScmRunString, scm_from_latin1_string("(set! va::vaeng::SendRawData va-c::vaeng::SendRawData)"));
    scm_call_1(ScmRunString, scm_from_latin1_string("(set! va::vaeng::CheckIfDeallocated va-c::vaeng::CheckIfDeallocated)"));

    SCM func_symbol = scm_c_lookup ( "DoYourJob-nopar" );
    LispManagement = scm_variable_ref ( func_symbol );

    SCM sKamRun = scm_c_lookup ( "KamRun" );
    KamRun = scm_variable_ref ( sKamRun ); //tutti posso accedere a questa funzione

#ifdef NEW_RELEASE_4
    //E, infine, il codice da eseguire in un thread per la gestione di tutte le richieste Scheme
    SCM sThreadGestoreComandi = scm_c_lookup ( "ThreadGestoreComandi" );
    ThreadGestoreComandi = scm_variable_ref ( sThreadGestoreComandi );

    SCM VaDomainServerSocketName = scm_from_latin1_string(LbUser_VaDomainServerSocketName.c_str());
    SCM call_ret = scm_call_1 (ThreadGestoreComandi, VaDomainServerSocketName);
    if (call_ret == SCM_BOOL_F)
    {
        mtfa_log("Unable to start the companion scheme server. Stop now.\n");
        exit(-1);
    }
#endif

    return nullptr;
}

//La funzione che invia richieste al domain server e torna stringhe
//-1 se ci sono stati errori!!
int CallDomainServerJson ( json & json_request, json & json_result )
{
    //esempio per farlo hidden
    //char *socket_path = "\0hidden";
    string socket_path = LbUser_VaDomainServerSocketName;
    struct sockaddr_un addr;

    int fd, rc;

    if ( ( fd = socket ( AF_UNIX, SOCK_STREAM, 0 ) ) == -1 ) {
        perror ( "socket error" );
        return ( -1 );
    }

    memset ( &addr, 0, sizeof ( addr ) );
    addr.sun_family = AF_UNIX;
    strncpy ( addr.sun_path, socket_path.c_str(), sizeof ( addr.sun_path )-1 );

    if ( connect ( fd, ( struct sockaddr* ) &addr, sizeof ( addr ) ) == -1 ) {
        perror ( "connect error" );
        return ( -1 );
    }

    static std::function<int ( int, string& ) > send_data = [] ( int sock, string & data ) {
        //invio una richiesta http
        char * p = data.data();
        int len = data.size();
        int sent = 0;
        while ( sent != len ) {
            int rc = write ( sock, &p[sent], len-sent );
            if ( rc < 0 )
                return -1;
            sent += rc;
        }
        return sent;
    };

    static std::function<int ( int, string& ) > recv_data = [] ( int sock, string& data ) {
        //riceve http
        char * p = new char[8192];
        int len;
        bool got = false;
        data.clear();
        while (!got)
        {
            int rc = read( sock, p, 8192);
            if ( rc < 0 )
            {
                delete [] p;
                return -1;
            }
            data += string( p, p+len );
            cout << "RECVDATA: " << data << endl;
            //cerco il fine header
            //prendo il content-length e poi verifico che i dati siano terminati. Oppure potrei verificare che termina con }, considerato che è una comunicazione con json semplificato
            if (data[data.size()-1] != '}')
                continue;
            else
                got = true;
//             int pos = data.find("\r\n\r\n");
//             if (pos == string::npos)
//                 continue;
        }
        delete [] p;
        return (int)data.size();
    };
    
    static const string post = "GET / HTTP/1.1\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: ";
    static const string eoln = "\r\n\r\n";

    string json_string = json_request.dump();
    string toBeSent = post + to_string(json_string.size()) + eoln + json_string;

    cout << "Send: <" << toBeSent << ">" << endl;
    rc = send_data(fd, toBeSent);
    if (rc < 0)
        return -1;

    cout << "Recv...\n";
    string toBeRecvd;
    rc = recv_data(fd, toBeRecvd);
    cout << "Recvd: <" << toBeRecvd << ">" << endl;
    int poseoln = toBeRecvd.find(eoln);
    if (rc > 0 && poseoln != string::npos)
        json_result = toBeRecvd.substr(poseoln+4);  //ne fa il parsing in automatico!
    else
        rc = -1;
        
    close(fd);
    return rc;
}


static void* ManageLBUserThread ( void* args );
vector<thread> vthreads_threadid;
vector<bool> vthreads_bool;

//void VerificaOpenClose()
//{
//    while (true)
//    {
//        int aperti=0;
//        int chiusi=0;
//        int totali = 0;
//        for (auto it: aperti_chiusi)
//        {
//            if (it.second.first!=0)
//                aperti++;
//            if (it.second.second!=0)
//                chiusi++;
//            totali++;
//        }
//        cout << __func__   << "Totali: " << totali << "(" << aperti << ", " << chiusi << ")" << endl;
//        sleep(5);
//    }
//}

/**
 * @brief ...
 *
 * @param vconf p_vconf:...
 * @param argc p_argc:...
 * @param argv ${p_argv:...}
 */
void ManageLBUserArgs ( void* vconf )
{
    //scm_init_guile();
    ConfigFile configurazione = * ( ( ConfigFile* ) vconf );
    std::cout << "Application Firewall afrrcainecroi SDK. ©afrrcainecroi 2025-. All rights reserved.\n"
              "Nothing shall be construed as granting, by implication, estoppel, or \n"
              "otherwise, any license or right to use any trademark, logo or service mark, registered or unregistered, without\n"
              "afrrcainecroi@gmail.com prior permission.\n\n";

//    void list_all_sockets();
//    std::thread ttt(list_all_sockets);
//    ttt.detach();

    //try
    {
        //Assegno tutti i mimetypes
        InitMimetypes();

        //per prima cosa inizializzo il LISP    
        LbUser_LspCode = configurazione.read<string> ( "LbUser_LspCode" );
        scm_with_guile(InitScmEnvironment, nullptr);

        try
        {
            LbUser_AsDaemon = configurazione.read<bool> ( "LbUser_AsDaemon" );
        }
        catch ( ConfigFile::key_not_found )
        {
            cerr << "Warning LbUser_AsDaemon <true/false> should be defined. Assumed as false." << endl;
            LbUser_AsDaemon = false;
        }

        //Mi lancio come daemon senza cambiare directory e senza chiudere stdin, stdout e stderr
        if ( LbUser_AsDaemon )
        {
            daemon ( 1, 1 );
        }

        //Mi leggo l'elenco degli IP da utilizzare per uscire
        LbUser_OutIp = configurazione.read<string> ( "LbUser_OutIp" );

        //        LbUser_PassTroughIp = configurazione.read<string>("LbUser_PassTroughIp");
        //        LbUser_PassTroughPort = configurazione.read<string>("LbUser_PassTroughPort");

        LbUser_ListenIp = configurazione.read<string> ( "LbUser_ListenIp" );
        LbUser_ListenPort = configurazione.read<string> ( "LbUser_ListenPort" );

        IpsAndPortsToListen.insert ( LbUser_ListenIp + string ( ":" ) + LbUser_ListenPort );

        //potrebbero essere tanti IP
        //bool assigned_base_ip = false;
        try
        {
            LbUser_VListenIpPort = configurazione.read<string> ( "LbUser_VListenIpPort" );

            T_V_Split vs = mtfa_string_split ( LbUser_VListenIpPort, ";" );

            for ( auto it : vs )
            {
                IpsAndPortsToListen.insert ( it );
            }
        }
        catch ( ConfigFile::key_not_found )
        {
            cerr << "Warning V_LbUser_ListenIpPort: " << endl;
        }

        try
        {
            LbUser_VSSLPort = configurazione.read<string> ( "LbUser_VSSLPort" );

            T_V_Split vs = mtfa_string_split ( LbUser_VSSLPort, ";" );

            for ( auto it : vs )
            {
                IpsAndPortsToListen.insert ( it ); //quelle che poi saranno lunghe 4 allora sono SSL quelle lunghe tre sono TCP
                //il terzo parametro del TCP è il nome della struttura SCM che lo gestisce!
            }
        }
        catch ( ConfigFile::key_not_found )
        {
            cerr << "Warning LbUser_VSSLPort: " << endl;
        }

        try
        {
            LbUser_VTCPPort = configurazione.read<string> ( "LbUser_VTCPPort" );

            T_V_Split vs = mtfa_string_split ( LbUser_VTCPPort, ";" );

            for ( auto it : vs )
            {
                IpsAndPortsToListen.insert ( it );
            }
        }
        catch ( ConfigFile::key_not_found )
        {
            cerr << "Warning LbUser_VSSLPort: " << endl;
        }

        try
        {
            LbUser_HowManyInstances = configurazione.read<int> ( "LbUser_HowManyInstances" );
        }
        catch ( ConfigFile::key_not_found )
        {

            cerr << "Warning LbUser_HowManyInstances: " << endl;
            LbUser_HowManyInstances = 1;
        }

        try
        {
            LbUser_PartialUrl = configurazione.read<int> ( "LbUser_PartialUrl" );
        }
        catch ( ConfigFile::key_not_found )
        {

            cerr << "Warning LbUser_PartialUrl: " << endl;
            LbUser_PartialUrl = 0; //0: tutta la url
        }

        LbUser_LogFile = configurazione.read<string> ( "LbUser_LogFile" );

        LbUser_ThPoolSize = configurazione.read<int> ( "LbUser_ThPoolSize" );

#if EIS_LEVEL & EIS_LEVEL_7
        try
        {
            LbUser_VaDomainServerSocketName = configurazione.read<string>("LbUser_VaDomainServerSocketName");
        }
        catch ( ConfigFile::key_not_found )
        {
            cerr << "LbUser_VaDomainServerSocketName: domain name not given, use default </tmp/va_domain.sock>" << endl;
            LbUser_VaDomainServerSocketName = "/tmp/va_domain.sock";
        }

#endif


#ifdef VADEVEL

        try
        {
            LbUser_VAObservedApps  = configurazione.read<string> ( "LbUser_VAObservedApps" );
            LbUser_VA_VRules = configurazione.read<string> ( "LbUser_VA_VRules" );
            LbUser_VA_VReports  = configurazione.read<string> ( "LbUser_VA_VReports" );
            LbUser_VA_GetAllHTTP =  configurazione.read<bool> ( "LbUser_VA_GetAllHTTP" );
            LbUser_VA_QueryFilters =  configurazione.read<string> ( "LbUser_VA_QueryFilters" );

            LbUser_VA_KAM_CommandPort =  configurazione.read<unsigned short> ( "LbUser_VA_KAM_CommandPort" );
            LbUser_VA_KAM_CommandIp =  configurazione.read<string> ( "LbUser_VA_KAM_CommandIp" );
            IpsAndPortsToListen.insert ( LbUser_VA_KAM_CommandIp + string ( ":" ) + to_string(LbUser_VA_KAM_CommandPort) );

            LbUser_FromVACommands =  configurazione.read<string> ( "LbUser_FromVACommands" );
            //IpsAndPortsToListen.insert ( LbUser_FromVACommands );
            
            LbUser_VA_HwAgentPort = 0;
            LbUser_VA_HwAgentIp =  "empty";
            LbUser_VA_SwAgentPort = 0;
            LbUser_VA_SwAgentIp =  "empty";
            try
            {
                LbUser_VA_HwAgentPort =  configurazione.read<unsigned short> ( "LbUser_VA_HwAgentPort" );
            }
            catch ( ... )
            {
                mtfa_log("LbUser_VA_HwAgentPort is empty. Are you sure?\n");
            }

            try
            {
                LbUser_VA_HwAgentIp =  configurazione.read<string> ( "LbUser_VA_HwAgentIp" );
            }
            catch ( ... )
            {
                mtfa_log("LbUser_VA_HwAgentIp is empty. Are you sure?\n");
            }

            try
            {
                LbUser_VA_SwAgentPort =  configurazione.read<unsigned short> ( "LbUser_VA_SwAgentPort" );
            }
            catch ( ... )
            {
                mtfa_log("LbUser_VA_SwAgentPort is empty. Are you sure?\n");
            }

            try
            {
                LbUser_VA_SwAgentIp =  configurazione.read<string> ( "LbUser_VA_SwAgentIp" );
            }
            catch ( ... )
            {
                mtfa_log("LbUser_VA_SwAgentIp is empty. Are you sure?\n");
            }
            
            if (LbUser_VA_HwAgentPort!=0)
                IpsAndPortsToListen.insert ( LbUser_VA_HwAgentIp + string ( ":" ) + to_string(LbUser_VA_HwAgentPort) );

            if (LbUser_VA_SwAgentPort!=0)
                IpsAndPortsToListen.insert ( LbUser_VA_SwAgentIp + string ( ":" ) + to_string(LbUser_VA_SwAgentPort) );

            try
            {
                LbUser_VA_AskForUrl =  configurazione.read<string> ( "LbUser_VA_AskForUrl" );
            }
            catch ( ... )
            {
                mtfa_log("LbUser_VA_AskForUrl not set. Using </va/askfor>\n");
                LbUser_VA_AskForUrl = "/va/askfor";
            }
            
//             LbUser_VA_LocalSH_path = "/volume1/sources/NEW_DEVS/DEPLOYED/GENERAL/sonda_linux";
//             LbUser_VA_LocalSH_attach = "any";
//             LbUser_VA_LocalSH_promiscous = "0";
//             LbUser_VA_LocalSH_VA_IP = "127.0.0.1";
//             LbUser_VA_LocalSH_VA_PORT = "50000";
//             LbUser_VA_LocalSH_pcapfilter = ""; //"portrange 8000-9000";
            LbUser_VA_LocalSH_on = false;

            try
            {
                LbUser_VA_LocalSH_on =  configurazione.read<bool> ( "LbUser_VA_LocalSH_on");
//                 LbUser_VA_LocalSH_path =  configurazione.read<string> ( "LbUser_VA_LocalSH_path");
//                 LbUser_VA_LocalSH_attach =  configurazione.read<string> ( "LbUser_VA_LocalSH_attach");
//                 LbUser_VA_LocalSH_promiscous =  configurazione.read<string> ( "LbUser_VA_LocalSH_promiscous");
//                 LbUser_VA_LocalSH_VA_IP =  configurazione.read<string> ( "LbUser_VA_LocalSH_VA_IP");
//                 LbUser_VA_LocalSH_VA_PORT =  configurazione.read<string> ( "LbUser_VA_LocalSH_VA_PORT");
//                 LbUser_VA_LocalSH_pcapfilter =  configurazione.read<string> ( "LbUser_VA_LocalSH_pcapfilter");
            }
            catch ( ... )
            {
                mtfa_log("To activate the internal sonda, use LbUser_VA_LocalSH_on=true\n");
//                 (LbUser_VA_LocalSH_on?"ACTIVATE":"NOT ACTIVATE"),
//                 LbUser_VA_LocalSH_path.c_str(),
//                 LbUser_VA_LocalSH_attach.c_str(),
//                 LbUser_VA_LocalSH_promiscous.c_str(),
//                 LbUser_VA_LocalSH_VA_IP.c_str(),
//                 LbUser_VA_LocalSH_VA_PORT.c_str(),
//                 LbUser_VA_AskForUrl.c_str(),
//                 LbUser_VA_LocalSH_pcapfilter.c_str());
            }
            if (LbUser_VA_LocalSH_on)
            {
                mtfa_log("LbUser_VA_LocalSH_on is true, so the internal sonda functionalities are activated\n");
                try {
                    LbUser_VA_LocalSH_HTTPMethods = configurazione.read<string> ( "LbUser_VA_LocalSH_HTTPMethods");
                } catch (...)
                {
                    mtfa_log("LbUser_VA_LocalSH_HTTPMethods not found. Uses the default methods: GET|POST|HEAD|PUT|DELETE|CONNECT|OPTIONS|TRACE|PATCH\n");
                }
                thread rshs(RunSondaHwSimulator);  //Parte la sonda interna!!
                rshs.detach();
            }
//             else
//             {
//                 //se sonda_linux è presente nella cartella di docker, lo esegue
//                 if (false && mtfa_FileIo::Exists("/volume1/sources/NEW_DEVS/DEPLOYED/GENERAL/sonda_linux"))
//                 {
//                     mtfa_log("Launching sonda_linux\n.");
//                     string cmd =    LbUser_VA_LocalSH_path + " " + LbUser_VA_LocalSH_attach + " " + LbUser_VA_LocalSH_promiscous + " " + 
//                                     LbUser_VA_LocalSH_VA_IP + " " + LbUser_VA_LocalSH_VA_PORT + " " + 
//                                     LbUser_VA_AskForUrl + " " + LbUser_VA_LocalSH_pcapfilter;
//                     mtfa_log("Comando di default: %s\n", "/volume1/sources/NEW_DEVS/DEPLOYED/GENERAL/sonda_linux \"any\"  0 \"127.0.0.1\" 50000 \"/va/askfor\" \"portrange 8000-9000\"\n");
//                     mtfa_log("Esegue invece il comando: %s\n", cmd.c_str());
//                     auto fun = [] (string cmd) {
//                         mtfa_log("EXITED FROM SYSTEM CALL: %d\n", system(cmd.c_str()));
//                     };
//                     thread th(fun, cmd);
// //                  th.detach();
//                 }
//                 else
//                 {
//                     mtfa_log("Sonda not executed because sonda_linux is not available in the docker container.\n");
//                 }
//             }

            try
            {
                LbUser_VA_TraceUrl =  configurazione.read<string> ( "LbUser_VA_TraceUrl" );
            }
            catch ( ... )
            {
                cout << "LbUser_VA_TraceUrl, using </va/trace>" << endl;
                LbUser_VA_TraceUrl = "/va/trace";
            }

            try
            {
                LbUser_VA_TraceQuery =  configurazione.read<string> ( "LbUser_VA_TraceQuery" );
            }
            catch ( ... )
            {
                cout << "LbUser_VA_TraceQuery, using <__X1p_5Tq-t_a_1-0>" << endl;
                LbUser_VA_TraceQuery = "__X1p_5Tq-t_a_1-0";
            }
            
            try
            {
                auto sLbUser_Id_WhoAmI =  configurazione.read<string> ( "LbUser_Id_WhoAmI" );
                if (sLbUser_Id_WhoAmI == "VA")
                    LbUser_Id_WhoAmI = WhoAmI::eVA;
                else if (sLbUser_Id_WhoAmI == "KAM")
                    LbUser_Id_WhoAmI = WhoAmI::eKAM;
                else if (sLbUser_Id_WhoAmI == "SVA")
                    LbUser_Id_WhoAmI = WhoAmI::eSuperVA;
                else 
                {
                    cout << "LbUser_Id_WhoAmI, is empty. You must specify: KAM, VA, SVA" << endl;
                    exit(0);
                }
            }
            catch ( ... )
            {
                cout << "LbUser_Id_WhoAmI, is empty" << endl;
                exit(0);
            }
            
            try
            {
                LbUser_SecureVa =  configurazione.read<string> ( "LbUser_SecureVa" );
            }
            catch ( ... )
            {
                cout << "LbUser_SecureVa (ip:port:url), is empty" << endl;
                exit(0);
            }

            
            LbUser_Hy_VA_Sisters.clear();   //No need to have sisters. This is not a VA
            LbUser_Hy_KAM_Children.clear(); //No need to have children. This is not a VA
            try
            {
                LbUser_Hy_VA_Sisters =  configurazione.read<string> ( "LbUser_Hy_VA_Sisters" );
            }
            catch ( ... )
            {
                cout << "LbUser_Hy_VA_Sisters is empty. Are you sure?\x07" << endl;
            }

            try
            {
                LbUser_Hy_KAM_Children =  configurazione.read<string> ( "LbUser_Hy_KAM_Children" );
            }
            catch ( ... )
            {
                cout << "LbUser_Hy_KAM_Children, is empty. Are you sure?\x07" << endl;
            }

            LbUser_SVA_Listen.clear();
            try
            {
                LbUser_SVA_Listen =  configurazione.read<string> ( "LbUser_SVA_Listen" );
            }
            catch ( ... )
            {
                if (LbUser_Id_WhoAmI == WhoAmI::eSuperVA) {
                    cout << "LbUser_SVA_Listen, must be configured" << endl;
                    exit(0);
                }
                else
                    cout << "LbUser_SVA_Listen, not configured" << endl;
            }
            
            try
            {
                LbUser_Initialize =  configurazione.read<string> ( "LbUser_Initialize" );
            }
            catch ( ... )
            {
                cout << "LbUser_Initialize not configured. Set to default: 127.0.0.1:60000" << endl;
                LbUser_Initialize = "127.0.0.1:60000";
            }
            
            IpsAndPortsToListen.insert ( LbUser_Initialize );

            
            try
            {
                LbUser_VA_mtfa_rnd =  configurazione.read<string> ( "LbUser_VA_mtfa_rnd" );
            }
            catch ( ... )
            {
                cout << "LbUser_VA_mtfa_rnd, using <_r_mtfa>" << endl;
                LbUser_VA_mtfa_rnd = "_r_mtfa";
            }

            try
            {
                LbUser_VA_TraceDomain =  configurazione.read<string> ( "LbUser_VA_TraceDomain" );
            }
            catch ( ... )
            {
                cout << "LbUser_VA_TraceDomain is mandatory. It is a domain name, with the port (domain:port) or ip:port" << endl
                     << "it is the site used by the users to reach special services of the VA, like the tracking one" << endl
                     << "As basic suggestion, use one of the AM sites (using ip:port)" << endl;
                exit ( -1 );
            }
            IpsAndPortsToListen.insert ( LbUser_VA_TraceDomain);

            try
            {
                LbUser_MyAgentKey = configurazione.read<string> ( "LbUser_MyAgentKey" );
            }
            catch ( ConfigFile::key_not_found )
            {
                cerr << "Warning LbUser_MyAgentKey: " << endl;
                LbUser_MyAgentKey = string ( "01234567890" );
            }
            
            try
            {
                LbUser_MyAgentId = configurazione.read<string> ( "LbUser_MyAgentId" );
            }
            catch ( ConfigFile::key_not_found )
            {
                cerr << "Warning LbUser_MyAgentId: " << endl;
                LbUser_MyAgentId = string ( "09876543210" );
            }
            
            try
            {
                LbUser_VA_DB_Addr = configurazione.read<string> ( "LbUser_VA_DB_Addr" );
                LbUser_VA_DB_Port = configurazione.read<string> ( "LbUser_VA_DB_Port" );
                LbUser_VA_DB_Name = configurazione.read<string> ( "LbUser_VA_DB_Name" );
                LbUser_VA_DB_User = configurazione.read<string> ( "LbUser_VA_DB_User" );
                LbUser_VA_DB_Pass = configurazione.read<string> ( "LbUser_VA_DB_Pass" );
                
                //Aggiungere il DB dell'organizzazione per accedere ai loro dati di monitoraggio
            }
            catch ( ConfigFile::key_not_found )
            {
                cerr << "Warning LbUser_VA_DB: " << endl;
                
                if ( LbUser_Id_WhoAmI == WhoAmI::eVA )
                {
                    mtfa_log("VA should have an agents DB\n");
                }
            }
            
            
            //Splitta e configura la ds
            T_V_Split vrfq = mtfa_string_split ( LbUser_VA_QueryFilters, ";" );

            for ( auto& rfq : vrfq )
            {
                T_V_Split kv = mtfa_string_split ( rfq, ":" );

                if ( kv.size() > 0 )
                {
                    VA_remove_from_query[kv[0]].push_back ( kv[1] );
                }
            }

            //Compila le regole e prova.

            //Qui crea l'engine con i nuovi valori
            //Do compile rules
            bool ok = DoCompileRules ( LbUser_VA_VRules, LbUser_VA_VReports, vaeng );

            if ( !ok )
            {
                mtfa_log("Error compiling rules: %s\n", LbUser_VA_VRules.c_str());
                exit ( 0 );
            }

            //Qui faccio la verifica di coerenza semantica (variabili utilizzate e altre cosine)
            bool coerent = vaeng.CheckCoherence();

            //Ok costruisco le osservazioni!
            {
                set<string> hosts = BuildHostsListFromSetsAndHosts ( vaeng );

                auto other = BuildOtherListFromCriteria ( vaeng );

                for ( auto it : hosts )
                {
                    cout << __func__   << "Host: " << it << endl;
                }

                for ( auto it : other.first )
                {
                    cout << __func__   << "-> " << it << endl;
                }

                for ( auto it : other.second )
                {
                    cout << __func__   << "<- " << it << endl;
                }

                int rule = 2;
                //le due basiche.
                AS_Rules = {"COND0=" + LbUser_VAObservedApps, "DESCR1=rule al attach", "COND1=@EVENT==PROCESS_ATTACH", "LISTA1=@LOGGED_USER;0;"};

                //Per ogni host metto tutte le cosine che mi servono
                for ( auto it : hosts )
                {
                    string nr = to_string ( rule );
                    string descr = "DESCR" + nr + "=" "Rule HTTP " + nr + ". SEND TO " + it;
                    string cond;

                    if ( LbUser_VA_GetAllHTTP )
                    {
                        cond  = "COND" + nr + "=" "@EVENT==SEND AND @HTTP_host<<" + it;     //ne metto comunque due, una con .html e l'altra con il content-type
                    }
                    else
                    {
                        cond  = "COND" + nr + "=" "@EVENT==SEND AND @HTTP_URI_URL<<.html AND @HTTP_host<<" + it;     //ne metto comunque due, una con .html e l'altra con il content-type
                    }

                    string lista = "LISTA" + nr + "=@HTTP_HOST;0;";

                    for ( auto c2s : other.first )
                    {
                        lista += c2s + ";0;";
                    }

                    //lista += "\n";
                    string lista_resp = "LISTA_RESP" + nr + "=";

                    for ( auto c2s : other.second )
                    {
                        lista_resp += c2s + ";0;";
                    }

                    //lista_resp += "\n";
                    string action = "ACTION" + nr + "=TAG;";
                    AS_Rules.push_back ( descr );
                    AS_Rules.push_back ( cond );
                    AS_Rules.push_back ( lista );
                    AS_Rules.push_back ( lista_resp );
                    AS_Rules.push_back ( action );

                    rule++; //cambia id di regola
                }

                //lo stesso ma con il test sul content type
                for ( auto it : hosts )
                {
                    string nr = to_string ( rule );
                    string descr = "DESCR" + nr + "=" "Rule HTTP " + nr + ". SEND TO " + it;
                    string cond;

                    if ( LbUser_VA_GetAllHTTP )
                    {
                        cond  = "COND" + nr + "=" "@EVENT==SEND AND @HTTP_host<<" + it;     //ne metto comunque due, una con .html e l'altra con il content-type
                    }
                    else
                    {
                        cond  = "COND" + nr + "=" "@EVENT==SEND AND @HTTP_Accept<<text/html AND @HTTP_host<<" + it;     //ne metto comunque due, una con .html e l'altra con il content-type
                    }

                    string lista = "LISTA" + nr + "=@HTTP_HOST;0;";

                    for ( auto c2s : other.first )
                    {
                        lista += c2s + ";0;";
                    }

                    //lista += "\n";
                    string lista_resp = "LISTA_RESP" + nr + "=";

                    for ( auto c2s : other.second )
                    {
                        lista_resp += c2s + ";0;";
                    }

                    //lista_resp += "\n";
                    string action = "ACTION" + nr + "=TAG;";
                    AS_Rules.push_back ( descr );
                    AS_Rules.push_back ( cond );
                    AS_Rules.push_back ( lista );
                    AS_Rules.push_back ( lista_resp );
                    AS_Rules.push_back ( action );

                    rule++; //cambia id di regola
                }

//string lista = "LISTA" + nr + "GET /;0d 0a 0d 0a;0;POST /;0d 0a 0d 0a;0;HEAD /;0d 0a 0d 0a;0;" + "\n";
//string lista_resp = "LISTA_RESP" + nr + "HTTP/1.;0d 0a 0d 0a;0";
//DESCR50=rule html su send con html page
//COND50=@EVENT==SEND AND @HTTP_METHOD==GET AND @HTTP_URI_URL<<.html AND @HTTP_host<<repubblica.it
//LISTA50=@HTTP_METHOD;0;
//LISTA_RESP50=@HTTP_RESPONSE_CODE;0;
//ACTION50=TAG;
                cout << __func__   << endl << endl << "RULES GENERATED" << endl << endl;

                for ( auto it : AS_Rules )
                {
                    cout << __func__   << "Rule: " << it << endl;
                }

                cout << __func__   << endl << endl;

                if ( number_of_arguments != 2 )
                {
                    exit ( 0 );
                }

                //inizializzo valutatore
                vaeng.InitRun();
            }
        }
        catch ( ConfigFile::key_not_found& k )
        {
            cerr << "Key " << k.key << " not found in configuration file." << endl;
            exit ( -1 );
        }

#endif  //VADEVEL

    }
//    catch (ConfigFile::key_not_found ex) {
//        cout << __func__   << ex.key << ": not found." << endl;
//        exit(-1);
//    }
//    catch (std::exception &ex) {
//        cout << __func__   << ex.what() << endl;
//        exit(-1);
//    }

//load the file LbUser_Rules and build rules.
    CheckAndLoadRules();

//Inizializzo la lista degli eventi e metto in esecuzione in thread di run
    listaEventi = new C_ListaEventi();
    listaEventi->StartPurge ( 5 ); //Ogni 10 secondi purgo la lista

//    SetRsaKeys(); //Setto la chiave RSA del server WEB per cifrare e decifrare noi

//INIZIALIZZO IL THPOOL
    InitPools ( LbUser_ThPoolSize );

#ifdef NEW_RELEASE_4
    {
        //Qui provo a chiamare i comandi e anche la velocità
        sleep(5);
        int i=0;
        while (true)
        {
            try {
                json in, out;
                in = R"(
                        {
                            "cmd": "uno",
                            "p1": 2,
                            "p2": true
                        }
                    )"_json;
                cout << "Call with: " << in.dump() << endl;
                int rc = CallDomainServerJson(in, out);
                if (rc > 0)
                    cout << out.dump() << endl;
                else
                    cout << "Errore\n";
                cout << ++i << endl;
                getchar();
            } catch (exception ex)
            {
                cout << ex.what() << endl;
                exit(-1);
            }
        }
    }
#endif

/*
//Aggiungo le funzioni di gestione dei set da lisp
	scm_c_define_gsubr ( "va-c::vaeng::AddToSet", 4, 0, 0, ( void* ) InsertIntoSet );
	scm_c_define_gsubr ( "va-c::vaeng::AddToDict", 5, 0, 0, ( void* ) InsertIntoDict );
	scm_c_define_gsubr ( "va-c::vaeng::DelFromSet", 4, 0, 0, ( void* ) RemoveFromSet );
	scm_c_define_gsubr ( "va-c::vaeng::GetFromSet", 2, 0, 0, ( void* ) GetFromSet );
	

//Se sono in level 7 e se ho definito un codice da caricare, lo carico
#if EIS_LEVEL & EIS_LEVEL_7
    scm_c_primitive_load ( LbUser_LspCode.c_str() ); //carica il compilato!
    SCM func_symbol;

    if ( !LbUser_NewCall )
        func_symbol = scm_c_lookup ( "DoYourJob" ); //default is "DoYourJob"
    else
        func_symbol = scm_c_lookup ( "DoYourJob-nopar" );

    LispManagement = scm_variable_ref ( func_symbol );
#endif
#ifdef VADEVEL
    SCM sKamRun = scm_c_lookup ( "KamRun" );
    KamRun = scm_variable_ref ( sKamRun ); //tutti posso accedere a questa funzione
	
	//Oltre alla KAMRUN, qui potrò associare le funzioni AddToSet alle variabili globali
	//dichiarate in va.scm, in modo da non avere problemi di linker e nomi non dichiarati a
	//tempo di compilazione del LISP
	SCM sScmRunString = scm_c_lookup ( "va::vaeng::run-string" );
	SCM ScmRunString = scm_variable_ref ( sScmRunString ); //tutti posso accedere a questa funzione
	
	scm_call_1(ScmRunString, scm_from_latin1_string("(set! va::vaeng::AddToSet   va-c::vaeng::AddToSet)"));
	scm_call_1(ScmRunString, scm_from_latin1_string("(set! va::vaeng::AddToDict  va-c::vaeng::AddToDict)"));
	scm_call_1(ScmRunString, scm_from_latin1_string("(set! va::vaeng::DelFromSet va-c::vaeng::DelFromSet)"));
	scm_call_1(ScmRunString, scm_from_latin1_string("(set! va::vaeng::GetFromSet va-c::vaeng::GetFromSet)"));
#endif
*/
    vs_out_ip = mtfa_string_split ( LbUser_OutIp, "; " );

    vthreads_bool.resize ( LbUser_HowManyInstances );
    vthreads_threadid.resize ( LbUser_HowManyInstances );

    for ( int i = 0; i < LbUser_HowManyInstances; ++i )
    {
        //Qui partono i thread e per ogni thread che parte farò la scm_init al suo interno
        T_ManageLBUserArgs* args = new T_ManageLBUserArgs();
        args->in_ip = LbUser_ListenIp; //(*configIt).second->IP_BindingIn;
        args->in_port = atoi ( LbUser_ListenPort.c_str() );
        args->ipsAndPorts = IpsAndPortsToListen;

        cout << __func__   << "Run instance " << i << endl;

        args->thnum = i;
        vthreads_bool[i] = false;
        vthreads_threadid[i] = thread (ManageLBUserThread, (void*)args);

        std::cout << __func__   << "Launched " << i << ": id = " << vthreads_threadid[i].get_id() << "\n" << "\tnative_handle = " << vthreads_threadid[i].native_handle() << "\n";
        vthreads_threadid[i].detach();
    }

    bool allok = false;

    while ( !allok )
    {
        allok = true;
        usleep ( 1000000 );
        this_thread::yield();

        for ( int i = 0; i < LbUser_HowManyInstances; ++i )
        {
            if ( !vthreads_bool[i] )
            {
                allok = false;
                break;
            }

//            else
//                cout << __func__   << "Thread " << i << " running\n";
        }
    }

    cout << __func__   << "Definitively started" << endl;

    //Ora è il momento di inizializzare le strutture di comunicazione verso le VA (sorelle o madri) o della SuperVa con le sue sorelle
    //LbUser_Initialize è fatto da IP:PORT
#ifdef VADEVEL
    try
    {
        T_V_Split vs = mtfa_string_split ( LbUser_Initialize, ":" );

        if ( vs.size() != 2 )
        {
            mtfa_log ( "Error. The value LbUser_Initialize must be: LbUser_Initialize=IP:PORT\n", LbUser_Initialize.c_str() );
            //se non c'è, non serve
//            exit ( -1 );
        }
        else
        {
            string page, headers;
            GetPageFromUrlProxy ( vs[0], vs[1], "http", "GET", "/?what=init", "", "", "", "", 0, page, headers );

            if ( page.size() == 0 || page != "OK" )
            {
                mtfa_log ( "Errore. Il valore in LbUser_Initialize non e' corretto\n", LbUser_Initialize.c_str() );
                //mtfa_log("Dovrebbe uscire ma non lo fa\n");
    #ifndef _DEBUG
                //exit ( -1 );
    #endif
            }
            else
            {
                mtfa_log ( "LbUser_Initialize correctly started\n" );
            }
        }
    }
    catch ( exception ex )
    {
        mtfa_log ( "Errore di uso di LbUser_Initialize: %s (%s)\n", ex.what(), LbUser_Initialize.c_str() );
        exit ( -1 );
    }
#endif


//     while ( true )
//     {
// //         int mtfa_trunc_string( int from, int to);
// //         mtfa_trunc_string(10, 20);
//         sleep ( 1000 );
//     }

    return;
}


static unordered_map<string, SSL_CTX*> server_names_and_context;

// /* Callback is called for each certificate of the chain during a verify
//    ok is set to 1 if preverify detect no error on current certificate.
//    Returns 0 to break the handshake, 1 otherwise. */
// int ssl_sock_bind_verifycbk(int ok, X509_STORE_CTX *x_store)
// {
// 	SSL *ssl;
// 	struct connection *conn;
// 	int err, depth;
// 
// 	ssl = X509_STORE_CTX_get_ex_data(x_store, SSL_get_ex_data_X509_STORE_CTX_idx());
// 	conn = SSL_get_ex_data(ssl, ssl_app_data_index);
// 
// 	conn->xprt_st |= SSL_SOCK_ST_FL_VERIFY_DONE;
// 
// 	if (ok) /* no errors */
// 		return ok;
// 
// 	depth = X509_STORE_CTX_get_error_depth(x_store);
// 	err = X509_STORE_CTX_get_error(x_store);
// 
// 	/* check if CA error needs to be ignored */
// 	if (depth > 0) {
// 		if (!SSL_SOCK_ST_TO_CA_ERROR(conn->xprt_st)) {
// 			conn->xprt_st |= SSL_SOCK_CA_ERROR_TO_ST(err);
// 			conn->xprt_st |= SSL_SOCK_CAEDEPTH_TO_ST(depth);
// 		}
// 
// 		if (__objt_listener(conn->target)->bind_conf->ca_ignerr & (1ULL << err)) {
// 			ssl_sock_dump_errors(conn);
// 			ERR_clear_error();
// 			return 1;
// 		}
// 
// 		conn->err_code = CO_ER_SSL_CA_FAIL;
// 		return 0;
// 	}
// 
// 	if (!SSL_SOCK_ST_TO_CRTERROR(conn->xprt_st))
// 		conn->xprt_st |= SSL_SOCK_CRTERROR_TO_ST(err);
// 
// 	/* check if certificate error needs to be ignored */
// 	if (__objt_listener(conn->target)->bind_conf->crt_ignerr & (1ULL << err)) {
// 		ssl_sock_dump_errors(conn);
// 		ERR_clear_error();
// 		return 1;
// 	}
// 
// 	conn->err_code = CO_ER_SSL_CRT_FAIL;
// 	return 0;
// }


// static void ssl_sock_switchctx_set ( SSL* ssl, SSL_CTX* ctx )
// {
//     SSL_set_SSL_CTX ( ssl, ctx );
//     auto calist = SSL_CTX_get_client_CA_list ( ctx ) ;
//     if (calist)
//         SSL_set_client_CA_list ( ssl, SSL_dup_CA_list ( calist ) );
//     SSL_set_verify(ssl, SSL_CTX_get_verify_mode(ctx), SSL_CTX_get_verify_callback(ctx));
//     SSL_set_verify_depth(ssl, SSL_CTX_get_verify_depth(ctx));
//     SSL_clear_options(ssl, SSL_get_options(ssl) & ~SSL_CTX_get_options(ctx));
//     SSL_set_options(ssl, SSL_CTX_get_options(ctx));
// }

static void ssl_sock_switchctx_set(SSL *ssl, SSL_CTX *ctx)
{
	SSL_set_verify(ssl, SSL_CTX_get_verify_mode(ctx), SSL_CTX_get_verify_callback(ctx));
	SSL_set_client_CA_list(ssl, SSL_dup_CA_list(SSL_CTX_get_client_CA_list(ctx)));
	SSL_set_SSL_CTX(ssl, ctx);
}

#if 0
//La nuova gestione del servername (openssl 1.1.1)
static int serverNameCallback_1_1(SSL *ssl, int *al, void *arg)
{
//     SSL_CLIENT_HELLO_SUCCESS
//     SSL_CLIENT_HELLO_RETRY
//     SSL_CLIENT_HELLO_ERROR

    if ( ssl == NULL )  //Qui non saprei cosa fare!
    {
        mtfa_log("SSL is NULL\n");
        return 0;
    }

    const uint8_t *servername=nullptr;
    size_t servername_len=0;

    const uint8_t *extension_data=nullptr;
    size_t extension_len=0;

    if (SSL_client_hello_get0_ext(ssl, TLSEXT_TYPE_server_name, &extension_data, &extension_len)) {
		/*
		 * The server_name extension was given too much extensibility when it
		 * was written, so parsing the normal case is a bit complex.
		 */
		size_t len;
		if (extension_len <= 2)
			goto abort;
		/* Extract the length of the supplied list of names. */
		len = (*extension_data++) << 8;
		len |= *extension_data++;
		if (len + 2 != extension_len)
			goto abort;
		/*
		 * The list in practice only has a single element, so we only consider
		 * the first one.
		 */
		if (len == 0 || *extension_data++ != TLSEXT_NAMETYPE_host_name)
			goto abort;
		extension_len = len - 1;
		/* Now we can finally pull out the byte array with the actual hostname. */
		if (extension_len <= 2)
			goto abort;
		len = (*extension_data++) << 8;
		len |= *extension_data++;
		if (len == 0 || len + 2 > extension_len || len > TLSEXT_MAXLEN_host_name
		    || memchr(extension_data, 0, len) != NULL)
			goto abort;
		servername = extension_data;
		servername_len = len;
	}
	
    if ( servername )
    {
        string sServerName(servername, servername+servername_len);
        //mtfa_log( "ServerName: %s\n", sServerName.c_str() );
        auto ctx = server_names_and_context[sServerName];
        //mtfa_log("Il CTX: %llu\n", (unsigned long long)ctx);
        if (ctx)
            ssl_sock_switchctx_set ( ssl,  ctx);
        else
        {
            mtfa_log("CTX non trovato per il server name: %s. Usa il default ctx.\n", sServerName.c_str());
            auto ctx = server_names_and_context.begin()->second;
            mtfa_log("Il default CTX: %llu\n", (unsigned long long)ctx);
            if (ctx)
                ssl_sock_switchctx_set ( ssl,  ctx);
        }
        return 1;
    }
    else
    {
        mtfa_log("ServerName is NULL\n"); //proseguo con il default ctx
        auto ctx = server_names_and_context.begin()->second;
        mtfa_log("Il default CTX: %llu\n", (unsigned long long)ctx);
        if (ctx)
            ssl_sock_switchctx_set ( ssl,  ctx);
        return 1;
    }
    
abort:
    *al = SSL_AD_UNRECOGNIZED_NAME;
    return 0;

}
#endif

// Server Name Indication callback from OpenSSL
static int serverNameCallback(SSL *ssl, int *al, void *priv);

static int serverNameCallback ( SSL* ssl, int* ad, void* arg )
{
    if ( ssl == NULL )
    {
        cout << "SSL-ERR: SSL_TLSEXT_ERR_NOACK" << endl;
        return SSL_TLSEXT_ERR_NOACK;
    }

    const char* servername = SSL_get_servername ( ssl, TLSEXT_NAMETYPE_host_name );

    if ( servername )
    {
        //mtfa_log("Callback called: %s\n", servername);

        auto sServerName = string(servername);
        //Lo inserisco nel buffer
        ((T_EventoStruct*)arg)->server_name = sServerName;
        
        //mtfa_log( "SSL-ERR: ServerName: %s\n", sServerName.c_str() );
        auto ctx = server_names_and_context[sServerName];
        //mtfa_log("Il CTX: %llu\n", (unsigned long long)ctx);
        if (ctx)
            ssl_sock_switchctx_set ( ssl,  ctx);
        else
        {
            mtfa_log("CTX non trovato\n");
            for (auto it: server_names_and_context)
            {
                cout << it.first << ", " << it.second << endl;
            }
        }
        //SSL_set_SSL_CTX ( ssl, server_names_and_context[servername] );
        return SSL_TLSEXT_ERR_OK;
    }
    else
    {
        //mtfa_log("SSL-ERR: ServerName is NULL\n");
        return SSL_TLSEXT_ERR_NOACK;
    }
}


static int verify_client_certificate ( X509_STORE_CTX* ctx, void* data )
{
    //mtfa_log("verifica torna 1\n");
    return 1;
}

static int verify_callback ( int v, X509_STORE_CTX* ctx )
{
    //mtfa_log("verifica torna 1\n");
    return 1;
}


static void* ManageLBUserThread ( void* v_args )
{
    int i;
    int socklen;

    T_ManageLBUserArgs* args = ( T_ManageLBUserArgs* ) v_args;

    std::cout << __func__   << "I=" << args->thnum << ". Thread: my id = " << std::this_thread::get_id() << "\n" << "        my pthread id = " << pthread_self() << "\n";

    //Rilascia il thread!
    vthreads_bool[args->thnum] = true;
    cout << __func__ << " - " << "DONE THREAD " << args->thnum << endl;
    usleep ( 100000 );

    event_base* base;
    base = event_base_new();

    if ( !base )
    {
        std::cout << __func__   << " - " << StrTime() << ": not based" << std::endl;
        exit ( -1 );
    }

    if ( evthread_make_base_notifiable ( base ) < 0 )
    {
        std::cout << __func__   << " - " << StrTime() << ": base not managed by threads." << std::endl;
    }

    vector<string> v_in_http_addr;
    v_in_http_addr.push_back ( args->in_ip + ":" + to_string ( ( unsigned long long ) args->in_port ) );

    if ( args->ipsAndPorts.size() > 0 )
        v_in_http_addr.assign ( args->ipsAndPorts.begin(), args->ipsAndPorts.end() );

    mtfa_log("Listening on: ");

    for ( auto it : v_in_http_addr )
        cout << it << ", ";

    cout << endl << endl;

    //isSSL, sockaddr(ip,port), ip, port, cert, key, ctx*
    //oppure TCP...
    vector<T_argument_tuple> v_listen_on_addr ( v_in_http_addr.size() );

    for ( int i = 0; i < v_listen_on_addr.size(); ++i )
    {
        //se qui v_in_http_addr[i] contiene anche i certificati, allora è un ssl
        vector<string> quadruple = mtfa_string_split ( v_in_http_addr[i], ":" );

        if ( quadruple.size() == 2 )   //HTTP
        {
            mtfa_log("HTTP: %s\n", v_in_http_addr[i].c_str());
            struct sockaddr_storage sas;
            memset ( & ( sas ), 0, sizeof ( sas ) );
            socklen = sizeof ( sas );

            if ( evutil_parse_sockaddr_port ( ( quadruple[0] + ":" + quadruple[1] ).c_str(), ( struct sockaddr* ) &sas, &socklen ) < 0 )
            {
                mtfa_log("Errore parsing the input address: %s\n", v_in_http_addr[i].c_str());
                exit ( -1 );
            }

            v_listen_on_addr[i] = make_tuple ( Protocols::eProtHTTP, sas, quadruple[0], atoi ( quadruple[1].c_str() ), "", "", nullptr, base, "", "" );
        }
        else if ( quadruple.size() == 3 )     //TCP
        {
            mtfa_log("TCP: %s\n", v_in_http_addr[i].c_str());
            struct sockaddr_storage sas;
            memset ( & ( sas ), 0, sizeof ( sas ) );
            socklen = sizeof ( sas );

	    std::cerr << "FFFFF: " << quadruple[0] << ":" << quadruple[1] << std::endl;
            if ( evutil_parse_sockaddr_port ( ( quadruple[0] + ":" + quadruple[1] ).c_str(), ( struct sockaddr* ) &sas, &socklen ) < 0 )
            {
                cout << __func__   << " - " << StrTime() << "Errore parsing the input address: " << v_in_http_addr[i] << endl;
                exit ( -1 );
            }

            v_listen_on_addr[i] = make_tuple ( Protocols::eProtTCP, sas, quadruple[0], atoi ( quadruple[1].c_str() ), string ( "M; " + Base64::encode ( quadruple[2] ) ), "", nullptr, base, "", "" );
        }
        else if ( quadruple.size() == 5 )     //SSL
        {
            mtfa_log("SSL: %s\n", v_in_http_addr[i].c_str());
            struct sockaddr_storage sas;
            memset ( & ( sas ), 0, sizeof ( sas ) );
            socklen = sizeof ( sas );

            if ( evutil_parse_sockaddr_port ( ( quadruple[0] + ":" + quadruple[1] ).c_str(), ( struct sockaddr* ) &sas, &socklen ) < 0 )
            {
                mtfa_log("Errore parsing the input address: %s\n", v_in_http_addr[i].c_str());
                exit ( -1 );
            }

            v_listen_on_addr[i] = make_tuple ( Protocols::eProtSSL, sas, quadruple[0], atoi ( quadruple[1].c_str() ), quadruple[2], quadruple[3], nullptr, base, quadruple[4], "" );
        }
        else if ( quadruple.size() == 6 )     //SSL with mutual authentication
        {
            mtfa_log("SSL: %s", v_in_http_addr[i].c_str());
            struct sockaddr_storage sas;
            memset ( & ( sas ), 0, sizeof ( sas ) );
            socklen = sizeof ( sas );

            if ( evutil_parse_sockaddr_port ( ( quadruple[0] + ":" + quadruple[1] ).c_str(), ( struct sockaddr* ) &sas, &socklen ) < 0 )
            {
                mtfa_log("Errore parsing the input address: %s\n", v_in_http_addr[i].c_str());
                exit ( -1 );
            }

            v_listen_on_addr[i] = make_tuple ( Protocols::eProtSSL, sas, quadruple[0], atoi ( quadruple[1].c_str() ), quadruple[2], quadruple[3], nullptr, base, quadruple[4], quadruple[5] );
        }
        else
        {
            mtfa_log("protocol not recognized.\n");
            exit ( -1 );
        }
    }

//    memset(&listen_on_addr, 0, sizeof (listen_on_addr));
//    socklen = sizeof (listen_on_addr);
//
//    if (evutil_parse_sockaddr_port(in_http_addr.c_str(), (struct sockaddr*) &listen_on_addr, &socklen) < 0) {
//        std::cout << __func__   << StrTime() << "Errore parsing the input address: " << in_http_addr << std::endl;
//        exit(-1);
//    }

    vector<struct evconnlistener*> v_listener ( v_listen_on_addr.size() );

    for ( int i = 0; i < v_listen_on_addr.size(); ++i )
    {
        //ma è un SSL?
        if ( get<0> ( v_listen_on_addr[i] ) == Protocols::eProtSSL )
        {
            //è un SSL
            mtfa_log("INIZIALIZZA SSL\n");
            
            //Tutto è deciso in base alla chiave privata
            auto kpriAndOther = get<5> ( v_listen_on_addr[i] );
            T_V_Split vkpri=mtfa_string_split(kpriAndOther, "|");
            //Deve essere lungo 3
            if (vkpri.size() != 3)
            {
                mtfa_log("Error. The private key field must be structured in <privatekey|server method|cipher list\n"
                "Example: :multiple_domains/privkey4.pem|dtls|HIGH,!aNULL,!kRSA,!PSK,!SRP,!MD5,!RC4:\n"
                "Note the <,> instead of <:> in the cipher list. The translation to <:> is made inside the code\n"
                "If the fileds are empty, the default values are the same of the example\n");
                exit(-1);
            }

            //allora, la chiave privata
            auto kpri = vkpri[0];

            //il metodo server
            auto serverMethod = vkpri[1];
            if (serverMethod.size()==0)
                serverMethod = "dtls";

            //la cipher list
            auto cipherList = vkpri[2];
            if (cipherList.size()==0)
                cipherList = "HIGH:!aNULL:!kRSA:!PSK:!SRP:!MD5:!RC4";
            else
                cipherList = mtfa_replace_matching_pattern(cipherList, ",", ":", true);

            //il metodo server
            SSL_CTX* ctx = nullptr;
            if (mtfa_find_pattern(serverMethod, "[sS][sS][lL]"))
            {
                mtfa_log("Server method: SSL\n");
                ctx = SSL_CTX_new ( SSLv23_server_method());
            }
            else if (mtfa_find_pattern(serverMethod, "[dD][tT][lL][sS]"))
            {
                mtfa_log("Server method: DTLS (flexible)\n");
                ctx = SSL_CTX_new ( DTLS_server_method());
            }
            else if (mtfa_find_pattern(serverMethod, "[tT][lL][sS]"))
            {
                mtfa_log("Server method: TLS\n");
                ctx = SSL_CTX_new ( TLS_server_method());
            }
            else
            {
                mtfa_log("Server method: DTLS (flexible)\n");
                ctx = SSL_CTX_new ( DTLS_server_method());
            }
            
            long mode =
                SSL_MODE_ENABLE_PARTIAL_WRITE |
                SSL_MODE_ACCEPT_MOVING_WRITE_BUFFER |
                SSL_MODE_RELEASE_BUFFERS;
//                 |
//                 SSL_MODE_ASYNC;

            SSL_CTX_set_mode(ctx, mode);

            get<6> ( v_listen_on_addr[i] ) = ctx;
            
//             SSL_CTX_set_options(ctx, SSL_OP_NO_ANTI_REPLAY);
//             SSL_CTX_set_max_early_data(ctx, global.tune.bufsize - global.tune.maxrewrite);


            //La cipherlist: assegna la lista di algoritmi di cifra utilizzabili
            if ( !SSL_CTX_set_cipher_list(ctx, (const char*)cipherList.c_str()))
            {
                mtfa_log("Failed to SSL_CTX_set_cipher_list: %s\n", cipherList.c_str());
                exit ( -1 );
            }

            if ( !SSL_CTX_use_certificate_chain_file ( ctx, get<4> ( v_listen_on_addr[i] ).c_str() ) )
            {
                mtfa_log("Failed to load cert chain: %s\n", get<4> ( v_listen_on_addr[i] ).c_str() );
                exit ( -1 );
            }

            //La chiave privata
            if ( !SSL_CTX_use_PrivateKey_file ( ctx, kpri.c_str(), SSL_FILETYPE_PEM ) )
            {
                mtfa_log("Failed to load private key: %s\n", kpri.c_str());
                exit ( -1 );
            }

            //cout << "Set della callback\n";
            server_names_and_context[get<8> ( v_listen_on_addr[i] )] = ctx;
            //FRANCO!!! SSL_CTX_set_client_hello_cb(ctx, serverNameCallback_1_1, nullptr);
            SSL_CTX_set_tlsext_servername_callback ( ctx, serverNameCallback );

            //Ora verifico se siamo in un regime di mutua autenticazione
            if ( get<9> ( v_listen_on_addr[i] ).length() != 0 )
            {
                mtfa_log("SSL Client enabled\n");

                //carico l'elenco dei nomi delle ca che hanno firmato i certificati client che accettiamo
                STACK_OF ( X509_NAME ) *cert_names;

                mtfa_log("Chain delle CA accettate dal server: %s\n", get<9> ( v_listen_on_addr[i] ).c_str());
                cert_names = SSL_load_client_CA_file ( get<9> ( v_listen_on_addr[i] ).c_str() );

                if ( cert_names != NULL )
                    SSL_CTX_set_client_CA_list ( ctx, cert_names );
                else
                    cout << __func__ << "Error in SSL_load_client_CA_file" << endl;

                //anche se siamo andati in errore, in ogni caso carico le verify locations
                SSL_CTX_load_verify_locations ( ctx, get<9> ( v_listen_on_addr[i] ).c_str(), 0 );

                //Attivo l'opzione di mutua autenticazione
                SSL_CTX_set_verify ( ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, verify_callback); //verify_callback ); // | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, 0);
                //SSL_CTX_set_cert_verify_callback ( ctx, verify_client_certificate, nullptr );
            }
        }

        v_listener[i] = evconnlistener_new_bind (
                            base,
                            accept_cb,
                            ( void* ) &v_listen_on_addr[i], //v_args,
                            LEV_OPT_CLOSE_ON_FREE | LEV_OPT_CLOSE_ON_EXEC | LEV_OPT_REUSEABLE_PORT | LEV_OPT_REUSEABLE | LEV_OPT_THREADSAFE | LEV_OPT_DEFERRED_ACCEPT,
                            65536,
                            ( struct sockaddr* ) &get<1> ( v_listen_on_addr[i] ),
                            sizeof ( get<1> ( v_listen_on_addr[i] ) )
                        );

        if ( v_listener[i] == NULL )
        {
            mtfa_log("");
            perror ( "Error in listening" );
			//cout << get<0>(v_listen_on_addr[i]) << endl;
			cout << get<1>(v_listen_on_addr[i]).ss_family << " ";
			cout << get<2>(v_listen_on_addr[i]) <<  " ";
			cout << (unsigned short)get<3>(v_listen_on_addr[i]) <<  " ";
			cout << get<4>(v_listen_on_addr[i]) <<  " ";
			cout << get<5>(v_listen_on_addr[i]) <<  " ";
			cout << get<6>(v_listen_on_addr[i]) <<  " ";
			cout << get<7>(v_listen_on_addr[i]) <<  " ";
			cout << get<8>(v_listen_on_addr[i]) <<  " ";
			cout << get<9>(v_listen_on_addr[i]) << endl;
			exit ( -1 );
        }
    }

    event_base_dispatch ( base );

    return 0;
}



