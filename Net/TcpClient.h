#include "INet.h"
class TcpClient : public INet {
public:
	TcpClient(INetMediator* p);
	~TcpClient();
	//初始化网络
	bool initNet();
	//发送数据:(udp: 通过ip(ulong)类型，决定发给谁 tcp:socket uint,决定了发给谁)
	bool sendData(char* data, int len, unsigned long to);
	//接受数据:放在线程里
	void recvData();
	//关闭网络(关闭套接字，卸载库)
	void closeNet();

	static unsigned _stdcall recvThread(void* lpVoid);
private:
	//线程句柄
	HANDLE m_handle;
};