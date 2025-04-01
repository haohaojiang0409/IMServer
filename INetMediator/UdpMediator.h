#pragma once
#include "INetMediator.h"
class UdpMediator : public INetMediator {
public:
	UdpMediator();
	~UdpMediator();
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