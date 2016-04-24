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
//! main�ļ�


#include "header.h"
#include "Schedule.h"
#include "Process.h"
using namespace zsummer::log4z;
int g_nTotalLinked = 0;
int g_nTotalCloesed = 0;


int mainNOTUSE(int argc, char* argv[])
{

#ifndef _WIN32
	//! linux����Ҫ���ε�һЩ�ź�
	signal( SIGHUP, SIG_IGN );
	signal( SIGALRM, SIG_IGN ); 
	signal( SIGPIPE, SIG_IGN );
	signal( SIGXCPU, SIG_IGN );
	signal( SIGXFSZ, SIG_IGN );
	signal( SIGPROF, SIG_IGN ); 
	signal( SIGVTALRM, SIG_IGN );
	signal( SIGQUIT, SIG_IGN );
	signal( SIGCHLD, SIG_IGN);
#endif
	//! ������־����
	ILog4zManager::GetInstance()->Config("server.cfg");
	ILog4zManager::GetInstance()->Start();
//ILog4zManager::GetInstance()->ChangeLoggerDisplay(ILog4zManager::GetInstance()->GetMainLogger(), false);
//ILog4zManager::GetInstance()->ChangeLoggerLevel(ILog4zManager::GetInstance()->GetMainLogger(), LOG_LEVEL_INFO);

	//! ����������
	CSchedule schedule;
	schedule.Start();

	//main�߳����ڷ���״̬ͳ�������
	unsigned long long nLastRecv = 0;
	unsigned long long nLastSend = 0;
	unsigned long long nLastRecvCount = 0;
	unsigned long long nLastSendCount = 0;
	for (;;)
	{
		SleepMillisecond(5000);
		unsigned long long temp1 = 0;
		unsigned long long temp2 = 0;
		unsigned long long temp3 = 0;
		unsigned long long temp4 = 0;
		for (std::vector<CProcess *>::const_iterator iter = schedule.m_process.begin(); iter != schedule.m_process.end(); ++iter)
		{
			temp1 += (*iter)->GetTotalRecvLen();
			temp2 += (*iter)->GetTotalSendLen();
			temp3 += (*iter)->GetTotalRecvCount();
			temp4 += (*iter)->GetTotalSendCount();
		}
		LOGD("allLink[" << g_nTotalLinked 
			<<"]  allClosed[" << g_nTotalCloesed
			<< "]  Recv[" << (temp1 - nLastRecv)/1024.0/1024.0/5.0
			<< "]M  Send[" << (temp2 - nLastSend)/1024.0/1024.0/5.0
			<< "]M  RecvSpeed[" << (temp3 - nLastRecvCount)/5.0
			<< "]  SendSpeed[" << (temp4 - nLastSendCount)/5.0
			<< "].");
		nLastRecv = temp1;
		nLastSend = temp2;
		nLastRecvCount = temp3;
		nLastSendCount = temp4;
	}

	schedule.Stop();

	//��־������Ҫ��ʽ����ֹͣ�ӿ�
	//InterfaceLogger::GetInstance()->Stop();
	return 0;
}

