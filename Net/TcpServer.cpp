#include "TcpServer.h"

TcpServer::TcpServer(INetMediator* p):m_handle(nullptr)
{
	m_pMediator = p;
}

TcpServer::~TcpServer()
{
}
unsigned _stdcall TcpServer::recvThread(void* lpVoid)
{
	TcpServer* pTcpServer = (TcpServer*)lpVoid;
	pTcpServer->recvData();
	return 0;
}

unsigned _stdcall TcpServer::acceptThread(void* lpVoid) {
	cout << __func__ << endl;
	TcpServer* pTcpServer = (TcpServer*)lpVoid;
	SOCKET sock = INVALID_SOCKET;
	sockaddr_in addr;
	int size = sizeof(addr);
	HANDLE anotherHandle;
	//线程ID
	unsigned int threadId;
	while (pTcpServer->m_isRunning) {
		//这个socket用来和对应的客户端进行收发数据
		sock = accept(pTcpServer->m_socket, (sockaddr*)&addr, &size);
		if (INVALID_SOCKET == sock) {
			cout << "accept sock error : " << WSAGetLastError() << endl;
			break;
		}
		else {
			cout << "client ip : " << inet_ntoa(addr.sin_addr) << endl;
			//创建接受这个客户端数据的线程，连接之后才创建接受数据的线程
			anotherHandle = (HANDLE)_beginthreadex(0, 0, &recvThread, pTcpServer, 0, &threadId);
			if (anotherHandle) {
				pTcpServer->m_listHandle.push_back(anotherHandle);
			}
			pTcpServer->m_mapThreadToSocket[threadId] = sock;
			
		}
	}
	return 0;
}

bool TcpServer::initNet()
{
	//1.链接库

	WSADATA wData;
	WORD version = MAKEWORD(_DEF_VERSION_HIGH, _DEF_VERSION_LOW);
	int err = WSAStartup(version, &wData);
	if (0 != err) {
		cout << "Tcp WSAStartup fail" << endl;
		return false;
	}
	if (_DEF_VERSION_HIGH == HIBYTE(wData.wVersion) || _DEF_VERSION_LOW == LOBYTE(wData.wVersion)) {
		cout << "Tcp WSAStartup version success" << endl;
	}else {
		cout << "Tcp WSAStartup version error" << endl;
		return false;
	}
	//2.创建套接字
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == m_socket) {
		cout << "TcpServer socket error " << endl;
		return false;
	}else {
		cout << " Tcp Server socket create success " << endl;
	}
	//3.绑定
	sockaddr_in addrServer;
	addrServer.sin_family = AF_INET;
	addrServer.sin_port = htons(_DEF_TCP_CLIENT_PORT);
	addrServer.sin_addr.S_un.S_addr = INADDR_ANY;
	err = bind(m_socket, (sockaddr*)&addrServer, sizeof(addrServer));
	if (SOCKET_ERROR == err) {
		cout << "bind error :" << WSAGetLastError() << endl;
		return false;
	}else{
		cout << "bind success " << endl;
	}
	//4.监听
	err = listen(m_socket, _DEF_TCP_LISTEN_MAX);
	if (SOCKET_ERROR == err) {
		cout << "listen error " << WSAGetLastError() << endl;
	}else{
		cout << "listen success" << endl;
	}
	//5.创建接受连接的线程
	//createThread 和 ExitThread是一对函数，如果在线程中调用C++运行时库，例如strcpy，这些函数会自己申请空间，并且不释放，
	//ExitThread在退出线程的时候不会回收申请的空间，就会造成内存泄露
	//_beginthreadex() 和 _endthreadex在退出线程的时候会先回收空间，在调用ExitThread
	m_handle = (HANDLE)_beginthreadex(0,
		0,
		&acceptThread/*线程要执行的函数的起始地址*/,
		this/*参数列表*/,
		0/*线程创建的状态，0是创建以后立刻开始运行，CREATE_SUSPENDED是创建以后立刻挂起*/,
		nullptr/*创建线程以后，操作系统给每个线程分配的线程ID*/);


	return true;
}


bool TcpServer::sendData(char* data, int len, unsigned long to)
{
	cout << "TcpServer : " << __func__ << endl;
	//1.校验参数的合法性
	if (nullptr == data || len <= 0) {
		cout << "parameter error " << endl;
		return false;
	}
	//2.先发包大小，再发包数据
	int nSendNum = send(to, (char*)&len, sizeof(int), 0);
	if (SOCKET_ERROR == nSendNum) {
		cout << "error : " << WSAGetLastError() << endl;
		return false;
	}
	//3.再发包内容
	nSendNum = send(to, data, len, 0);
	if (SOCKET_ERROR == nSendNum) {
		cout << "error :" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

void TcpServer::recvData()
{
	cout << "TcpServer : " << __func__ << endl;
	//休眠一会
	Sleep(5);

	//取出当前线程的socket，用这个socket去接受数据
	unsigned int threadId = GetCurrentThreadId();
	SOCKET sock = INVALID_SOCKET;
	if (m_mapThreadToSocket.count(threadId) > 0) {
		sock = m_mapThreadToSocket[threadId];
	}

	//接受数据的长度
	int nRecvNum = 0;
	//接收包的长度
	int nPackLen = 0;
	//记录一个包中累计接收到多少数据
	int offset = 0;
	while (m_isRunning) {
		//先收包长度，再收包内容
		nRecvNum = recv(sock, (char*)&nPackLen, sizeof(int), 0);
		if (nRecvNum > 0) {
			char* packBuf = new char[nPackLen];
			while (nPackLen > 0) {
				nRecvNum = recv(sock, packBuf + offset, nPackLen, 0);
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
			//char s[] = "djhsaifhaifaihfiow";
			//sendData(s, sizeof(s), sock);
			m_pMediator->forwardData(packBuf, offset, sock);

			//偏移量要清零
			offset = 0;
		}else {
			cout << "recv error1: " << WSAGetLastError() << endl;
			break;
		}
	}
}

void TcpServer::closeNet()
{
	//1.回收线程资源
	m_isRunning = false;
	HANDLE handle;
	for (auto ite = m_listHandle.begin(); ite != m_listHandle.end();) {
		handle = *ite;
		if (handle) {
			//WaitForSingleObject的返回值如果等于WAIT_TIMEOUT，就说明等待的线程在等待时间结束之后，还在继续运行
			if (WAIT_TIMEOUT == WaitForSingleObject(handle, 1000)) {
				//如果线程还在继续运行，就强制杀死线程
				TerminateThread(handle, -1);//-1是退出码
			}
			CloseHandle(handle);
			handle = nullptr;
		}
		//从map中移除无效节点，erase移除迭代器指向的当前节点，返回下一个有效节点
		ite = m_listHandle.erase(ite);
	}
	//2.关闭套接字
	if (!m_socket && INVALID_SOCKET != m_socket)
	{
		closesocket(m_socket);
	}
	SOCKET socket = INVALID_SOCKET;
	for (auto ite = m_mapThreadToSocket.begin(); ite != m_mapThreadToSocket.end();) {
		socket = ite->second;
		if (!socket && INVALID_SOCKET != socket) {
			closesocket(socket);
		}
		//从map中移除无效节点，erase移除迭代器指向的当前节点，返回下一个有效节点
		ite = m_mapThreadToSocket.erase(ite);
	}
	//3.卸载库
	WSACleanup();
}
