#pragma once 
#include "INetMediator.h"

class TcpClientMediator :public INetMediator {
public:
	TcpClientMediator();
	~TcpClientMediator();
	//������
	bool openNet();
	//�ر�����
	void closeNet();
	//��������
	bool sendData(char* data, int len, unsigned long to);
	//ת������(net����յ������ݴ������Ĵ�����)
	//�����udp��ô����from ���� ip �����TCP��ô����socket
	void forwardData(char* data, int len, unsigned long from);
};