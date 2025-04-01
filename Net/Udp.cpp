#include "Udp.h"
#include "../INetMediator/UdpMediator.h"
Udp::Udp(INetMediator* p) :m_handle(nullptr)
{
	m_pMediator = p;
}

Udp::~Udp()
{
}

//接受数据的线程函数定义处
unsigned _stdcall Udp::recvThread(void* lpVoid)
{
	Udp* pUdp = (Udp*)lpVoid;
	pUdp->recvData();
	return 0;
}


bool Udp::initNet()
{
	//1.链接库
	WSADATA wData;
	WORD dVersion = MAKEWORD(_DEF_VERSION_HIGH, _DEF_VERSION_LOW);
	int err = WSAStartup(dVersion, &wData);
	if (0 != err) {
		cout << "Udp WSAStartUp fail" << endl;
		return false;
	}
	if (_DEF_VERSION_HIGH == HIBYTE(wData.wVersion) || _DEF_VERSION_LOW == LOBYTE(wData.wVersion)) {
		cout << "Udp WSAStartup success " << endl;
	}
	else {
		cout << "Udp WSAStartup version error" << endl;
		return false;
	}
	//2.创建套接字
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_socket) {
		cout << "Udp socket error" << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "Udp socket success" << endl;
	}
	//3.绑定网络端口
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_DEF_UDP_PORT);
	//绑定所有网卡
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	err = bind(m_socket, (sockaddr*)&addr, sizeof(addr));
	if (SOCKET_ERROR == err) {
		cout << "Udp bind error" << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "Udp bind success" << endl;
	}
	//4.创建线程
	//createThread 和 ExitThread是一对函数，如果在线程中调用C++运行时库，例如strcpy，这些函数会自己申请空间，并且不释放，
	//ExitThread在退出线程的时候不会回收申请的空间，就会造成内存泄露
	//_beginthreadex() 和 _endthreadex在退出线程的时候会先回收空间，在调用ExitThread
	m_handle = (HANDLE)_beginthreadex(0,
		0,
		&recvThread/*线程要执行的函数的起始地址*/,
		this/*参数列表*/,
		0/*线程创建的状态，0是创建以后立刻开始运行，CREATE_SUSPENDED是创建以后立刻挂起*/,
		nullptr/*创建线程以后，操作系统给每个线程分配的线程ID*/);
	return true;
}

bool Udp::sendData(char* data, int len, unsigned long to)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_DEF_UDP_PORT);
	addr.sin_addr.S_un.S_addr = to;
	int size = sizeof(addr);
	int nSendNum = sendto(m_socket, data, len, 0, (sockaddr*)&addr, size);
	if (SOCKET_ERROR == nSendNum) {
		cout << "Udp sendto error" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}
//接受数据放在线程里，绑定完成之后就可以创建接受数据的线程了
//一直循环等待接收
void Udp::recvData()
{
	int nRecvNum = 0;
	char recvBuf[4096] = "";
	sockaddr_in addrFrom;
	int size = sizeof(addrFrom);
	while (m_isRunning) {
		nRecvNum = recvfrom(m_socket, recvBuf, sizeof(recvBuf), 0, (sockaddr*)&addrFrom, &size);
		if (nRecvNum > 0) {
			//接受一个数据包成功

			//根据接受数据的大小申请一个新空间
			char* pPacket = new char[nRecvNum];
			//把接收到的数据拷贝到新的空间里
			/*寄存器拷贝*/memcpy(pPacket, recvBuf, nRecvNum);
			m_pMediator->forwardData(pPacket, nRecvNum, addrFrom.sin_addr.S_un.S_addr);
		}
		else {
			cout << "UdpRecvFrom error : " << WSAGetLastError() << endl;
			break;
		}
	}
}

void Udp::closeNet()
{
	//回收线程资源
	//创建线程时，操作系统给每个线程分配三个资源：句柄，线程ID，内核对象，引用计数器是2
	//想回收线程资源的时候，必须让引用计数器是2
	//1.结束线程工作	2.关闭线程句柄
	m_isRunning = false;
	if (m_handle) {
		//WaitForSingleObject的返回值如果等于WAIT_TIMEOUT，就说明等待的线程在等待时间结束之后，还在继续运行
		if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 1000)) {
			//如果线程还在继续运行，就强制杀死线程
			TerminateThread(m_handle, -1);//-1是退出码
		}
		CloseHandle(m_handle);
		m_handle = nullptr;
	}

	//关闭套接字
	if (!m_socket && m_socket != INVALID_SOCKET) {
		closesocket(m_socket);
	}
	//卸载库
	WSACleanup();
}

