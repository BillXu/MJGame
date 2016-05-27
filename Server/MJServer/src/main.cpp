#include <windows.h>
#include "MJServer.h"
#include <iostream>
#include <WinDef.h>
#include <Dbghelp.h>
#pragma comment( lib, "DbgHelp" )
#pragma comment(lib,"JsonDll.lib")
#include "LogManager.h"
DWORD WINAPI ThreadProc(LPVOID lpParam)
{
	bool bRunning = true;
	char pBuffer[255] ;
	while(bRunning)
	{
		memset(pBuffer,0,sizeof(pBuffer)) ;
		scanf_s("%s",pBuffer,sizeof(pBuffer)) ;
		if ( strcmp(pBuffer,"exit") == 0 || strcmp(pBuffer,"Q") == 0 )
		{
			bRunning = false ;
			IServerApp* pAp = (IServerApp*)lpParam ;
			pAp->stop();
			printf("Closing!!!\n");
		}
		else
		{
			printf("Input exit or Q , to close the app, current = %s \n", pBuffer) ;
		}
	}
	return 0;
}

void CreateThred( IServerApp* pApp )
{
	DWORD threadID;
	HANDLE hThread;
	hThread = CreateThread(NULL,0,ThreadProc,pApp,0,&threadID); // �����߳�
}


LONG WINAPI MyUnhandledExceptionFilter( struct _EXCEPTION_POINTERS* ExceptionInfo )
{
	HANDLE lhDumpFile = CreateFile(L"DumpFileForNiuNiuServer.dmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL ,NULL);

	MINIDUMP_EXCEPTION_INFORMATION loExceptionInfo;
	loExceptionInfo.ExceptionPointers = ExceptionInfo;
	loExceptionInfo.ThreadId = GetCurrentThreadId();
	loExceptionInfo.ClientPointers = TRUE;
	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(),lhDumpFile, MiniDumpNormal, &loExceptionInfo, NULL, NULL);

	CloseHandle(lhDumpFile);

	return EXCEPTION_EXECUTE_HANDLER;
}


void RunFunc ( IServerApp* pApp )
{
	// exception 
	__try
	{
		pApp->run() ;
	}
	__except(MyUnhandledExceptionFilter(GetExceptionInformation()))
	{
	}
}


//#include "NiuNiuPeerCard.h"
int main()
{
	//CNiuNiuPeerCard tC ;
	//tC.reset();
	//CCard t ;
	//t.SetCard(CCard::eCard_Club,8);
	//tC.addCompositCardNum(t.GetCardCompositeNum()) ;

	//t.SetCard(CCard::eCard_Club,1);
	//tC.addCompositCardNum(t.GetCardCompositeNum()) ;

	//t.SetCard(CCard::eCard_Club,8);
	//tC.addCompositCardNum(t.GetCardCompositeNum()) ;

	//t.SetCard(CCard::eCard_Club,2);
	//tC.addCompositCardNum(t.GetCardCompositeNum()) ;

	//t.SetCard(CCard::eCard_Club,13);
	//tC.addCompositCardNum(t.GetCardCompositeNum()) ;

	//uint8_t n = t.GetType();
	//return 0 ;

	CMJServerApp* theApp = CMJServerApp::getInstance() ;
	bool bok = theApp->init() ;
	CLogMgr::SharedLogMgr()->SetOutputFile("MJSvr");
	if ( !bok )
	{
		printf("init data svr error , start up error\n");
		char c ;
		scanf_s("%c",&c);
		return 0 ;
	}
	CreateThred(theApp);
#ifdef NDEBUG
	RunFunc(theApp);
#endif // _DEBUG
#ifdef _DEBUG
	theApp->run();
#endif // _DEBUG
	return 0 ;
}