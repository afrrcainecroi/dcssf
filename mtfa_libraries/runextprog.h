#pragma once

#ifdef _WIN32

#include <windows.h>
#include <strsafe.h>
#include <stdio.h>
#include <ULL_Rng.h>
#include <NumberTheory.h>
#include <string>

using namespace std;

class RunExternalProcess
{
private:
#define BUFSIZE 4096
    HANDLE std_out_pipe_wr;
    HANDLE std_out_pipe_rd;
    HANDLE std_err_pipe_wr;
    HANDLE std_err_pipe_rd;
    HANDLE std_in_pipe_wr;
    HANDLE std_in_pipe_rd;

    int CreateChildProcess ( const string prog,
                             const string in_parameters );

    int WriteToPipe ( const string commands );

    string ReadFromPipe ( int ms_timeout );

public:
    RunExternalProcess()
    {
        std_out_pipe_wr = nullptr;
        std_out_pipe_rd = nullptr;
        std_err_pipe_wr = nullptr;
        std_err_pipe_rd = nullptr;
        std_in_pipe_wr  = nullptr;
        std_in_pipe_rd  = nullptr;
    }

    ~RunExternalProcess()
    {
#ifdef _WIN32
        if ( nullptr != std_out_pipe_wr ) {
            CloseHandle ( std_out_pipe_wr );
        }

        if ( nullptr != std_out_pipe_rd ) {
            CloseHandle ( std_out_pipe_rd );
        }

        if ( nullptr != std_err_pipe_wr ) {
            CloseHandle ( std_err_pipe_wr );
        }

        if ( nullptr != std_err_pipe_rd ) {
            CloseHandle ( std_err_pipe_rd );
        }

        if ( nullptr != std_in_pipe_wr ) {
            CloseHandle ( std_in_pipe_wr );
        }

        if ( nullptr != std_in_pipe_rd ) {
            CloseHandle ( std_in_pipe_rd );
        }
#else
#endif

        std_out_pipe_wr = nullptr;
        std_out_pipe_rd = nullptr;
        std_err_pipe_wr = nullptr;
        std_err_pipe_rd = nullptr;
        std_in_pipe_wr  = nullptr;
        std_in_pipe_rd  = nullptr;
    }

    int Run ( const string in_prog,
              const string in_parameters,
              const string in_commands,
              string &     sout );
};
#else

#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <paths.h>
#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string>

using namespace std;

#define READ 0
#define WRITE 1
typedef struct __popen_plus_process__ {
    pthread_mutex_t mutex;
    pid_t           pid;
    FILE *          read_fp;
    FILE *          read_err_fp;
    FILE *          write_fp;
} T_popen_plus_process;


T_popen_plus_process * popen_plus ( const char * command );

int popen_plus_close ( T_popen_plus_process * process );

int popen_plus_kill ( T_popen_plus_process * process );

int popen_plus_kill_by_id ( int process_id );

int popen_plus_terminate ( T_popen_plus_process * process );

int popen_plus_terminate_with_id ( int process_id );
#endif

