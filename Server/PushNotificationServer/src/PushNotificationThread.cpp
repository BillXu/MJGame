#include "PushNotificationThread.h"
#include "PushRequestQueue.h"
#include "LogManager.h"
#pragma comment(lib, "ws2_32.lib")
#define RECONNECT_TIME (60*26)
#define CHK_NULL(x) if ((x)==NULL) exit (1)
#define CHK_ERR(err,s) if ((err)==-1) { perror(s); exit(1); }
#define CHK_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); exit(2); }


#define CERTF "./pem/ck.pem" /*����˵�֤��(�辭CAǩ��)*/
#define KEYF "./pem/ck.pem" /*����˵�˽Կ(������ܴ洢)*/

#ifdef NDEBUG
#define HOST_NAME "gateway.push.apple.com"
#else
#define HOST_NAME "gateway.sandbox.push.apple.com"
#endif

#define PORT 2195 /*����˵Ķ˿�*/

CPushNotificationThread::CPushNotificationThread()
{
	m_bRunning = true;
	m_nSocket = 0 ;  
	m_pctx = NULL;  
	m_pSSL = NULL;  
	memset(m_vSeed_int,0,sizeof(m_vSeed_int)) ;
}

CPushNotificationThread::~CPushNotificationThread()
{
	Disconnect();
}

bool CPushNotificationThread::InitSSLContex()
{
	//SSL_library_init(); 
	OpenSSL_add_ssl_algorithms(); /*��ʼ��*/  
	SSL_load_error_strings(); /*Ϊ��ӡ������Ϣ��׼��*/  
	m_pctx = SSL_CTX_new (SSLv23_client_method());  
	CHK_NULL(m_pctx); 

	if (SSL_CTX_use_certificate_file(m_pctx, CERTF, SSL_FILETYPE_PEM) <= 0)   
	{  
		CLogMgr::SharedLogMgr()->ErrorLog("�ͻ���֤����ʧ�ܣ���\n");
		return false;  
	}  

	if (SSL_CTX_use_PrivateKey_file(m_pctx, KEYF, SSL_FILETYPE_PEM) <= 0)   
	{  
		CLogMgr::SharedLogMgr()->ErrorLog("�ͻ���key���ʧ�ܣ�\n");
		return false;  
	}  

	if (!SSL_CTX_check_private_key(m_pctx))   
	{  
		CLogMgr::SharedLogMgr()->ErrorLog("�ͻ���֤���key��ƥ��!\n");
		return false;  
	}
	return true ;
}

bool CPushNotificationThread::ConnectToAPNs()
{
	WSADATA wsaData;  

	if(WSAStartup(MAKEWORD(2,2),&wsaData) != 0){  
		printf("WSAStartup()fail:%d\n",GetLastError());  
		return false;  
	}


	srand( (unsigned)time( NULL ) );  
	for( int i = 0; i < 100;i++ ) m_vSeed_int[i] = rand();  

	RAND_seed(m_vSeed_int, sizeof(m_vSeed_int));  

	m_nSocket = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);   
	if(m_nSocket == INVALID_SOCKET)  
	{  
		CLogMgr::SharedLogMgr()->ErrorLog("�׽��ִ���ʧ��!\n");
		return false ;
	}  

	struct sockaddr_in sa;  
	memset(&sa,'\0', sizeof(sa));  
	struct hostent *hp; 
	if(!(hp=gethostbyname(HOST_NAME))) 
	{
		CLogMgr::SharedLogMgr()->ErrorLog("GET HOST BY NAME EEROR , name = %s\n",HOST_NAME) ;
		return false ;
	}
	sa.sin_family = AF_INET;  
	sa.sin_addr = *(struct in_addr*)hp->h_addr_list[0]; 
	sa.sin_port = htons (PORT); /* Server Port number */  

	//TCP����  
	int err = 0;   
	err = connect(m_nSocket, (struct sockaddr*) &sa,sizeof(sa));  
	if(err == -1)  
	{  
		CLogMgr::SharedLogMgr()->ErrorLog( "TCP����ʧ�ܣ�\n");
		return false;  
	}  
	else  
	{  
		CLogMgr::SharedLogMgr()->SystemLog( "TCP���ӳɹ���\n");
	}  
	//SSL����  
	//�½�SSL  
	m_pSSL = SSL_new (m_pctx);   
	if(m_pSSL == NULL)  
	{  
		CLogMgr::SharedLogMgr()->ErrorLog("�½�SSLʧ��!\n");
		return false ;
	}  
	//�׽��ֺ�SSL��  
	SSL_set_fd (m_pSSL, m_nSocket);  
	//SLL����  
	err = SSL_connect (m_pSSL);  
	if(err <= 0 )  
	{  
		int nRet = SSL_get_error(m_pSSL,err);
		CLogMgr::SharedLogMgr()->ErrorLog("SSL����ʧ�� nRet = %d \n",nRet) ;
		return false ;
	}  
	else  
	{  
		CLogMgr::SharedLogMgr()->SystemLog("SSL���ӳɹ�\n") ;
	}  
	//��ӡ������Ϣ  
#ifndef NDEBUG
	printf("SSL�����㷨��Ϣ��%s\n",SSL_get_cipher (m_pSSL));
	/*�õ�����˵�֤�鲢��ӡЩ��Ϣ(��ѡ) */  
	X509* server_cert  = SSL_get_peer_certificate (m_pSSL);  
	CHK_NULL(server_cert);
	if (server_cert != NULL)
	{  
		CLogMgr::SharedLogMgr()->SystemLog("������֤��:\n");
		CLogMgr::SharedLogMgr()->SystemLog("subject: %s \n",X509_NAME_oneline (X509_get_subject_name (server_cert),0,0));
		CLogMgr::SharedLogMgr()->SystemLog("issuer: %s",X509_NAME_oneline(X509_get_issuer_name(server_cert),0,0)); 
		X509_free (server_cert);/*�粻����Ҫ,�轫֤���ͷ� */  
	}  
	else
	{
		CLogMgr::SharedLogMgr()->ErrorLog("������û��֤����Ϣ��\n") ;//����������֤ʧ��  
	}
#endif
	return true;
}

void CPushNotificationThread::ProcessWork()
{
	static time_t nLastActive = time(NULL);
	while(m_bRunning)  
	{     
		LIST_NOTICES vGetNotice ;
		CPushRequestQueue::SharedPushRequestQueue()->GetAllNoticeToProcess(vGetNotice);
		LIST_NOTICES::iterator iter = vGetNotice.begin() ;
		char* pBuffer = 0  ;
		int nLen = 0 ;
		stNotice* pNotice = NULL ;
		time_t nCurNow = time(NULL) ;
		time_t nTimeIdle = 0 ;
		for ( ; iter != vGetNotice.end() ; ++iter )
		{
			nTimeIdle = nCurNow - nLastActive;
			if (  nTimeIdle > RECONNECT_TIME )
			{
				CLogMgr::SharedLogMgr()->SystemLog("idle too long , we reconnected ; idle Time = %d ",nTimeIdle ) ;
				Reconnect();
			}
			pNotice = *iter ;
			ProduceSendBuffer(&pBuffer,nLen,pNotice);
			if ( nLen > 0 && pBuffer != NULL )
			{	
				unsigned char nTryTimes = 1 ;
				
				while( SSL_write(m_pSSL, pBuffer, nLen) <= 0 )  // maybe disconnect ;
				{
					time_t nRunnedTime = time(NULL) - nLastActive ;
					CLogMgr::SharedLogMgr()->SystemLog("SSL connect to APNS Disconnected , trying to Reconnect times = %d ! runned time = %d",nTryTimes,nRunnedTime ) ;
					Reconnect();
					++nTryTimes ;
					if ( nTryTimes >= 5 )
					{
						break;   // avoid unlimit loop ;
					}
				}
				nLastActive = time(NULL) ;   /// test value ;
			}
		}
		CPushRequestQueue::SharedPushRequestQueue()->PushProcessedNotice(vGetNotice) ;
		vGetNotice.clear();
		Sleep(20) ;
	} 
}

void CPushNotificationThread::ProduceSendBuffer(char** pBuffer , int& nLen , stNotice* pSendNotice )
{
	static char payloadBinary[300] ={0};
	memset(payloadBinary,0,sizeof(payloadBinary)) ;
	if ( strlen(pSendNotice->cSound) == 0 )
	{
		sprintf_s(payloadBinary,"{\"aps\":{\"alert\":\"%s\",\"badge\":%d ,\"sound\" : \"default\"} }",pSendNotice->pAlert,pSendNotice->nBadge);
	}
	else
	{
		sprintf_s(payloadBinary,"{\"aps\":{\"alert\":\"%s\",\"badge\":%d ,\"sound\" : \"%s\"}}",pSendNotice->pAlert,pSendNotice->nBadge,pSendNotice->cSound);
	}
	static char pTemp[293] ;
	memset(pTemp,0,sizeof(pTemp)) ;
	size_t payloadLength = strlen(payloadBinary);

	// Define some variables.
	unsigned char command = 0;
	*pBuffer = pTemp ;
	char *pointer = pTemp;
	unsigned short networkTokenLength = htons(32);
	unsigned short networkPayloadLength = htons(payloadLength);

	// Compose message.
	memcpy(pointer, &command, sizeof(unsigned char));
	pointer += sizeof(unsigned char);
	memcpy(pointer, &networkTokenLength, sizeof(unsigned short));
	pointer += sizeof(unsigned short);
	memcpy(pointer,pSendNotice->pDeveiceToken, 32);
	pointer += 32;
	memcpy(pointer, &networkPayloadLength, sizeof(unsigned short));
	pointer += sizeof(unsigned short);
	memcpy(pointer, payloadBinary, payloadLength);
	pointer += payloadLength;
	nLen = pointer - pTemp ;
}

void CPushNotificationThread::Disconnect()
{
	if ( m_pSSL )
	{
		SSL_shutdown (m_pSSL);  
		SSL_free (m_pSSL);  
		SSL_CTX_free (m_pctx);  
		m_pSSL = NULL ;
		m_pctx = NULL ;
	}

	if ( m_nSocket )
	{
		shutdown (m_nSocket,2);  
		closesocket(m_nSocket);
		m_nSocket = 0 ;
	}
}

void CPushNotificationThread::Reconnect()
{
	if ( m_pSSL )
	{
		SSL_shutdown (m_pSSL);  
		SSL_free (m_pSSL);  
		m_pSSL = NULL ;
	}

	if ( m_nSocket )
	{
		shutdown(m_nSocket,2);  
		closesocket(m_nSocket);
		m_nSocket = 0 ;
	}
	ConnectToAPNs();
}