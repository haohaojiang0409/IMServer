#include "INet.h"
#include <list>
#include <map>
class TcpServer : public INet {
public:
	TcpServer(INetMediator* p);
	~TcpServer();
	//初始化网络
	bool initNet();
	//发送数据:(udp: 通过ip(ulong)类型，决定发给谁 tcp:socket uint,决定了发给谁)
	bool sendData(char* data, int len, unsigned long to);
	//接受数据:放在线程里
	void recvData();
	//关闭网络(关闭套接字，卸载库)
	void closeNet();

	static unsigned _stdcall acceptThread(void* lpVoid);

	static unsigned _stdcall recvThread(void* lpVoid);

private:
	HANDLE m_handle;
	//存放接受数据的句柄
	list<HANDLE> m_listHandle;
	//保存对应的socket和线程ID
	map<unsigned int, SOCKET> m_mapThreadToSocket;
};