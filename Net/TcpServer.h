#include "INet.h"
#include <list>
#include <map>
class TcpServer : public INet {
public:
	TcpServer(INetMediator* p);
	~TcpServer();
	//��ʼ������
	bool initNet();
	//��������:(udp: ͨ��ip(ulong)���ͣ���������˭ tcp:socket uint,�����˷���˭)
	bool sendData(char* data, int len, unsigned long to);
	//��������:�����߳���
	void recvData();
	//�ر�����(�ر��׽��֣�ж�ؿ�)
	void closeNet();

	static unsigned _stdcall acceptThread(void* lpVoid);

	static unsigned _stdcall recvThread(void* lpVoid);

private:
	HANDLE m_handle;
	//��Ž������ݵľ��
	list<HANDLE> m_listHandle;
	//�����Ӧ��socket���߳�ID
	map<unsigned int, SOCKET> m_mapThreadToSocket;
};