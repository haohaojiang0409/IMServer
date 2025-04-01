#include "TcpServerMediator.h"
#include "../Net/TcpServer.h"
#include "../CKernel.h"
TcpServerMediator::TcpServerMediator()
{
	m_pNet = new TcpServer(this);
}

TcpServerMediator::~TcpServerMediator()
{
	if (m_pNet) {
		m_pNet->closeNet();
		delete m_pNet;
		m_pNet = nullptr;
	}
}

bool TcpServerMediator::openNet()
{
	return m_pNet->initNet();
}

void TcpServerMediator::closeNet()
{
	m_pNet->closeNet();
}

bool TcpServerMediator::sendData(char* data, int len, unsigned long to)
{
	return m_pNet->sendData(data, len, to);
}

void TcpServerMediator::forwardData(char* data, int len, unsigned long from)
{
	//TODO:传给核心处理类
	CKernel::pCkernel->dealData(data, len, from);
}
