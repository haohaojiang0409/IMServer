#pragma once
#include "INetMediator/INetMediator.h"
#include "MySQL/CMySql.h"
#include "Net/def.h"
class CKernel;
typedef void (CKernel::*PFUN)(char* data, int len, unsigned long from);

class CKernel
{
public:
	CKernel();
	~CKernel();
	//������ָ�������ʼ����������
	void setProtocol();

	//�򿪷�����(�����硢�������ݿ�)
	bool startServer();

	//�رշ�����(������Դ���ر����硢�Ͽ������ݿ������)
	void closeServer();

	//������������
	void dealData(char* data, int len, unsigned long from);
	//����ע������
	void dealRegisterRq(char* data, int len, unsigned long from);
	//�����¼����
	void dealLoginRq(char* data, int len, unsigned long from);
	static CKernel* pCkernel;
private:
	INetMediator* m_pTcpServerMediator;
	CMySql mysql;
	//������������
	PFUN m_protocol[_DEF_PROTOCOL_COUNT];
};

