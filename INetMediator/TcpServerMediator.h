#include "INetMediator.h"
class TcpServerMediator : public INetMediator {
public:
	TcpServerMediator();
	~TcpServerMediator();
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