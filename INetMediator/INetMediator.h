#pragma once 
//#include "../Net/INet.h"
class INet;
class INetMediator {
public:
	INetMediator():m_pNet(nullptr){

	}
	~INetMediator() {

	}
	//������
	virtual bool openNet() = 0;
	//�ر�����
	virtual void closeNet() = 0;
	//��������
	virtual bool sendData(char* data, int len, unsigned long to) = 0;
	//ת������(net����յ������ݴ������Ĵ�����)
	//�����udp��ô����from ���� ip �����TCP��ô����socket
	virtual void forwardData(char* data, int len, unsigned long from) = 0;
protected:
	//������ָ��
	INet* m_pNet;
};