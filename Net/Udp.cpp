#include "Udp.h"
#include "../INetMediator/UdpMediator.h"
Udp::Udp(INetMediator* p) :m_handle(nullptr)
{
	m_pMediator = p;
}

Udp::~Udp()
{
}

//�������ݵ��̺߳������崦
unsigned _stdcall Udp::recvThread(void* lpVoid)
{
	Udp* pUdp = (Udp*)lpVoid;
	pUdp->recvData();
	return 0;
}


bool Udp::initNet()
{
	//1.���ӿ�
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
	//2.�����׽���
	m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (INVALID_SOCKET == m_socket) {
		cout << "Udp socket error" << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "Udp socket success" << endl;
	}
	//3.������˿�
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(_DEF_UDP_PORT);
	//����������
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	err = bind(m_socket, (sockaddr*)&addr, sizeof(addr));
	if (SOCKET_ERROR == err) {
		cout << "Udp bind error" << WSAGetLastError() << endl;
		return false;
	}
	else {
		cout << "Udp bind success" << endl;
	}
	//4.�����߳�
	//createThread �� ExitThread��һ�Ժ�����������߳��е���C++����ʱ�⣬����strcpy����Щ�������Լ�����ռ䣬���Ҳ��ͷţ�
	//ExitThread���˳��̵߳�ʱ�򲻻��������Ŀռ䣬�ͻ�����ڴ�й¶
	//_beginthreadex() �� _endthreadex���˳��̵߳�ʱ����Ȼ��տռ䣬�ڵ���ExitThread
	m_handle = (HANDLE)_beginthreadex(0,
		0,
		&recvThread/*�߳�Ҫִ�еĺ�������ʼ��ַ*/,
		this/*�����б�*/,
		0/*�̴߳�����״̬��0�Ǵ����Ժ����̿�ʼ���У�CREATE_SUSPENDED�Ǵ����Ժ����̹���*/,
		nullptr/*�����߳��Ժ󣬲���ϵͳ��ÿ���̷߳�����߳�ID*/);
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
//�������ݷ����߳�������֮��Ϳ��Դ����������ݵ��߳���
//һֱѭ���ȴ�����
void Udp::recvData()
{
	int nRecvNum = 0;
	char recvBuf[4096] = "";
	sockaddr_in addrFrom;
	int size = sizeof(addrFrom);
	while (m_isRunning) {
		nRecvNum = recvfrom(m_socket, recvBuf, sizeof(recvBuf), 0, (sockaddr*)&addrFrom, &size);
		if (nRecvNum > 0) {
			//����һ�����ݰ��ɹ�

			//���ݽ������ݵĴ�С����һ���¿ռ�
			char* pPacket = new char[nRecvNum];
			//�ѽ��յ������ݿ������µĿռ���
			/*�Ĵ�������*/memcpy(pPacket, recvBuf, nRecvNum);
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

