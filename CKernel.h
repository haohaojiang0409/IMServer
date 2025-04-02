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
	//给函数指针数组初始化并存数据
	void setProtocol();

	//打开服务器(打开网络、连接数据库)
	bool startServer();

	//关闭服务器(回收资源、关闭网络、断开跟数据库的连接)
	void closeServer();

	//处理所有数据
	void dealData(char* data, int len, unsigned long from);
	//处理注册请求
	void dealRegisterRq(char* data, int len, unsigned long from);
	//处理登录请求
	void dealLoginRq(char* data, int len, unsigned long from);
	static CKernel* pCkernel;
private:
	INetMediator* m_pTcpServerMediator;
	CMySql mysql;
	//声明函数数组
	PFUN m_protocol[_DEF_PROTOCOL_COUNT];
};

