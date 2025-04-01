#include "TcpClient.h"

TcpClient::TcpClient(INetMediator* p) :m_handle(nullptr){
	m_pMediator = p;
}

TcpClient::~TcpClient()
{

}
//�̺߳���
unsigned _stdcall TcpClient::recvThread(void* lpVoid) {
	TcpClient* pTcp = (TcpClient*)lpVoid;
	pTcp->recvData();
	return 0;
}
//���ؿ⣬�����׽��֣����ӷ������������������ݵ��߳�
bool TcpClient::initNet()
{
	//1.���ؿ�
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
	//2.�����׽���
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == m_socket) {
		cout << "TcpClient socket error" << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "TcpClient socket success" << endl;
	}
	//3.��������
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
	//4.�����߳�
	m_handle = (HANDLE)_beginthreadex(0, 0, &recvThread, this, 0, nullptr);
	return true;
}

bool TcpClient::sendData(char* data, int len, unsigned long to)
{
	//sockaddr_in addrServer;
	//addrServer.sin_family = AF_INET;
	//addrServer.sin_port = htons(_DEF_TCP_CLIENT_PORT);
	//addrServer.sin_addr.S_un.S_addr = 
	//1.У������ĺϷ���
	if (nullptr == data || len <= 0) {
		cout << "parameter error " << endl;
		return false;
	}
	//2.�ȷ�����С���ٷ�������
	int nSendNum = send(m_socket, (char*)&len, sizeof(int), 0);
	if (SOCKET_ERROR == nSendNum) {
		cout << "error : " << WSAGetLastError() << endl;
		return false;
	}
	//3.�ٷ�������
	nSendNum = send(m_socket, data, len, 0);
	if (SOCKET_ERROR == nSendNum) {
		cout << "error :" << WSAGetLastError() << endl;
		return false;
	}
	return true;
}

void TcpClient::recvData()
{
	//�������ݵĳ���
	int nRecvNum = 0;
	//���հ��ĳ���
	int nPackLen = 0;
	//��¼һ�������ۼƽ��յ���������
	int offset = 0;
	while (m_isRunning) {
		//���հ����ȣ����հ�����
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
			//һ���������ݽ�����ɣ���ʱnPackLen = 0��nOffset ����˰��ĳ���
			//TODO:���յ������ݴ����н�����
			//��ӡ���յ�������
			//cout << "TcpClient say : " << packBuf << endl;
			//ƫ����Ҫ����
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
//�����߳���Դ
//�����߳�ʱ������ϵͳ��ÿ���̷߳���������Դ��������߳�ID���ں˶������ü�������2
//������߳���Դ��ʱ�򣬱��������ü�������2
//1.�����̹߳���	2.�ر��߳̾��
	m_isRunning = false;
	if (m_handle) {
		//WaitForSingleObject�ķ���ֵ�������WAIT_TIMEOUT����˵���ȴ����߳��ڵȴ�ʱ�����֮�󣬻��ڼ�������
		if (WAIT_TIMEOUT == WaitForSingleObject(m_handle, 1000)) {
			//����̻߳��ڼ������У���ǿ��ɱ���߳�
			TerminateThread(m_handle, -1);//-1���˳���
		}
		CloseHandle(m_handle);
		m_handle = nullptr;
	}

	//�ر��׽���
	if (!m_socket && m_socket != INVALID_SOCKET) {
		closesocket(m_socket);
	}
	//ж�ؿ�
	WSACleanup();
}
