#include "INet.h"
class Udp : public INet{
public:
	Udp(INetMediator* p);
	~Udp();
	//��ʼ������
	bool initNet();
	//��������:(udp: ͨ��ip(ulong)���ͣ���������˭ tcp:socket uint,�����˷���˭)
	bool sendData(char* data, int len, unsigned long to);
	//��������:�����߳���
	void recvData();
	//�ر�����(�ر��׽��֣�ж�ؿ�)
	void closeNet();
	//�̺߳���
	static unsigned _stdcall recvThread(void* lpVoid);
private:
	HANDLE m_handle;

};