#include <WinSock2.h>
#include "ClientNetworkImp.h"
#define CLIENT_HEAT_BET_CHECK_TIEM  (TIME_HEAT_BET + 1 )
CClientNetworkImp::CClientNetworkImp()
	:m_pReadIngBuffer(new CInternalBuffer()),m_tHeatBeat(m_io_service),m_tKeepIOServiecWork(m_io_service)
{
	m_socket = nullptr ;
	m_pIOThread = nullptr ;
	m_pEndpoint = nullptr ;
	m_nHeatBeatTimes = 0 ;
}

CClientNetworkImp::~CClientNetworkImp()
{
	m_tHeatBeat.cancel();
	m_tKeepIOServiecWork.cancel();

	if ( m_pEndpoint )
	{
		delete m_pEndpoint ;
		m_pEndpoint = nullptr ;
	}

	if ( m_pIOThread )
	{
		m_io_service.stop() ;
		delete m_pIOThread ;
		m_pIOThread = nullptr ;
	}

	if ( m_socket )
	{
		m_socket->close() ;
		delete m_socket ;
		m_socket = nullptr ;
	}
}

bool CClientNetworkImp::init()
{
	if ( nullptr == m_socket )
	{
		m_socket = new tcp::socket(m_io_service);	
	}
	m_nState = eState_None ;
	m_nHeatBeatTimes = 0 ;
	return true ;
}

void CClientNetworkImp::shutdown()
{
	//m_io_service.post(boost::bind(&CClientNetworkImp::doClose, this)); //���close�����ǿͻ���Ҫ������ֹʱ����  do_close�����Ǵӷ�������  
	//������ʧ��ʱ����  
}

bool CClientNetworkImp::connectToServer(const char* pIP, unsigned short nPort )
{
	if ( m_nState == eState_Connecting || m_nState == eState_Connected )
	{
		printf("connecting or connected , don't do twice\n") ;
		return false;
	}

	if ( nullptr == m_socket )
	{
		m_socket = new tcp::socket(m_io_service);	
	}

	if ( m_pEndpoint )
	{
		delete m_pEndpoint ;
		m_pEndpoint = nullptr ;
	}

	m_tHeatBeat.cancel();
	m_nState = eState_Connecting ;
	printf("connect ting \n");

	m_pEndpoint =  new boost::asio::ip::tcp::endpoint( boost::asio::ip::address::from_string(pIP),nPort );
	m_socket->async_connect(*m_pEndpoint,boost::bind(&CClientNetworkImp::handleConnect, this,  
		boost::asio::placeholders::error));

	if ( m_pIOThread == nullptr )
	{
		m_pIOThread = new  boost::thread(boost::bind(&boost::asio::io_service::run, &m_io_service)); 

		m_tKeepIOServiecWork.expires_from_now(boost::posix_time::seconds(888888));
		m_tKeepIOServiecWork.async_wait(boost::bind(&CClientNetworkImp::onKeepIoServeicWork, this,boost::asio::placeholders::error));
	}
	

	return true ;
}

void CClientNetworkImp::onKeepIoServeicWork( const boost::system::error_code& ec )
{
	if ( !ec )
	{
		m_tKeepIOServiecWork.expires_from_now(boost::posix_time::seconds(888888));
		m_tKeepIOServiecWork.async_wait(boost::bind(&CClientNetworkImp::onKeepIoServeicWork, this,boost::asio::placeholders::error));
	}
}

bool CClientNetworkImp::getAllPacket(LIST_PACKET& vOutPackets ) // must delete out side ;
{
	WriteLock wLock(m_PacketMutex) ;
	vOutPackets.swap(m_vRecivedPackets) ;
	return !vOutPackets.empty();
}

bool CClientNetworkImp::getFirstPacket(Packet** ppPacket ) // must delete out side ;
{
	WriteLock wLock(m_PacketMutex) ;
	if ( m_vRecivedPackets.empty() )
	{
		return false ;
	}

	LIST_PACKET::iterator iter = m_vRecivedPackets.begin() ;
	Packet* p = m_vRecivedPackets.front() ;
	*ppPacket = p ;
	m_vRecivedPackets.erase(iter) ;
	return true ;
}

void CClientNetworkImp::addPacket(Packet* pPacket ) 
{
	WriteLock wLock(m_PacketMutex) ;
	m_vRecivedPackets.push_back(pPacket) ;
}

bool CClientNetworkImp::sendMsg(const char* pData , size_t nLen ) 
{
	if ( nLen >= CInternalBuffer::max_body_length )
	{
		return false ;
	}
	InternalBuffer_ptr msg (new CInternalBuffer());
	msg->setData(pData,nLen) ;
	m_io_service.post(boost::bind(&CClientNetworkImp::doWrite, this, msg)); //����Ϣ����Ͷ�ݸ�io_service  
	return true ;
}

void CClientNetworkImp::handleConnect(const boost::system::error_code& error)  
{  
	bool bSucce = !error;
	Packet* pack = new Packet ;
	pack->_brocast = false ;
	pack->_packetType = bSucce ?_PACKET_TYPE_CONNECTED : _PACKET_TYPE_CONNECT_FAILED ;
	pack->_connectID = 0 ;
	if (bSucce)  
	{  
		boost::asio::async_read(*m_socket,  
			boost::asio::buffer(m_pReadIngBuffer->data(), CInternalBuffer::header_length), //��ȡ���ݱ�ͷ  
			boost::bind(&CClientNetworkImp::handleReadHeader, this,  
			boost::asio::placeholders::error));  
		printf("connected success\n");
		m_nHeatBeatTimes = 3 ;

		// start heat bet check ;
		m_tHeatBeat.expires_from_now(boost::posix_time::seconds(CLIENT_HEAT_BET_CHECK_TIEM));
		m_tHeatBeat.async_wait(boost::bind(&CClientNetworkImp::sendHeatBeat, this,boost::asio::placeholders::error));
	} 
	else
	{
		printf("connected Failed\n");
	}
	m_nState = bSucce ? eState_Connected : eState_ConnectedFailed ;
	addPacket(pack);
	//m_tConnectTimeOut.cancel();
}  

void CClientNetworkImp::handleReadHeader(const boost::system::error_code& error)  
{  
	if (!error && m_pReadIngBuffer->decodeHeader()) //�ֱ������ݱ�ͷ�����ݲ���  
	{  
		boost::asio::async_read(*m_socket,  
			boost::asio::buffer(m_pReadIngBuffer->body(),m_pReadIngBuffer->bodyLength()),//��ȡ���ݰ����ݲ���  
			boost::bind(&CClientNetworkImp::handleReadBody, this,  
			boost::asio::placeholders::error));  
	}  
	else  
	{  
		doClose();  
	}  
}  

void CClientNetworkImp::handleReadBody(const boost::system::error_code& error)  
{  
	if (!error)  
	{  
		if ( m_pReadIngBuffer->bodyLength() )
		{
			Packet* pack = new Packet ;
			pack->_brocast = false ;
			pack->_packetType = _PACKET_TYPE_MSG ;
			pack->_connectID = 0 ;
			pack->_len = m_pReadIngBuffer->bodyLength() ;
			memcpy(pack->_orgdata,m_pReadIngBuffer->body(),m_pReadIngBuffer->bodyLength()) ;
			addPacket(pack) ;
		}
		else
		{
			m_nHeatBeatTimes = 3;
			//printf("recived heat bet\n") ;
		}

		boost::asio::async_read(*m_socket,   
			boost::asio::buffer(m_pReadIngBuffer->data(), CInternalBuffer::header_length), //�������ȡ��һ�����ݰ�ͷ  
			boost::bind(&CClientNetworkImp::handleReadHeader, this,    
			boost::asio::placeholders::error)); //���һ�ζ�������������һ�����ݰ���  ������һ�ζ�����  
		
		
	}  
	else  
	{  
		doClose();  
	}  
}  

void CClientNetworkImp::doWrite(InternalBuffer_ptr msg)  
{  
	bool write_in_progress = !m_vWillSendBuffers.empty(); //�յĻ�����Ϊfalse  
	m_vWillSendBuffers.push_back(msg); //��Ҫд������push��д����  
	if (!write_in_progress)//���г�ʼΪ�� pushһ��msg�����һ��Ԫ����  
	{  
		boost::asio::async_write(*m_socket,  
			boost::asio::buffer(m_vWillSendBuffers.front()->data(),  
			m_vWillSendBuffers.front()->length()),  
			boost::bind(&CClientNetworkImp::handleWrite, this,   
			boost::asio::placeholders::error));  
	}  
}  

void CClientNetworkImp::handleWrite(const boost::system::error_code& error)//��һ����Ϣ��������ʣ�µĲŸ��ò���  
{  
	if (!error)  
	{  
		m_vWillSendBuffers.pop_front();//�ղŴ�����һ������ ����Ҫpopһ��  
		if (!m_vWillSendBuffers.empty())    
		{  
			boost::asio::async_write(*m_socket,  
				boost::asio::buffer(m_vWillSendBuffers.front()->data(),  
				m_vWillSendBuffers.front()->length()),  
				boost::bind(&CClientNetworkImp::handleWrite, this,  
				boost::asio::placeholders::error)); //ѭ������ʣ�����Ϣ  
		}  
	}  
	else  
	{  
		doClose();  
	}  
} 

void CClientNetworkImp::sendHeatBeat( const boost::system::error_code& ec )
{
	if ( !ec )
	{
		if ( --m_nHeatBeatTimes <= 0 )
		{
			printf("heat bet check time out \n") ;
			doClose() ;
		}
		else
		{
			m_tHeatBeat.expires_from_now(boost::posix_time::seconds(CLIENT_HEAT_BET_CHECK_TIEM));
			m_tHeatBeat.async_wait(boost::bind(&CClientNetworkImp::sendHeatBeat, this,boost::asio::placeholders::error));
		}
	}
}

void CClientNetworkImp::doClose()  
{  
	if ( m_nState != eState_Connected && m_nState != eState_Connecting )
	{
		printf("socket already closed\n") ;
		return ;
	}
	m_nState = eState_None ;
	m_socket->shutdown(boost::asio::socket_base::shutdown_type::shutdown_both);
	m_socket->close();

	Packet* pack = new Packet ;
	pack->_brocast = false ;
	pack->_packetType = _PACKET_TYPE_DISCONNECT ;
	pack->_connectID = 0 ;
	pack->_len = 0 ;
	addPacket(pack) ;

	m_tHeatBeat.cancel();
	printf("server dis connected\n");
}  