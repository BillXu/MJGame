#include "Application.h"
#include "log4z.h"
#ifdef _DEBUG
#pragma comment(lib,"jsoncppD.lib")
#else
#pragma comment(lib,"jsoncpp.lib")
#endif 
#include <cassert>
CApplication::CApplication(IServerApp* pApp )
{
	//CLogMgr::SharedLogMgr()->SetOutputFile(nullptr);
	m_pApp = pApp ;
}

IServerApp* CApplication::getApp()
{
	return m_pApp ;
}

void CApplication::stopRuning()
{
	m_isRunning = false ;
}

bool& CApplication::isRunning()
{
	return this->m_isRunning ;
}

void CApplication::startApp()
{
	_CrtSetReportMode(_CRT_ASSERT, 0);
	zsummer::log4z::ILog4zManager::GetInstance()->Start() ;
	auto nRet = m_pApp->init();
	assert(nRet && "init svr error");
	if ( nRet == false )
	{
		LOGFMTE("svr init error") ;
		m_pApp = nullptr ;
		return ;
	}

	// create console input thread 
	DWORD threadID;
	HANDLE hThread;
	m_isRunning = true ;
	hThread = CreateThread(NULL,0,CApplication::consoleInput,this,0,&threadID); // �����߳�

	// run loop ;
	uint32_t nRunloop = 0 ;
	while ( m_pApp && isRunning() )
	{
		++nRunloop ;
		runAppLoop();
		LOGFMTE("try another loop = %u",nRunloop) ;
		Sleep(800);
	}
	zsummer::log4z::ILog4zManager::GetInstance()->Stop();
}

void CApplication::runAppLoop()
{
#ifdef NDEBUG
	__try
	{
		m_pApp->run() ;
	}
	__except(CatchDumpFile::CDumpCatch::UnhandledExceptionFilterEx(GetExceptionInformation()))
	{
		LOGFMTE("try to recover from exception") ;
	}
#else
	m_pApp->run();
#endif
}

DWORD WINAPI CApplication::consoleInput(LPVOID lpParam)
{
	auto application = ((CApplication*)lpParam);
	char pBuffer[255] ;
	while(application->isRunning())
	{
		memset(pBuffer,0,sizeof(pBuffer)) ;
		scanf_s("%s",pBuffer,sizeof(pBuffer)) ;
		if ( strcmp(pBuffer,"exit") == 0 || strcmp(pBuffer,"Q") == 0 )
		{
			application->stopRuning();
			IServerApp* pAp = application->getApp() ;
			pAp->stop();
			printf("Closing!!!\n");
		}
		else
		{
			printf("Input exit or Q , to close the app \n") ;
		}
	}
	return 0;
}