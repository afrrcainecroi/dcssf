#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>

#include <boost/dynamic_bitset.hpp>

#include <iostream>
#include <fstream>

//il vr_language
#include "va_scanner.h"
#include "va_parser.hpp"
#include "interpreter.h"
#include "command.h"

#include "mtfafs.h"
#include "mtfa_fs3.h"
#include "mtfa_utils.h"
#include "mtfa_db.h"
#include "VA_ast_parsing.h"
#include "VAengine.h"
#include "base64.h"
#include "mtfa_find.h"
#include <stddef.h>
#include "lb_user.h"
#include "va_commands.h"

using namespace VaParser;

using namespace std;

extern SCM U8;  //"u8"

//static string szero = string("\0x00", 1);

//Questa funzione la metto in VaEngine in modo da poterla utilizzare durante le ACTIONS di KAM
//ma posso usarla sempre, ogni volta che debbo fare la valutazione di una stringa
//è il modo che ho introdotto per definire funzioni e interpretarle con guile all'interno del KAM
//la tmpmap è utilizzata all'interno di una unità di esecuzione di KAM (oppure di VA)

string KamStringEval(const string &s, T_EventoStruct* bufs_in_out, T_um_ss & temporary_kv_assoc)
{
    if (s[0] != '?')
        return s;
    //la stringa è fatta di due parti. Fino al primo spazio bianco è la chiave, per discernere nel lisp tra differenti operazioni, poi è un insieme di valori da passare alla funzione KamRun del LISP, scritta dall'utente
    //la funzione utilizza un insieme di variabili globali (per lei) che sono relative alla connessione in corso
    //e sono quelle prese da bufs_in_out
    int space = s.find_first_of(" ");
    string key;
    string fun;
    if (space == -1)
    {
        key = s.substr(1);
        fun = "";
    }
    else
    {
        key = s.substr(1, space-1);
        fun = s.substr(space+1);
    }
    //Tolgo spazi in fondo
    while (key.back() == ' ')
        key.pop_back();
    while (fun.back() == ' ')
        fun.pop_back();

    //Valuta fun con i dati del bufs in out
//     if (fun.size() == 0)
//     {
//         return temporary_kv_assoc[key];
//     }
//     else
//     {
//         //prendo tutte le variabili che utilizzo e lo sostituisco
//         std::list<string> keys = mtfa_get_all_submatching_patterns ( fun, "EXT-[a-zA-Z][a-zA-Z0-9_-]+");
//         for (auto &kit: keys)
//         {
//             auto val = temporary_kv_assoc[kit.substr(4)];
//             fun = mtfa_replace_matching_pattern(fun, "("+kit+")", val, true);
//         }
//         cout << "Funzione: " << fun << endl;
    //ricorda che fun è un insieme di valori separati da space (blank)
    string sret = DoKamRun(key, temporary_kv_assoc[key], fun, bufs_in_out);
    temporary_kv_assoc[key] = sret;
    return sret;
//     }
}

static string GetValueFromObservation ( VAobservations *obs, VA_ItemTypes tipo, const T_um_ss & varwith, const string name="")
{
    switch ( tipo )
    {
    case VA_ItemTypes::AGENT_ID:
    case VA_ItemTypes::SA_OS:
    case VA_ItemTypes::SA_LOGGED_USER:
    case VA_ItemTypes::SA_PROCESS_NAME:
    case VA_ItemTypes::OBS_NAME:
    case VA_ItemTypes::OBS_TIME:
    case VA_ItemTypes::OBS_OBSID:
    case VA_ItemTypes::OBS_STIME:
    case VA_ItemTypes::OBS_ETIME:
    case VA_ItemTypes::OBS_TAG:
    case VA_ItemTypes::OBS_EVENT:
    case VA_ItemTypes::NET_RESULT:
    case VA_ItemTypes::NET_SESID:
    case VA_ItemTypes::HTTP_METHOD:
    case VA_ItemTypes::HTTP_HOST:
    case VA_ItemTypes::HTTP_HEADER_HOST:
    case VA_ItemTypes::HTTP_URL_HOST:
    case VA_ItemTypes::NET_TCP_IPDST:
    case VA_ItemTypes::NET_TCP_IPSRC:
    case VA_ItemTypes::HTTP_URL:
    case VA_ItemTypes::HTTP_URI:
    {
        //questi sono a valore secco
        auto idx = mtfa_enum_to_int<VA_ItemTypes>(tipo);
        auto it = obs->find(idx);
        if (it == obs->end())
            return "";
        else
            return it->second.value;
    }
    break;

    case VA_ItemTypes::HTTP_COOKIES:
    case VA_ItemTypes::HTTP_HEADERS:
    case VA_ItemTypes::HTTP_QUERY:
    case VA_ItemTypes::HTTP_DATA_POST:
    case VA_ItemTypes::HTTP_ANSWER_CODE:
    case VA_ItemTypes::HTTP_ANSWER_HEADERS:
    case VA_ItemTypes::HTTP_ANSWER_COOKIES:
    case VA_ItemTypes::HTTP_ANSWER_DATA:
    {
        //questi sono a valore indicizzato
        auto it = obs->find(mtfa_enum_to_int<VA_ItemTypes>(tipo));
        if (it == obs->end())
            return "";
        else
        {
            auto it1 = it->second.values.find(name);
            if (it1==it->second.values.end())
                return "";
            else
                return it1->second;
        }
    }
    break;

    case VA_ItemTypes::VARIABLE:
    {
        auto t = varwith.find(name);
        if (t == varwith.end())
            return name;
        else
            return t->second;
    }
    break;
    case VA_ItemTypes::AR_TIME:
    case VA_ItemTypes::NONE:
        cout << __func__ << "Happens also here\n";
        return "";
        break;
    }
}



//Per implementare i comando per verificare l'esistenza di un valore

//bool VAengine::Insert_For_Existence(VA_ItemTypes t, const string & optional_index) {
//    switch (t) {
//        case VA_ItemTypes::OBS_TAG:
//            break;
//        case VA_ItemTypes::HTTP_COOKIES:
//            break;
//        case VA_ItemTypes::HTTP_HEADERS:
//            break;
//        case VA_ItemTypes::HTTP_QUERY:
//            break;
//        case VA_ItemTypes::HTTP_DATA_POST:
//            break;
//        case VA_ItemTypes::HTTP_ANSWER_HEADERS:
//            break;
//        case VA_ItemTypes::HTTP_ANSWER_COOKIES:
//            break;
//        case VA_ItemTypes::HTTP_ANSWER_DATA:
//            break;
//        case VA_ItemTypes::VARIABLE: //l'esistenza di una variabile indica che è stata valorizzata
//            break;
//    }
//}

std::string VA_ActionTypesToString ( VA_ActionTypes t )
{
    switch ( t ) {
    case VA_ActionTypes::ADD_VALUE_TO_SET:
        return "VA_ActionTypes::ADD_VALUE_TO_SET";
    case VA_ActionTypes::DEL_VALUE_FROM_SET:
        return "VA_ActionTypes::DEL_VALUE_FROM_SET";
    case VA_ActionTypes::DISABLE_AR:
        return "VA_ActionTypes::DISABLE_AR";
    case VA_ActionTypes::DISABLE_VR:
        return "VA_ActionTypes::DISABLE_VR";
    case VA_ActionTypes::ENABLE_AR:
        return "VA_ActionTypes::ENABLE_AR";
    case VA_ActionTypes::ENABLE_VR:
        return "VA_ActionTypes::ENABLE_VR";
    case VA_ActionTypes::ISSUE_VREP:
        return "VA_ActionTypes::ISSUE_VREP";
    case VA_ActionTypes::ISSUE_MVREP:
        return "VA_ActionTypes::ISSUE_MVREP";
    case VA_ActionTypes::SET_VAR_OF_AR:
        return "VA_ActionTypes::SET_VAR_OF_AR";
    case VA_ActionTypes::NOP:
        return "VA_ActionTypes::NOP";
    case VA_ActionTypes::MYSQL_APPEND:
        return "VA_ActionTypes::MYSQL_APPEND";
    default:
        cout << __func__ << __func__ << ": error decoding type" << endl;
    }
    return "VA_ActionTypes::NOP";
}

// static void AddItemToBitSet ( unordered_map<string, boost::dynamic_bitset<>> &aggr, string value, int pos )
// {
//      aggr[value].set ( pos );
// }

bool VAengine::CheckCoherence()
{
    //parto dalle acrules
    //    for (auto acrule: acrules)
    //    {
    //        acrule.second.ar_vars
    //    }
    //    this->acrules

    return true;
}


//static void AddItemToBitSet(unordered_map<string, boost::dynamic_bitset<>> & aggr, mtfa_conc_uset<string> &uss, int pos)
//{
//    for (auto value: uss.um)
//    {
//        auto it = aggr.find(value);
//        if (it == aggr.end())
//        {
//            boost::dynamic_bitset<> * dbs = new boost::dynamic_bitset<>(maxsize);
//            dbs->set(pos, true);
//            aggr.insert(make_pair(value, dbs));
//        }
//        else
//            it->second->set(pos, true);
//    }
//}

// static void prepare_starip ( mtfa_sequence_star * baseds, const unordered_set<string>& data, int ident )
// {
//      if ( data.size() == 0 )
//           return;
//
//      for ( auto it : data ) {
//           //per ogni ip presente, faccio un vettore di short, compresa la porta
//           string port, ip;
//           T_V_Split vs = mtfa_string_split ( it, ":" );
//           ip = vs[0];
//           if ( vs.size() > 1 )
//                port = vs[1];
//           else
//                port = "*";
//
//           //ora splitto ip
//           T_V_Split vip = mtfa_string_split ( ip, "." );
//
//           //a questo punto ho i quattro ip e ho la porta. Converto in network order (6 bytes -> 6 short)
//           vector<unsigned short> item ( 6 );
//           for ( int i = 0; i < 4; ++i ) {
//                if ( vip[i].compare ( "*" ) == 0 )
//                     item[i] = mtfa_sequence_star::jollyvalue;
//                else
//                     item[i] = atoi ( vip[i].c_str() );
//           }
//
//           //ora la porta
//           if ( port.compare ( "*" ) == 0 ) {
//                item[4] = mtfa_sequence_star::jollyvalue;
//                item[5] = mtfa_sequence_star::jollyvalue;
//           } else {
//                //big endian (network order)
//                short p = atoi ( port.c_str() );
//                item[4] = ( ( p >> 8 ) & 0x00FF );
//                item[5] = ( p & 0x00FF );
//           }
//           baseds->Insert ( item, ident );
//      }
//      //    baseds->Build();
//      //    return baseds;
// }


//Attenzione! va modificata per gestire 3 byte e non 4!!

// static void prepare_starurl ( mtfa_sequence_star * baseds, const unordered_set<string>& data, PHash &ph_pages, int ident )
// {
//      unordered_set<string> base = data;
//      if ( base.size() == 0 )
//           return;
//
//      for ( auto it : base ) {
//           //per ogni url presente, faccio un vettore di short
//           T_V_Split vs = mtfa_string_split ( it, "/" );
//
//           //Se ci sono elementi vuoti, li elimino, ad esempio il primo elemento oppure l'ultimo!!!
//           auto jt = vs.begin();
//           while ( jt != vs.end() ) {
//                if ( ( *jt ).size() == 0 ) {
//                     jt = vs.erase ( jt );
//                } else
//                     ++jt;
//           }
//
//           //A questo punto ho un vettore che contiene stringhe oppure asterischi
//           //Lo devo convertire in array di interi e poi, per ogni intero, un array di 4 short
//           vector<unsigned short> item ( 4 * VASets::url_deep ); //se sono 4 byte per intero (e quindi per item della url) allora sono 4*url_deep short
//           int value;
//           const unsigned char * pvalue = ( unsigned char* ) &value;
//           for ( unsigned int i = 0; i < VASets::url_deep; ++i ) {
//                if ( vs.size() <= i ) {
//                     item[4 * i] = mtfa_sequence_star::jollyvalue;
//                     item[4 * i + 1] = mtfa_sequence_star::jollyvalue;
//                     item[4 * i + 2] = mtfa_sequence_star::jollyvalue;
//                     item[4 * i + 3] = mtfa_sequence_star::jollyvalue;
//                } else if ( vs[i].size() == 0 ) {
//                     value = 0;
//                } else if ( vs[i].compare ( "*" ) == 0 ) {
//                     item[4 * i] = mtfa_sequence_star::jollyvalue;
//                     item[4 * i + 1] = mtfa_sequence_star::jollyvalue;
//                     item[4 * i + 2] = mtfa_sequence_star::jollyvalue;
//                     item[4 * i + 3] = mtfa_sequence_star::jollyvalue;
//                } else {
//                     value = ph_pages.insert ( vs[i] );
//
//                     //converto in network order (big endian)
//                     item[4 * i + 3] = value & 0x000000FF;
//                     value >>= 8;
//                     item[4 * i + 2] = value & 0x000000FF;
//                     value >>= 8;
//                     item[4 * i + 1] = value & 0x000000FF;
//                     value >>= 8;
//                     item[4 * i] = value & 0x000000FF;
//                }
//
//
//           }
//           baseds->Insert ( item, ident );
//      }
//      //    baseds->Build();
//      //    return baseds;
// }

bool VAengine::RebuildARDataSet()
{
#if 0
    //Ho una struttura dati che, per ogni elemento (stringa) che inserisco, dichiaro anche in quali regole (indicate dal loro indice numerico) si trova
    PHash acrules_ph_pages;
    unordered_map<int, string> index2acrule;
    int numofacrules = acrules.size();

    //Ogni agent_id ha associate le ar che lo trattano
    //Ma i bitset hanno come dimensione il numero di AR
    vector<unordered_map<string, boost::dynamic_bitset<>>> aggr_values(mtfa_enum_to_int(VA_ItemTypes::VA_ItemTypes_MaxIndex)); //Gli indici sono i numeri associati ai tipi di VA_ItemTypes

    //Per prima cosa, costruisco per ogni osservabile, l'insieme dei valori e, per ogni valore, l'insieme delle ar che li utilizzano.
    //poi faccio le strutture a accesso diretto
    int index = 0;
    for ( auto & ar : acrules )
    {
        //Di ogni access rule, definisco un indice: index e associo il nome della ar all'indice, per ricordarmelo!
        index2acrule[index] = ar.first;

        //Per ogni criterio di accesso della access rule, riorganizzo gli elementi.
        for ( VAcriteria & bac : ar.second.basic_access_criteria )
        {
            int i_bac_itype= mtfa_enum_to_int(bac.itype);
            switch ( bac.itype )
            {
            //In base al tipo, aggrego in modi differenti
            case VA_ItemTypes::AGENT_ID:
            case VA_ItemTypes::HTTP_ANSWER_CODE:
            case VA_ItemTypes::HTTP_HOST:
            case VA_ItemTypes::HTTP_METHOD:
            case VA_ItemTypes::HTTP_URL_HOST:
            case VA_ItemTypes::HTTP_HEADER_HOST:
            case VA_ItemTypes::NET_RESULT:
            case VA_ItemTypes::NET_SESID:
            case VA_ItemTypes::OBS_TAG:
            case VA_ItemTypes::HTTP_URI:
            case VA_ItemTypes::HTTP_URL:
                //ricorda di aggiungere anche il TIME!!!
            {
                switch ( bac.op )
                {
                case VA_OpTypes::EQUAL:
                    if ( aggr_values[i_bac_itype][bac.op1].size() == 0 )
                    {
                        aggr_values[i_bac_itype][bac.op1].resize ( numofacrules, false );
                    }
                    aggr_values[i_bac_itype][bac.op1].set ( index, true );
                    break;
                case VA_OpTypes::EXISTS: //non applicable
                    break;
                case VA_OpTypes::IN:
                {
                    auto ds = vasets.datasets.find ( bac.op1 );
                    if ( ds != vasets.datasets.end() )
                    {
                        for ( auto & it : ds->second->uss->um )
                        {
                            if ( aggr_values[i_bac_itype][it].size() == 0 )
                            {
                                aggr_values[i_bac_itype][it].resize ( numofacrules, false );
                            }
                            aggr_values[i_bac_itype][it].set ( index, true );
                        }
                    }
                }
                break;
                }
            }
            break;
            case VA_ItemTypes::HTTP_COOKIES:
            case VA_ItemTypes::HTTP_ANSWER_COOKIES:
            case VA_ItemTypes::HTTP_ANSWER_DATA:
            case VA_ItemTypes::HTTP_ANSWER_HEADERS:
            case VA_ItemTypes::HTTP_DATA_POST:
            case VA_ItemTypes::HTTP_HEADERS:
            case VA_ItemTypes::HTTP_QUERY:
            {
                switch ( bac.op )
                {
                case VA_OpTypes::EQUAL:
                    if ( aggr_values[bac.itype][bac.op1 + "|" + bac.op2].size() == 0 )
                    {
                        aggr_values[bac.itype][bac.op1 + "|" + bac.op2].resize ( numofacrules, false );
                    }
                    aggr_values[bac.itype][bac.op1 + "|" + bac.op2].set ( index, true );
                    break;
                case VA_OpTypes::EXISTS:
                    if ( aggr_values[bac.itype][bac.op1].size() == 0 )
                    {
                        aggr_values[bac.itype][bac.op1].resize ( numofacrules, false );
                    }
                    aggr_values[bac.itype][bac.op1].set ( index, true );
                    break;
                case VA_OpTypes::IN:
                    auto ds = vasets.dataset().find ( bac.op1 );
                    if ( ds != vasets.dataset().end() )
                    {
                        for ( auto it : ds->second->uss.um )
                        {
                            if ( aggr_values[bac.itype][bac.op2 + "|" + it].size() == 0 )
                            {
                                aggr_values[bac.itype][bac.op2 + "|" + it].resize ( numofacrules, false );
                            }
                            aggr_values[bac.itype][bac.op2 + "|" + it].set ( index, true );
                        }
                    }
                    break;
                }
            }
            break;
            }
        }
    }

    //ora che ho aggregato, posso costruire le strutture dati...
    //ma prima stampo il tutto
    cout << endl << endl << endl;
    for ( auto &it : aggr_values )
    {
        cout << "Type: " << VA_ItemTypesToString ( it.first ) << endl;
        for ( auto &val : it.second )
        {
            cout << "\t" << val.first << ": " << val.second.to_ulong() << endl;
        }
    }
#endif
    return true;
}

void VAengine::InsertIntoTable ( const string & connection_pars, T_Vrep & lvrep, T_um_ss & varwith, VAobservations *obs )
{
    //in var1 i parametri di accesso.
    // action.val1 =                Base64::encode(ip_port) + ":" +
    //                              Base64::encode(s_db) + ":" +
    //                              Base64::encode(s_user) + ":" +
    //                              Base64::encode(s_password) + ":" +
    //                              Base64::encode(s_query);

    T_V_Split vs = mtfa_string_split ( connection_pars, ":" );
    if ( vs.size() != 5 ) {
        cerr << __func__ << ": error in number of parameters." << endl;
        return;
    }
    //ora debaso
    for ( int i = 0; i < vs.size(); ++i ) {
        vs[i] = Base64::decode ( vs[i] );
    }

    string s_ip;
    string s_port;
    T_V_Split ippo = mtfa_string_split ( vs[0], ":" );
    if ( ippo.size() == 2 ) {
        s_ip = ippo[0];
        s_port = ippo[1];
    } else {
        s_ip = ippo[0];
        s_port = "3306";
    }
    string s_user = vs[2];
    string s_password = vs[3];
    string s_db = vs[1];
    string s_query = vs[4];

    //ora, per ogni $[0-9] in s_query, lo sostituisco con il valore corrispondente.
    //calcolo i valori corrispondenti e poi proseguo con le sostituzioni

    vector<string> data;

    //Prima di fare SQL devo calcolare il valore di riferimento

    for ( auto &it : lvrep ) {
        switch ( get<0>(it) ) {
        case VA_ItemTypes::AGENT_ID:
        case VA_ItemTypes::SA_OS:
        case VA_ItemTypes::SA_LOGGED_USER:
        case VA_ItemTypes::SA_PROCESS_NAME:
        case VA_ItemTypes::OBS_NAME:
        case VA_ItemTypes::OBS_TIME:
        case VA_ItemTypes::OBS_OBSID:
        case VA_ItemTypes::OBS_STIME:
        case VA_ItemTypes::OBS_ETIME:
        case VA_ItemTypes::OBS_TAG:
        case VA_ItemTypes::OBS_EVENT:
        case VA_ItemTypes::NET_RESULT:
        case VA_ItemTypes::NET_SESID:
        case VA_ItemTypes::HTTP_METHOD:
        case VA_ItemTypes::HTTP_HOST:
        case VA_ItemTypes::HTTP_HEADER_HOST:
        case VA_ItemTypes::HTTP_URL_HOST:
        case VA_ItemTypes::NET_TCP_IPDST:
        case VA_ItemTypes::NET_TCP_IPSRC:
        case VA_ItemTypes::HTTP_URL:
        case VA_ItemTypes::HTTP_URI: {
            auto found = obs->find ( mtfa_enum_to_int(get<0>(it)) );
            if ( found != obs->end() ) {
                data.push_back ( found->second.value );
            } else {
                cout << "non trovato!\n";
            }
        }
        break;

        case VA_ItemTypes::HTTP_COOKIES:
        case VA_ItemTypes::HTTP_HEADERS:
        case VA_ItemTypes::HTTP_QUERY:
        case VA_ItemTypes::HTTP_DATA_POST:
        case VA_ItemTypes::HTTP_ANSWER_CODE:
        case VA_ItemTypes::HTTP_ANSWER_HEADERS:
        case VA_ItemTypes::HTTP_ANSWER_COOKIES:
        case VA_ItemTypes::HTTP_ANSWER_DATA: {
            auto found = obs->find ( mtfa_enum_to_int(get<0>(it)) );
            if ( found != obs->end() ) {
                data.push_back ( found->second.values[get<1>(it)] );
            } else {
                cout << "non trovato!\n";
            }
        }
        break;

        case VA_ItemTypes::VARIABLE: {
            auto val = varwith.find ( get<1>(it) );
            if ( val == varwith.end() ) {
                data.push_back ( get<1>(it) );
            } else {
                data.push_back ( val->second );
            }
        }
        break;
        case VA_ItemTypes::AR_TIME:
            cout << __func__ << "IN " << __func__ << ": " << "VA_ItemTypes::AR_TIME" << endl;
            break;
        default:
            cout << __func__ << "IN " << __func__ << ": " << "default" << endl;
            break;
        }
    }
    if ( data.size() == 0 ) {
        cout << __func__ << __func__ << ": warning. Data value not acquired" << endl;
    }

    //uri, url, host, size
//     if ( data.size() != 4 ) {
//         for ( int i = 0; i < data.size(); ++i ) {
//             cout << __func__ << ". Data[" << i << "]: " << data[i] << endl;
//         }
//         cout << endl;
//     }

    //rimpiazzo i dollari con i valori
    for ( int i = 0; i < data.size(); ++i ) {
        string replace = "$" + to_string ( i );
        size_t pos = s_query.find ( replace );
        while ( pos != string::npos ) {
            s_query = s_query.replace ( pos, 2, "'" + data[i] + "'" );
            pos = s_query.find ( replace );
        }
    }

    //Se sono avanzati dei dollari è perché sono assenti le osservazioni relative: metto a vuoto
    {
        static pcre2_code * reCompiled = mtfa_compile_pattern( "\\$[0-9]" );
        s_query = mtfa_replace_matching_pattern ( s_query, reCompiled, "''", true );
    }

    //ora abbiamo sostituito tutto e quindi possiamo effettuare la query
    mysql_db_class * db = new mysql_db_class ( s_ip.c_str(), s_user.c_str(), s_password.c_str(), s_db.c_str(), atoi ( s_port.c_str() ) );
    bool bret = db->do_connect();
    if ( !bret ) {
        cout << __func__ << __func__ << ": unable to connect to the db: " << vs[1] << " with user: " << vs[2] << " and password: " << vs[3] << " on host: " << s_ip << ":" << s_port << endl;
        return;
    }

    cout << __func__ << ". QUERY: " << s_query << endl;
    list<list < string>> dbresult;
    bret = db->doMySqlQuery ( &dbresult, s_query );
    if ( !bret ) {
        cout << __func__ << ": unable to execute query: " << s_query << endl << "on the db: " <<
             vs[1] << " with user: " << vs[2] << " and password: " << vs[3] << " on host: " << s_ip << ":" << s_port << endl;
    }
    db->do_disconnect();
    delete db;
}

//Issued_memory_validation_reports

// void VAengine::IssueMemoryValidationReport ( string & name, list<pair<VA_ItemTypes, string>> &lvrep, unordered_map<string, string>&varwith, VAobservations *obs )
// {
//    auto found = issued_memory_validation_reports.find(name);
//    if (found == issued_memory_validation_reports.end())
//    {
//        T_MemoryVRData &p = found->second;
//        p.counter=1;
//        p.first_added=GetULLTime();
//        p.last_time_added=p.first_added;
//    }
//    else
//    {
//        T_MemoryVRData &p = found->second;
//        p.counter++;
//        p.last_time_added=GetULLTime();
//    }
// }

void VAengine::IssueValidationReport ( string & name, T_Vrep &lvrep, unordered_map<string, string>&varwith, VAobservations *obs )
{
    if ( obs->size() == 0 ) {
        return;
    }

    //Posso avere due thread che scrivono nello stesso VReport? Potrei poichè quando arriva
    //una comunicazione questa viene presa da un solo thread per volta!

    std::ofstream fou;
    //string validation_report_file_name = validation_report_directory_name + "/" + to_string ( GetULLTime() ) + "_" + GetStrTime ( time ( nullptr ) ) + "_" + name;
    string validation_report_file_name = validation_report_directory_name + "/" + name;
    //mtfa_log("Opened %s output file\n", validation_report_file_name.c_str());
    fou.open ( validation_report_file_name, ios::app | ios::out );
    //fou << GetStrTime ( time ( nullptr ) ) << "|" << GetULLTime() << "|";
    //mtfa_log("Wrote %s into output file\n", GetStrTime ( time ( nullptr ) ).c_str());

    for ( auto vrep : lvrep ) {
        switch ( get<0>(vrep) ) {
        case VA_ItemTypes::AGENT_ID:
        case VA_ItemTypes::SA_OS:
        case VA_ItemTypes::SA_LOGGED_USER:
        case VA_ItemTypes::SA_PROCESS_NAME:
        case VA_ItemTypes::OBS_NAME:
        case VA_ItemTypes::OBS_TIME:
        case VA_ItemTypes::OBS_OBSID:
        case VA_ItemTypes::OBS_STIME:
        case VA_ItemTypes::OBS_ETIME:
        case VA_ItemTypes::OBS_TAG:
        case VA_ItemTypes::OBS_EVENT:
        case VA_ItemTypes::NET_RESULT:
        case VA_ItemTypes::NET_SESID:
        case VA_ItemTypes::HTTP_METHOD:
        case VA_ItemTypes::HTTP_HOST:
        case VA_ItemTypes::HTTP_HEADER_HOST:
        case VA_ItemTypes::HTTP_URL_HOST:
        case VA_ItemTypes::NET_TCP_IPDST:
        case VA_ItemTypes::NET_TCP_IPSRC:
        case VA_ItemTypes::HTTP_ANSWER_CODE:
        case VA_ItemTypes::HTTP_URL:
        case VA_ItemTypes::HTTP_URI: {
            auto found = obs->find ( mtfa_enum_to_int(get<0>(vrep)) );
            if ( found != obs->end() ) {
                auto tmp0 = get<0>(vrep);
                auto tmp1 = get<1>(vrep);
                auto tmp2 = get<2>(vrep);
                auto tmp3 = get<3>(vrep);
                fou << VA_ItemTypesToString ( get<0>(vrep) ) << ": " << remove_pattern(get<2>(vrep), found->second.value, get<3>(vrep)) << "|";
                //mtfa_log("0-Wrote a varinto output file\n");
            }
        }
        break;

        case VA_ItemTypes::HTTP_COOKIES:
        case VA_ItemTypes::HTTP_HEADERS:
        case VA_ItemTypes::HTTP_QUERY:
        case VA_ItemTypes::HTTP_DATA_POST:
        case VA_ItemTypes::HTTP_ANSWER_HEADERS:
        case VA_ItemTypes::HTTP_ANSWER_COOKIES:
        case VA_ItemTypes::HTTP_ANSWER_DATA: {
            auto found = obs->find ( mtfa_enum_to_int(get<0>(vrep)) );
            if ( found != obs->end() ) {
                auto tmp0 = get<0>(vrep);
                auto tmp1 = get<1>(vrep);
                auto tmp2 = get<2>(vrep);
                auto tmp3 = get<3>(vrep);
                fou << VA_ItemTypesToString ( get<0>(vrep) ) << "[" << get<1>(vrep) << "]=" << remove_pattern(get<2>(vrep), found->second.values[get<1>(vrep)], get<3>(vrep)) << "|";
                //mtfa_log("1-Wrote a varinto output file\n");
            }
        }
        break;

        case VA_ItemTypes::VARIABLE: {
            fou << VA_ItemTypesToString ( get<0>(vrep) ) << "[" << get<1>(vrep) << "]=" <<
                remove_pattern(get<2>(vrep), varwith[get<1>(vrep)], get<3>(vrep))  << "|";
                mtfa_log("2-should Wrote a varinto output file\n");
//                for ( auto p : varwith ) {
//                     fou << p.first << ": " << p.second << endl;
//                     //cout << p.first << ": " << p.second << endl;
//                }
        }
        break;
        case VA_ItemTypes::CAT: {
            string value;
            //Questa va stampata, quindi, la stampo
            fou << "CAT{";
            string data;
            for (auto & ti: get<4>(vrep))
            {
                auto s = GetValueFromObservation ( obs, get<0>(ti), varwith, get<1>(ti) );
                //cout << "<" << s << ">" << endl;
                if (s.size() == 0)
                    continue;
                data += remove_pattern(get<2>(ti), s, get<3>(ti)); // + "\x00";
                //fou << VA_ItemTypesToString(get<0>(ti));
                //if (get<1>(ti).size() > 0)
//                 {
//                     //auto s = GetValueFromObservation ( obs, get<0>(ti), get<1>(ti) );
//
//                     auto found = obs->find ( mtfa_enum_to_int(get<0>(ti)) );
//                     if ( found != obs->end() ) {
//                         auto tmp0 = get<0>(ti);
//                         auto tmp1 = get<1>(ti);
//                         auto tmp2 = get<2>(ti);
//                         auto tmp3 = get<3>(ti);
//                         auto s = remove_pattern(get<2>(ti), (found->second.values.size()==0)?found->second.value:found->second.values[tmp1], get<3>(ti));
//                         fou << s;
//                     }
//                 }
            }
            //mtfa_log("3-Wrote %s varinto output file\n", data.c_str());
            fou << data << "}|\n";
        }
        break;
        case VA_ItemTypes::AR_TIME:
            cout << __func__ << "IN " << __func__ << ": " << "VA_ItemTypes::AR_TIME" << "|";
            break;
        default:
            cout << __func__ << "IN " << __func__ << ": " << "default" << "|";
            break;
        }
    }

    fou << endl;
//    fou << "Ciao come va"<<endl;
    fou.close();
//    mtfa_log("Closes report file!!\n");
}

string GetFromVarWith(const unordered_map<string, string>&varwith, const string & s)
{
    auto found = varwith.find(s);
    if (found == varwith.end())
        return s;
    else
        return found->second;
}

void VAengine::AddDelValueToSet ( VAaction action, unordered_map<string, string>&varwith, VAobservations *obs, T_um_ss & temporary_kv_assoc )
{
    switch ( action.observable ) {
    case VA_ItemTypes::AGENT_ID:
    case VA_ItemTypes::SA_OS:
    case VA_ItemTypes::SA_LOGGED_USER:
    case VA_ItemTypes::SA_PROCESS_NAME:
    case VA_ItemTypes::OBS_NAME:
    case VA_ItemTypes::OBS_TIME:
    case VA_ItemTypes::OBS_OBSID:
    case VA_ItemTypes::OBS_STIME:
    case VA_ItemTypes::OBS_ETIME:
    case VA_ItemTypes::OBS_TAG:
    case VA_ItemTypes::OBS_EVENT:
    case VA_ItemTypes::NET_RESULT:
    case VA_ItemTypes::NET_SESID:
    case VA_ItemTypes::HTTP_METHOD:
    case VA_ItemTypes::HTTP_HOST:
    case VA_ItemTypes::HTTP_HEADER_HOST:
    case VA_ItemTypes::HTTP_URL_HOST:
    case VA_ItemTypes::NET_TCP_IPDST:
    case VA_ItemTypes::NET_TCP_IPSRC:
    case VA_ItemTypes::HTTP_URL:
    case VA_ItemTypes::HTTP_URI: {
        auto found = obs->find ( mtfa_enum_to_int(action.observable) );
        if ( found != obs->end() ) {
            auto value = remove_pattern(action.pattern, found->second.value, action.replace);
            if ( action.atype == VA_ActionTypes::ADD_VALUE_TO_SET ) {
                vasets.AddItem ( action.val1, value, atoi ( action.val2.c_str() ), true, true );
            } else {
                vasets.DelItem ( action.val1, value, atoi ( action.val2.c_str() ), true, true );
            }
        }
    }
    break;

    case VA_ItemTypes::HTTP_COOKIES:
    case VA_ItemTypes::HTTP_HEADERS:
    case VA_ItemTypes::HTTP_QUERY:
    case VA_ItemTypes::HTTP_DATA_POST:
    case VA_ItemTypes::HTTP_ANSWER_CODE:
    case VA_ItemTypes::HTTP_ANSWER_HEADERS:
    case VA_ItemTypes::HTTP_ANSWER_COOKIES:
    case VA_ItemTypes::HTTP_ANSWER_DATA: {
        auto found = obs->find ( mtfa_enum_to_int(action.observable) );
        if ( found != obs->end() ) {
            if ( action.atype == VA_ActionTypes::ADD_VALUE_TO_SET ) {
                vasets.AddItem ( action.val1, remove_pattern(action.pattern, found->second.values[action.obs_index], action.replace), atoi ( action.val2.c_str() ), true, true );
            } else {
                vasets.DelItem ( action.val1, remove_pattern(action.pattern, found->second.values[action.obs_index], action.replace), atoi ( action.val2.c_str() ), true, true );
            }
        }
    }
    break;

    case VA_ItemTypes::VARIABLE: {
        mtfa_log("The value must be taken from the with variables\n");
        auto value = remove_pattern(action.pattern, KamStringEval(GetFromVarWith(varwith, action.val2), nullptr, temporary_kv_assoc), action.replace);
        if ( action.atype == VA_ActionTypes::ADD_VALUE_TO_SET ) {
            vasets.AddItem ( action.val1, value, atoi ( action.val3.c_str() ), true, true );
        } else {
            vasets.DelItem ( action.val1, value, atoi ( action.val3.c_str() ), true, true );
        }
        //vasets.AddItem(action.val1, action.val2, atoi(action.val3.c_str()), true);
    }
    break;
    case VA_ItemTypes::CAT: {
        string data;
        for (auto & ti: action.cat)
        {
            auto s = GetValueFromObservation ( obs, get<0>(ti), varwith, get<1>(ti) );
            if (s.size() == 0)
                continue;
            data += remove_pattern(get<2>(ti), s, get<3>(ti)); // + "\x00";
        }
        //data.pop_back();   //Tolgo ultimo 0
        data = KamStringEval(data, nullptr, temporary_kv_assoc);
        data = remove_pattern(action.pattern, data, action.replace);
        if ( action.atype == VA_ActionTypes::ADD_VALUE_TO_SET ) {
            vasets.AddItem ( action.val1, data, atoi ( action.val2.c_str() ), true, true );
        } else {
            vasets.DelItem ( action.val1, data, atoi ( action.val2.c_str() ), true, true );
        }
    }
    break;
    case VA_ItemTypes::AR_TIME:
        cout << __func__ << "IN " << __func__ << ": " << "VA_ItemTypes::AR_TIME" << endl;
        break;
    default:
        cout << __func__ << "IN " << __func__ << ": " << "default" << endl;
        break;
    }
}

void VAengine::AddDelValueToSet ( KAMaction &action, unordered_map<string, string>&varwith, VAengine& vaeng, T_EventoStruct* bufs_in_out, string agent_id, string session_id, T_um_ss & temporary_kv_assoc)
{
    /*ESEMPIO:
     *             case KAM_ActionTypes::TCP_REDIRECT_VHOST:
     *            {
     *                //se non trova torna init del tipo (stirnga vuota).
     *                auto val = ar.second.ar_vars.find ( arop.var_name );
     *                thetcpredirect = ( val->second.size() == 0 ) ? bufs_in_out->s_loc_host : val->second;
     *                theAction = KAM_ActionTypes::TCP_REDIRECT_HOST;
     *                client_cert = arop.client_cert;
     *                server_cert = arop.server_cert;
                    }
    */
    //l'unico observable ammesso qui è ident or string
    switch (action.observable)
    {
    case VA_ItemTypes::VARIABLE:
    {
        mtfa_log("KAM add/del item %s to set %s for %s seconds\n", action.val2.c_str(), action.val1.c_str(), action.val3.c_str());
        string newval2 = KamStringEval(GetFromVarWith(varwith, action.val2), bufs_in_out, temporary_kv_assoc);
        newval2 = remove_pattern(action.replace_pattern, newval2, action.replace);
        if ( action.action == KAM_ActionTypes::ADD_VALUE_TO_SET ) {
            vasets.AddItem ( action.val1, newval2, atoi ( action.val3.c_str() ), true, false);
        } else {
            vasets.DelItem ( action.val1, newval2, atoi ( action.val3.c_str() ), true, false );
        }
    }
    break;
    //Ora il cat
    case VA_ItemTypes::CAT:
    {
        //Per prima cosa devo estrarre tutti i valori dalle osservazioni correnti
        string newval2;
        for (auto & ti: action.cat)
        {
            auto st = GetValueFromRunningData ( agent_id, session_id, bufs_in_out, get<0>(ti), get<1>(ti));
            newval2 += (remove_pattern(get<2>(ti), st, get<3>(ti)) );
        }
        newval2 = KamStringEval(newval2, bufs_in_out, temporary_kv_assoc);
        newval2 = remove_pattern(action.replace_pattern, newval2, action.replace);
        if ( action.action == KAM_ActionTypes::ADD_VALUE_TO_SET ) {
            vasets.AddItem ( action.val1,
                             newval2,
                             atoi ( action.val3.c_str() ),
                             true, false );
        } else {
            vasets.DelItem ( action.val1, newval2, atoi ( action.val3.c_str() ), true, false );
        }
    }
    break;
    default:
        mtfa_log("Only VARIABLES are admitted. Call FA\n");
    }
}


//TODO Va riesaminata!!
void VAengine::SetVarOfAccessRule ( VAaction action, unordered_map<string, string>&varwith, VAobservations *obs )
{
    switch ( action.observable ) {
    case VA_ItemTypes::AGENT_ID:
    case VA_ItemTypes::SA_OS:
    case VA_ItemTypes::SA_LOGGED_USER:
    case VA_ItemTypes::SA_PROCESS_NAME:
    case VA_ItemTypes::OBS_NAME:
    case VA_ItemTypes::OBS_TIME:
    case VA_ItemTypes::OBS_OBSID:
    case VA_ItemTypes::OBS_STIME:
    case VA_ItemTypes::OBS_ETIME:
    case VA_ItemTypes::OBS_TAG:
    case VA_ItemTypes::OBS_EVENT:
    case VA_ItemTypes::NET_RESULT:
    case VA_ItemTypes::NET_SESID:
    case VA_ItemTypes::HTTP_METHOD:
    case VA_ItemTypes::HTTP_HOST:
    case VA_ItemTypes::HTTP_HEADER_HOST:
    case VA_ItemTypes::HTTP_URL_HOST:
    case VA_ItemTypes::NET_TCP_IPDST:
    case VA_ItemTypes::NET_TCP_IPSRC:
    case VA_ItemTypes::HTTP_URL:
    case VA_ItemTypes::HTTP_URI: {
        auto found = obs->find ( mtfa_enum_to_int(action.observable) );
        if ( found != obs->end() ) {
            auto ar = acrules.find ( action.val1 );
            if ( ar != acrules.end() ) {
                ar->second.ar_vars[action.val2] = found->second.value;
            }
        }
    }
    break;

    case VA_ItemTypes::HTTP_COOKIES:
    case VA_ItemTypes::HTTP_HEADERS:
    case VA_ItemTypes::HTTP_QUERY:
    case VA_ItemTypes::HTTP_DATA_POST:
    case VA_ItemTypes::HTTP_ANSWER_CODE:
    case VA_ItemTypes::HTTP_ANSWER_HEADERS:
    case VA_ItemTypes::HTTP_ANSWER_COOKIES:
    case VA_ItemTypes::HTTP_ANSWER_DATA: {
        auto found = obs->find ( mtfa_enum_to_int(action.observable) );
        if ( found != obs->end() ) {
            auto ar = acrules.find ( action.val1 );
            if ( ar != acrules.end() ) {
                ar->second.ar_vars[action.val2] = found->second.values[action.obs_index];
            } else {
                cout << __func__ << " - " << StrTime() << ": " << "AR named " << action.val1 << " not found" << endl;
            }
        }
    }
    break;

    case VA_ItemTypes::VARIABLE: {
        cout << __func__ << __func__ << ": the value must be taken from the with variables\n";
        auto ar = acrules.find ( action.val1 );
        if ( ar != acrules.end() ) {
            ar->second.ar_vars[action.val2] = action.val3;
        }
    }
    break;
    case VA_ItemTypes::AR_TIME:
        cout << __func__ << "IN " << __func__ << ": " << "VA_ItemTypes::AR_TIME" << endl;
        break;
    default:
        cout << __func__ << "IN " << __func__ << ": " << "default" << endl;
        break;
    }
}

void VAengine::ExecuteActions ( VA_NodeTree * node, unordered_map<string, string>&varwith, VAobservations *obs, T_um_ss & temporary_kv_assoc )
{
    for ( VAaction & action : node->actions ) {
        switch ( action.atype ) {
        case VA_ActionTypes::CALL_LISP_FUNCTION:
        {
            scm_init_guile();
            //Per prima cosa devo estrarre tutti i valori dalle osservazioni correnti
            list<string> lvalues;
            for (auto & ti: action.cat)
                lvalues.push_back(remove_pattern(get<2>(ti), GetValueFromObservation ( obs, get<0>(ti), varwith, get<1>(ti)), get<3>(ti)));
            
            //Ora posso chiamare la funzione!!! nota che potrei aver voluto associare, e l'ho fatto, a tempo di compilazione il nome della funzione al suo puntatore all'interno del LISP
            //costruisco la lista delle variabili (necessariamente in formato lista)
            SCM lista = SCM_EOL;
            for (auto &s: lvalues)
            {
                char * p = (char*)malloc(s.size());
                std::copy(s.begin(), s.end(), p);
                SCM ptr = scm_pointer_to_bytevector (
                    scm_from_pointer ( ( void* ) p, free ),
                    scm_from_int ( s.size() ),
                    scm_from_int ( 0 ),
                    scm_string_to_symbol ( U8 ) );
                lista = scm_cons(ptr, lista);
            }
            lista = scm_reverse(lista);
            //Ora posso chiamare la funzione, passandogli i parametri e verificare il risultato. Se falso, allora torna false
            scm_call_1(action.func, lista);
        }
        break;
            
        case VA_ActionTypes::ISSUE_VREP:
            IssueValidationReport ( action.val1, action.vrep, varwith, obs );
            break;
//           case VA_ActionTypes::ISSUE_MVREP:
//                IssueMemoryValidationReport ( action.val1, action.vrep, varwith, obs );
//                break;
        case VA_ActionTypes::ADD_VALUE_TO_SET:
        case VA_ActionTypes::DEL_VALUE_FROM_SET:
            AddDelValueToSet ( action, varwith, obs, temporary_kv_assoc );
            break;
        case VA_ActionTypes::DISABLE_AR:
            acrules[action.val1].enabled = false;
            break;
        case VA_ActionTypes::ENABLE_AR:
            acrules[action.val1].enabled = true;
            break;
        case VA_ActionTypes::DISABLE_VR:
            varules[action.val1].enabled = false;
            break;
        case VA_ActionTypes::ENABLE_VR:
            varules[action.val1].enabled = true;
            break;
        case VA_ActionTypes::SET_VAR_OF_AR:
            SetVarOfAccessRule ( action, varwith, obs );
            break;
        case VA_ActionTypes::MYSQL_APPEND: {
            InsertIntoTable ( action.val1, action.vrep, varwith, obs );
        }
        break;
        default:
            cout << __func__ << "Action not executed: " << VA_ActionTypesToString ( action.atype );
            break;
        }
    }
}

void VAengine::AssignWiths ( VA_NodeTree * node, unordered_map<string, string>&varwith, VAobservations *obs )
{
    //assegna i valori delle variabili WITH
    for ( auto with : node->with ) {
        switch ( with.itype ) {
        case VA_ItemTypes::AGENT_ID:
        case VA_ItemTypes::SA_OS:
        case VA_ItemTypes::SA_LOGGED_USER:
        case VA_ItemTypes::SA_PROCESS_NAME:
        case VA_ItemTypes::OBS_NAME:
        case VA_ItemTypes::OBS_TIME:
        case VA_ItemTypes::OBS_OBSID:
        case VA_ItemTypes::OBS_STIME:
        case VA_ItemTypes::OBS_ETIME:
        case VA_ItemTypes::OBS_TAG:
        case VA_ItemTypes::OBS_EVENT:
        case VA_ItemTypes::NET_RESULT:
        case VA_ItemTypes::NET_SESID:
        case VA_ItemTypes::HTTP_METHOD:
        case VA_ItemTypes::HTTP_HOST:
        case VA_ItemTypes::HTTP_HEADER_HOST:
        case VA_ItemTypes::HTTP_URL_HOST:
        case VA_ItemTypes::NET_TCP_IPDST:
        case VA_ItemTypes::NET_TCP_IPSRC:
        case VA_ItemTypes::HTTP_URL:
        case VA_ItemTypes::HTTP_URI: {
            auto fromobs = obs->find ( mtfa_enum_to_int(with.itype) );
            if ( fromobs != obs->end() ) {
                varwith[with.vname] = remove_pattern ( with.pattern, fromobs->second.value, with.replace );
            } else {
                if ( varwith.find ( with.vname ) == varwith.end() ) {
                    varwith[with.vname] = "%EMPTY";
                }
            }
        }
        break;
        case VA_ItemTypes::HTTP_COOKIES:
        case VA_ItemTypes::HTTP_HEADERS:
        case VA_ItemTypes::HTTP_QUERY:
        case VA_ItemTypes::HTTP_DATA_POST:
        case VA_ItemTypes::HTTP_ANSWER_CODE:
        case VA_ItemTypes::HTTP_ANSWER_HEADERS:
        case VA_ItemTypes::HTTP_ANSWER_COOKIES:
        case VA_ItemTypes::HTTP_ANSWER_DATA: {
            auto fromobs = obs->find ( mtfa_enum_to_int(with.itype) );
            if ( fromobs != obs->end() ) {
                varwith[with.vname] = remove_pattern ( with.pattern, fromobs->second.values[with.op1], with.replace );
            } else {
                if ( varwith.find ( with.vname ) == varwith.end() ) {
                    varwith[with.vname] = "%EMPTY";
                }
            }
        }
        break;
        case VA_ItemTypes::CAT:
        {
            //Per prima cosa devo estrarre tutti i valori dalle osservazioni correnti
            string value;
            for (auto & ti: with.cats)
            {
                value += remove_pattern(get<2>(ti), GetValueFromObservation ( obs, get<0>(ti), varwith, get<1>(ti) ), get<3>(ti)); //  + szero;
            }
            //value.pop_back();     //Tolgo ultimo elemento
            varwith[with.vname] = value;
        }
        break;

        case VA_ItemTypes::VARIABLE:
        {
            auto fromobs = obs->find ( mtfa_enum_to_int(with.itype) );
            if ( fromobs != obs->end() ) {
                varwith[with.vname] = remove_pattern ( with.pattern, fromobs->second.value, with.replace );
            } else {
                varwith[with.vname] = remove_pattern ( with.pattern, with.op1, with.replace );
            }
        }
        break;

        case VA_ItemTypes::AR_TIME:
        case VA_ItemTypes::NONE:
            cout << __func__ << "Happens also here\n";
            break;
        }
    }
}

//usata localmente per eliminare i dati che soddisfano il mtfa_compile_pattern
string remove_pattern(pcre2_code * pattern, const string & s, const string &r)
{
    if (pattern)
    {
        auto ret = mtfa_replace_matching_pattern(s, pattern, r, true);
        //cerr << "Remove pattern con pattern. Entra; " << s << ". Rimpiazza con: " << r << ". Torna: " << ret << endl;
        return ret;
    }
    else {
        //cerr << "Remove pattern senza pattern. Torna: " << s << endl;
        return s;
    }
}

//verifica tutti i criteri associati a un nodo con l'osservazione corrente!
bool VAengine::CheckCriteria ( VA_NodeTree * node, T_um_ss & varwith, VAobservations *obs )
{
    for ( auto cr : node->criteria ) {
        switch ( cr.itype ) {

        case VA_ItemTypes::CALL_LISP_FUNCTION:
        {
            scm_init_guile();
            //Per prima cosa devo estrarre tutti i valori dalle osservazioni correnti
            list<string> lvalues;
            for (auto & ti: cr.cats)
                lvalues.push_back(remove_pattern(get<2>(ti), GetValueFromObservation ( obs, get<0>(ti), varwith, get<1>(ti)), get<3>(ti)));
            
            //Ora posso chiamare la funzione!!! nota che potrei aver voluto associare, e l'ho fatto, a tempo di compilazione il nome della funzione al suo puntatore all'interno del LISP
            //costruisco la lista delle variabili (necessariamente in formato lista)
            SCM lista = SCM_EOL;
            for (auto &s: lvalues)
            {
                char * p = (char*)malloc(s.size());
                std::copy(s.begin(), s.end(), p);
                SCM ptr = scm_pointer_to_bytevector (
                    scm_from_pointer ( ( void* ) p, free ),
                    scm_from_int ( s.size() ),
                    scm_from_int ( 0 ),
                    scm_string_to_symbol ( U8 ) );
                lista = scm_cons(ptr, lista);
            }
            lista = scm_reverse(lista);
            //Ora posso chiamare la funzione, passandogli i parametri e verificare il risultato. Se falso, allora torna false
            SCM scm_ret = scm_call_1(cr.func, lista);
            if (scm_ret == SCM_BOOL_F)
                return false;
        }
        break;
            
        case VA_ItemTypes::CAT:
        {
            //Per prima cosa devo estrarre tutti i valori dalle osservazioni correnti
            string value;
            for (auto & ti: cr.cats)
            {
                value += (remove_pattern(get<2>(ti), GetValueFromObservation ( obs, get<0>(ti), varwith, get<1>(ti)), get<3>(ti))); // + szero);
            }
            //value.pop_back(); //Tolgo ultimo elemento

            //Ora questo valore devo trovarlo nell'insieme
            if ( cr.op == VA_OpTypes::IN )
            {
                if ( cr.op1.size() == 0 )
                {
                    if ( cr.donot )
                        break;
                    else
                        return false;
                }

                auto theset = vasets.datasets.find ( cr.op1 );

                if ( !theset->second )
                {
                    if ( cr.donot )
                        break;
                    else
                        return false;
                }
                DO_VASET_LOCK(theset->second->rmtx);
                if ( !theset->second->uss )
                {
                    if ( cr.donot )
                        break;
                    else
                        return false;
                }

                if ( theset->second->uss->size() == 0 )
                {
                    if ( cr.donot )
                        break;
                    else
                        return false;
                }

//                     unsigned int ident;
//                     vector<unsigned short> to_be_searched;
//                     to_be_searched = vasets.UrlStringToVS ( value , szero );
                auto found = theset->second->uss->find(value);

//                     for (auto it: *theset->second->uss)
//                     {
//                         cout << it.size() << ": " << "<" << it << ">" << endl;
//                     }

                bool isin = (found != theset->second->uss->end());

                if ( !isin )
                {
                    if ( cr.donot )
                        break;
                    else
                        return false;
                }
            }

            if ( cr.donot )
                return false;
        }
        break;
        case VA_ItemTypes::AGENT_ID:
        case VA_ItemTypes::SA_OS:
        case VA_ItemTypes::SA_LOGGED_USER:
        case VA_ItemTypes::SA_PROCESS_NAME:
        case VA_ItemTypes::OBS_NAME:
        case VA_ItemTypes::OBS_TIME:
        case VA_ItemTypes::OBS_OBSID:
        case VA_ItemTypes::OBS_STIME:
        case VA_ItemTypes::OBS_ETIME:
        case VA_ItemTypes::OBS_TAG:
        case VA_ItemTypes::OBS_EVENT:
        case VA_ItemTypes::NET_RESULT:
        case VA_ItemTypes::NET_SESID:
        case VA_ItemTypes::HTTP_METHOD:
        case VA_ItemTypes::HTTP_ANSWER_CODE:
        case VA_ItemTypes::HTTP_HOST:
        case VA_ItemTypes::HTTP_HEADER_HOST:
        case VA_ItemTypes::HTTP_URL_HOST: 
        case VA_ItemTypes::HTTP_URI: {
            if ( cr.op == VA_OpTypes::IN ) {
                auto theset = vasets.datasets.find ( cr.op1 );
                if ( theset == vasets.datasets.end() ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                if ( !theset->second ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                DO_VASET_LOCK(theset->second->rmtx);
                if ( theset->second->uss->size() == 0 ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                auto fromobs = obs->find ( mtfa_enum_to_int(cr.itype) );
                if ( fromobs == obs->end() ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                auto isin = theset->second->uss->find ( remove_pattern(cr.pattern, fromobs->second.value, cr.replace));
                if ( isin == theset->second->uss->end()) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
            } else if ( cr.op == VA_OpTypes::EQUAL ) {
                if ( cr.op1.size() == 0 ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                auto fromobs = obs->find ( mtfa_enum_to_int(cr.itype) );
                if ( fromobs == obs->end() ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                string value = cr.op1;
                auto fromwith = varwith.find ( cr.op1 );
                if ( fromwith != varwith.end() ) {
                    value = fromwith->second;
                }
                if ( remove_pattern(cr.pattern, fromobs->second.value, cr.replace).compare(value)!=0) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
            } else {
                //VA_OpTypes::EXISTS
                auto fromobs = obs->find ( mtfa_enum_to_int(cr.itype) );
                if ( fromobs == obs->end() ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
            }
            if (cr.donot)
                return false;
        }
        break;
        case VA_ItemTypes::HTTP_COOKIES:
        case VA_ItemTypes::HTTP_HEADERS:
        case VA_ItemTypes::HTTP_QUERY:
        case VA_ItemTypes::HTTP_DATA_POST:
        case VA_ItemTypes::HTTP_ANSWER_HEADERS:
        case VA_ItemTypes::HTTP_ANSWER_COOKIES:
        case VA_ItemTypes::HTTP_ANSWER_DATA: {
            if ( cr.op == VA_OpTypes::IN ) {
                auto theset = vasets.datasets.find ( cr.op2 );
                if ( theset == vasets.datasets.end() ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                if ( !theset->second ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                DO_VASET_LOCK(theset->second->rmtx);
                if ( theset->second->uss->size() == 0 ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                auto fromobs = obs->find ( mtfa_enum_to_int(cr.itype) );
                if ( fromobs == obs->end() ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                auto isobserved = fromobs->second.values.find ( cr.op1 );
                if ( isobserved == fromobs->second.values.end() ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                auto isin = theset->second->uss->find ( remove_pattern(cr.pattern, isobserved->second, cr.replace) );
                if ( isin == theset->second->uss->end()) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
            } else if ( cr.op == VA_OpTypes::EQUAL ) {
                if ( cr.op1.size() == 0 ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                auto fromobs = obs->find ( mtfa_enum_to_int(cr.itype) );
                if ( fromobs == obs->end() ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                auto isobserved = fromobs->second.values.find ( cr.op1 );
                if ( isobserved == fromobs->second.values.end() ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                string value = cr.op2;
                auto fromwith = varwith.find ( cr.op2 );
                if ( fromwith != varwith.end() ) {
                    value = fromwith->second;
                }
                if ( remove_pattern(cr.pattern, isobserved->second, cr.replace).compare(value)!=0 ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
            } else {
                //VA_OpTypes::EXISTS
                if ( cr.op1.size() == 0 ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                auto fromobs = obs->find ( mtfa_enum_to_int(cr.itype) );
                if ( fromobs == obs->end() ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                auto isobserved = fromobs->second.values.find ( cr.op1 );
                if ( isobserved == fromobs->second.values.end() ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
            }
            if (cr.donot)
                return false;
        }
        break;

        case VA_ItemTypes::NET_TCP_IPDST:
        case VA_ItemTypes::NET_TCP_IPSRC:
        case VA_ItemTypes::HTTP_URL: {
            if ( cr.op == VA_OpTypes::IN ) {
                if ( cr.op1.size() == 0 ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                auto theset = vasets.datasets.find ( cr.op1 );
                if ( !theset->second ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                DO_VASET_LOCK(theset->second->rmtx);
                if ( !theset->second->mss ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                if ( theset->second->mss->db.size() == 0 ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                auto fromobs = obs->find ( mtfa_enum_to_int(cr.itype) );
                if ( fromobs == obs->end() ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                unsigned int ident;
                vector<unsigned short> to_be_searched;
                if ( cr.itype == VA_ItemTypes::HTTP_URL ) {
                    //cout << "Cerca: " << fromobs->second.value << endl;
                    to_be_searched = vasets.UrlStringToVS ( remove_pattern(cr.pattern, fromobs->second.value, cr.replace) );
                } else {
                    to_be_searched = vasets.IpPortStringToVS ( remove_pattern(cr.pattern, fromobs->second.value, cr.replace) );
                }

                bool isin = theset->second->mss->Search ( to_be_searched, ident );
                if ( !isin ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
            } else if ( cr.op == VA_OpTypes::EQUAL ) {
                if ( cr.op1.size() == 0 ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                auto fromobs = obs->find ( mtfa_enum_to_int(cr.itype) );
                if ( fromobs == obs->end() ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
                string value = cr.op1;
                auto fromwith = varwith.find ( cr.op1 );
                if ( fromwith != varwith.end() ) {
                    value = fromwith->second;
                }
                if ( remove_pattern(cr.pattern, fromobs->second.value, cr.replace).compare(value ) != 0) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
            } else {
                //VA_OpTypes::EXISTS
                auto fromobs = obs->find ( mtfa_enum_to_int(cr.itype) );
                if ( fromobs == obs->end() ) {
                    if (cr.donot)
                        break;
                    else
                        return false;
                }
            }
            if (cr.donot)
                return false;
        }
        break;

        case VA_ItemTypes::VARIABLE:
        case VA_ItemTypes::AR_TIME:
        case VA_ItemTypes::NONE:
            cout << __func__ << "Happens also here\n";
            break;
        }
    }
    return true;
}

void VAengine::InitRun()
{
    //Inizializza le strutture di running
    runtime_varules.Clear();
    for ( auto it : varules ) {
        VAengineExecutionEnvironment runtime = VAengineExecutionEnvironment();
        runtime.root = CloneForRunTime ( it.second.root );
        runtime.current_node = runtime.root;
        runtime_varules.WriteLock();
        runtime_varules.ds[it.first].push_back ( runtime );
        runtime_varules.WriteUnlock();
    }
}

void VAengine::PurgeTracedValues(mtfa_unordered_map<string, pair<string, time_t> > * ds)
{
    while (true)
    {
        if (ds)
        {
            auto now = time(nullptr);
            ds->DeleteIf([now] (const string & k, const pair<string, time_t> & v, void * p) -> bool
            {
                return (now - v.second) > 10;
            }, nullptr);
        }
        sleep(30);   //ogni secondo fai questo controllo
    }
}


static bool VA_NodeTreeRunTimeComplete ( VA_NodeTreeRunTime *r )
{
    if ( !r ) {
        return true;
    }
    for ( auto it : r->children_done )
        if ( !it ) {
            return false;
        }
    return true;
}

static list<VA_NodeTreeRunTime*> MakeListOfEquivalentNodes ( VA_NodeTreeRunTime * cur )
{
    list<VA_NodeTreeRunTime*> ln;
    if ( !cur ) {
        return ln;
    }

    switch ( cur->original->op ) {
    case VA_LogicalTypes::ITEM: {
        ln.push_back ( cur );
    }
    break;
    case VA_LogicalTypes::AND:
    case VA_LogicalTypes::OR: {
        //Trovo tutti i nodi figli che sono equivalenti
        for ( int i = 0; i < cur->children.size(); ++i ) {
            if ( !cur->children_done[i] ) {
                auto lnret = MakeListOfEquivalentNodes ( cur->children[i] );
                ln.splice ( ln.end(), lnret );
            }
        }
    }
    break;
    case VA_LogicalTypes::BEFORE: {
        //devo cercare il primo figlio (diretto) che non è stato ancora completato
        for ( int i = 0; i < cur->children.size(); ++i ) {
            if ( !cur->children_done[i] ) {
                auto lnret = MakeListOfEquivalentNodes ( cur->children[i] );
                ln.splice ( ln.end(), lnret );
                break; //esco dal ciclo for
            }
        }
    }
    break;
    }
    return ln;
}

static VA_NodeTreeRunTime* GoBackUp ( VA_NodeTreeRunTime*r )
{
    while ( true ) {
        //Questo nodo è terminato, torno su fino al primo padre che non è terminato e metto il punto di esecuzione su di lui.
        int myposonfather = r->position_on_father;
        r = r->father;
        if ( r == nullptr ) {
            //sono arrivato alla radice dell'albero, ho terminato l'esecuzione della VR
            return r; //eseguito!
        }

        //Vado al primo nodo che non è completamente eseguito! ma prima marco il figlio come risolto!
        r->children_done[myposonfather] = true;

        //Questo nuovo nodo è completo? se sì vado verso l'alto, altrimenti mi fermo qui per ora.
        if ( !VA_NodeTreeRunTimeComplete ( r ) ) {
            return r;
        }

        //il nodo è completo, vado al padre.
        //nel senso che proseguo con il while
    }
}

//Torna true quando il nodo è stato eseguito, false se non ha matchato

bool VAengine::ExecuteCurrentInstance ( VAengineExecutionEnvironment & execenv, VAobservations *obs )
{
    T_um_ss temporary_kv_assoc;  //per operazioni LISP on line???

    auto current_original = execenv.current_node->original;
    auto current_node = execenv.current_node;
    switch ( current_original->op ) {
    //se è un nodo ITEM, criteri ecc e poi torno indietro.
    case VA_LogicalTypes::ITEM: {
        //Criteri e altro! Se sono qui lo devo eseguire!
        bool matched = CheckCriteria ( current_original, execenv.varwith, obs );
        if ( !matched ) {
            return false;     //Nodo non eseguito. I criteri non sono soddisfatti. Vado alla prossima istanza di VR.
        }

        //Go with actions e poi se non siamo in un nodo terminale, andiamo avanti
        execenv.last_matching_time = time ( nullptr );
        AssignWiths ( current_original, execenv.varwith, obs );
        ExecuteActions ( current_original, execenv.varwith, obs, temporary_kv_assoc );

        execenv.current_node = GoBackUp ( current_node );
        return true;
    }
    break;
    //se è un nodo AND, figli, figli figli...
    case VA_LogicalTypes::OR:
    case VA_LogicalTypes::AND:
    {
        list<VA_NodeTreeRunTime*> le = MakeListOfEquivalentNodes ( current_node );
        //se le è vuoto, allora il nodo è stato completato ma siamo in errore
        if ( le.size() == 0 ) {
            cout << __func__ << __func__ << " case VA_LogicalTypes::AND completed but not expected\n";
            execenv.current_node = GoBackUp ( current_node );
            return true;
        } else {
            //Prendo il primo che matcha. Ma se fossero in OR ok, in AND, no.
            for ( auto it : le ) {
                //Criteri e altro! Se sono qui lo devo eseguire!
                bool matched = CheckCriteria ( it->original, execenv.varwith, obs );
                if ( !matched ) {
                    continue;
                }

                //Go with actions e poi se non siamo in un nodo terminale, andiamo avanti
                execenv.last_matching_time = time ( nullptr );
                AssignWiths ( it->original, execenv.varwith, obs );
                ExecuteActions ( it->original, execenv.varwith, obs, temporary_kv_assoc  );
                //se è anche l'unico, oppure se l'operazione era un OR, allora torno su, altrimenti no.
                if ( le.size() == 1 ) {
                    //qui devo anche specificare che tutti i figli sono stati eseguiti, anche se è solo uno, tato per non sbagliare!
                    for (auto ch = current_node->children_done.begin(); ch != current_node->children_done.end(); ++ch)
                        *ch = true;
                    execenv.current_node = GoBackUp ( it );
                    return true;
                } else {
                    if ( current_node->original->op == VA_LogicalTypes::OR) {
                        //qui devo anche specificare che tutti i figli sono stati eseguiti, poiché siamo in presenza di un or
                        for (auto ch = current_node->children_done.begin(); ch != current_node->children_done.end(); ++ch)
                            *ch = true;
                        execenv.current_node = GoBackUp ( it );
                        return true;
                    } else {
                        if ( current_node->original->op == VA_LogicalTypes::AND) {
                            //qui non devo specificare che tutti i figli sono stati eseguiti, + un AND, li voglio tutti!
                            execenv.current_node = GoBackUp ( it );
                            return true;
                        } else {
                            execenv.current_node = it;
                            return true;
                        }
                    }
                }
            }
            return false;
        }
/*



        //allora, se sono qui è poichè questo nodo non è ancora stato completato.
        //Faccio la lista dei nodi che sono in AND tra loro
        // cout << __func__ << ": AndOr node. MakeListOfEquivalentNodes" << endl;

        list<VA_NodeTreeRunTime*> le = MakeListOfEquivalentNodes ( current_node );

//         for ( auto it : le )
//         {
//             cout << "position_on_father: " << it->position_on_father << ". Children done (size): " << it->children_done.size() << endl;
//         }

        //se le è vuoto, allora il nodo è stato completato ma siamo in errore
        if ( le.size() == 0 )
        {
            cout << __func__ << __func__ << " case VA_LogicalTypes::AND/OR completed but not expected\n";
            execenv.current_node = GoBackUp ( current_node );
            return true;
        }
        else
        {
            //prendo il primo che matcha
            auto it = le.begin();

            while ( it != le.end() )
            {
                //Criteri e altro! Se sono qui lo devo eseguire!
                bool matched = CheckCriteria ( ( *it )->original, execenv.varwith, obs );

                if ( !matched )
                {
                    it++;
                    continue;
                }

                //Go with actions e poi se non siamo in un nodo terminale, andiamo avanti
                execenv.last_matching_time = time ( nullptr );
                AssignWiths ( ( *it )->original, execenv.varwith, obs );
                ExecuteActions ( ( *it )->original, execenv.varwith, obs, temporary_kv_assoc  );

#if 1   //TODO: completare gestione dellOR
                if ( (*it)->father->original->op == VA_LogicalTypes::OR )
                {
                    //devo considerare i suoi fratelli come eseguiti e devo toglierli da le
                    for (int i=0; i<(*it)->father->children.size(); ++i)
                    {
                        if ((*it)->position_on_father != i)
                        {
                            (*it)->father->children_done[i]=true;
                            le.remove((*it)->father->children[i]);
                        }
                    }
                }
#endif

                //se è anche l'unico, allora torno su, altrimenti no.
                if ( le.size() == 1 )
                {
                    execenv.current_node = GoBackUp ( ( *it ) );
                    return true;
                }
                else
                {
                    execenv.current_node = ( *it );
                    return true;
                }
            }

            //se non match, esce
            return false;
        }
*/
    }
    break;

    case VA_LogicalTypes::BEFORE: {
        //allora, se sono qui è poichè questo nodo non è ancora stato completato.
        //cerco il primo non completo
        int posprimo = -1;
        for ( auto it : current_node->children_done ) {
            ++posprimo;
            if ( !it ) {
                break;
            }
        }

        //Il mio nodo corrente diventa posprimo e su questo lavoro
        if ( posprimo == -1 ) {
            cout << __func__ << __func__ << " case VA_LogicalTypes::BEFORE completed but not expected\n";
            execenv.current_node = GoBackUp ( current_node );
            return true;
        }

        //Faccio la lista dei nodi che sono equivalenti tra loro
        current_node = current_node->children[posprimo];
        list<VA_NodeTreeRunTime*> le = MakeListOfEquivalentNodes ( current_node );
        //se le è vuoto, allora il nodo è stato completato ma siamo in errore
        if ( le.size() == 0 ) {
            cout << __func__ << __func__ << " case VA_LogicalTypes::AND completed but not expected\n";
            execenv.current_node = GoBackUp ( current_node );
            return true;
        } else {
            //Prendo il primo che matcha. Ma se fossero in OR ok, in AND, no.
            for ( auto it : le ) {
                //Criteri e altro! Se sono qui lo devo eseguire!
                bool matched = CheckCriteria ( it->original, execenv.varwith, obs );
                if ( !matched ) {
                    continue;
                }

                //Go with actions e poi se non siamo in un nodo terminale, andiamo avanti
                execenv.last_matching_time = time ( nullptr );
                AssignWiths ( it->original, execenv.varwith, obs );
                ExecuteActions ( it->original, execenv.varwith, obs, temporary_kv_assoc  );
                //se è anche l'unico, oppure se l'operazione era un OR, allora torno su, altrimenti no.
                if ( le.size() == 1 || current_node->original->op == VA_LogicalTypes::OR) {
                    //qui devo anche specificare che tutti i figli sono stati eseguiti
                    for (auto ch = current_node->children_done.begin(); ch != current_node->children_done.end(); ++ch)
                        *ch = true;
                    execenv.current_node = GoBackUp ( it );
                    return true;
                } else {
                    execenv.current_node = it;
                    return true;
                }
            }
            return false;
        }
    }
    break;
    }
    return false;
}

bool VAengine::Runner ( VAobservations *obs )
{
    if ( !obs || obs->size() == 0 ) {
        return false;
    }
    //Per ogni VR in attesa, verifico se questa match.
    //Il match è effettuato sulle va running e sulle va statiche.
//                     for (auto it: *theset->second->uss)
//                     {
//                         cout << it.size() << ": " << "<" << it << ">" << endl;
//                     }

    //Stampo l'osservazione
//      {
//          for (auto &it: *obs)
//          {
//              cout << "OBS: " << it.first << "\t" <<  VA_ItemTypesToString(it.second.itype) << "\t" << it.second.value << endl;
//          }
//      }
    bool ok = false;
    runtime_varules.ReadLock();
//     cout << "\n\n\nList of active validation rules" << endl; //se tra quelle che devo eseguire non ho l'originale e nessuna si conclude in modo da ripristinare l'originale, allora devo eseguire anche l'originale.
//     for ( auto & it : runtime_varules.ds )
//     {
//         cout << it.first << endl;
//         for (auto & jt: it.second)
//         {
//             cout << "\t" << jt.current_node << ", " << jt.last_matching_time << ", " << jt.root << ", " << jt.runned << ", " << jt.starting_time << endl;
//             for (auto & kt: jt.varwith)
//             {
//                 cout << "\t\t" << kt.first << ": " << kt.second << endl;
//             }
//         }
//     }
    //ci sono root per tutte?
    /*
    for ( auto & it : runtime_varules.ds )
    {
        for (auto & runned: it.second)
        {
            bool done = false;
            for (auto d: runned.current_node->children_done)
            {
                if (d)
                {
                    done = true;
                    break;
                }
            }
            cout << it.first << ". In execution: " <<  (done?"true":"false") << endl;
        }
    }
    */
    
    
    for ( auto & it : runtime_varules.ds )  //it è il nome e l'elenco di quelle che sono in esecuzione
    {
        //le macchine da ripristinare
        set<VA_NodeTree*> toberestarted;

        //ma di ogni vr che è in esecuzione, ce n'è almeno una che non è runned?
        int conta = 0;
        for (auto & i:it.second) {
            if (i.runned)
                conta++;
        }
        if (conta == it.second.size()) {
            //cout << __func__ << "Restarting vr: " << it.first << endl;
            VAengineExecutionEnvironment runtime;
            auto found = varules.find(it.first);
            runtime.root = CloneForRunTime ( found->second.root );
            runtime.current_node = runtime.root;
            it.second.push_back ( runtime );
        }
    }
    
    
    //La stessa validation rule potrebbe essere in differenti stadi di validazione
    //e potrebbe accadere che per uno di questi possano coincidere e quindi
    //un'osservazioen fa chiudere n istanze della stessa VR
    //questo non deve accadere e quindi non appena un'istanza matcha, esce e va alla prossima!!!
    for ( auto & it : runtime_varules.ds )  //it è il nome e l'elenco di quelle che sono in esecuzione
    {
        //I criteri matchano?
        ValidationRule vr = varules[it.first]; //questa punta alla vr reale (enabled, ...)
        if ( !vr.enabled ) {
            continue;     //Vai alla prossima, questa non matcha poiché è disabilitata
        }

        auto samevr = it.second.begin();
        while ( samevr != it.second.end() ) {
            bool done = ExecuteCurrentInstance ( *samevr, obs );
            if ( done ) {
                samevr->runned=true;
                if ( samevr->current_node == nullptr ) {
                    cout << __func__ << "Execution of istance of Validation Rule: " << vr.name << " IS TERMINATED CORRECTLY." << endl;
                    DeleteRunTimeNodeTRee ( samevr->root );
                    samevr = it.second.erase ( samevr );
                    ok = true;
                } else {
                    ++samevr;
                }
                break;
            } else {
                ++samevr;
            }
        }
    }
//      for ( auto & it : runtime_varules.ds )
//      {
//          cout << it.first << endl;
//          for (auto & jt: it.second)
//          {
//              cout << "\t" << jt.current_node << ", " << jt.last_matching_time << ", " << jt.root << ", " << jt.runned << ", " << jt.starting_time << endl;
//              for (auto & kt: jt.varwith)
//              {
//                  cout << "\t\t" << kt.first << ": " << kt.second << endl;
//              }
//          }
//      }
    runtime_varules.ReadUnlock();
    return ok;
}

//TODO: terminare serialize e deserialize per VAEngine e succ.
void VAengine::Serialize ( mtfa_binary_stream_save * bs )
{
    mtfa_write_lock writing(VAengine_lock);
    /*
     * Cosa abbiamo da salvare?
     *     //Archivio degli insiemi di dati utilizzati come verificatori a run time
        VASets vasets;

        unordered_map<string, T_MemoryVRData> issued_memory_validation_reports;

        unordered_map<string, ValidationRule> varules;

        //PHash acrules_ph_pages;
        unordered_map<string, AccessRule> acrules;
        list<string> acrules_ordering;

        //Ora, a run time, posso avere più validation rules con lo stesso nome ma in stati differenti.
        unordered_map<string, list<VAengineExecutionEnvironment>> runtime_varules;
     */

    vasets.Serialize ( bs );

    //issued_memory_validation_reports
//      bs->Append<int> ( issued_memory_validation_reports.size() );
//      for ( auto & it: issued_memory_validation_reports ) {
//           //unordered_map<string, T_MemoryVRData>
//           bs->Append<string> ( it.first );
//           //it.second->Serialize(bs);
//
//      }
}

void VAengine::Deserialize ( mtfa_binary_stream_load * bs )
{
    mtfa_write_lock writing(VAengine_lock);
}


VA_NodeTreeRunTime * CloneForRunTime ( VA_NodeTree*r )
{
    VA_NodeTreeRunTime * rt;
    if ( !r ) {
        return nullptr;
    }

    rt = new VA_NodeTreeRunTime();
    rt->children.resize ( r->children.size(), nullptr );
    rt->children_done.resize ( r->children.size(), false );
    rt->father = nullptr;
    rt->original = r;
    rt->position_on_father = 0;

    //se ci sono i figli, allora li crea
    for ( int i = 0; i < rt->children.size(); ++i ) {
        rt->children[i] = CloneForRunTime ( r->children[i] );
        rt->children[i]->father = rt;
        rt->children[i]->position_on_father = i;
    }
    return rt;
}

void DeleteRunTimeNodeTRee ( VA_NodeTreeRunTime *rt )
{
    if ( !rt ) {
        return;
    }
    for ( int i = 0; i < rt->children.size(); ++i ) {
        DeleteRunTimeNodeTRee ( rt->children[i] );
    }
    delete rt;
    return;
}

void VA_NodeTree::Print ( int lev, VA_NodeTree *r )
{
    if ( !r ) {
        return;
    }

    switch ( r->op ) {
    case VA_LogicalTypes::ITEM:
        cout << __func__ << tabs ( lev ) << "ITEM: " << r->criteria.size() << ". WITH: " << r->with.size() << ". ACTIONS: " << r->actions.size() << endl;
        return;
        break;
    case VA_LogicalTypes::AND: {
        cout << __func__ << tabs ( lev ) << "AND: " << endl;
        for ( auto it : r->children ) {
            Print ( lev + 1, it );
        }
    }
    break;
    case VA_LogicalTypes::OR: {
        cout << __func__ << tabs ( lev ) << "OR: " << endl;
        for ( auto it : r->children ) {
            Print ( lev + 1, it );
        }
    }
    break;
    case VA_LogicalTypes::BEFORE: {
        cout << __func__ << tabs ( lev ) << "BEFORE: " << endl;
        for ( auto it : r->children ) {
            Print ( lev + 1, it );
        }
    }
    break;
    }
}

VA_NodeTree* VA_NodeTree::SimplifyNodeTree ( VA_NodeTree*r )
{
    //Se r è op e se un figlio di r è come r, allora allungo il vettore
    if ( !r ) {
        return r;
    }
    switch ( r->op ) {
    case VA_LogicalTypes::ITEM:
        return r;
        break;
    case VA_LogicalTypes::AND:
    case VA_LogicalTypes::OR:
    case VA_LogicalTypes::BEFORE: {
        cout << "Trovato and/or/before" << endl;
        //Se il primo figlio è AND, allora prendo i figli del primo figlio e li metto qui
        int figlio = 0;
        while ( r->children.size() > figlio ) {
            r->children[figlio] = SimplifyNodeTree ( r->children[figlio] );
            if ( r->children[figlio]->op == r->op ) { // && r->op != VA_LogicalTypes::BEFORE)
                list<VA_NodeTree*> figli;
                for ( auto & it : r->children[figlio]->children ) {
                    figli.push_back ( it );
                    //it = nullptr;
                }
                r->children[figlio]->children.clear();
                auto vi = r->children.begin();
                std::advance ( vi, figlio );
                r->children.erase ( vi );
                r->children.insert ( vi, figli.begin(), figli.end() );
            } else {
                ++figlio;
            }
        }
    }
    break;
    }
    return r;
}

/*
    switch (t) {
        case VA_ItemTypes::AGENT_ID:
            break;
        case VA_ItemTypes::SA_OS:
            break;
        case VA_ItemTypes::SA_LOGGED_USER:
            break;
        case VA_ItemTypes::SA_PROCESS_NAME:
            break;
        case VA_ItemTypes::OBS_NAME:
            break;
        case VA_ItemTypes::OBS_TIME:
            break;
        case VA_ItemTypes::OBS_OBSID:
            break;
        case VA_ItemTypes::OBS_STIME:
            break;
        case VA_ItemTypes::OBS_ETIME:
            break;
        case VA_ItemTypes::OBS_TAG:
            break;
        case VA_ItemTypes::OBS_EVENT:
            break;
        case VA_ItemTypes::NET_TCP_IPDST:
            break;
        case VA_ItemTypes::NET_TCP_IPSRC:
            break;
        case VA_ItemTypes::NET_RESULT:
            break;
        case VA_ItemTypes::NET_SESID:
            break;
        case VA_ItemTypes::HTTP_METHOD:
            break;
        case VA_ItemTypes::HTTP_HOST:
            break;
        case VA_ItemTypes::HTTP_URL:
            break;
        case VA_ItemTypes::HTTP_HEADER_HOST:
            break;
        case VA_ItemTypes::HTTP_URL_HOST:
            break;
        case VA_ItemTypes::HTTP_COOKIES:
            break;
        case VA_ItemTypes::HTTP_HEADERS:
            break;
        case VA_ItemTypes::HTTP_QUERY:
            break;
        case VA_ItemTypes::HTTP_DATA_POST:
            break;
        case VA_ItemTypes::HTTP_ANSWER_CODE:
            break;
        case VA_ItemTypes::HTTP_ANSWER_HEADERS:
            break;
        case VA_ItemTypes::HTTP_ANSWER_COOKIES:
            break;
        case VA_ItemTypes::HTTP_ANSWER_DATA:
            break;
        case VA_ItemTypes::VARIABLE:
            break;
        case VA_ItemTypes::VR_KV:
            break;
        case VA_ItemTypes::AR_TIME:
            break;
    }
 */


PHash::PHash()
{
    counter = -1;
    mapper.clear();
}

PHash::~PHash()
{
    counter = -1;
    mapper.clear();
}

void PHash::clear()
{
    counter = 0;
    mapper.clear();
}

int PHash::insert ( const string & s )
{
    auto found = mapper.find ( s );
    if ( found == mapper.end() ) {
        mapper[s] = ++counter;
        return counter;
    } else {
        return found->second;
    }
}

int PHash::find ( const string &s )
{
    auto found = mapper.find ( s );
    if ( found == mapper.end() ) {
        return -1;
    } else {
        return found->second;
    }
}

int PHash::erase ( const string &s )
{
    return mapper.erase ( s );
}

int PHash::size()
{
    return counter + 1;
}

PHash & PHash::operator= ( const PHash & asn )
{
    //ma la = fa la move oppure no? NO, non la fa
    this->counter = asn.counter;
    this->mapper = asn.mapper;
    return *this;
}

void PHash::Serialize ( mtfa_binary_stream_save * bs )
{
    bs->Append<int> ( counter );
    bs->Append<int> ( ( int ) mapper.size() );
    for ( auto & it: mapper ) {
        bs->Append<string> ( it.first );
        bs->Append<int> ( it.second );
    }
}

void PHash::Deserialize ( mtfa_binary_stream_load * bs )
{
    counter = bs->Read<int>();
    int len = bs->Read<int>();
    mapper.clear();
    for ( int i=0; i<len; ++i ) {
        string s = bs->Read<string>();
        int l = bs->Read<int>();
        mapper[s] = l;
    }
}


IntSet::IntSet()
{
    data.clear();
}

IntSet::~IntSet()
{
    data.clear();
}

void IntSet::clear()
{
    data.clear();
}

int IntSet::insert ( const int s )
{
    data.insert ( s );
    return 0;
}

int IntSet::find ( const int s )
{
    if ( data.find ( s ) == data.end() ) {
        return false;
    } else {
        return true;
    }
}

int IntSet::erase ( const int s )
{
    return data.erase ( s );
}

int IntSet::size()
{
    return data.size();
}

IntSet & IntSet::operator= ( const IntSet & asn )
{
    this->data = asn.data;
    return *this;
}

void IntSet::Serialize ( mtfa_binary_stream_save * bs )
{
    bs->Append<int> ( ( int ) data.size() );
    for ( auto & it: data ) {
        bs->Append<int> ( it );
    }
}

void IntSet::Deserialize ( mtfa_binary_stream_load * bs )
{
    int len = bs->Read<int>();
    data.clear();
    for ( int i=0; i<len; ++i ) {
        int l = bs->Read<int>();
        data.insert ( l );
    }
}




VASet::VASet()
{
    mss = nullptr; //e allo stesso modo tutti gli elementi della union
    stype = VA_SetTypes::Undefined;
    sname.clear();
}

VASet::~VASet()
{
    switch ( stype ) {
    case VA_SetTypes::SetOfIpS:
    case VA_SetTypes::SetOfUrlS: {
        if ( mss ) {
            delete mss;
        }
        mss = nullptr;
    }
    break;
    case VA_SetTypes::SetOfStrings: {
        if ( uss ) {
            delete uss;
        }
        uss = nullptr;
    }
    break;
    case VA_SetTypes::SetofTimes: {
        if ( vb ) {
            delete vb;
        }
        vb = nullptr;
    }
    break;
    case VA_SetTypes::Undefined:
        break;
    }
}

void VASet::Serialize ( mtfa_binary_stream_save * bs )
{
    DO_VASET_LOCK(rmtx);

    //stype
    bs->Append<int> ( mtfa_enum_to_int ( stype ) );

    //sname
    bs->Append<string> ( sname );

    //Added_items
    {
        bs->Append<int> ( ( int ) added_items.size() );
        for ( auto & it: added_items ) {
            bs->Append<string> ( it.first );
            bs->Append<time_t> ( it.second );
        }
    }

    //removed_items
    {
        bs->Append<int> ( ( int ) removed_items.size() );
        for ( auto & it: removed_items ) {
            bs->Append<string> ( it.first );
            bs->Append<time_t> ( it.second );
        }
    }

    //initial_values
    bs->Append<int> ( ( int ) initial_values.size() );
    for ( auto & it: initial_values ) {
        bs->Append<string> ( it );
    }

    //Added_items
    {
        bs->Append<int> ( ( int ) added_items_pairs.size() );
        for ( auto & it: added_items_pairs ) {
            bs->Append<string> ( it.first );
            bs->Append<string> ( it.second.first );
            bs->Append<time_t> ( it.second.second );
        }
    }

    //Removed_items
    {
        bs->Append<int> ( ( int ) removed_items_pairs.size() );
        for ( auto & it: removed_items_pairs ) {
            bs->Append<string> ( it.first );
            bs->Append<string> ( it.second.first );
            bs->Append<time_t> ( it.second.second );
        }
    }

    //initial_values
    bs->Append<int> ( ( int ) initial_values_pairs.size() );
    for ( auto & it: initial_values_pairs ) {
        bs->Append<string> ( it.first );
        bs->Append<string> ( it.second );
    }

    //pars
    bs->Append<int> ( ( int ) pars.size() );
    for ( auto & it: pars ) {
        bs->Append<string> ( it );
    }

    //operative_data, per quelli che le hanno
    bs->Append<int> ( ( int ) operative_data.size() );
    for ( auto & it: operative_data ) {
        bs->Append<string> ( it );
    }

    //i dati presenti nelle DSs di run time, non sono salvati poiché saranno ricostruiti alla deserialize di VASets
}

void VASet::Deserialize ( mtfa_binary_stream_load * bs )
{
    //i dati presenti nelle DSs di run time, non sono salvati poiché saranno ricostruiti alla deserialize di VASets
    DO_VASET_LOCK(rmtx);

    //stype
    int istype = bs->Read<int>();
    stype = mtfa_int_to_enum<VA_SetTypes> ( istype );

    //sname
    sname = bs->Read<string>();

    //Added_items
    {
        added_items.clear();
        int len = bs->Read<int>();
        for ( int i=0; i<len; ++i ) {
            string s = bs->Read<string>();
            time_t t = bs->Read<time_t>();
            added_items[s] = t;
        }
    }

    //removed_items
    {
        removed_items.clear();
        int len = bs->Read<int>();
        for ( int i=0; i<len; ++i ) {
            string s = bs->Read<string>();
            time_t t = bs->Read<time_t>();
            removed_items[s] = t;
        }
    }

    //initial_values
    initial_values.clear();
    int len = bs->Read<int>();
    for ( int i=0; i<len; ++i ) {
        initial_values.push_back ( bs->Read<string>() );
    }


    //Added_items
    {
        added_items_pairs.clear();
        int len = bs->Read<int>();
        for ( int i=0; i<len; ++i ) {
            string s1 = bs->Read<string>();
            string s2 = bs->Read<string>();
            time_t t = bs->Read<time_t>();
            added_items_pairs[s1] = make_pair(s2, t );
        }
    }

    //removed_items
    {
        removed_items_pairs.clear();
        int len = bs->Read<int>();
        for ( int i=0; i<len; ++i ) {
            string s1 = bs->Read<string>();
            string s2 = bs->Read<string>();
            time_t t = bs->Read<time_t>();
            removed_items_pairs[s1] = make_pair(s2, t);
        }
    }

    //initial_values
    initial_values_pairs.clear();
    len = bs->Read<int>();
    for ( int i=0; i<len; ++i ) {
        initial_values_pairs.push_back ( make_pair(bs->Read<string>(), bs->Read<string>()));
    }

    //pars
    pars.clear();
    len = bs->Read<int>();
    for ( int i=0; i<len; ++i ) {
        pars.push_back ( bs->Read<string>() );
    }

    //operative_data
    operative_data.clear();
    len = bs->Read<int>();
    for ( int i=0; i<len; ++i ) {
        operative_data.insert ( bs->Read<string>() );
    }
}

VASets::VASets()
{
    datasets.clear();
}

VASets::~VASets()
{
    for ( auto it : datasets ) {
        if ( it.second ) {
            delete it.second;
        }
        it.second = nullptr;
    }
    datasets.clear();
}

//unordered_map<string, VA_Set_Of_Values*> & VAset::dataset() {
//    return _dataset;
//}

void VASets::PurgeAddedAndRemovedValues ( VASets * vasets )
{
    //Every T seconds check if items are expired
    while ( true ) {
        sleep ( T );
        time_t t = time ( nullptr );
        for ( auto & theset: vasets->datasets ) {
            //l'operazione viene fatta per ogni dataset
            DO_VASET_LOCK(theset.second->rmtx);
            {
                auto valtime = theset.second->added_items.begin();
                while ( valtime != theset.second->added_items.end() ) {
                    if ( t >= valtime->second ) {
                        vasets->DelItem ( theset.first, valtime->first, 0, false, false);
                        valtime = theset.second->added_items.erase ( valtime );
                    } else {
                        ++valtime;
                    }
                }
            }

            //e ora i delete items
            {
                auto valtime = theset.second->removed_items.begin();
                while ( valtime != theset.second->removed_items.end() ) {
                    if ( t >= valtime->second ) {
                        vasets->AddItem ( theset.first, valtime->first, 0, false, false );
                        valtime = theset.second->removed_items.erase ( valtime );
                    } else {
                        ++valtime;
                    }
                }
            }

            //e anche i pairs
            {
                auto valtime = theset.second->added_items_pairs.begin();
                while ( valtime != theset.second->added_items_pairs.end() ) {
                    if ( t >= valtime->second.second ) {
                        vasets->DelItem ( theset.first, valtime->first, 0, false, false );
                        valtime = theset.second->added_items_pairs.erase ( valtime );
                    } else {
                        ++valtime;
                    }
                }
            }

            //e ora i delete items
            {
                auto valtime = theset.second->removed_items_pairs.begin();
                while ( valtime != theset.second->removed_items_pairs.end() ) {
                    if ( t >= valtime->second.second ) {
                        vasets->AddItem ( theset.first, valtime->first, 0, false, false );
                        valtime = theset.second->removed_items_pairs.erase ( valtime );
                    } else {
                        ++valtime;
                    }
                }
            }
        }
    }
}


void VASets::AddItem(const string& setname, const string& item_key, const string& item_value, time_t duration, bool archive, bool from_vr_action)
{
    bool mustAdd = false;
    if (duration==1000000000)
        mustAdd = true;

    duration += time ( nullptr );

    //Il set esiste?
    auto theset = datasets.find ( setname );
    if ( theset == datasets.end() ) {
        cout << __func__ << "set " << setname << " not found.\n";
        return;
    }

    DO_VASET_LOCK(theset->second->rmtx);
    //Vengo da una VR?
    if (from_vr_action)
    {
        //costruisco la struttura e la invio
        SendItemToSisterAndChildren(setname, item_key, item_value, duration, true);
    }

    //L'elemento già esiste tra quelli aggiunti?
    //Se devo archiviare e se già esiste tra gli aggiunti, aggiorno solo la scadenza
    if ( archive ) {
        auto it = theset->second->added_items_pairs.find ( item_key );
        if ( it != theset->second->added_items_pairs.end() ) {
            it->second.second = duration;
            if (!mustAdd)
                return;
        }
    }

    //Aggiungo l'elemento
    switch ( theset->second->stype ) {
    case VA_SetTypes::Dictionary:
    {
        auto found = theset->second->umss->find(item_key); //->operative_data.find ( item_value );
        if ( found != theset->second->umss->end()) { //operative_data.end() ) {
            cout << __func__ << "item " << item_key << " already exists in the original set " << setname << endl;
            return;
        }
        (*(theset->second->umss))[item_key] = item_value; //operative_data.insert ( item_value );
        cout << __func__ << "ADDED: item " << item_key << " as " << found->second << setname << endl;


        if ( archive ) {
            theset->second->added_items[item_key] = duration;
        }
    }
    break;
    default:
        mtfa_log("Switch Case not found: %d", (int)theset->second->stype);
    }
}

void VASets::AddItem ( const string &setname, const string &item_value, time_t duration, bool archive, bool from_vr_action)
{
    bool mustAdd = false;
    if (duration==1000000000)
        mustAdd = true;
    duration += time ( nullptr );

    //Il set esiste?
    auto theset = datasets.find ( setname );
    if ( theset == datasets.end() ) {
        cout << __func__ << "set " << setname << " not found.\n";
        return;
    }
    else
    {
        mtfa_log("Adding <%s> to set <%s> for %d seconds (%s)\n", item_value.c_str(), setname.c_str(), duration-time(nullptr), (archive?"Archiva":"Non archivia"));
    }

    DO_VASET_LOCK(theset->second->rmtx);
    //Vengo da una VR?
    if (from_vr_action)
    {
        //costruisco la struttura e la invio
        SendItemToSisterAndChildren(setname, item_value, duration, true);
    }

    //L'elemento già esiste tra quelli aggiunti?
    //Se devo archiviare e se già esiste tra gli aggiunti, aggiorno solo la scadenza
    if ( archive ) {
        auto it = theset->second->added_items.find ( item_value );
        if ( it != theset->second->added_items.end() ) {
            it->second = duration;
            if (!mustAdd)
                return;
        }
    }

    //Aggiungo l'elemento
    switch ( theset->second->stype ) {
    case VA_SetTypes::SetOfIpS: {
        //ma è presente?
        auto toBeSearched = IpPortStringToVS(item_value);
        unsigned int pos;
        auto found = theset->second->mss->Search(toBeSearched, pos);
        if ( found ) {
            cout << __func__ << "item " << item_value << " already exists in the original set " << setname << endl;
            return;
        }
        theset->second->operative_data.insert ( item_value );
        mtfa_sequence_star *newmss = domakestarip ( theset->second->operative_data );
        auto oldmss = theset->second->mss;
        theset->second->mss = newmss;
        if ( oldmss ) {
            delete oldmss;
        }
        if ( archive ) {
            theset->second->added_items[item_value] = duration;
        }
    }
    break;
    case VA_SetTypes::SetOfStrings: {
        auto found = theset->second->uss->find(item_value); //->operative_data.find ( item_value );
        if ( found != theset->second->uss->end()) { //operative_data.end() ) {
            cout << __func__ << "item " << item_value << " already exists in the original set " << setname << endl;
            return;
        }
        theset->second->uss->insert(item_value); //operative_data.insert ( item_value );
//           for ( auto tmpit : theset->second->operative_data ) {
//                theset->second->uss->insert ( tmpit );
//           }

        if ( archive ) {
            theset->second->added_items[item_value] = duration;
        }
    }
    break;
    case VA_SetTypes::Dictionary: {
        //L'item va splittato in chiave, valore: splitting: |
        T_V_Split vs = mtfa_string_split(item_value, "|");
        if (vs.size() != 2)
        {
            cerr << "requested insert into dictionary to a value not containing |" << endl;
            return;
        }
        theset->second->umss->insert(make_pair(vs[0], vs[1])); //operative_data.insert ( item_value );
//           for ( auto tmpit : theset->second->operative_data ) {
//                theset->second->uss->insert ( tmpit );
//           }

        if ( archive ) {
            theset->second->added_items[vs[0]] = duration;
        }
    }
    break;
    case VA_SetTypes::SetOfUrlS: {
        auto toBeSearched = UrlStringToVS(item_value);
        unsigned int pos;
        auto found = theset->second->mss->Search(toBeSearched, pos);
        if ( found ) {
            cout << __func__ << "item " << item_value << " already exists in the original set " << setname << endl;
            return;
        }
        theset->second->operative_data.insert ( item_value );
        mtfa_sequence_star *newmss = domakestarurl ( theset->second->operative_data );
        auto oldmss = theset->second->mss;
        theset->second->mss = newmss;
        if ( oldmss ) {
            delete oldmss;
        }

        if ( archive ) {
            theset->second->added_items[item_value] = duration;
        }
    }
    break;
    case VA_SetTypes::SetofTimes: {
        auto found = theset->second->operative_data.find ( item_value );
        if ( found != theset->second->operative_data.end() ) {
            cout << __func__ << "item " << item_value << " already exists in the original set " << setname << endl;
            return;
        }
        theset->second->operative_data.insert ( item_value );
        vector<bool> *newtr = domaketimerange ( theset->second->operative_data );
        auto oldvb = theset->second->vb;
        theset->second->vb = newtr; //deallocazione automatica
        if ( oldvb ) {
            delete oldvb;
        }

        if ( archive ) {
            theset->second->added_items[item_value] = duration;
        }
    }
    break;
    case VA_SetTypes::Undefined:
        break;
    }
}

void VASets::DelItem ( const string &setname, const string &item_value, time_t duration, bool archive, bool from_vr_action )
{
    //Il set esiste?
    auto theset = datasets.find ( setname );
    if ( theset == datasets.end() ) {
        mtfa_log("set %s not found\n", setname.c_str());
        return;
    }
    
    bool mustRemove = false;
    if (duration==1000000000)
        mustRemove = true;

    duration += time ( nullptr );

    DO_VASET_LOCK(theset->second->rmtx);
    //Vengo da una VR?
    if (from_vr_action)
    {
        //costruisco la struttura e la invio
        SendItemToSisterAndChildren(setname, item_value, duration, false);
    }

    if ( archive ) {
        auto it = theset->second->removed_items.find ( item_value );
        if ( it != theset->second->removed_items.end() ) {
            it->second = duration;
            if (!mustRemove)
                return;
        }
    }

    //Rimuovo l'elemento
    switch ( theset->second->stype ) {
    case VA_SetTypes::SetOfIpS: {
        //ma è presente?
        auto toBeSearched = IpPortStringToVS(item_value);
        unsigned int pos;
        auto found = theset->second->mss->Search(toBeSearched, pos);
        if ( found ) {
            mtfa_log("Impossible to remove item %s  because of it does not exists in the set %s\n", item_value.c_str(), setname.c_str());
            return;
        }
        //Rimuovo il valore dall'insieme
        theset->second->operative_data.erase ( item_value );
        mtfa_sequence_star *newmss = domakestarip ( theset->second->operative_data );
        auto oldmss = theset->second->mss;
        theset->second->mss = newmss;
        if ( oldmss ) {
            delete oldmss;
        }

        if ( archive ) {
            theset->second->removed_items[item_value] = duration;
        }
    }
    break;
    case VA_SetTypes::Dictionary: {
        //Ma è presente?
        auto found = theset->second->umss->find(item_value); //operative_data.find ( item_value );
        if ( found == theset->second->umss->end() ) { //operative_data.end() ) {
            cout << __func__ << "Impossible to remove item " << item_value << " because of it does not exists in the set " << setname << endl;
            return;
        }
        //Rimuovo il valore dall'insieme
        pair<string, time_t> second = make_pair(found->second, duration);
        theset->second->umss->erase ( item_value );
//          theset->second->uss->clear();
//          for ( auto tmpit : theset->second->operative_data ) {
//              theset->second->uss->insert ( tmpit );
//          }

        if ( archive ) {
            theset->second->removed_items_pairs[item_value] = second;
        }
    }
    break;
    case VA_SetTypes::SetOfStrings: {
        //Ma è presente?
        auto found = theset->second->uss->find(item_value); //operative_data.find ( item_value );
        if ( found == theset->second->uss->end()) { //operative_data.end() ) {
            cout << __func__ << "Impossible to remove item " << item_value << " because of it does not exists in the set " << setname << endl;
            return;
        }

        //Rimuovo il valore dall'insieme
        theset->second->uss->erase( item_value );
//           theset->second->uss->clear();
//           for ( auto tmpit : theset->second->operative_data ) {
//                theset->second->uss->insert ( tmpit );
//           }
//           //set->second->uss() = set->second->operative_data();

        if ( archive ) {
            theset->second->removed_items[item_value] = duration;
        }
    }
    break;
    case VA_SetTypes::SetOfUrlS: {
        //ma è presente?
        auto toBeSearched = UrlStringToVS(item_value);
        unsigned int pos;
        auto found = theset->second->mss->Search(toBeSearched, pos);
        if ( found ) {
            cout << __func__ << "Impossible to remove item " << item_value << " because of it does not exists in the set " << setname << endl;
            return;
        }
        //Rimuovo il valore dall'insieme
        theset->second->operative_data.erase ( item_value );
        mtfa_sequence_star *newmss = domakestarurl ( theset->second->operative_data );
        auto oldmss = theset->second->mss;
        theset->second->mss = newmss;
        if ( oldmss ) {
            delete oldmss;
        }

        if ( archive ) {
            theset->second->removed_items[item_value] = duration;
        }
    }
    break;
    case VA_SetTypes::SetofTimes: {
        //ma è presente?
        auto found = theset->second->operative_data.find ( item_value );
        if ( found == theset->second->operative_data.end() ) {
            cout << __func__ << "Impossible to remove item " << item_value << " because of it does not exists in the set " << setname << endl;
            return;
        }
        theset->second->operative_data.erase ( item_value );
        vector<bool> *newtr = domaketimerange ( theset->second->operative_data );
        auto oldvb = theset->second->vb;
        theset->second->vb = newtr; //deallocazione automatica

        if ( oldvb ) {
            delete oldvb;
        }

        if ( archive ) {
            theset->second->removed_items[item_value] = duration;
        }
    }
    break;
    case VA_SetTypes::Undefined:
        break;
    }
}

bool VASets::CheckItem ( const string &setname, const string &item_key, string & item_value )
{
    //lock_guard<mutex> lock(lockme);
    item_value.clear();

    //Il set esiste?
    auto theset = datasets.find ( setname );
    if ( theset == datasets.end() ) {
        cout << __func__ << "set " << setname << " not found.\n";
        return false;
    }

    //Cerco l'elemento
    DO_VASET_LOCK(theset->second->rmtx);
    switch ( theset->second->stype ) {
    case VA_SetTypes::SetOfIpS: {
        //ma è presente?
        auto toBeSearched = IpPortStringToVS(item_key);
        unsigned int pos;
        auto found = theset->second->mss->Search(toBeSearched, pos);
        if ( !found ) {
            return false;
        }
        else
            return true;
    }
    break;
    case VA_SetTypes::SetOfStrings: {
        //Ma è presente?
        auto found = theset->second->uss->find ( item_key );
        if ( found == theset->second->uss->end() ) {
            return false;
        }
        else
            return true;
    }
    break;
    case VA_SetTypes::Dictionary: {
        //Ma è presente?
        auto found = theset->second->umss->find ( item_key );
        if ( found == theset->second->umss->end() ) {
            return false;
        }
        else
        {
            item_value = found->second;
            return true;
        }
    }
    break;
    case VA_SetTypes::SetOfUrlS: {
        //ma è presente?
        auto toBeSearched = UrlStringToVS(item_key);
        unsigned int pos;
        auto found = theset->second->mss->Search(toBeSearched, pos);
        if ( !found ) {
            return false;
        }
        else
            return true;
    }
    break;
    case VA_SetTypes::SetofTimes: {
        //ma è presente?
        auto found = theset->second->operative_data.find ( item_value );
        if ( found == theset->second->operative_data.end() ) {
            return true;
        }
        else
            return true;
    }
    break;
    case VA_SetTypes::Undefined:
        break;
    }
    return false;
}

vector<bool> * VASets::domaketimerange ( const unordered_set<string>& data )
{
    vector<bool> * vb = new vector<bool> ( 1440 );

    if ( data.size() == 0 ) {
        //timerange vuoto! Non passa nulla!
        std::fill ( vb->begin(), vb->end(), false );
    }
    //in un giorno ci sono 60*24 minuti = 1440 minuti
    for ( auto it : data ) {
        T_V_Split vs = mtfa_string_split ( it, ":" );
        //converto i numeri in minuti
        //00.00:12.00, 13.00:23.59
        T_V_Split du = mtfa_string_split ( vs[0], "." );
        int inizio = atoi ( du[0].c_str() ) *10 + atoi ( du[1].c_str() );
        du = mtfa_string_split ( vs[1], "." );
        int fine = atoi ( du[0].c_str() ) *10 + atoi ( du[1].c_str() );
        auto it_inizio = std::next ( vb->begin(), inizio );
        auto it_fine = std::next ( vb->begin(), fine );
        fill ( it_inizio, it_fine, true );
    }
    return vb;
}

vector<unsigned short> VASets::IpPortStringToVS ( string s )
{
    string port, ip;
    T_V_Split vs = mtfa_string_split ( s, ":" );
    ip = vs[0];
    if ( vs.size() > 1 ) {
        port = vs[1];
    } else {
        port = "*";
    }

    //ora splitto ip
    T_V_Split vip = mtfa_string_split ( ip, "." );

    //A questo punto ho i quattro ip e ho la porta. Converto in network order (6 bytes -> 6 short)
    vector<unsigned short> item ( 6 );
    for ( int i = 0; i < 4; ++i ) {
        item[i] = atoi ( vip[i].c_str() );
    }

    //ora la porta
    if ( port.compare ( "*" ) == 0 ) {
        item[4] = mtfa_sequence_star::jollyvalue;
        item[5] = mtfa_sequence_star::jollyvalue;
    } else {
        //big endian (network order)
        short p = atoi ( port.c_str() );
        item[4] = ( ( p >> 8 ) & 0x00FF );
        item[5] = ( p & 0x00FF );
    }

    return item;
}

//Sono sempre ip con asterischi e eventualmente il due punti
mtfa_sequence_star * VASets::domakestarip ( const unordered_set< string > & data )
{
    if ( data.size() == 0 ) {
        return nullptr;
    }

    int count = 1;
    mtfa_sequence_star * baseds = new mtfa_sequence_star();
    for ( auto it : data ) {
        //per ogni ip presente, faccio un vettore di short, compresa la porta
        string port, ip;
        T_V_Split vs = mtfa_string_split ( it, ":" );
        ip = vs[0];
        if ( vs.size() > 1 ) {
            port = vs[1];
        } else {
            port = "*";
        }

        //Ora splitto ip
        T_V_Split vip = mtfa_string_split ( ip, "." );

        //a questo punto ho i quattro ip e ho la porta. Converto in network order (6 bytes -> 6 short)
        vector<unsigned short> item ( 6 );
        for ( int i = 0; i < 4; ++i ) {
            if ( vip[i].compare ( "*" ) == 0 ) {
                item[i] = mtfa_sequence_star::jollyvalue;
            } else {
                item[i] = atoi ( vip[i].c_str() );
            }
        }

        //ora la porta
        if ( port.compare ( "*" ) == 0 ) {
            item[4] = mtfa_sequence_star::jollyvalue;
            item[5] = mtfa_sequence_star::jollyvalue;
        } else {
            //big endian (network order)
            short p = atoi ( port.c_str() );
            item[4] = ( ( p >> 8 ) & 0x00FF );
            item[5] = ( p & 0x00FF );
        }
        baseds->Insert ( item, count++ );
    }
    baseds->Build();
    return baseds;
}

//Converte una url in vector<short>

vector<unsigned short> VASets::UrlStringToVS ( const string s, const string splitter)
{
    T_V_Split vs = mtfa_string_split ( s, splitter );

    //Se ci sono elementi vuoti, li elimino, ad esempio il primo elemento oppure l'ultimo!!!
    auto jt = vs.begin();
    while ( jt != vs.end() ) {
        if ( ( *jt ).size() == 0 ) {
            jt = vs.erase ( jt );
        } else {
            ++jt;
        }
    }

    //in testa metto lo "/"!
    vs.insert ( vs.begin(), splitter );

    vector<unsigned short> item ( 4 * url_deep ); //se sono 4 byte per intero (e quindi per item della url) allora sono 4*url_deep short
    int value;
    const unsigned char * pvalue = ( unsigned char* ) &value;
    for ( int i = 0; i < url_deep; ++i ) {
        if ( vs.size() <= i ) {
            item[4 * i] = mtfa_sequence_star::emptyvalue;
            item[4 * i + 1] = mtfa_sequence_star::emptyvalue;
            item[4 * i + 2] = mtfa_sequence_star::emptyvalue;
            item[4 * i + 3] = mtfa_sequence_star::emptyvalue;
        } else {
            value = ph_pages.find ( vs[i] );
            if ( value == -1 ) {
                value = 0xFFFFFFFF;
            }

            //Converto in network order (big endian)
            item[4 * i] = value & 0x000000FF;
            value >>= 8;
            item[4 * i + 1] = value & 0x000000FF;
            value >>= 8;
            item[4 * i + 2] = value & 0x000000FF;
            value >>= 8;
            item[4 * i + 3] = value & 0x000000FF;
        }
    }
    return item;
}

mtfa_sequence_star * VASets::domakestarurl ( const unordered_set<string>& data )
{
    unordered_set<string> base = data;
    if ( base.size() == 0 ) {
        return nullptr;
    }

    int count = 1;
    auto baseds = new mtfa_sequence_star();
    for ( auto it : base ) {
        //Se ci sono elementi vuoti, li elimino, ad esempio il primo elemento oppure l'ultimo!!!
        //nel senso che doppio / lo trasformo in unico
        static pcre2_code * reCompiled = mtfa_compile_pattern( "/+" );
        string inputs = mtfa_replace_matching_pattern ( it, reCompiled, "/", true );

        //ora ho una stringa di input senza doppi slash
        //la splitto e poi mi ricordo se terminava con * oppure no
        bool last_is_star = inputs.back() == '*';

        //per ogni url presente, faccio un vettore di short
        T_V_Split vs = mtfa_string_split ( inputs, "/" );

        //Se ci sono elementi vuoti, li elimino, ad esempio il primo elemento oppure l'ultimo!!!
        auto jt = vs.begin();
        while ( jt != vs.end() ) {
            if ( ( *jt ).size() == 0 ) {
                jt = vs.erase ( jt );
            } else {
                ++jt;
            }
        }

        //in testa metto lo "/"!
        vs.insert ( vs.begin(), "/" );

        //se più lungo di url_deep, ne diminuisco la lunghezza
        if ( vs.size() > url_deep ) {
            vs.resize ( url_deep );
        }

        //A questo punto ho un vettore che contiene stringhe oppure asterischi
        //Lo devo convertire in array di interi e poi, per ogni intero, un array di 4 short
        //se termina con *, lo porto fino in fondo.
        //se termina in modo diverso, mi fermo dove termina.
        vector<unsigned short> item;
        item.resize ( 4 * url_deep ); //se sono 3 byte per intero (e quindi per item della url) allora sono 3*url_deep short
        int value;
        const unsigned char * pvalue = ( unsigned char* ) &value;
        for ( int i = 0; i < url_deep; ++i ) {
            if ( vs.size() <= i ) {
                if ( !last_is_star ) {
                    //Metto gli empty!
                    item[4 * i] = mtfa_sequence_star::emptyvalue;
                    item[4 * i + 1] = mtfa_sequence_star::emptyvalue;
                    item[4 * i + 2] = mtfa_sequence_star::emptyvalue;
                    item[4 * i + 3] = mtfa_sequence_star::emptyvalue;
                } else {
                    item[4 * i] = mtfa_sequence_star::jollyvalue;
                    item[4 * i + 1] = mtfa_sequence_star::jollyvalue;
                    item[4 * i + 2] = mtfa_sequence_star::jollyvalue;
                    item[4 * i + 3] = mtfa_sequence_star::jollyvalue;
                }
            } else if ( vs[i].compare ( "*" ) == 0 ) {
                item[4 * i] = mtfa_sequence_star::jollyvalue;
                item[4 * i + 1] = mtfa_sequence_star::jollyvalue;
                item[4 * i + 2] = mtfa_sequence_star::jollyvalue;
                item[4 * i + 3] = mtfa_sequence_star::jollyvalue;
            } else {
                value = ph_pages.insert ( vs[i] );

                //converto in little endian
                item[4 * i] = value & 0x000000FF;
                value >>= 8;
                item[4 * i + 1] = value & 0x000000FF;
                value >>= 8;
                item[4 * i + 2] = value & 0x000000FF;
                value >>= 8;
                item[4 * i + 3] = value & 0x000000FF;
            }
        }
        baseds->Insert ( item, count++ );
    }
    baseds->Build();
    return baseds;
}

bool VASets::AddSetofStringPair ( const string & name, list<pair<string, string>> initial_values, list<string> db_pars )
{
    if ( datasets.find ( name ) != datasets.end() ) {
        return false;     //already exists
    }

    VASet * aset = new VASet();
    aset->initial_values_pairs.assign ( initial_values.begin(), initial_values.end() ); //non cambia mai!
    //aset->operative_data.insert ( initial_values.begin(), initial_values.end() );      //utilizzato per inserire e eliminare valori

    aset->pars.assign ( db_pars.begin(), db_pars.end() ); //se ci sono, altrimenti lista vuota
    aset->sname = name;
    aset->stype = VA_SetTypes::Dictionary;
    aset->umss = new unordered_map<string, string>();
    for ( auto it : initial_values ) {
        aset->umss->insert ( it );
    }

    datasets[name] = aset;

    return true;
}

bool VASets::AddSetofString ( const string & name, list<string> initial_values, list<string> db_pars )
{
    if ( datasets.find ( name ) != datasets.end() ) {
        return false;     //already exists
    }

    VASet * aset = new VASet();
    aset->initial_values.assign ( initial_values.begin(), initial_values.end() ); //non cambia mai!
    //aset->operative_data.insert ( initial_values.begin(), initial_values.end() );      //utilizzato per inserire e eliminare valori

    aset->pars.assign ( db_pars.begin(), db_pars.end() ); //se ci sono, altrimenti lista vuota
    aset->sname = name;
    aset->stype = VA_SetTypes::SetOfStrings;
    aset->uss = new unordered_set<string>();
    for ( auto it : initial_values ) {
        aset->uss->insert ( it );
    }

    datasets[name] = aset;

    return true;
}

bool VASets::AddSetofIpS ( const string & name, list<string> initial_values, list<string> db_pars )
{
    if ( datasets.find ( name ) != datasets.end() ) {
        return false;     //already exists
    }

    VASet * aset = new VASet();
    aset->initial_values.assign ( initial_values.begin(), initial_values.end() ); //non cambia mai!
    aset->operative_data.insert ( initial_values.begin(), initial_values.end() );
    aset->pars.assign ( db_pars.begin(), db_pars.end() ); //se ci sono, altrimenti lista vuota
    aset->sname = name;
    aset->stype = VA_SetTypes::SetOfIpS;
    aset->mss = domakestarip ( aset->operative_data );

    datasets[name] = aset;
    return true;
}

bool VASets::AddSetofUrlS ( const string &name, list<string> initial_values, list<string> db_pars )
{
    if ( datasets.find ( name ) != datasets.end() ) {
        return false;     //already exists
    }

    VASet * aset = new VASet();
    aset->initial_values.assign ( initial_values.begin(), initial_values.end() ); //non cambia mai!
    aset->operative_data.insert ( initial_values.begin(), initial_values.end() );
    aset->pars.assign ( db_pars.begin(), db_pars.end() ); //se ci sono, altrimenti lista vuota
    aset->sname = name;
    aset->stype = VA_SetTypes::SetOfUrlS;
    aset->mss = domakestarurl ( aset->operative_data );

    datasets[name] = aset;
    return true;
}

bool VASets::AddSetofTime ( const string &name, list<string> initial_values, list<string> db_pars )
{
    if ( datasets.find ( name ) != datasets.end() ) {
        return false;     //already exists
    }

    VASet * aset = new VASet();
    aset->initial_values.assign ( initial_values.begin(), initial_values.end() ); //non cambia mai!
    aset->operative_data.insert ( initial_values.begin(), initial_values.end() );
    aset->pars.assign ( db_pars.begin(), db_pars.end() ); //se ci sono, altrimenti lista vuota
    aset->sname = name;
    aset->stype = VA_SetTypes::SetofTimes;
    aset->vb = domaketimerange ( aset->operative_data );

    datasets[name] = aset;
    return true;
}

void VASets::Serialize ( mtfa_binary_stream_save * bs )
{
    //questi due non cambiano
    //this->T;
    //this->url_deep;

    ph_pages.Serialize ( bs );

    //i dataset. Quanti elementi sono?
    bs->Append<int> ( ( int ) datasets.size() );
    for ( auto & itds: datasets ) {
        bs->Append<string> ( itds.first );
        itds.second->Serialize ( bs );
    }
}

void VASets::Deserialize ( mtfa_binary_stream_load * bs )
{
    ph_pages.clear();
    ph_pages.Deserialize ( bs );

    //Svuoto i dataset
    for ( auto & it: datasets )
        if ( it.second ) {
            delete it.second;
        }

    datasets.clear();

    //i dataset. Quanti elementi sono?
    //unordered_map<string, VASet*> datasets;
    int len = bs->Read<int>();
    for ( int i=0; i<len; ++i ) {
        string s = bs->Read<string>();
        auto vaset = new VASet();
        vaset->Deserialize ( bs );
        datasets[s] = vaset;
    }
    RebuildVaSets ( false );
}

void VASets::RebuildVaSets ( bool redo_queries )
{
    //if redo:queries, does again queries and rebuild the original_data sets
    //rebuild from initial_values and insert/remove based on insert/remove sets

    time_t now = time ( nullptr );

    for ( auto vaset : datasets )
    {
        DO_VASET_LOCK(vaset.second->rmtx);
        //Riprendo i dati dal DB, se richiesto e se presenti i parametri di accesso
        if ( redo_queries && vaset.second->pars.size() > 0 )
        {
            if ( vaset.second->stype != VA_SetTypes::Dictionary )
            {
                list<string> slist = GetFromDb ( list<string> ( vaset.second->pars.begin(), vaset.second->pars.end() ) );
                vaset.second->initial_values.assign ( slist.begin(), slist.end() );
                vaset.second->operative_data.clear();
                vaset.second->operative_data.insert ( vaset.second->initial_values.begin(), vaset.second->initial_values.end() );

                //Per ogni added item, lo riaggiungo, per ogni del item lo tolgo
                for ( auto added : vaset.second->added_items )
                {
                    if ( now < added.second )
                    {
                        vaset.second->operative_data.insert ( added.first );
                    }
                }

                for ( auto removed : vaset.second->removed_items )
                {
                    if ( now < removed.second )
                    {
                        vaset.second->operative_data.erase ( removed.first );
                    }
                }
            }
            else
            {
                list<pair<string, string>> slist = GetFromDbPair ( list<string> ( vaset.second->pars.begin(), vaset.second->pars.end() ) );
                vaset.second->initial_values_pairs.assign ( slist.begin(), slist.end() );
                //Per ogni added item, lo riaggiungo, per ogni del item lo tolgo

                for ( auto added : vaset.second->added_items_pairs )
                {
                    if ( now < added.second.second )
                    {
                        vaset.second->operative_data_pairs[added.first] = added.second.first;
                    }
                }

                for ( auto removed : vaset.second->removed_items_pairs )
                {
                    if ( now < removed.second.second )
                    {
                        vaset.second->operative_data_pairs.erase ( removed.first );
                    }
                }
            }
        }
    }

    //ora ho i dati riallineati, ricostruisco gli insiemi di ricerca!

    for ( auto vaset : datasets ) {
        DO_VASET_LOCK(vaset.second->rmtx);
        cout << "DATASET NAME " << vaset.first << endl;
        switch ( vaset.second->stype ) {
        case VA_SetTypes::SetOfIpS: {
            cout << "\tSetOfIpS" << endl;
//
//                if ( vaset.second->pars.size() >0 ) {
//                     for ( auto it: vaset.second->pars ) {
//                          cout << it << " ";
//                     }
//                     cout << endl;
//                } else {
//                     for ( auto it : vaset.second->operative_data ) {
//                          cout << it << " ";
//                     }
//                     cout << endl;
//                }
            //e ora ricostruisco la struttura
            mtfa_sequence_star *newmss = domakestarip ( vaset.second->operative_data );
            auto oldmss = vaset.second->mss;
            vaset.second->mss = newmss;
            if ( oldmss ) {
                delete oldmss;
            }
        }
        break;
        case VA_SetTypes::SetOfStrings: {
            cout << "\tSetOfStrings" << endl;
            //e ora ricostruisco la struttura
            vaset.second->uss->clear();
            *(vaset.second->uss) = move(vaset.second->operative_data);
        }
        break;
        case VA_SetTypes::Dictionary: {
            cout << "\tDictionary" << endl;
            //e ora ricostruisco la struttura
            vaset.second->umss->clear();
            *(vaset.second->umss) = move(vaset.second->operative_data_pairs);
        }
        break;
        case VA_SetTypes::SetOfUrlS: {
            cout << "\tSetOfUrlS" << endl;
//                if ( vaset.second->pars.size() >0 ) {
//                     for ( auto it: vaset.second->pars ) {
//                          cout << it << " ";
//                     }
//                     cout << endl;
//                } else {
//                     for ( auto it : vaset.second->operative_data ) {
//                          cout << it << " ";
//                     }
//                     cout << endl;
//                }
            //e ora ricostruisco la struttura
            mtfa_sequence_star *newmss = domakestarurl ( vaset.second->operative_data );
            auto oldmss = vaset.second->mss;
            vaset.second->mss = newmss;
            if ( oldmss ) {
                delete oldmss;
            }
        }
        break;
        case VA_SetTypes::SetofTimes: {
            cout << "\tSetofTimes" << endl;
//                if ( vaset.second->pars.size() >0 ) {
//                     for ( auto it: vaset.second->pars ) {
//                          cout << it << " ";
//                     }
//                     cout << endl;
//                } else {
//                     for ( auto it : vaset.second->operative_data ) {
//                          cout << it << " ";
//                     }
//                     cout << endl;
//                }
            //e ora ricostruisco la struttura
            vector<bool> * newvb = domaketimerange ( vaset.second->operative_data );
            auto oldvb = vaset.second->vb;
            vaset.second->vb = newvb;
            if ( oldvb ) {
                delete oldvb;
            }
        }
        break;
        case VA_SetTypes::Undefined:
            cout << "\tUndefined" << endl;
            break;
        }
    }
}




//bool VASets::Add(const string name, const VA_SetTypes itype, const string parameters, const list<string>& data) {
//    auto it = dataset.find(name);
//    if (it != dataset.end())
//        return false;
//
//    switch (itype) {
//        case VA_SetTypes::SetOfPars:
//        {
//            //Per prima cosa risolvo la query e poi proseguo
//            //10.211.55.9:888, "db", "utente", "pwd", "select id from sonde"
//            auto par = data.begin();
//            T_V_Split vipp = mtfa_string_split(*par, ":");
//            short port = 3306;
//            if (vipp.size() == 2)
//                port = atoi(vipp[1].c_str());
//
//            ++par;
//            string dbname = *par;
//            ++par;
//            string username = *par;
//            ++par;
//            string password = *par;
//            ++par;
//            string thequery = *par;
//            mysql_db_class * db = new mysql_db_class(vipp[0], username, password, dbname, port);
//            bool bret = db->do_connect();
//            if (!bret) {
//                cout << __func__ << "Unable to connect to the db: " << dbname << " with user: " << username << " and password: " << password << " on host: " << vipp[0] << ":" << port << endl;
//                exit(-1);
//            }
//
//            list<list < string>> dbresult;
//            bret = db->doMySqlQuery(&dbresult, thequery);
//            if (!bret) {
//                cout << __func__ << "Unable to execute query: " << thequery << endl << "on the db: " <<
//                        dbname << " with user: " << username << " and password: " << password << " on host: " << vipp[0] << ":" << port << endl;
//                exit(-1);
//            }
//            if (db->getCols() == 1) {
//                //Sono nella versione a 1 colonna
//                unordered_set<string> data;
//                for (auto it : dbresult) {
//                    data.insert(it.front());
//                }
//                dataset[name] = new VA_Set_Of_Values(VA_SetTypes::SetOfStrings, name, data);
//            } else {
//                cout << __func__ << "The query: " << thequery << " returns " << db->getCols() << " columns instead than 1 column. Error." << endl;
//                exit(-1);
//            }
//            db->do_disconnect();
//            delete db;
//
//            //            else if (db->getCols() == 2)
//            //            {
//            //                //Sono nella versione a 2 colonna
//            //                unordered_map<string, string> data;
//            //                for (auto it: dbresult)
//            //                {
//            //                    auto p = it.begin();
//            //                    string k = *p; ++p;
//            //                    string v = *p;
//            //                    data.insert(make_pair(k,v));
//            //                }
//            //                dataset()[name] = new VA_Set_Of_Values(VA_SetTypes::SetOfKVStrings, name, data);
//            //            }
//
//            //iIf the result is 1 column, it is a one value set
//        }
//            break;
//    }
//    return true;
//}

//bool VAset::Del(string name) {
//    auto it = dataset.find(name);
//    if (it == dataset.end())
//        return false;
//    dataset.erase(it);
//    return true;
//}

static set<string> GetHosts ( VAengine & vaeng, VA_NodeTree * r )
{
    set<string> hosts;
    if ( r ) {
        for ( auto &it : r->criteria ) {
            if ( it.itype == VA_ItemTypes::HTTP_HOST ) {
                if ( it.op == VA_OpTypes::EQUAL ) {
                    hosts.insert ( it.op1 );
                } else if ( it.op == VA_OpTypes::IN ) {
                    auto uss = vaeng.vasets.datasets[it.op1];
                    DO_VASET_LOCK(uss->rmtx);
                    hosts.insert ( uss->initial_values.begin(), uss->initial_values.end() );
                    //                        for (auto jt: uss->original_data())
                    //                        {
                    //                            hosts.insert(jt);
                    //                        }
                }
            }
        }
        for ( int i = 0; i < r->children.size(); ++i ) {
            if ( r->children[i] ) {
                set<string> ss = GetHosts ( vaeng, r->children[i] );
                hosts.insert ( ss.begin(), ss.end() );
            }
        }
    }
    return hosts;
}

static pair<set<string>, set<string>> GetOther ( VAengine & vaeng, VA_NodeTree * r )
{
    pair<set<string>, set < string>> ss;
    for ( auto it : r->criteria ) {
        switch ( it.itype ) {
        case VA_ItemTypes::HTTP_ANSWER_CODE:
            ss.second.insert ( "@HTTP_RESPONSE_CODE" );
            break;
        case VA_ItemTypes::HTTP_ANSWER_COOKIES:
            ss.second.insert ( "@HTTP_Set-Cookie" );
            break;
        case VA_ItemTypes::HTTP_COOKIES:
            ss.first.insert ( "@HTTP_Cookie" );
            break;
        case VA_ItemTypes::HTTP_METHOD:
            ss.first.insert ( "@HTTP_METHOD" );
            break;
        case VA_ItemTypes::HTTP_URL:
            ss.first.insert ( "@HTTP_URI_URL" );
            break;
        case VA_ItemTypes::HTTP_URI:
            ss.first.insert ( "@HTTP_URI_URL" );
            ss.first.insert ( "@HTTP_URI_DATA" );
            break;
        case VA_ItemTypes::HTTP_URL_HOST:
            ss.first.insert ( "@HTTP_URI_HOST" );
            break;
        case VA_ItemTypes::SA_OS:
            ss.first.insert ( "@OPERATIVE_SYSTEM" );
            break;
        case VA_ItemTypes::HTTP_HEADER_HOST:
            ss.first.insert ( "@HTTP_HOST" );
            break;

        case VA_ItemTypes::HTTP_ANSWER_DATA:
        case VA_ItemTypes::HTTP_DATA_POST: {
            //ss.first.insert("@HTTP_DATA_"+it.op1);
            if ( it.op1.compare ( "0_FULL_QUERY" ) == 0 ) {
                ss.first.insert ( "@HTTP_DATA" );
            } else {
                ss.first.insert ( "@HTTP_DATA_" + it.op1 );
            }
        }
        break;
        case VA_ItemTypes::HTTP_HEADERS:
            ss.first.insert ( "@HTTP_" + it.op1 );
            break;
        case VA_ItemTypes::HTTP_QUERY: {
            if ( it.op1.compare ( "0_FULL_QUERY" ) == 0 ) {
                ss.first.insert ( "@HTTP_DATA" );
            } else {
                ss.first.insert ( "@HTTP_URI_DATA_" + it.op1 );
            }
        }
        break;
        default:
            cout << __func__ << " - " << __LINE__ << endl;
        }
    }

    for ( auto it : r->actions ) {
        switch ( it.observable ) {
        case VA_ItemTypes::HTTP_ANSWER_CODE:
            ss.second.insert ( "@HTTP_RESPONSE_CODE" );
            break;
        case VA_ItemTypes::HTTP_ANSWER_COOKIES:
            ss.second.insert ( "@HTTP_Set-Cookie" );
            break;
        case VA_ItemTypes::HTTP_COOKIES:
            ss.first.insert ( "@HTTP_Cookie" );
            break;
        case VA_ItemTypes::HTTP_METHOD:
            ss.first.insert ( "@HTTP_METHOD" );
            break;
        case VA_ItemTypes::HTTP_URL:
            ss.first.insert ( "@HTTP_URI_URL" );
            break;
        case VA_ItemTypes::HTTP_URI:
            ss.first.insert ( "@HTTP_URI_URL" );
            ss.first.insert ( "@HTTP_URI_DATA" );
            break;
        case VA_ItemTypes::HTTP_URL_HOST:
            ss.first.insert ( "@HTTP_URI_HOST" );
            break;
        case VA_ItemTypes::SA_OS:
            ss.first.insert ( "@OPERATIVE_SYSTEM" );
            break;
        case VA_ItemTypes::HTTP_HEADER_HOST:
            ss.first.insert ( "@HTTP_HOST" );
            break;

        case VA_ItemTypes::HTTP_ANSWER_DATA:
        case VA_ItemTypes::HTTP_DATA_POST: {
            //ss.first.insert("@HTTP_DATA_"+it.val1);
            if ( it.val1.compare ( "0_FULL_QUERY" ) == 0 ) {
                ss.first.insert ( "@HTTP_DATA" );
            } else {
                ss.first.insert ( "@HTTP_DATA_" + it.val1 );
            }
        }
        break;
        case VA_ItemTypes::HTTP_HEADERS:
            ss.first.insert ( "@HTTP_" + it.val1 );
            break;
        //case VA_ItemTypes::HTTP_QUERY: ss.first.insert("@HTTP_URI_DATA_"+it.val1); break;
        case VA_ItemTypes::HTTP_QUERY: {
            if ( it.val1.compare ( "0_FULL_QUERY" ) == 0 ) {
                ss.first.insert ( "@HTTP_URI_DATA" );
            } else {
                ss.first.insert ( "@HTTP_URI_DATA_" + it.val1 );
            }
        }
        break;
        default:
            cout << __func__ << " - " << __LINE__ << endl;
        }

        for ( auto jt : it.vrep ) {
            switch ( get<0>(jt) ) {
            case VA_ItemTypes::HTTP_ANSWER_CODE:
                ss.second.insert ( "@HTTP_RESPONSE_CODE" );
                break;
            case VA_ItemTypes::HTTP_ANSWER_COOKIES:
                ss.second.insert ( "@HTTP_Set-Cookie" );
                break;
            case VA_ItemTypes::HTTP_COOKIES:
                ss.first.insert ( "@HTTP_Cookie" );
                break;
            case VA_ItemTypes::HTTP_METHOD:
                ss.first.insert ( "@HTTP_METHOD" );
                break;
            case VA_ItemTypes::HTTP_URL:
                ss.first.insert ( "@HTTP_URI_URL" );
                break;
            case VA_ItemTypes::HTTP_URI:
                ss.first.insert ( "@HTTP_URI_URL" );
                ss.first.insert ( "@HTTP_URI_DATA" );
                break;
            case VA_ItemTypes::HTTP_URL_HOST:
                ss.first.insert ( "@HTTP_URI_HOST" );
                break;
            case VA_ItemTypes::SA_OS:
                ss.first.insert ( "@OPERATIVE_SYSTEM" );
                break;
            case VA_ItemTypes::HTTP_HEADER_HOST:
                ss.first.insert ( "@HTTP_HOST" );
                break;

            case VA_ItemTypes::HTTP_ANSWER_DATA:
            case VA_ItemTypes::HTTP_DATA_POST: {
                //ss.first.insert("@HTTP_DATA_"+jt.second);
                if ( get<1>(jt).compare ( "0_FULL_QUERY" ) == 0 ) {
                    ss.first.insert ( "@HTTP_DATA" );
                } else {
                    ss.first.insert ( "@HTTP_DATA_" + get<1>(jt) );
                }
            }
            break;
            case VA_ItemTypes::HTTP_HEADERS:
                ss.first.insert ( "@HTTP_" + get<1>(jt) );
                break;
            //case VA_ItemTypes::HTTP_QUERY: ss.first.insert("@HTTP_URI_DATA_"+jt.second); break;
            case VA_ItemTypes::HTTP_QUERY: {
                if ( get<1>(jt).compare ( "0_FULL_QUERY" ) == 0 ) {
                    ss.first.insert ( "@HTTP_URI_DATA" );
                } else {
                    ss.first.insert ( "@HTTP_URI_DATA_" + get<1>(jt) );
                }
            }
            break;
            default:
                cout << __func__ << " - " << __LINE__ << endl;
            }
        }
    }

    for ( auto it : r->with ) {
        switch ( it.itype ) {
        case VA_ItemTypes::HTTP_ANSWER_CODE:
            ss.second.insert ( "@HTTP_RESPONSE_CODE" );
            break;
        case VA_ItemTypes::HTTP_ANSWER_COOKIES:
            ss.second.insert ( "@HTTP_Set-Cookie" );
            break;
        case VA_ItemTypes::HTTP_COOKIES:
            ss.first.insert ( "@HTTP_Cookie" );
            break;
        case VA_ItemTypes::HTTP_METHOD:
            ss.first.insert ( "@HTTP_METHOD" );
            break;
        case VA_ItemTypes::HTTP_URL:
            ss.first.insert ( "@HTTP_URI_URL" );
            break;
        case VA_ItemTypes::HTTP_URI:
            ss.first.insert ( "@HTTP_URI_URL" );
            ss.first.insert ( "@HTTP_URI_DATA" );
            break;
        case VA_ItemTypes::HTTP_URL_HOST:
            ss.first.insert ( "@HTTP_URI_HOST" );
            break;
        case VA_ItemTypes::SA_OS:
            ss.first.insert ( "@OPERATIVE_SYSTEM" );
            break;
        case VA_ItemTypes::HTTP_HEADER_HOST:
            ss.first.insert ( "@HTTP_HOST" );
            break;

        case VA_ItemTypes::HTTP_ANSWER_DATA:
        case VA_ItemTypes::HTTP_DATA_POST: {
            //ss.first.insert("@HTTP_DATA_"+it.op1);
            if ( it.op1.compare ( "0_FULL_QUERY" ) == 0 ) {
                ss.first.insert ( "@HTTP_DATA" );
            } else {
                ss.first.insert ( "@HTTP_DATA_" + it.op1 );
            }
        }
        break;
        case VA_ItemTypes::HTTP_HEADERS:
            ss.first.insert ( "@HTTP_" + it.op1 );
            break;
        //case VA_ItemTypes::HTTP_QUERY: ss.first.insert("@HTTP_URI_DATA_"+it.op1); break;
        case VA_ItemTypes::HTTP_QUERY: {
            if ( it.op1.compare ( "0_FULL_QUERY" ) == 0 ) {
                ss.first.insert ( "@HTTP_URI_DATA" );
            } else {
                ss.first.insert ( "@HTTP_URI_DATA_" + it.op1 );
            }
        }
        break;
        default:
            cout << __func__ << " - " << __LINE__ << endl;
        }
    }


    for ( int i = 0; i < r->children.size(); ++i ) {
        pair<set<string>, set < string>> ss1 = GetOther ( vaeng, r->children[i] );
        ss.first.insert ( ss1.first.begin(), ss1.first.end() );
        ss.second.insert ( ss1.second.begin(), ss1.second.end() );
    }


    return ss;
}

set<string> BuildHostsListFromSetsAndHosts ( VAengine &vaeng )
{
    set<string> hosts;
    for ( auto & vr : vaeng.varules ) {
        set<string> ss = GetHosts ( vaeng, vr.second.root );

        for ( auto it = ss.begin(); it != ss.end(); ++it ) {
            hosts.insert ( *it );
        }
        //hosts.insert(ss.begin(), ss.end());
    }

    return hosts;
}

pair<set<string>, set<string>> BuildOtherListFromCriteria ( VAengine &vaeng )
{
    pair<set<string>, set < string>> other;
    for ( auto & vr : vaeng.varules ) {
        pair<set<string>, set < string>> ss = GetOther ( vaeng, vr.second.root );
        other.first.insert ( ss.first.begin(), ss.first.end() );
        other.second.insert ( ss.second.begin(), ss.second.end() );
    }

    return other;
}

AccessRule::AccessRule()
{
    enabled = false;
    //auto tmp = time(nullptr); start_time=tmp - tmp % 60;
}

AccessRule::~AccessRule()
{
}

AccessRule::AccessRule ( const AccessRule&oth )
{
    enabled = oth.enabled;
    name = oth.name;
    basic_access_criteria = oth.basic_access_criteria;
    ar_vars = oth.ar_vars;
    access_operations = oth.access_operations;
}






/* Strutture e funzioni per astparsing */

