#include "CKernel.h"
#include "INetMediator/TcpServerMediator.h"
#include <iostream>
#include "Net/def.h"
using namespace std;
CKernel* CKernel::pCkernel = nullptr;
CKernel::CKernel():m_pTcpServerMediator(nullptr)
{
	pCkernel = this;
}

CKernel::~CKernel()
{
}

bool CKernel::startServer()
{
	//1.������
	m_pTcpServerMediator = new TcpServerMediator;
	if (!m_pTcpServerMediator->openNet()) {
		//��ʼ������ʧ��
		cout << "��ʼ������ʧ��" << endl;
		return false;
	}

	//2.�������ݿ�����
	char ip[] = "127.0.0.1";
	char password[] = "colin123";
	char name[] = "root";
	char dbName[] = "20250401Test";
	if (!mysql.ConnectMySql(ip, name, password, dbName)) {
		cout << "���ݿ⿪��ʧ��" << endl;
		return false;
	}
	else {
		cout << "connect the database :" << dbName << endl;
	}
	return true;
}

//�رշ�������������Դ���ر����磬�Ͽ������ݿ������
void CKernel::closeServer()
{
	if (m_pTcpServerMediator) {
		m_pTcpServerMediator->closeNet();
		delete m_pTcpServerMediator;
		m_pTcpServerMediator = nullptr;
	}
}
//������������
void CKernel::dealData(char* data, int len, unsigned long from)
{
	cout << __func__ << endl;
}

void CKernel::dealRegisterRq(char* data, int len, unsigned long from)
{
	cout << __func__ << endl;
	_STRU_REGISTER_RQ* rq = (_STRU_REGISTER_RQ*)data;
	cout << "name : " << rq->name << ", password : " << rq->password << " , tel : " << rq->tel << endl;
	packType pack = *(int*)data;
	_STRU_REGISTER_RS rs;
}

void CKernel::dealLoginRq(char* data, int len, unsigned long from)
{
	cout << __func__ << endl;
	_STRU_LOGIN_RQ* rq = (_STRU_LOGIN_RQ*)data;
	cout << "password : " << rq->password << ", tel : " << rq->tel << endl;
	packType pack = *(int*)data;
	_STRU_LOGIN_RS rs;
}
