#include "TcpClientMediator.h"
#include "../Net/TcpClient.h"
TcpClientMediator::TcpClientMediator()
{
	m_pNet = new TcpClient(this);
}

TcpClientMediator::~TcpClientMediator()
{
}

bool TcpClientMediator::openNet()
{
	return m_pNet->initNet();
}

void TcpClientMediator::closeNet()
{
	m_pNet->closeNet();
}

bool TcpClientMediator::sendData(char* data, int len, unsigned long to)
{
	return m_pNet->sendData(data, len, to);
}

void TcpClientMediator::forwardData(char* data, int len, unsigned long from)
{
}
