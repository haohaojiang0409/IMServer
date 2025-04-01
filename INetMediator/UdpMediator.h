#pragma once
#include "INetMediator.h"
class UdpMediator : public INetMediator {
public:
	UdpMediator();
	~UdpMediator();
	//打开网络
	bool openNet();
	//关闭网络
	void closeNet();
	//发送数据
	bool sendData(char* data, int len, unsigned long to);
	//转发数据(net层接收到的数据传给核心处理类)
	//如果是udp那么就是from 就是 ip 如果是TCP那么就是socket
	void forwardData(char* data, int len, unsigned long from);
};