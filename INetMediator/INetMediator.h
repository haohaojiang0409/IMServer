#pragma once 
//#include "../Net/INet.h"
class INet;
class INetMediator {
public:
	INetMediator():m_pNet(nullptr){

	}
	~INetMediator() {

	}
	//打开网络
	virtual bool openNet() = 0;
	//关闭网络
	virtual void closeNet() = 0;
	//发送数据
	virtual bool sendData(char* data, int len, unsigned long to) = 0;
	//转发数据(net层接收到的数据传给核心处理类)
	//如果是udp那么就是from 就是 ip 如果是TCP那么就是socket
	virtual void forwardData(char* data, int len, unsigned long from) = 0;
protected:
	//网络类指针
	INet* m_pNet;
};