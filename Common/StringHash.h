#include <string>

using namespace std;

#pragma once

#define MAXTABLELEN 1024    // Ĭ�Ϲ�ϣ�������С 
//////////////////////////////////////////////////////////////////////////  
// ��ϣ��������  
typedef struct  _HASHTABLE
{  
	long nHashA;  
	long nHashB;  
	bool bExists;  
}HASHTABLE, *PHASHTABLE ;  

class StringHash
{
public:
	StringHash(const long nTableLength = MAXTABLELEN);
	~StringHash(void);
private:  
	unsigned long cryptTable[0x500];  
	unsigned long m_tablelength;    // ��ϣ��������  
	HASHTABLE *m_HashIndexTable; 
private:
	void InitCryptTable();                                               // �Թ�ϣ������Ԥ���� 
	unsigned long HashString(const string &lpszString, unsigned long dwHashType); // ��ȡ��ϣֵ      
public:
	bool Hash(string url);
	unsigned long Hashed(string url);    // ���url�Ƿ�hash��
};