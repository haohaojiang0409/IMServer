#include "UdpMediator.h"
#include "../Net/Udp.h"
UdpMediator::UdpMediator()
{
	m_pNet = new Udp(this);
}

UdpMediator::~UdpMediator()
{
	if (m_pNet) {
		m_pNet->closeNet();
		delete m_pNet;
		m_pNet = nullptr;
	}
}

bool UdpMediator::openNet()
{
	
	return m_pNet->initNet();
}

void UdpMediator::closeNet()
{
	m_pNet->closeNet();
}

bool UdpMediator::sendData(char* data, int len, unsigned long to)
{
	return m_pNet->sendData(data, len, to);
}

void UdpMediator::forwardData(char* data, int len, unsigned long from)
{
	//TODO:传给核心处理类
	//测试代码：打印接收到的数据
	cout << __func__ << " ： " << data << endl;
}
