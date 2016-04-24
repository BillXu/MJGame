/*
 * ZSUMMER License
 * -----------
 * 
 * ZSUMMER is licensed under the terms of the MIT license reproduced below.
 * This means that ZSUMMER is free software and can be used for both academic
 * and commercial purposes at absolutely no cost.
 * 
 * 
 * ===============================================================================
 * 
 * Copyright (C) 2010-2013 YaweiZhang <yawei_zhang@foxmail.com>.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 * 
 * ===============================================================================
 * 
 * (end of COPYRIGHT)
 */


//! zsummer�Ĳ��Է���ģ��(��Ӧzsummer�ײ������װ���ϲ���Ʋ��Է���) ����Ϊ����˼ܹ��е� gateway����/agent����/ǰ�˷���, �ص��Ǹ߲�����������
//! Socket Clientͷ�ļ�

#ifndef ZSUMMER_CLIENT_H_
#define ZSUMMER_CLIENT_H_
#include "header.h"

//! ǰ������
class CProcess;

//! �ϲ�Socekt Client�Ķ��η�װ
class CClient :public ITcpSocketCallback
{
public:
	CClient();
	~CClient();
	void InitSocket(CProcess *proc, ITcpSocket *s);
	virtual bool OnRecv(unsigned int n);
	virtual bool OnConnect(bool bConnected);
	virtual bool OnSend(unsigned int nSentLen);
	virtual bool OnClose();
	void MessageEntry(zsummer::protocol4z::ReadStream<zsummer::protocol4z::DefaultStreamHeadTraits> & rs);
	void Send(char * buf, unsigned int len);
	CProcess  * m_process;
	ITcpSocket * m_socket;
	
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

	//! cache
	std::string m_textCache;
};

#endif


