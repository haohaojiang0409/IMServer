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
	//初始化网络
	virtual bool initNet() = 0;
	//发送数据:(udp: 通过ip(ulong)类型，决定发给谁 tcp:socket uint,决定了发给谁)
	virtual bool sendData(char* data, int len, unsigned long to) = 0;
	//接受数据:放在线程里
	virtual void recvData() = 0;
	//关闭网络(关闭套接字，卸载库)
	virtual void closeNet() = 0;
protected:
	//套接字
	SOCKET m_socket;
	//循环控制变量
	bool m_isRunning;
	//中介者父类指针
	INetMediator* m_pMediator;
};