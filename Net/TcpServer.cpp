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
	//�߳�ID
	unsigned int threadId;
	while (pTcpServer->m_isRunning) {
		//���socket�����Ͷ�Ӧ�Ŀͻ��˽����շ�����
		sock = accept(pTcpServer->m_socket, (sockaddr*)&addr, &size);
		if (INVALID_SOCKET == sock) {
			cout << "accept sock error : " << WSAGetLastError() << endl;
			break;
		}
		else {
			cout << "client ip : " << inet_ntoa(addr.sin_addr) << endl;
			//������������ͻ������ݵ��̣߳�����֮��Ŵ����������ݵ��߳�
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
	//1.���ӿ�

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
	//2.�����׽���
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == m_socket) {
		cout << "TcpServer socket error " << endl;
		return false;
	}else {
		cout << " Tcp Server socket create success " << endl;
	}
	//3.��
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
	//4.����
	err = listen(m_socket, _DEF_TCP_LISTEN_MAX);
	if (SOCKET_ERROR == err) {
		cout << "listen error " << WSAGetLastError() << endl;
	}else{
		cout << "listen success" << endl;
	}
	//5.�����������ӵ��߳�
	//createThread �� ExitThread��һ�Ժ�����������߳��е���C++����ʱ�⣬����strcpy����Щ�������Լ�����ռ䣬���Ҳ��ͷţ�
	//ExitThread���˳��̵߳�ʱ�򲻻��������Ŀռ䣬�ͻ�����ڴ�й¶
	//_beginthreadex() �� _endthreadex���˳��̵߳�ʱ����Ȼ��տռ䣬�ڵ���ExitThread
	m_handle = (HANDLE)_beginthreadex(0,
		0,
		&acceptThread/*�߳�Ҫִ�еĺ�������ʼ��ַ*/,
		this/*�����б�*/,
		0/*�̴߳�����״̬��0�Ǵ����Ժ����̿�ʼ���У�CREATE_SUSPENDED�Ǵ����Ժ����̹���*/,
		nullptr/*�����߳��Ժ󣬲���ϵͳ��ÿ���̷߳�����߳�ID*/);


	return true;
}


bool TcpServer::sendData(char* data, int len, unsigned long to)
{
	cout << "TcpServer : " << __func__ << endl;
	//1.У������ĺϷ���
	if (nullptr == data || len <= 0) {
		cout << "parameter error " << endl;
		return false;
	}
	//2.�ȷ�����С���ٷ�������
	int nSendNum = send(to, (char*)&len, sizeof(int), 0);
	if (SOCKET_ERROR == nSendNum) {
		cout << "error : " << WSAGetLastError() << endl;
		return false;
	}
	//3.�ٷ�������
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
	//����һ��
	Sleep(5);

	//ȡ����ǰ�̵߳�socket�������socketȥ��������
	unsigned int threadId = GetCurrentThreadId();
	SOCKET sock = INVALID_SOCKET;
	if (m_mapThreadToSocket.count(threadId) > 0) {
		sock = m_mapThreadToSocket[threadId];
	}

	//�������ݵĳ���
	int nRecvNum = 0;
	//���հ��ĳ���
	int nPackLen = 0;
	//��¼һ�������ۼƽ��յ���������
	int offset = 0;
	while (m_isRunning) {
		//���հ����ȣ����հ�����
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
			//һ���������ݽ�����ɣ���ʱnPackLen = 0��nOffset ����˰��ĳ���
			//TODO:���յ������ݴ����н�����
			//��ӡ���յ�������
			//cout << "TcpClient say : " << packBuf << endl;
			//char s[] = "djhsaifhaifaihfiow";
			//sendData(s, sizeof(s), sock);
			m_pMediator->forwardData(packBuf, offset, sock);

			//ƫ����Ҫ����
			offset = 0;
		}else {
			cout << "recv error1: " << WSAGetLastError() << endl;
			break;
		}
	}
}

void TcpServer::closeNet()
{
	//1.�����߳���Դ
	m_isRunning = false;
	HANDLE handle;
	for (auto ite = m_listHandle.begin(); ite != m_listHandle.end();) {
		handle = *ite;
		if (handle) {
			//WaitForSingleObject�ķ���ֵ�������WAIT_TIMEOUT����˵���ȴ����߳��ڵȴ�ʱ�����֮�󣬻��ڼ�������
			if (WAIT_TIMEOUT == WaitForSingleObject(handle, 1000)) {
				//����̻߳��ڼ������У���ǿ��ɱ���߳�
				TerminateThread(handle, -1);//-1���˳���
			}
			CloseHandle(handle);
			handle = nullptr;
		}
		//��map���Ƴ���Ч�ڵ㣬erase�Ƴ�������ָ��ĵ�ǰ�ڵ㣬������һ����Ч�ڵ�
		ite = m_listHandle.erase(ite);
	}
	//2.�ر��׽���
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
		//��map���Ƴ���Ч�ڵ㣬erase�Ƴ�������ָ��ĵ�ǰ�ڵ㣬������һ����Ч�ڵ�
		ite = m_mapThreadToSocket.erase(ite);
	}
	//3.ж�ؿ�
	WSACleanup();
}
