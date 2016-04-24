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
//! ����ͷ�ļ�

#ifndef ZSUMMER_HEADER_H_
#define ZSUMMER_HEADER_H_

#include "../../utility/utility.h"
#include "../../depends/thread4z/thread.h"
#include "../../depends/log4z/log4z.h"
#include "../../network/SocketInterface.h"
#include "../../depends/protocol4z/protocol4z.h"
#include <iostream>
#include <queue>
#include <iomanip>
#include <string.h>
#include <signal.h>
using namespace std;

using namespace zsummer::utility;
using namespace zsummer::thread4z;
using namespace zsummer::network;

//! ��Ϣ����������С
#define _MSG_BUF_LEN	(4*1024)
// 10 miao xin tiao ;
#define _HEAT_BET_TIME_OUT 10
#define _HEAT_BET_SEND_TIME 5

typedef unsigned int CONNECT_ID ;
#define INVALID_CONNECT_ID (unsigned int)-1

//! ��Ϣ�� 
struct Packet
{
	CONNECT_ID _connectID ;
	bool _brocast ;
	unsigned char _packetType ;
#define _PACKET_TYPE_MSG 1 
#define _PACKET_TYPE_CONNECTED 2 
#define _PACKET_TYPE_DISCONNECT 3
#define _PACKET_TYPE_CONNECT_FAILED 4
	unsigned short _len;
	char		   _orgdata[_MSG_BUF_LEN];
};

struct ConnectInfo
{
	unsigned char strAddress[16];
	unsigned short nPort ;
};


//! ��������������������ܹر�������
extern int g_nTotalLinked;
extern int g_nTotalCloesed; //! ���߳���ʹ�� ����д��������Ϊԭ�Ӳ���



#endif

