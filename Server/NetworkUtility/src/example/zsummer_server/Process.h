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
//! IOServerͷ�ļ� ���ฺ����Schedule�׹�����client socket. ÿ��IOServer����һ���߳�.

#ifndef ZSUMMER_PROCESS_H_
#define ZSUMMER_PROCESS_H_

#include "header.h"

class CProcess : public CThread, public IIOServerCallback
{
public:
	CProcess();
	//! ������ֹͣ
	bool Start();
	void Stop();
	//! �߳�
	virtual void Run();
	//! Ͷ���̼߳���Ϣ ��ǰ��ƽ���ScheduleͶ��client socket��
	virtual void Post(void * pUser);
	//!IOServer����ȫֹͣ
	virtual bool OnStop();
	//���յ��߳���Ϣ 
	virtual bool OnPost(void *pUser);
	virtual bool OnTimer();

	//! һЩ״̬ͳ���ýӿ�
	inline unsigned long long GetTotalRecvLen() {return m_nTotalRecvLen;}
	inline void AddTotalRecvLen(unsigned long long len) { m_nTotalRecvLen += len;}

	inline unsigned long long GetTotalSendLen() {return m_nTotalSendLen;}
	inline void AddTotalSendLen(unsigned long long len) { m_nTotalSendLen += len;}

	inline unsigned long long GetTotalRecvCount() {return m_nTotalRecvCount;}
	inline void AddTotalRecvCount(unsigned long long len) { m_nTotalRecvCount += len;}

	inline unsigned long long GetTotalSendCount() {return m_nTotalSendCount;}
	inline void AddTotalSendCount(unsigned long long len) { m_nTotalSendCount += len;}
	
private:
	IIOServer * m_ios;
	bool	m_bRunning;
	unsigned long long  m_nTotalRecvLen;
	unsigned long long  m_nTotalSendLen;
	unsigned long long  m_nTotalRecvCount;
	unsigned long long  m_nTotalSendCount;
};

#endif