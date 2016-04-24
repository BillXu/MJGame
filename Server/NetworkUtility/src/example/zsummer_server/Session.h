#pragma once
#include "header.h"
class INetwork ;
class CSession 
	:public ITcpSocketCallback
{
public:
	CSession();
	virtual ~CSession();
	void InitSocketForServer(INetwork *proc, ITcpSocket *s, CONNECT_ID nConnectID);
	bool InitSocketForClient(INetwork *proc, zsummer::network::IIOServer* pIO,const char* pip, unsigned short nPort, CONNECT_ID nConnectID);
	virtual bool OnRecv(unsigned int n);
	virtual bool OnConnect(bool bConnected);
	virtual bool OnSend(unsigned int nSentLen);
	virtual bool OnClose();
	void Send(char * buf, unsigned int len);
	char* GetIPString(){ return m_pIPString ; }
	void Close();
	bool CheckHeatbet();
protected:
	INetwork  * m_pNetwork;
	ITcpSocket * m_socket;
	bool m_bSeverMode ;

	//! ÿ����Ϣ�������ηֱ��ȡͷ���Ͱ���
	unsigned char m_type;
	//! ����
	Packet m_recving;
	unsigned short m_curRecvLen;
	//! д������
	std::queue<Packet *> m_sendque;

	//! ��ǰд��
	Packet m_sending;
	unsigned short m_curSendLen;

	CONNECT_ID m_nConnectID ;

	time_t m_nHeatBetTimeOut ;
	bool m_bWaitDelete ;

	time_t m_nTimeSendBet;

	char m_pIPString[32];
};