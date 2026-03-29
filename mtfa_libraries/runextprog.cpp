#include "runextprog.h"

#ifdef _WIN32
int RunExternalProcess::Run ( const string prog,
                              const string parameters,
                              const string commands,
                              string &     sout )
{
     sout.clear();

     SECURITY_ATTRIBUTES saAttr;

     // Set the bInheritHandle flag so pipe handles are inherited.
     saAttr.nLength              = sizeof ( SECURITY_ATTRIBUTES );
     saAttr.bInheritHandle       = TRUE;
     saAttr.lpSecurityDescriptor = NULL;

     // Create a pipe for the child process's STDIN.
     if ( !CreatePipe ( &std_in_pipe_rd, &std_in_pipe_wr, &saAttr, 0 ) ) {
          return -1;    // ErrorExit("Stdin CreatePipe");
     }

     // Ensure the write handle to the pipe for STDIN is not inherited.
     if ( !SetHandleInformation ( std_in_pipe_wr, HANDLE_FLAG_INHERIT, 0 ) ) {
          return -1;    // ErrorExit("Stdin SetHandleInformation");
     }

     if ( !CreatePipe ( &std_out_pipe_rd, &std_out_pipe_wr, &saAttr, 0 ) ) {
          return -1;    // ErrorExit("StdoutRd CreatePipe");
     }

     // Ensure the read handle to the pipe for STDOUT is not inherited.
     if ( !SetHandleInformation ( std_out_pipe_rd, HANDLE_FLAG_INHERIT, 0 ) ) {
          return -1;    // ErrorExit("Stdout SetHandleInformation");
     }

     // Create a pipe for the child process's STDERR.
     if ( !CreatePipe ( &std_err_pipe_rd, &std_err_pipe_wr, &saAttr, 0 ) ) {
          return -1;    // ErrorExit("StderrRd CreatePipe");
     }

     // Ensure the read handle to the pipe for STDERR is not inherited.
     if ( !SetHandleInformation ( std_err_pipe_rd, HANDLE_FLAG_INHERIT, 0 ) ) {
          return -1;    // ErrorExit("StdERR SetHandleInformation");
     }

     // Create the child process.
     int iRet = CreateChildProcess ( prog, parameters );

     if ( iRet != 0 ) {
          return iRet;
     }

     iRet = WriteToPipe ( commands );

     if ( iRet > 0 ) {
          sout = ReadFromPipe ( 2000 );

          return 0;
     } else {
          return iRet;
     }
}

int RunExternalProcess::CreateChildProcess ( const string prog,
          const string                                    parameters )

// Create a child process that uses the previously created pipes for STDIN and STDOUT.
{
     PROCESS_INFORMATION piProcInfo;
     STARTUPINFO         siStartInfo;
     BOOL                bSuccess = FALSE;

     ZeroMemory ( &piProcInfo, sizeof ( PROCESS_INFORMATION ) );

     // Set up members of the STARTUPINFO structure.
     // This structure specifies the STDIN and STDOUT handles for redirection.
     ZeroMemory ( &siStartInfo, sizeof ( STARTUPINFO ) );

     siStartInfo.cb         = sizeof ( STARTUPINFO );
     siStartInfo.hStdError  = std_out_pipe_wr;    // g_hChildStd_OUT_Wr;
     siStartInfo.hStdOutput = std_out_pipe_wr;    // g_hChildStd_OUT_Wr;
     siStartInfo.hStdInput  = std_in_pipe_rd;     // g_hChildStd_IN_Rd;
     siStartInfo.dwFlags    |= STARTF_USESTDHANDLES;

     // Create the child process.
     // std::cout << __func__   << "NOME: <" << prog << ">  PARMS: <" << parameters << ">\n";
     bSuccess = CreateProcess ( ( prog.size() == 0 ) ? NULL : ( ( char * ) prog.c_str() ),           // command line
                                ( parameters.size() == 0 ) ? nullptr : ( ( char * ) parameters.c_str() ), // parameters
                                NULL,                                                                  // process security attributes
                                NULL,                                                                  // primary thread security attributes
                                TRUE,                                                                  // handles are inherited
                                0,                                                                     // creation flags
                                NULL,                                                                  // use parent's environment
                                NULL,                                                                  // use parent's current directory
                                &siStartInfo,                                                          // STARTUPINFO pointer
                                &piProcInfo );   // receives PROCESS_INFORMATION

     if ( !bSuccess ) {
          int iRet = GetLastError();

          CloseHandle ( piProcInfo.hProcess );
          CloseHandle ( piProcInfo.hThread );

          return iRet;
     } else {
          CloseHandle ( std_out_pipe_wr );

          std_out_pipe_wr = nullptr;

          CloseHandle ( std_out_pipe_wr );

          std_out_pipe_wr = nullptr;

          CloseHandle ( std_in_pipe_rd );

          std_in_pipe_rd = nullptr;

          CloseHandle ( piProcInfo.hProcess );
          CloseHandle ( piProcInfo.hThread );

          return 0;
     }
}

int RunExternalProcess::WriteToPipe ( const string commands )
{
     BOOL bSuccess = FALSE;
     int  scritti  = 0;

     while ( scritti < commands.size() ) {
          DWORD dwWritten = 0;

          bSuccess = WriteFile ( std_in_pipe_wr, commands.substr ( scritti ).c_str(), commands.size() - scritti, &dwWritten,
                                 NULL );

          if ( !bSuccess ) {
               int err = GetLastError();

               break;    // Errore!!
          }

          scritti += dwWritten;
     }

     CloseHandle ( std_in_pipe_wr );

     std_in_pipe_wr = nullptr;

     return 1;
}

string RunExternalProcess::ReadFromPipe ( int ms_timeout )
{
     int    letti;
     char * chBuf;
     BOOL   bSuccess = FALSE;
     int    dimBuf   = BUFSIZE;

     chBuf = ( char * ) malloc ( BUFSIZE );
     letti = 0;

     int timing = 0;

     while ( true ) {
          DWORD dim = 0;

          chBuf    = ( char * ) realloc ( chBuf, dimBuf + BUFSIZE );
          dimBuf   += BUFSIZE;
          bSuccess = ReadFile ( std_out_pipe_rd, &chBuf[letti], dimBuf - letti, &dim, nullptr );

          if ( !bSuccess ) {
               break;
          } else {
               letti += dim;
               dim   = 0;
          }
     }

     string sout;

     if ( letti > 0 ) {
          sout.assign ( chBuf, &chBuf[letti] );
          free ( chBuf );

          return sout;
     } else {
          free ( chBuf );

          return sout;
     }
}
#else
T_popen_plus_process * popen_plus ( const char * command )
{
     int                    inpipe[2];
     int                    outpipe[2];
     int                    errpipe[2];
     char *                 argv[4];
     T_popen_plus_process * process = new T_popen_plus_process();

     if ( !process ) {
          goto error_out;
     }

     if ( pipe ( inpipe ) != 0 ) {
          goto clean_process_out;
     }

     if ( pipe ( outpipe ) != 0 ) {
          goto clean_inpipe_out;
     }

     if ( pipe ( errpipe ) != 0 ) {
         goto clean_outpipe_out;
     }

     process->read_fp = fdopen ( outpipe[READ], "r" );
     if ( !process->read_fp ) {
          goto clean_errpipe_out;
     }

     process->read_err_fp = fdopen ( errpipe[READ], "r" );
     if ( !process->read_err_fp ) {
          goto clean_read_fp_out;;
     }

     process->write_fp = fdopen ( inpipe[WRITE], "w" );
     if ( !process->write_fp ) {
          goto clean_read_err_fp_out;
     }

     if ( pthread_mutex_init ( &process -> mutex, NULL ) != 0 ) {
          goto clean_write_fp_out;
     }

     process -> pid = fork();

     if ( process -> pid == -1 ) {
          goto clean_mutex_out;
     }

     if ( process -> pid == 0 ) {
          close ( outpipe[READ] );
          close ( inpipe[WRITE] );
          close ( errpipe[READ] );

          if ( inpipe[READ] != STDIN_FILENO ) {
               dup2 ( inpipe[READ], STDIN_FILENO );
               close ( inpipe[READ] );
          }

          if ( errpipe[READ] != STDERR_FILENO ) {
               dup2 ( errpipe[READ], STDERR_FILENO );
               close ( errpipe[READ] );
          }

          if ( outpipe[WRITE] != STDOUT_FILENO ) {
               dup2 ( outpipe[WRITE], STDOUT_FILENO );
               close ( outpipe[WRITE] );
          }

          argv[0] = ( char* ) "sh";
          argv[1] = ( char* ) "-c";
          argv[2] = ( char * ) command;
          argv[3] = NULL;

          execv ( _PATH_BSHELL, argv );
          exit ( 127 );
     }

     close ( outpipe[WRITE] );
     close ( inpipe[READ] );
     close ( errpipe[READ] );

     return process;
clean_mutex_out:
     pthread_mutex_destroy ( &process -> mutex );
clean_write_fp_out:
     fclose ( process -> write_fp );
clean_read_err_fp_out:
     fclose ( process -> read_err_fp );
clean_read_fp_out:
     fclose ( process -> read_fp );
clean_errpipe_out:
     close ( errpipe[READ] );
     close ( errpipe[WRITE] );
clean_outpipe_out:
     close ( outpipe[READ] );
     close ( outpipe[WRITE] );
clean_inpipe_out:
     close ( inpipe[READ] );
     close ( inpipe[WRITE] );
clean_process_out:
     delete ( process );
error_out:
     return NULL;
}

int popen_plus_close ( T_popen_plus_process * process )
{
     int   pstat;
     pid_t pid;

     /*
      * If someone else destrys this mutex, then this call will fail and we know
      * that another thread already cleaned up the process so we can safely return
      * and since we are destroying this mutex bellow then we don't need to unlock
      * it...
      */
     if ( pthread_mutex_lock ( &process -> mutex ) != 0 ) {
          return 0;
     }

     if ( process -> pid != -1 ) {
          do {
               pid = waitpid ( process -> pid, &pstat, 0 );
          } while ( ( pid == -1 ) && ( errno == EINTR ) );
     }

     if ( process -> read_fp ) {
          fclose ( process -> read_fp );
     }

     if ( process -> read_err_fp ) {
          fclose ( process -> read_err_fp );
     }

     if ( process -> write_fp ) {
          fclose ( process -> write_fp );
     }

     pthread_mutex_destroy ( &process -> mutex );
     delete ( process );

     return ( ( pid == -1 ) ? -1 : pstat );
}

int popen_plus_kill ( T_popen_plus_process * process )
{
     char command[64];

     sprintf ( command, "kill -9 %d", process -> pid );
     int iret = system ( command );

     return 0;
}

int popen_plus_kill_by_id ( int process_id )
{
     char command[64];

     sprintf ( command, "kill -9 %d", process_id );
     int iret = system ( command );

     return 0;
}

int popen_plus_terminate ( T_popen_plus_process * process )
{
     char command[64];

     sprintf ( command, "kill -TERM %d", process -> pid );
     int iret = system ( command );

     return 0;
}

int popen_plus_terminate_with_id ( int process_id )
{
     char command[64];

     sprintf ( command, "kill -TERM %d", process_id );
     int iret = system ( command );

     return 0;
}
#endif                           //WIN32

#if 0

#include <mtfa_baselib.h>
#include "runextprog.h"

int RunExternalProcess::Run ( const string prog,
                              const string parameters,
                              const string commands,
                              string &     sout )
{
     sout.clear();

     SECURITY_ATTRIBUTES saAttr;

     // Set the bInheritHandle flag so pipe handles are inherited.
     saAttr.nLength              = sizeof ( SECURITY_ATTRIBUTES );
     saAttr.bInheritHandle       = TRUE;
     saAttr.lpSecurityDescriptor = NULL;

     // Create a pipe for the child process's STDIN.
     if ( !CreatePipe ( &std_in_pipe_rd, &std_in_pipe_wr, &saAttr, 0 ) ) {
          return -1;    // ErrorExit("Stdin CreatePipe");
     }

     // Ensure the write handle to the pipe for STDIN is not inherited.
     if ( !SetHandleInformation ( std_in_pipe_wr, HANDLE_FLAG_INHERIT, 0 ) ) {
          return -1;    // ErrorExit("Stdin SetHandleInformation");
     }

     if ( !CreatePipe ( &std_out_pipe_rd, &std_out_pipe_wr, &saAttr, 0 ) ) {
          return -1;    // ErrorExit("StdoutRd CreatePipe");
     }

     // Ensure the read handle to the pipe for STDOUT is not inherited.
     if ( !SetHandleInformation ( std_out_pipe_rd, HANDLE_FLAG_INHERIT, 0 ) ) {
          return -1;    // ErrorExit("Stdout SetHandleInformation");
     }

     // Create a pipe for the child process's STDERR.
     if ( !CreatePipe ( &std_err_pipe_rd, &std_err_pipe_wr, &saAttr, 0 ) ) {
          return -1;    // ErrorExit("StderrRd CreatePipe");
     }

     // Ensure the read handle to the pipe for STDERR is not inherited.
     if ( !SetHandleInformation ( std_err_pipe_rd, HANDLE_FLAG_INHERIT, 0 ) ) {
          return -1;    // ErrorExit("StdERR SetHandleInformation");
     }

     // Create the child process.
     int iRet = CreateChildProcess ( prog, parameters );

     if ( iRet != 0 ) {
          return iRet;
     }

     iRet = WriteToPipe ( commands );

     if ( iRet > 0 ) {
          sout = ReadFromPipe ( 2000 );

          return 0;
     } else {
          return iRet;
     }
}

int RunExternalProcess::CreateChildProcess ( const string prog,
          const string                                    parameters )

// Create a child process that uses the previously created pipes for STDIN and STDOUT.
{
     PROCESS_INFORMATION piProcInfo;
     STARTUPINFO         siStartInfo;
     BOOL                bSuccess = FALSE;

     ZeroMemory ( &piProcInfo, sizeof ( PROCESS_INFORMATION ) );

     // Set up members of the STARTUPINFO structure.
     // This structure specifies the STDIN and STDOUT handles for redirection.
     ZeroMemory ( &siStartInfo, sizeof ( STARTUPINFO ) );

     siStartInfo.cb         = sizeof ( STARTUPINFO );
     siStartInfo.hStdError  = std_out_pipe_wr;    // g_hChildStd_OUT_Wr;
     siStartInfo.hStdOutput = std_out_pipe_wr;    // g_hChildStd_OUT_Wr;
     siStartInfo.hStdInput  = std_in_pipe_rd;     // g_hChildStd_IN_Rd;
     siStartInfo.dwFlags    |= STARTF_USESTDHANDLES;

     // Create the child process.
     // std::cout << __func__   << "NOME: <" << prog << ">  PARMS: <" << parameters << ">\n";
     bSuccess = CreateProcess ( ( prog.size() == 0 ) ? NULL : ( ( char * ) prog.c_str() ),           // command line
                                ( parameters.size() == 0 ) ? nullptr : ( ( char * ) parameters.c_str() ), // parameters
                                NULL,                                                                  // process security attributes
                                NULL,                                                                  // primary thread security attributes
                                TRUE,                                                                  // handles are inherited
                                0,                                                                     // creation flags
                                NULL,                                                                  // use parent's environment
                                NULL,                                                                  // use parent's current directory
                                &siStartInfo,                                                          // STARTUPINFO pointer
                                &piProcInfo );   // receives PROCESS_INFORMATION

     if ( !bSuccess ) {
          int iRet = GetLastError();

          CloseHandle ( piProcInfo.hProcess );
          CloseHandle ( piProcInfo.hThread );

          return iRet;
     } else {
          CloseHandle ( std_out_pipe_wr );

          std_out_pipe_wr = nullptr;

          CloseHandle ( std_out_pipe_wr );

          std_out_pipe_wr = nullptr;

          CloseHandle ( std_in_pipe_rd );

          std_in_pipe_rd = nullptr;

          CloseHandle ( piProcInfo.hProcess );
          CloseHandle ( piProcInfo.hThread );

          return 0;
     }
}

int RunExternalProcess::WriteToPipe ( const string commands )
{
     BOOL bSuccess = FALSE;
     int  scritti  = 0;

     while ( scritti < commands.size() ) {
          DWORD dwWritten = 0;

          bSuccess = WriteFile ( std_in_pipe_wr, commands.substr ( scritti ).c_str(), commands.size() - scritti, &dwWritten,
                                 NULL );

          if ( !bSuccess ) {
               int err = GetLastError();

               break;    // Errore!!
          }

          scritti += dwWritten;
     }

     CloseHandle ( std_in_pipe_wr );

     std_in_pipe_wr = nullptr;

     return 1;
}

string RunExternalProcess::ReadFromPipe ( int ms_timeout )
{
     int    letti;
     char * chBuf;
     BOOL   bSuccess = FALSE;
     int    dimBuf   = BUFSIZE;

     chBuf = ( char * ) malloc ( BUFSIZE );
     letti = 0;

     int timing = 0;

     while ( true ) {
          DWORD dim = 0;

          chBuf    = ( char * ) realloc ( chBuf, dimBuf + BUFSIZE );
          dimBuf   += BUFSIZE;
          bSuccess = ReadFile ( std_out_pipe_rd, &chBuf[letti], dimBuf - letti, &dim, nullptr );

          if ( !bSuccess ) {
               break;
          } else {
               letti += dim;
               dim   = 0;
          }
     }

     string sout;

     if ( letti > 0 ) {
          sout.assign ( chBuf, &chBuf[letti] );
          free ( chBuf );

          return sout;
     } else {
          free ( chBuf );

          return sout;
     }
}
#endif                           //WIN32

#if 0
UNA
CLASSE
PER
ESEGUIRE

#include "ProcessStarter.h"
#include "userenv.h"
#include "wtsapi32.h"
#include "winnt.h"

ProcessStarter::ProcessStarter ( const std::string & processPath,
                                 const std::string & arguments ) :
     processPath_ ( processPath ),
     arguments_ ( arguments )
{
}

PHANDLE ProcessStarter::GetCurrentUserToken()
{
     PHANDLE           currentToken = 0;
     PHANDLE           primaryToken = 0;
     int               dwSessionId  = 0;
     PHANDLE           hUserToken   = 0;
     PHANDLE           hTokenDup    = 0;
     PWTS_SESSION_INFO pSessionInfo = 0;
     DWORD             dwCount      = 0;

     WTSEnumerateSessions ( WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &dwCount );

     int dataSize = sizeof ( WTS_SESSION_INFO );

     for ( DWORD i = 0; i < dwCount; ++i ) {
          WTS_SESSION_INFO si = pSessionInfo[i];

          if ( WTSActive == si.State ) {
               dwSessionId = si.SessionId;

               break;
          }
     }

     WTSFreeMemory ( pSessionInfo );

     BOOL bRet      = WTSQueryUserToken ( dwSessionId, currentToken );
     int  errorcode = GetLastError();

     if ( bRet == false ) {
          return 0;
     }

     bRet = DuplicateTokenEx ( currentToken, TOKEN_ASSIGN_PRIMARY | TOKEN_ALL_ACCESS, 0, SecurityImpersonation,
                               TokenPrimary, primaryToken );
     errorcode = GetLastError();

     if ( bRet == false ) {
          return 0;
     }

     return primaryToken;
}

BOOL ProcessStarter::Run()
{
     PHANDLE primaryToken = GetCurrentUserToken();

     if ( primaryToken == 0 ) {
          return FALSE;
     }

     STARTUPINFO         StartupInfo;
     PROCESS_INFORMATION processInfo;

     StartupInfo.cb = sizeof ( STARTUPINFO );

     SECURITY_ATTRIBUTES Security1;
     SECURITY_ATTRIBUTES Security2;
     std::string         command = "\"" + processPath_ + "\"";

     if ( arguments_.length() != 0 ) {
          command += " " + arguments_;
     }

     void * lpEnvironment = NULL;
     BOOL   resultEnv     = CreateEnvironmentBlock ( &lpEnvironment, primaryToken, FALSE );

     if ( resultEnv == 0 ) {
          long nError = GetLastError();
     }

     BOOL result = CreateProcessAsUser ( primaryToken, 0, ( LPSTR ) ( command.c_str() ), &Security1, &Security2, FALSE,
                                         CREATE_NO_WINDOW | NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT,
                                         lpEnvironment, 0, &StartupInfo, &processInfo );

     DestroyEnvironmentBlock ( lpEnvironment );
     CloseHandle ( primaryToken );

     return result;
}

#ifndef _PROCESS_STARTER_H_
#define _PROCESS_STARTER_H_

#include "windows.h"
#include "winbase.h"
#include <string>

class ProcessStarter {
public:
     ProcessStarter ( const std::string & processPath,
                      const std::string & arguments = "" );

     PHANDLE GetCurrentUserToken();

     BOOL Run();

private:
     std::string processPath_;
     std::string arguments_;
};
#endif //_PROCESS_STARTER_H_

#endif


//~ Formatted by Jindent --- http://www.jindent.com
