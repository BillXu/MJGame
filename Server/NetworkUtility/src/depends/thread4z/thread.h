
/*
 * Thread4z License
 * -----------
 * 
 * Thread4z is licensed under the terms of the MIT license reproduced below.
 * This means that Thread4z is free software and can be used for both academic
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


/*
 * AUTHORS:  YaweiZhang <yawei_zhang@foxmail.com>
 * VERSION:  1.0.0
 * PURPOSE:  A lightweight &  cross platform library for multi-thread.
 * CREATION: 2010.9.26
 * LCHANGE:  2013.03.-
 * LICENSE:  Expat/MIT License, See Copyright Notice at the begin of this file.
 */


/*
 *
 * QQ Group: 19811947
 * Web Site: www.zsummer.net
 * mail: yawei_zhang@foxmail.com
 */


/*
 * UPDATES
 *
 * VERSION 0.1.0 <DATE: 2010.9.26>
 * 	packet CThread, CLock under the Windows.
 * 
 * VERSION 0.2.0 <DATE: 2011.3.19>
 * 	packet CThread, CLock under the Linux.
 * 	packet CSem under the Windows and Linux.
 * 
 * VERSION 0.2.1 <DATE: 2012.4.27>
 * 	packet Atom under the Windows and Linux.
 *
 * VERSION 0.2.2 <DATE: 2012.12.21>
 * 	change CLock detail that set mutex's attribute 'PTHREAD_MUTEX_RECURSIVE' under the linux.
 *
 * VERSION 1.0.0 <DATE: 2013.06.10>
 * 	update 1.0.0
 *
 *
 */
#pragma once

#ifndef _ZSUMMER_THREAD_H_
#define _ZSUMMER_THREAD_H_

#ifdef WIN32
#include <WinSock2.h>
#include <Windows.h>
#else
#include<pthread.h>
#include<semaphore.h>
#endif

#ifndef _ZSUMMER_BEGIN
#define _ZSUMMER_BEGIN namespace zsummer {
#endif
#ifndef _ZSUMMER_THREAD_BEGIN
#define _ZSUMMER_THREAD_BEGIN namespace thread4z {
#endif
_ZSUMMER_BEGIN
_ZSUMMER_THREAD_BEGIN

//! �߳����װ
//! ͨ���̳и��ಢ��дRun()��������ʹ��
//! Start���ý�����һ���߳�ȥִ��Run����.
//! Wait���ý�������ǰ�������߳�ֱ��CThread�߳��˳�.
//! Terminate���ý�ǿ����ֹCThread�߳�, �ĵ���Ӧ����!
class CThread
{
public:
	CThread();
	virtual ~CThread();

	//interface
public:
	bool Start();
	virtual void Run() = 0;
	bool Wait();
	bool Terminate();

	//��ͨ���Է���ֵǿ��ת��Ϊwindows�µ�handle
	inline unsigned long long GetThread() {return m_hThread;};
private:
	unsigned long long m_hThread;
};


//! �ź���
//! Create�����ź��� ��ָ����ʼ�ź�����
//! Open����һ���Ѵ��ڵľ����ź���
//! Wait���� �ȴ��ź�
//! Post���� Ͷ��һ���ź�
class CSem
{
public:
	CSem();
	virtual ~CSem();
public:
	//! initcount ��ʼ���ź�������
	//! name �����ΪNULL �򴴽�һ�������ź���, ����Open����̴�.
	bool Create(int initcount, const char *  name);

	//! ��һ�������ź�������
	bool Open(const char * name);

	//! timeout <= 0 Ϊֱ�����źŲŷ��� ����ʱҲ�᷵��
	bool Wait(int timeout = 0);

	//����һ���ź�
	bool Post();
private:
#ifdef WIN32
	HANDLE m_hSem;
#else
	sem_t m_semid;
	sem_t * m_psid;
	char	m_name[260];
#endif
};


//! �߳���
//! linuxʹ�õݹ�mutex, windows��ʹ���ٽ���, ��Ϊ�ݹ���ʵ��.
//! Lock���ÿ�ʼ������Դ
//! Unlock�����ͷ���Դ����.
class CLock
{
public:
	CLock();
	virtual ~CLock();
public:
	void Lock();
	void UnLock();
private:
#ifdef WIN32
	CRITICAL_SECTION m_crit;
#else
	pthread_mutex_t  m_crit;
#endif
};

//! �����߳���
class CAutoLock
{
public:
	explicit CAutoLock(CLock & lk):m_lock(lk)
	{
		m_lock.Lock();
	}
	~CAutoLock()
	{
		m_lock.UnLock();
	}
private:
	CLock & m_lock;
};


//! ԭ�Ӳ���
int AtomicAdd(volatile int * pt, int t);
int AtomicInc(volatile int * pt);
int AtomicDec(volatile int * pt);



#ifndef _ZSUMMER_END
#define _ZSUMMER_END }
#endif
#ifndef _ZSUMMER_THREAD_END
#define _ZSUMMER_THREAD_END }
#endif

_ZSUMMER_THREAD_END
_ZSUMMER_END

#endif


