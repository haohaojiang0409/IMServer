#include "TcpClient.h"

TcpClient::TcpClient(INetMediator* p) :m_handle(nullptr){
	m_pMediator = p;
}

TcpClient::~TcpClient()
{

}
//线程函数
unsigned _stdcall TcpClient::recvThread(void* lpVoid) {
	TcpClient* pTcp = (TcpClient*)lpVoid;
	pTcp->recvData();
	return 0;
}
//加载库，创建套接字，连接服务器，创建接受数据的线程
bool TcpClient::initNet()
{
	//1.加载库
	WSADATA wData;
	WORD dVersion = MAKEWORD(_DEF_VERSION_HIGH, _DEF_VERSION_LOW);
	int err = WSAStartup(dVersion, &wData);
	if (0 != err) {
		cout << "TcpClient WSAStartUp fail" << endl;
		return false;
	}
	if (_DEF_VERSION_HIGH == HIBYTE(wData.wVersion) || _DEF_VERSION_LOW == LOBYTE(wData.wVersion)) {
		cout << "TcpClient WSAStartup success " << endl;
	}
	else {
		cout << "TcpClient WSAStartup version error" << endl;
		return false;
	}
	//2.创建套接字
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_socket) {
		cout << "TcpClient socket error" << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "TcpClient socket success" << endl;
	}
	//3.接受连接
	sockaddr_in addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(_DEF_TCP_CLIENT_PORT);
	addrServer.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	err = connect(m_socket,(sockaddr*)&addrServer,sizeof(addrServer));
	if (SOCKET_ERROR == err) {
		cout << "connect error" << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "connect success" << endl;
	}
	//4.创建线程
	m_handle = (HANDLE)_beginthreadex(0, 0, &recvThread, this, 0, nullptr);
	return true;
}

bool TcpClient::sendData(char* data, int len, unsigned long to)
{
	//sockaddr_in addrServer;
	//addrServer.sin_family = AF_INET;
	//addrServer.sin_port = htons(_DEF_TCP_CLIENT_PORT);
	//addrServer.sin_addr.S_un.S_addr = 
	//1.校验参数的合法性
	if (nullptr == data || len <= 0) {
		cout << "parameter error " << endl;
		return false;
	}
	//2.先发包大小，再发包数据
	int nSendNum = send(m_socket, (char*)&len, sizeof(int), 0);
	if (SOCKET_ERROR == nSendNum) {
		cout << "error : " << WSAGetLastError() << endl;
		return false;
	}
	//3.再发包内容
	nSendNum = send(m_socket, data, len, 0);
	if (SOCKET_ERROR == nSendNum) {
		cout << "error :" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

void TcpClient::recvData()
{
	//接受数据的长度
	int nRecvNum = 0;
	//接收包的长度
	int nPackLen = 0;
	//记录一个包中累计接收到多少数据
	int offset = 0;
	while (m_isRunning) {
		//先收包长度，再收包内容
		nRecvNum = recv(m_socket, (char*)&nPackLen, sizeof(int), 0);
		if (nRecvNum > 0) {
			char* packBuf = new char[nPackLen];
			while (nPackLen > 0) {
				nRecvNum = recv(m_socket, packBuf + offset, nPackLen, 0);
				if (nRecvNum > 0) {
					offset += nRecvNum;
					nPackLen -= nRecvNum;
				}
				else {
					cout << "recv error2 :" << WSAGetLastError() << endl;
					break;
				}
			}
			//一个包的内容接收完成，此时nPackLen = 0，nOffset 变成了包的长度
			//TODO:接收到的数据传给中介者类
			//打印接收到的数据
			//cout << "TcpClient say : " << packBuf << endl;
			//偏移量要清零
			m_pMediator->forwardData(packBuf, offset, m_socket);
			offset = 0;
		}
		else {
			cout << "recv error1: " << WSAGetLastError() << endl;
			break;
		}
	}
}

void TcpClient::closeNet()
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
