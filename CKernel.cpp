#include "CKernel.h"
#include "INetMediator/TcpServerMediator.h"
#include <iostream>
#include "Net/def.h"
using namespace std;
CKernel* CKernel::pCkernel = nullptr;
CKernel::CKernel():m_pTcpServerMediator(nullptr)
{
	pCkernel = this;
	setProtocol();
}

CKernel::~CKernel()
{
}

void CKernel::setProtocol()
{
	cout << __func__ << endl;
	memset(m_protocol, 0, sizeof(m_protocol));
	//��������
	m_protocol[_DEF_REGISTER_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealRegisterRq;
	m_protocol[_DEF_LOGIN_RQ	- _DEF_PROTOCOL_BASE - 1] = &CKernel::dealLoginRq;
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
//��ȡ��ǰ�û���Ϣ�Լ����ѵ���Ϣ
void CKernel::getUserInfoAndFriendInfo(int userId)
{
	cout << __func__ << endl;
	//�����Լ���Id���Լ�����Ϣ
	_STRU_FRIEND_INFO userInfo;
	getInfoById(userId, &userInfo);
	//���Լ�����Ϣ�����ͻ���
	if () {
		m_pTcpServerMediator->sendData((char*)&userInfo, sizeof(userInfo), 16);
	}
	//�����Լ���Id��ѯ���ѵ�id�б�

	//�������ѵ�id�б�

	while (true) {
		//ȡ�����ѵ�id

		//���ݺ��ѵ�id��ѯ���ѵ���Ϣ

		//�Ѻ��ѵ���Ϣ���ظ��ͻ���

	}
}
void CKernel::getInfoById(int userId , _STRU_FRIEND_INFO* info)
{
	cout << __func__ << endl;

}
//������������
void CKernel::dealData(char* data, int len, unsigned long from)
{
	cout << __func__ << endl;
	packType type = *(packType*)data;
	int index = type - _DEF_PROTOCOL_BASE - 1;
	if (index >= 0 && index < _DEF_PROTOCOL_COUNT) {
		PFUN pf = m_protocol[index];
		if (pf) {
			(this->*pf)(data, len, from);
		}else{
			//1.����ṹ���ʱ��typeֵ���� 2.�Զ˷��͵Ľṹ�岻��
			cout << "type2 : " << type << endl;
		}
	}else{
		//1.�����Ľṹ��packTypeû�е�һ������ 2.�Զ˷��͵�ʱ��offsetû������
		cout << "type1 : " << type << endl;
	}
	//����������֮����տռ䣬��Ȼ���߳�û������ռ��������
	delete[]data;
}

void CKernel::dealRegisterRq(char* data, int len, unsigned long from)
{
	cout << __func__ << endl;
	//1.���
	_STRU_REGISTER_RQ* rq = (_STRU_REGISTER_RQ*)data;

	//2.У��绰�����Ƿ�ע��
	//���ݵ绰�����ѯ�绰����
	//�жϲ�ѯ����Ƿ�Ϊ��
	char sql[1024] = "";
	list<string> lstStr;
	sprintf_s(sql, "select tel from t_user where tel = '%s';", rq->tel);
	if (!mysql.SelectMySql(sql , 1 , lstStr)) {
		//1.û���������ݿ�		2.sql������﷨���� 3.�������б�Բ��ϣ�����־�е�sql��俽����workbenchִ��һ�£�
		cout << "��ѯ����ʧ��" << endl;
		return;
	}else {
		cout << "�绰����δ��ע��" << endl;
	}
	//3.У���ǳ��Ƿ�ע��

	if (0 != lstStr.size()) {
		//�жϲ�ѯ����Ƿ�Ϊ��
		_STRU_REGISTER_RS rs;
		rs.result = _def_login_tel_notbe_register;
		m_pTcpServerMediator->sendData((char*)&rs, sizeof(rs), from);
		return;
	}
	//�����ǳƲ�ѯ�ǳ�
	sprintf_s(sql, "select name from t_user where name = '%s';", rq->name);
	if (!mysql.SelectMySql(sql, 1, lstStr)) {
		//1.û���������ݿ�		2.sql������﷨���� 3.�������б�Բ��ϣ�����־�е�sql��俽����workbenchִ��һ�£�
		cout << "��ѯ����ʧ��" << endl;
		return;
	}
	if (0 != lstStr.size()) {
		//�жϲ�ѯ����Ƿ�Ϊ��
		_STRU_REGISTER_RS rs;
		rs.result = _def_reginster_name_exits;
		m_pTcpServerMediator->sendData((char*)&rs, sizeof(rs), from);
		return;
	}
	//4.��ע�����ݲ������ݿ�
	sprintf_s(sql, "insert into t_user (tel,passwd,name,feeling,iconid) values ('%s','%s', '%s' , 'ѧϰ̫����',8);"
		, rq->tel, rq->password, rq->name);
	if (!mysql.UpdateMySql(sql)) {
		cout << "�������ݿ�ʧ�� " << endl;
		return;
	}

	//5.ע��ɹ�
	_STRU_REGISTER_RS rs;
	rs.result = _def_register_success;
	m_pTcpServerMediator->sendData((char*)&rs, sizeof(rs), from);
}

void CKernel::dealLoginRq(char* data, int len, unsigned long from)
{
	cout << __func__ << endl;
	//1.���
	_STRU_LOGIN_RQ* rq = (_STRU_LOGIN_RQ*)data;
	//2.��ѯ�绰����
	//�����ݿ��ѯ�����ݵ绰���������
	char sql[1024] = "";
	list<string> lstStr;
	sprintf_s(sql, "select passwd,id from t_user where tel = '%s';", rq->tel);
	//3.�жϲ�ѯ����Ƿ�Ϊ��
	if (!mysql.SelectMySql(sql, 2, lstStr)) {
		//1.û���������ݿ�		2.sql������﷨���� 3.�������б�Բ��ϣ�����־�е�sql��俽����workbenchִ��һ�£�
		cout << "��ѯ����ʧ��" << endl;
		return;
	}
	_STRU_LOGIN_RS rs;
	//4.��ѯ���Ϊ�գ�˵���绰����δע�ᣬ��½ʧ��
	if (0 == lstStr.size()) {
		rs.result = _def_login_tel_notbe_register;
	}else {
		//5.�Ƚϲ�ѯ����������������
		string pass = lstStr.front();
		lstStr.pop_front();

		int userId = stoi(lstStr.front());
		lstStr.pop_front();
		if (pass == rq->password) {
			//��ȣ����ǵ�½�ɹ�
			rs.result = _def_login_success;
			getUserInfoAndFriendInfo(userId);
		}else {
			//����ȣ���½ʧ�ܣ��������
			rs.result = _def_login_password_error;
		}
	}
	//6.���ظ��ͻ���
	m_pTcpServerMediator->sendData((char*)&rs, sizeof(rs), from);
}
