#include "version.h"
#include <signal.h>
#include <errno.h>

#if PLATFORM_WINDOWS
#else
#include <unistd.h>
#endif

#include <stdio.h>

#include "buf.h"
#include "net.h"
#include "char.h"
#include "item.h"
#include "object.h"
#include "configfile.h"
#include "lssproto_serv.h"
#include "saacproto_cli.h"
#include "log.h"
#include "petmail.h"

/*------------------------------------------------------------
 * lsprotocol のワーキング領域を解放する関数を呼ぶ。
 * 引数、返り値
 *  なし
 ------------------------------------------------------------*/
static void endlsprotocol( void )
{
    lssproto_CleanupServer();
    saacproto_CleanupClient();
}

/*------------------------------------------------------------
 * すべての保存しなければならないデータをダンプする関数
 * それぞれのモジュールの関数を呼ぶのみ。
 * 引数、返り値
 *  なし
 ------------------------------------------------------------*/
static void allDataDump( void )
{
    closeAllLogFile();
#ifndef _SIMPLIFY_ITEMSTRING
	storeObjects( getStoredir() );
	storePetmail();
#endif
	storeCharaData();
}

/*------------------------------------------------------------
 * プログラムの終了処理のために呼び出される。
 * 引数、返り値
 *  なし
 ------------------------------------------------------------*/
void shutdownProgram( void )
{
    close( acfd );
    close( bindedfd );
    endlsprotocol();
    endConnect();
    memEnd();
}

char *DebugFunctionName = NULL;
int DebugPoint = 0;

void sigshutdown( int number )
{


#if USE_MTIO
    {
        void MTIO_join(void);
        MTIO_join();
    }
#endif        
    print( "Received signal : %d\n" , number  );
    if( number == 0 )print( "\ngmsv normal down\n" );
    print( "\nDebugPoint (%d)\n", DebugPoint );
    print( "\nLastFunc (%s)\n", DebugFunctionName );

	remove( "gmsvlog.err2");
	rename( "gmsvlog.err1", "gmsvlog.err2" );
	rename( "gmsvlog.err", "gmsvlog.err1" );
	rename( "gmsvlog", "gmsvlog.err" );

    allDataDump();
    

    signal( SIGINT , SIG_IGN );
// TODO: Fix signals
#if PLATFORM_WINDOWS
#else
	signal( SIGQUIT, SIG_IGN );
    signal( SIGKILL, SIG_IGN );
#endif
	signal( SIGSEGV, SIG_IGN );
#if PLATFORM_WINDOWS
#else
	signal(SIGPIPE, SIG_IGN);
#endif
	signal( SIGTERM, SIG_IGN );


    shutdownProgram();
    exit(number);
}



void signalset( void )
{
    // CoolFish: Test Signal 2001/10/26
    print("\nCoolFish Get Signal..\n");

	print("SIGINT:%d\n", SIGINT);
// TODO: Fix signals
#if PLATFORM_WINDOWS
#else
	print("SIGQUIT:%d\n", SIGQUIT);
	print("SIGKILL:%d\n", SIGKILL);
#endif
	print("SIGSEGV:%d\n", SIGSEGV);
#if PLATFORM_WINDOWS
#else
	print("SIGPIPE:%d\n", SIGPIPE);
#endif
	print("SIGTERM:%d\n", SIGTERM);
    
    signal( SIGINT , sigshutdown );
#if PLATFORM_WINDOWS
#else
    signal( SIGQUIT, sigshutdown );
    signal( SIGKILL, sigshutdown );
#endif
    signal( SIGSEGV, sigshutdown );
#if PLATFORM_WINDOWS
#else
    signal( SIGPIPE, SIG_IGN );
#endif
    signal( SIGTERM, sigshutdown );
}
