#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>
#endif

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>

#ifndef _WIN32
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <signal.h>
#endif

#include <event2/buffer.h>
#include <event2/bufferevent.h>
#include <event2/dns.h>
#include <event2/util.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/thread.h>
#include <unordered_map>
#include <unordered_set>
#include <mtfa_utils.h>
#include <mtfa_find.h>
#include <mtfafs.h>
#include <base64.h>
#include "VAengine.h"
#include "rulemgr.h"

//#include <crc.h>
#include "ConfigFile.h"

#include <string>
#include <list>
#include <thread>

#include <stdlib.h>
#include "libguile.h"

//La gestione del multithreading con openssl
#include <openssl/crypto.h> //In addition to other ssl headers
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "VA_utils.h"

#include <NumberTheory.h>
#include <json.hpp>
using json = nlohmann::json;

using namespace std;



/****************SETUP DEI THREADS PER LIBEVENT E OPENSSL */
#define MUTEX_TYPE pthread_mutex_t
#define MUTEX_SETUP(x) pthread_mutex_init(&(x), NULL)
#define MUTEX_CLEANUP(x) pthread_mutex_destroy(&(x))
#define MUTEX_LOCK(x) pthread_mutex_lock(&(x))
#define MUTEX_UNLOCK(x) pthread_mutex_unlock(&(x))
#define THREAD_ID pthread_self( )

/* This array will store all of the mutexes available to OpenSSL. */
static MUTEX_TYPE *mutex_buf = NULL;
static void locking_function(int mode, int n, const char * file, int line)
{
    if (mode & CRYPTO_LOCK)
        MUTEX_LOCK(mutex_buf[n]);
    else
        MUTEX_UNLOCK(mutex_buf[n]);
}
static unsigned long id_function(void)
{
    return ((unsigned long)THREAD_ID);
}
struct CRYPTO_dynlock_value
{
    MUTEX_TYPE mutex;
};
static struct CRYPTO_dynlock_value * dyn_create_function(const char *file,
        int line)
{
    struct CRYPTO_dynlock_value *value;
    value = (struct CRYPTO_dynlock_value *)malloc(sizeof(
                struct CRYPTO_dynlock_value));
    if (!value)
        return NULL;
    MUTEX_SETUP(value->mutex);
    return value;
}
static void dyn_lock_function(int mode, struct CRYPTO_dynlock_value *l,
                              const char *file, int line)
{
    if (mode & CRYPTO_LOCK)
        MUTEX_LOCK(l->mutex);
    else
        MUTEX_UNLOCK(l->mutex);
}
static void dyn_destroy_function(struct CRYPTO_dynlock_value *l,
                                 const char *file, int line)
{
    MUTEX_CLEANUP(l->mutex);
    free(l);
}


int THREAD_setup(void)
{
    int i;
    mutex_buf = (MUTEX_TYPE *)malloc(CRYPTO_num_locks( ) * sizeof(MUTEX_TYPE));
    if (!mutex_buf)
        return 0;
    for (i = 0; i < CRYPTO_num_locks( ); i++)
        MUTEX_SETUP(mutex_buf[i]);
    CRYPTO_set_id_callback(id_function);
    CRYPTO_set_locking_callback(locking_function);
    /* The following three CRYPTO_... functions are the OpenSSL functions
       for registering the callbacks we implemented above */
    CRYPTO_set_dynlock_create_callback(dyn_create_function);
    CRYPTO_set_dynlock_lock_callback(dyn_lock_function);
    CRYPTO_set_dynlock_destroy_callback(dyn_destroy_function);
    return 1;
}
int THREAD_cleanup(void)
{
    int i;
    if (!mutex_buf)
        return 0;
    CRYPTO_set_id_callback(NULL);
    CRYPTO_set_locking_callback(NULL);
    CRYPTO_set_dynlock_create_callback(NULL);
    CRYPTO_set_dynlock_lock_callback(NULL);
    CRYPTO_set_dynlock_destroy_callback(NULL);
    for (i = 0; i < CRYPTO_num_locks( ); i++)
        MUTEX_CLEANUP(mutex_buf[i]);
    free(mutex_buf);
    mutex_buf = NULL;
    return 1;
}

/*
// we have this global to let the callback get easy access to it
static pthread_mutex_t *lockarray;

std::string FromHostToIp ( const std::string hostName );
unsigned long long GetMicroSec();
void killServer ( void );

static void lock_callback ( int mode, int type, char *file, int line )
{
     ( void ) file;
     ( void ) line;
     if ( mode & CRYPTO_LOCK ) {
          pthread_mutex_lock ( & ( lockarray[type] ) );
     } else {
          pthread_mutex_unlock ( & ( lockarray[type] ) );
     }
}

static unsigned long thread_id ( void )
{
     unsigned long ret;

     ret= ( unsigned long ) pthread_self();
     return ( ret );
}

static void init_locks ( void )
{
     int i;

     lockarray= ( pthread_mutex_t * ) OPENSSL_malloc ( CRYPTO_num_locks() * sizeof ( pthread_mutex_t ) );
     for ( i=0; i<CRYPTO_num_locks(); i++ ) {
          pthread_mutex_init ( & ( lockarray[i] ),NULL );
     }

     CRYPTO_set_id_callback ( ( unsigned long ( * ) () ) thread_id );
     CRYPTO_set_locking_callback ( ( void ( * ) ( int, int, const char*, int ) ) lock_callback );
}

static void kill_locks ( void )
{
     int i;

     CRYPTO_set_locking_callback ( NULL );
     for ( i=0; i<CRYPTO_num_locks(); i++ ) {
          pthread_mutex_destroy ( & ( lockarray[i] ) );
     }

     OPENSSL_free ( lockarray );
}
*/
//END SETUP OPENSSL MULTITHREADING

std::string FromHostToIp ( const std::string hostName )
{
    std::string returnVal;
    char temp[256];

    const hostent* host_info = gethostbyname ( hostName.c_str() );

    if ( host_info ) {
        const in_addr* address = ( in_addr* ) host_info->h_addr_list[0];
        memset ( temp, 0, sizeof ( temp ) );
        strcpy ( temp, inet_ntoa ( *address ) );
    }

    returnVal = temp;
    return returnVal;
}

unsigned long long GetMicroSec()
{
    struct timeval tp;
    evutil_gettimeofday ( &tp, 0 );
    unsigned long long msecs = tp.tv_sec * 1000000 + tp.tv_usec;
    return msecs;
}

ConfigFile configurazione;

char * terzo_parametro_linea_comando;


void ManageLBUserArgs ( void * vconf );

ULL_Rng * glb_rng;
SimpleRng * simple_rng;

void killServer ( void )
{
    fprintf ( stdout, "Stopping socket listener event loop.\n" );
    fprintf ( stdout, "Stopping workers.\n" );
    exit ( 0 ); //da pulire
}

static char ** theArgv;
static char * current_dir = nullptr;
static void sighandler ( int signal )
{
    if ( signal == SIGPIPE ) {
        fprintf ( stdout, "Received signal %d: %s.  Ignoring it (write on a closed socket)\n", signal, strsignal ( signal ) );
    } else if ( signal == SIGUSR1 ) {
        fprintf ( stdout, "Received signal %d: %s.  Restarting.\n", signal, strsignal ( signal ) );
        {
            sigset_t sigs;
            sigprocmask(0, 0, &sigs);
            sigdelset(&sigs, SIGUSR1);
            sigprocmask(SIG_SETMASK, &sigs, 0);
        }

        chdir ( current_dir );
        execv ( theArgv[0], theArgv );
    } else {
        fprintf ( stdout, "Received signal %d: %s.  Shutting down.\n", signal, strsignal ( signal ) );
        killServer();
    }
}

int number_of_arguments;
#include "VAengine.h"

#include <mtfa_find.h>

// bool mtfa_find_patternnoijt ( const string& subject, pcre2_code* re );
//
// #include <type_traits>
// #include <shared_mutex>
//
// /*Strutture concorrenti*/
// template<class T>
// struct synchronized {
//   // one could argue that rvalue ref qualified version should not be
//   // synchronized...  but I think that is wrong
//   template<class F>
//   result_of_t< F(T const&) > read( F&& f ) const {
//     auto&& lock = read_lock();
//     return forward<F>(f)(t);
//   }
//   template<class F>
//   result_of_t< F(T&) > write( F&& f ) {
//     auto&& lock = write_lock();
//     return forward<F>(f)(t);
//   }
//   // common operations, useful rvalue/lvalue overloads:
//   // get a copy of the internal guts:
//   T copy() const& { return read([&](auto&&t)->T{return t;}); }
//   T copy() && { return move(); }
//   T move() { return move(*this).write([&](auto&&t)->T{return std::move(t);}); }
// private:
//   mutable std::shared_timed_mutex mutex;
//   std::shared_lock<std::shared_timed_mutex> read_lock() const {
//     return std::shared_lock<std::shared_timed_mutex>(mutex);
//   }
//   std::unique_lock<std::shared_timed_mutex> write_lock() {
//     return std::unique_lock<std::shared_timed_mutex>(mutex);
//   }
//   T t;
// public:
//   // relatively uninteresting boilerplate
//   // ctor:
//   template<class...Args>
//   explicit synchronized( Args&&... args ):
//     t(std::forward<Args>(args)...)
//   {}
//   // copy ctors: (forwarding constructor above means need all 4 overloads)
//   synchronized( synchronized const& o ) :t(std::forward<decltype(o)>(o).copy()) {}
//   synchronized( synchronized const&& o ):t(std::forward<decltype(o)>(o).copy()) {}
//   synchronized( synchronized & o )      :t(std::forward<decltype(o)>(o).copy()) {}
//   synchronized( synchronized && o )     :t(std::forward<decltype(o)>(o).copy()) {}
//   // copy-from-T ctors: (forwarding constructor above means need all 4 overloads)
//   synchronized( T const& o ) :t(std::forward<decltype(o)>(o)) {}
//   synchronized( T const&& o ):t(std::forward<decltype(o)>(o)) {}
//   synchronized( T & o )      :t(std::forward<decltype(o)>(o)) {}
//   synchronized( T && o )     :t(std::forward<decltype(o)>(o)) {}
// };

//#define PROMISES_CHECK 1

#ifdef PROMISES_CHECK
#include <vector>
#include <thread>
#include <future>
#include <numeric>
#include <iostream>
#include <chrono>

void my_accumulate(std::vector<int>::iterator first,
                   std::vector<int>::iterator last,
                   std::promise<int> accumulate_promise);
void do_work(std::promise<void> barrier);
void my_accumulate(std::vector<int>::iterator first,
                   std::vector<int>::iterator last,
                   std::promise<int> accumulate_promise)
{
    int sum = std::accumulate(first, last, 0);
    accumulate_promise.set_value(sum);  // Notify future
}

void do_work(std::promise<void> barrier)
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    cout << "exiting from do_work\n";
    barrier.set_value();
}
#endif

/*
#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
*/

using namespace std;

#if 0
// Condition to stop processing
void println() {}

void pippipippo(const string & s)
{
    cout << "String: " << s << endl;
}
void pippipippo(const char * s)
{
    cout << "CHAR *" << s << endl;
}
void pippipippo(const int &s)
{
    cout << "INT " << s  << endl;
}
void pippipippo(const long int &s)
{
    cout << "LONG INT " << s  << endl;
}
void pippipippo(const long long int &s)
{
    cout <<"LONG LONG INT " << s  << endl;
}
void pippipippo(const char &s)
{
    cout << "CHAR " << s  << endl;
}

// Println receives a list of arguments. We don't know it's type nor
// how many there are, so we just get the head and expand the rest
template <typename H, typename... T>
void println(H p, T... t)
{
    // Do something useful with the head
    pippipippo(p);
    // Expand the rest (pass it recursively to println)
    println(t...);
}
#endif

#include <gc.h>

int main ( int argc, char *argv[] )
{
    if ( argc==3 ) {
        VAengine vaeng;
        if ( strcmp ( argv[1], "compile" ) == 0 ) {
            DoCompileRules ( argv[2], "NA", vaeng );
            exit ( 0 );
        }
    }
    if (argc == 3 && strcmp(argv[1], "generate")==0)
    {
        //Genera una configurazione basica. Prende come parametri ip, porta di ascolto, nome config
        //genera quindi .conf .ars e .scm e .json, definite come variabili in questo file
#include "simpleconfiguration.h"
        
        string fconf = mtfa_replace_matching_pattern(conf_file, "<<prefix>>", argv[2], true);
        string fars  = mtfa_replace_matching_pattern(ars_file, "<<prefix>>", argv[2], true);
        string fscm  = mtfa_replace_matching_pattern(scm_file, "<<prefix>>", argv[2], true);
        string fjson = mtfa_replace_matching_pattern(json_file, "<<prefix>>", argv[2], true);
        
        const string nfconf = string(argv[2]) + ".conf";
        const string nfars  = string(argv[2]) + ".ars";
        const string nfscm  = string(argv[2]) + ".scm";
        const string nfjson = string(argv[2]) + ".json";
        
        mtfa_FileIo::Write(nfconf, fconf, true);
        mtfa_FileIo::Write(nfars, fars, true);
        mtfa_FileIo::Write(nfscm, fscm, true);
        mtfa_FileIo::Write(nfjson, fjson, true);
        exit ( 0 );
    }
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " compile <ars file>" << endl;
        cerr << "Usage: " << argv[0] << " generate <prefix>" << endl;
        cerr << "Usage: " << argv[0] << " <file conf>" << endl;
        exit(0);
    }

    theArgv = argv;
    current_dir = get_current_dir_name();

    number_of_arguments = argc;
    glb_rng = new ULL_Rng ( Ticks::get_micro() );
    simple_rng = new SimpleRng ( glb_rng->Next() );

    /* Set signal handlers */
    sigset_t sigset;
    sigemptyset ( &sigset );
    struct sigaction siginfo;
    siginfo.sa_handler = sighandler;
    siginfo.sa_mask = sigset;
    siginfo.sa_flags = SA_NODEFER | SA_RESTART;

    sigaction ( SIGPIPE, &siginfo, NULL );
    sigaction ( SIGINT, &siginfo, NULL );
    sigaction ( SIGTERM, &siginfo, NULL );
    sigaction ( SIGUSR1, &siginfo, NULL );

    printf ( "Started\nTo restart use: pkill -e -SIGUSR1 %s\n", argv[0] ); //A questo punto ci siamo, abbiamo un lsp attivo!

    //inizializza openssl locks
    THREAD_setup();
    //init_locks();

    //in ogni caso inizializza SSL
    SSL_library_init();
    SSL_load_error_strings();
    ERR_load_crypto_strings();
    ERR_load_BIO_strings();
    OpenSSL_add_ssl_algorithms();
    OpenSSL_add_all_algorithms();
    OpenSSL_add_all_digests();

    //inizializza uso dei pool di libevent
    evthread_use_pthreads();



    if ( argc < 2 ) {
        std::cerr << "Usage: " << argv[0] << " config_file" << std::endl;
        exit ( -1 );
    }
    configurazione = ConfigFile ( argv[1] );

    if ( argc >= 3 ) {
        terzo_parametro_linea_comando = new char[1024];
        strcpy ( terzo_parametro_linea_comando, argv[2] );
    } else {
        terzo_parametro_linea_comando = nullptr;
    }

    //Installa i signal handlers...o elimina segnali interrompenti...
#ifndef _WIN32
    signal ( SIGPIPE, SIG_IGN );
#endif

#ifdef _WIN32
    {
        //INIZIANO I SOCKET
        WORD wVersionRequested;
        WSADATA wsaData;
        int err;

        wVersionRequested = MAKEWORD ( 2, 2 );

        err = WSAStartup ( wVersionRequested, &wsaData );
        if ( err != 0 ) {
            /* Tell the user that we could not find a usable */
            /* WinSock DLL.                                  */
            return 1;
        }

        /* Confirm that the WinSock DLL supports 2.2.*/
        /* Note that if the DLL supports versions greater    */
        /* than 2.2 in addition to 2.2, it will still return */
        /* 2.2 in wVersion since that is the version we      */
        /* requested.                                        */

        if ( LOBYTE ( wsaData.wVersion ) != 2 ||
                HIBYTE ( wsaData.wVersion ) != 2 ) {
            /* Tell the user that we could not find a usable */
            /* WinSock DLL.                                  */
            WSACleanup();
            return 1;
        }

        /* The WinSock DLL is acceptable. Proceed. */
    }
#endif

    ManageLBUserArgs((void*)&configurazione);

    while ( true ) {
//         int mtfa_trunc_string( int from, int to);
//         mtfa_trunc_string(10, 20);
        sleep ( 60 );
    }

#ifdef _WIN32
    WSACleanup();
#endif

    //Termina openssl locks
    THREAD_cleanup();
    //kill_locks();

    return 0;
}

