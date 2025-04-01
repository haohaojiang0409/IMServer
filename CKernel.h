#pragma once
#include "INetMediator/INetMediator.h"
#include "MySQL/CMySql.h"
class CKernel
{
public:
	CKernel();
	~CKernel();
	//�򿪷�����(�����硢�������ݿ�)
	bool startServer();

	//�رշ�����(������Դ���ر����硢�Ͽ������ݿ������)
	void closeServer();

	//������������
	void dealData(char* data, int len, unsigned long from);

	static CKernel* pCkernel;
private:
	INetMediator* m_pTcpServerMediator;
	CMySql mysql;
};

