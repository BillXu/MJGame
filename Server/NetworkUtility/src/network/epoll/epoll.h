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

#ifndef _ZSUMMER_EPOLL_H_
#define _ZSUMMER_EPOLL_H_
#include "public.h"

/*
* epoll�� postʹ��socketpair��Ϊ�ҿ���epoll_wait�ϵ�֪ͨ�������� ����EPOLLLTģʽ
*/

/* ������Ϊ3.0�汾����
*  virtualbox ����� 8000������ ÿ������ƽ��1.5s����һ��200�ֽڵ����� ����echo����У��, �ȶ�.  40%CPU ����us0.5%, %6hi %24si %4sy  �󲿷������ж� ��������������������
*  virtualbox �����  28000������ ÿ������ƽ��10.5s����һ��200�ֽڵ����� 24%CPUռ�� ���� us0.3%  17.2$si  �󲿷��������ж�
*  ��������10%��CPUռ��,  �ͻ���6%��ռ��
*  vmware ����� 8000������ ÿ������ƽ��1.5s����һ��200�ֽڵ����� ����echo����У��, �ȶ�.  8.0%CPU ����us0.3%, %0.2hi %3.4si %0.7sy 
*  vmware ����� 16000������ ÿ������ƽ��1.5s����һ��200�ֽڵ����� ����echo����У��, �ȶ�.  17.0%CPU ����us0.5%, %0.2hi %7.1si %1.5sy 
*  vmware ����� 23000������ ÿ������ƽ��1.5s����һ��200�ֽڵ����� ����echo����У��, �ȶ�.  27.0%CPU ����us0.8%, %0.3hi %11.1si %2.1sy 
*
*/
namespace zsummer
{
	typedef std::vector<std::pair<int, void*> > MsgVct;
	class CIOServer: public IIOServer
	{
	public:
		CIOServer();
		virtual ~CIOServer();
		virtual bool Initialize(IIOServerCallback *cb);
		virtual void RunOnce();

		virtual void Post(void *pUser);
		virtual unsigned long long CreateTimer(unsigned int delayms, ITimerCallback * cb);
		virtual bool CancelTimer(unsigned long long timerID);

	public:
		void PostMsg(POST_COM_KEY pck, void * ptr); 
		void CheckTimer();
	public:
		int	m_epoll;
		IIOServerCallback	* m_cb;

		//! ������Ϣ
		epoll_event m_events[5000];

		//�߳���Ϣ
		int		m_sockpair[2];
		tagRegister m_recv;
		MsgVct	m_msgs;
		CLock	m_msglock;

		//! ��ʱ��
		std::map<unsigned long long, ITimerCallback*> m_queTimer;
		unsigned int m_queSeq; //! �������ɶ�ʱ��ID
		volatile unsigned long long m_nextExpire; //! ��촥��ʱ��
		zsummer::thread4z::CLock m_lockTimer; //! ��
	};

}





















#endif











