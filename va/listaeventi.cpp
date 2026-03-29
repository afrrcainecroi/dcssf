#include <iostream>
#include <sstream>

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/timeb.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif

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
#include <algorithm>
#include <functional>
#include <mutex>

#include <mtfa_utils.h>
#include <mutex>
#include <thread>
#include "listaeventi.h"
#include "version.h"
#include "lb_user.h"

using namespace std;

//Per indicare che la documentazione viene generata da altre sezioni del sw
C_ListaEventi::C_ListaEventi()
{
     mutexListaEventi = new mutex();
     lEventi = new T_ListaEventi();
     toBeFinished = false;
     finished = false;
}

C_ListaEventi::~C_ListaEventi()
{
     toBeFinished = true;
     delete mutexListaEventi;
     delete lEventi;
}

bool C_ListaEventi::AddEvento ( T_EventoStruct * evento )
{
     std::lock_guard<std::mutex> lock ( *mutexListaEventi );
     try {
          lEventi->push_back ( evento );
     } catch ( std::exception ex ) {
	mtfa_log("%s\n", ex.what());
     }
     return true;
}

//int local_conta_eliminati;
//class isToBeFreed
//{
//public:
//	bool operator() (T_EventoStruct value)
//	{
//            cout << __func__   << value.buf_out_freed << " - " << value.buf_in_freed << endl;
//            if (value.buf_out_freed && value.buf_in_freed)
//                local_conta_eliminati++;
//            return (value.buf_out_freed && value.buf_in_freed);
//            //return true;
///*
//		//std::cout << __func__   << "ANALIZZA UNIQUEID: " << value->UniqueId << "\n";
//		if (value->buf_in_freed && value->buf_out_freed)
//		{
//			if (value->second_selection==false)
//			{
//				//la prossima volta far\ufffd l'analisi!
//				value->second_selection = true;
//				return false;
//			}
//			else
//			{
//				//Se sono io come gestore, la comunicazione no la stampa
//				if (false && (value->PerComunicazioniAlbero==false))
//				{
//					//Questo lo posso calcolare come somma delle differenze di tempo tra gli elementi cs e sc.
//					unsigned long long trans = value->closed - value->accepted_client;
//					unsigned long long shorttrans = value->last_received_from_server - value->connected_to_server;
//					unsigned long long shorttrans_1 = value->first_received_from_server - value->connected_to_server;
//					unsigned long long shorttrans_2 = value->first_received_from_server - value->first_sent_to_server;
//
//					try
//					{
//						struct tm today;
//						time_t ltime;
//						time( &ltime );
//		#ifndef _WIN32
//						localtime_r( &ltime, &today);
//		#else
//						localtime_s(&today, &ltime);
//		#endif
//						char abstime[64];
//#ifndef _WIN32
//						ctime_r(&ltime, abstime);
//#else
//						ctime_s(abstime, 63, &ltime);
//#endif
//
//						//unsigned long long answtime = value->last_received_from_server - value->first_received_from_client;
//						std::string nomeFile;
//						std::ostringstream stmp;
//						stmp 	<< today.tm_year << "."
//							<< today.tm_mon << "."
//							<< today.tm_mday << "."
//							<< today.tm_hour << ".log";
//						nomeFile = stmp.str();
//						std::ostringstream sout;
//
// //DOCUF
//						sout  <<
//						"ABSTIME : " << abstime <<
//						"UNIQUEID: " << value->UniqueId << "\n" <<
//						//"CONFORME: " << (char*)((value->postazioneConforme==1)?"NON CONFORME":((value->postazioneConforme==0)?"CON BOUNDARY":"REGISTRATA")) << "\n" <<
//						"ERROR   : " << value->server_error << " - " << value->client_error << "\n" <<
//						"TRANSTIM: " << "CLS-ACPD: " << trans/1000000          << "." << trans        %1000000   << " "
//												 << "CON-LRCV: " << shorttrans/1000000     << "." << shorttrans   %1000000   << " "
//												 << "CON-FRCV: " << shorttrans_1/1000000   << "." << shorttrans_1 %1000000   << " "
//												 << "FST-FRCD: " << shorttrans_2/1000000   << "." << shorttrans_2 %1000000   << "\n" <<
//						//"ANSWTIME: " << answtime / 1000000 << " sec. " << answtime % 1000000 << " microsecs.\n" <<
//						"PROCTIME: " << value->processing_time / 1000000 << " sec. " << value->processing_time % 1000000 << " microsecs.\n" <<
//						"IP_SOURC: " << value->client_ip  << ":" << value->client_port << "\n" <<
//						"IP_LISTN: " << value->listen_ip  << ":" << value->listen_port << "\n" <<
//						"IP_BINDG: " << value->bind_ip  << "\n" <<
//						"USER_AGE: " << value->user_agent << "\n"  <<
//						"HOST    : " << value->host << "\n" <<
//						"URLS    : " << value->url.substr(0,128) << "\n" <<
//						"COOKIES : " << "\n";
//						for (std::list<std::string>::iterator i = value->l_cookie_cs.begin(); i != value->l_cookie_cs.end(); ++i)
//						{
//							sout << "\t" << (*i).substr(0,128) << "\n";
//						}
//						sout << "\n" <<
//						"ANSWER  : " << value->answer << "\n" <<
//						"USERNMSI: " << value->usernameAreaRiservata << "\n" <<
//						"DATAINPT: " << value->data_in << "\n" <<
//						"DATAOUTP: " << value->data_out << "\n\n";
//						FILE * fou = fopen(nomeFile.c_str(), "a");
//						if (fou != 0)
//						{
//							fprintf(fou, "%s", sout.str().c_str());
//							fclose(fou);
//						}
//						else
//						{
//							std::cerr << "Unable to print on file: " << sout.str();
//						}
//					}
//					catch (std::exception ex)
//					{
//						std::cerr << ex.what() << "\n";
//					}
//				}
//				delete value;
//				return true;
//			}
//		}
//		else
//			return false;
// */
//	}
//};

void WriteString ( int numf, string s )
{
     int len = s.size();
     WriteAll ( numf, ( unsigned char* ) &len, sizeof ( len ) );
     WriteAll ( numf, ( unsigned char* ) s.data(), len );
}

//extern set<int> open_sockets;
//extern mutex open_sockets_mutex;



//Questo è un thread
void C_ListaEventi::PurgeEvents ( void * args )
{
     C_ListaEventi * p = ( C_ListaEventi* ) args;

     while ( !p->toBeFinished ) {
#ifdef WIN32
          Sleep ( p->timeout * 1000 );
#else
          usleep ( p->timeout * 1000000 );
#endif
          long count = p->lEventi->size();
          p->lEventi->remove_if ( [&count] ( T_EventoStruct * value ) -> bool {
               //Qui ci entro solo se il mutex me lo consente
               mutexSetEventoStructAllocati.lock();
               if ( SetEventoStructAllocati.find((void*)value) != SetEventoStructAllocati.end() &&
                    value && value->buf_in_freed && value->buf_out_freed)
               {
                    mutexSetEventoStructAllocati.unlock();
                    //Qui stampo tutto!
#if EIS_LEVEL & EIS_LEVEL_4
                    extern string LbUser_DataAnalytics; //Il nome del file in cui scrivere i data analytics
                    if ( LbUser_DataAnalytics.size() > 0 ) {
                         FILE * fou = fopen ( LbUser_DataAnalytics.c_str(), "a" );
                         if ( fou != nullptr ) {
                              WriteAll ( fileno ( fou ), ( unsigned char* ) &value->UniqueId, sizeof ( value->UniqueId ) );
                              long long now = GetULLTime();
                              WriteAll ( fileno ( fou ), ( unsigned char* ) &now, sizeof ( now ) );
                              WriteString ( fileno ( fou ), value->header_cs );
                              WriteString ( fileno ( fou ), value->header_sc );
                              WriteAll ( fileno ( fou ), ( unsigned char* ) &value->toBeConnected, sizeof ( value->toBeConnected ) );
                              WriteAll ( fileno ( fou ), ( unsigned char* ) &value->processing_time, sizeof ( value->processing_time ) );
                              WriteAll ( fileno ( fou ), ( unsigned char* ) &value->accepted_client, sizeof ( value->accepted_client ) );
                              WriteAll ( fileno ( fou ), ( unsigned char* ) &value->connected_to_server, sizeof ( value->connected_to_server ) );
                              WriteAll ( fileno ( fou ), ( unsigned char* ) &value->first_received_from_client, sizeof ( value->first_received_from_client ) );
                              WriteAll ( fileno ( fou ), ( unsigned char* ) &value->first_received_from_server, sizeof ( value->first_received_from_server ) );
                              WriteAll ( fileno ( fou ), ( unsigned char* ) &value->first_sent_to_server, sizeof ( value->first_sent_to_server ) );
                              WriteAll ( fileno ( fou ), ( unsigned char* ) &value->last_received_from_server, sizeof ( value->last_received_from_server ) );
                              WriteAll ( fileno ( fou ), ( unsigned char* ) &value->closed, sizeof ( value->closed ) );
                              WriteString ( fileno ( fou ), value->listen_ip );
                              WriteAll ( fileno ( fou ), ( unsigned char* ) &value->listen_port, sizeof ( value->listen_port ) );
                              WriteString ( fileno ( fou ), value->client_ip );
                              WriteAll ( fileno ( fou ), ( unsigned char* ) &value->client_port, sizeof ( value->client_port ) );
                              WriteString ( fileno ( fou ), value->server_ip );
                              WriteAll ( fileno ( fou ), ( unsigned char* ) &value->server_port, sizeof ( value->server_port ) );
                              WriteString ( fileno ( fou ), value->bind_ip );
                              WriteAll ( fileno ( fou ), ( unsigned char* ) &value->data_in, sizeof ( value->data_in ) );
                              WriteAll ( fileno ( fou ), ( unsigned char* ) &value->data_out, sizeof ( value->data_out ) );
                              fclose ( fou );
                         }
                    }
#endif
                    //extern unordered_map<unsigned long long, pair<unsigned long long, unsigned long long>> aperti_chiusi;

                    //aperti_chiusi.erase((unsigned long long)value);
//                        if (value)
//                            socket_closed.erase((unsigned long long)value->p_buf_in);
//                        if (value)
//                            socket_closed.erase((unsigned long long)value->p_buf_out);
                    if (value->ssl_ctx)
                    {
                        SSL_CTX_free(value->ssl_ctx);
                        value->ssl_ctx=nullptr;
                    }

                    delete value;
                    value = 0;
                    return true;
               } else {
                    mutexSetEventoStructAllocati.unlock();
                    return false; }
          } );

//                try
//                {
//                    int numev = p->lEventi->size();
//                    if (numev)
//                    {
//                        static int old0=1;
//                        static int old1=2;
//                        open_sockets_mutex.lock();
//                        cout << __func__   << "left to be deleted: " << numev << " - " << open_sockets.size() << endl;
//                        open_sockets_mutex.lock();
//                        if (numev == old0)
//                        {
//                            if (numev == old1)
//                            {
//                                for (auto it: *p->lEventi)
//                                {
//                                    string GetLocalIpPort(int fd);
//                                    string GetRemoteIpPort(int fd);
//
//                                    if (it->p_buf_in)
//                                    {
//                                        int s = bufferevent_getfd(it->p_buf_in);
//                                        if (s > 0)
//                                        {
//                                            string ipp = GetRemoteIpPort(s);
//                                            cout << __func__ << " - " << StrTime() << ": " << "IN : " << GetRemoteIpPort(s) << " -> " << GetLocalIpPort(s) << ". " << (it->buf_in_freed?"FR":"OP") <<":"<<(it->buf_out_freed?"FR":"OP") << endl;
//                                        }
//                                    }
//                                    if (it->p_buf_out)
//                                    {
//                                        int s = bufferevent_getfd(it->p_buf_out);
//                                        if (s > 0)
//                                        {
//                                            string ipp = GetLocalIpPort(s);
//                                            cout << __func__ << " - " << StrTime() << ": " << "OUT: " << GetRemoteIpPort(s) << " -> " << GetLocalIpPort(s) << ". " << (it->buf_in_freed?"FR":"OP") <<":"<<(it->buf_out_freed?"FR":"OP") << endl;
//                                        }
//                                    }
//                                }
//                            }
//                            else
//                            {
//                                old1 = numev;
//                            }
//                        }
//                        else
//                        {
//                            old0 = numev;
//                        }
//                    }
//                }
//                catch (exception ex)
//                {
//                    cout << __func__ << " - " << StrTime() << ": " << "exception " << ex.what() << endl;
//                }
     }

     p->finished = true;
}

void C_ListaEventi::StartPurge ( int secs )
{
     //Fa partire un thread che elimina gli eventi marcati come "freed", scandendo ogni secs secondi la lista
     timeout = secs;
//	#ifdef _WIN32
//	unsigned int ppp;
//	#else
//	pthread_t ppp;
//	#endif
     thread t ( PurgeEvents, ( void* ) this );
     t.detach();
     //boost::thread t(PurgeEvents, (void*)this);
}

void C_ListaEventi::StopPurge()
{
     toBeFinished = true;
}

int C_ListaEventi::Size()
{
     return lEventi->size();
}
