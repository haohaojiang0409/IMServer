#pragma once 
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include "def.h"
#include <process.h>
#include "../INetMediator/INetMediator.h"
#pragma comment(lib,"Ws2_32.lib")
class INetMediator;
using namespace std;
class INet {
public:
	INet():m_socket(INVALID_SOCKET), m_isRunning(true), m_pMediator(nullptr){}
	~INet(){}
	//��ʼ������
	virtual bool initNet() = 0;
	//��������:(udp: ͨ��ip(ulong)���ͣ���������˭ tcp:socket uint,�����˷���˭)
	virtual bool sendData(char* data, int len, unsigned long to) = 0;
	//��������:�����߳���
	virtual void recvData() = 0;
	//�ر�����(�ر��׽��֣�ж�ؿ�)
	virtual void closeNet() = 0;
protected:
	//�׽���
	SOCKET m_socket;
	//ѭ�����Ʊ���
	bool m_isRunning;
	//�н��߸���ָ��
	INetMediator* m_pMediator;
};