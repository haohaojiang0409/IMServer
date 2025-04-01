#include "INet.h"
class Udp : public INet{
public:
	Udp(INetMediator* p);
	~Udp();
	//初始化网络
	bool initNet();
	//发送数据:(udp: 通过ip(ulong)类型，决定发给谁 tcp:socket uint,决定了发给谁)
	bool sendData(char* data, int len, unsigned long to);
	//接受数据:放在线程里
	void recvData();
	//关闭网络(关闭套接字，卸载库)
	void closeNet();
	//线程函数
	static unsigned _stdcall recvThread(void* lpVoid);
private:
	HANDLE m_handle;

};