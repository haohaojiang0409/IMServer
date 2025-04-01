#pragma once
#include "INetMediator/INetMediator.h"
#include "MySQL/CMySql.h"
class CKernel
{
public:
	CKernel();
	~CKernel();
	//打开服务器(打开网络、连接数据库)
	bool startServer();

	//关闭服务器(回收资源、关闭网络、断开跟数据库的连接)
	void closeServer();

	//处理所有数据
	void dealData(char* data, int len, unsigned long from);

	static CKernel* pCkernel;
private:
	INetMediator* m_pTcpServerMediator;
	CMySql mysql;
};

