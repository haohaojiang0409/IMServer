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
	//存入数据
	m_protocol[_DEF_REGISTER_RQ - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealRegisterRq;
	m_protocol[_DEF_LOGIN_RQ	- _DEF_PROTOCOL_BASE - 1] = &CKernel::dealLoginRq;
}

bool CKernel::startServer()
{
	//1.打开网络
	m_pTcpServerMediator = new TcpServerMediator;
	if (!m_pTcpServerMediator->openNet()) {
		//初始化网络失败
		cout << "初始化网络失败" << endl;
		return false;
	}

	//2.开启数据库连接
	char ip[] = "127.0.0.1";
	char password[] = "colin123";
	char name[] = "root";
	char dbName[] = "20250401Test";
	if (!mysql.ConnectMySql(ip, name, password, dbName)) {
		cout << "数据库开启失败" << endl;
		return false;
	}
	else {
		cout << "connect the database :" << dbName << endl;
	}
	return true;
}

//关闭服务器：回收资源，关闭网络，断开跟数据库的连接
void CKernel::closeServer()
{
	if (m_pTcpServerMediator) {
		m_pTcpServerMediator->closeNet();
		delete m_pTcpServerMediator;
		m_pTcpServerMediator = nullptr;
	}
}
//获取当前用户信息以及好友的信息
void CKernel::getUserInfoAndFriendInfo(int userId)
{
	cout << __func__ << endl;
	//根据自己的Id查自己的信息
	_STRU_FRIEND_INFO userInfo;
	getInfoById(userId, &userInfo);
	//把自己的信息发给客户端
	if (m_mapIdSocket.count(userId) > 0) {
		m_pTcpServerMediator->sendData((char*)&userInfo, sizeof(userInfo), m_mapIdSocket[userId]);
	}else {
		cout << "m_mapIdSocket中没有id ： " << userId << endl;
		return;
	}
	//根据自己的Id查询好友的id列表
	char sql[1024] = "";
	list<string> lstStr;
	sprintf_s(sql, "select idB from t_friend where idA = '%d';", userId);
	if (!mysql.SelectMySql(sql, 1, lstStr)) {
		//1.没有连接数据库		2.sql语句有语法错误 3.列名和列表对不上（把日志中的sql语句拷贝到workbench执行一下）
		cout << "查询数据失败" << endl;
		return;
	}else {
		cout << "存在好友" << endl;
	}
	//遍历好友的id列表
	int friendId = 0;
	_STRU_FRIEND_INFO friendInfo;
	while (lstStr.size() > 0) {
		//取出好友的id
		friendId = stoi(lstStr.front());
		lstStr.pop_front();
		//根据好友的id查询好友的信息
		getInfoById(friendId, &friendInfo);
		//把好友的信息发回给客户端
		if (m_mapIdSocket.count(userId) > 0) {
			m_pTcpServerMediator->sendData((char*)&friendInfo, sizeof(friendInfo), m_mapIdSocket[userId]);
		}else {
			cout << "m_mapIdToSocket 中没有Id " << userId << endl;
			return;
		}
	}
}
void CKernel::getInfoById(int userId , _STRU_FRIEND_INFO* info)
{
	cout << __func__ << endl;
	info->id = userId;
	if (m_mapIdSocket.count(userId) > 0) {
		//在线
		info->status = _def_status_online;
	}else {
		info->status = _def_status_offline;
	}
	//从数据库查询昵称，签名和头像ID
	char sql[1024] = "";
	list<string> lstStr;
	sprintf_s(sql, "select name,feeling,iconid from t_user where id = '%d';", userId);
	if (!mysql.SelectMySql(sql, 3, lstStr)) {
		//1.没有连接数据库		2.sql语句有语法错误 3.列名和列表对不上（把日志中的sql语句拷贝到workbench执行一下）
		cout << "查询数据失败" << endl;
		return;
	}
	if (3 == lstStr.size()) {
		//取出昵称
		strcpy_s(info->name, sizeof(info->name), lstStr.front().c_str());
		lstStr.pop_front();
		//取出签名
		strcpy_s(info->feeling, sizeof(info->feeling), lstStr.front().c_str());
		lstStr.pop_front();
		//取出头像ID
		info->iconId = stoi(lstStr.front());
		lstStr.pop_front();
	}else {
		cout << "查询昵称签名和头像id错误" << sql << endl;
	}
}
//处理所有数据
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
			//1.定义结构体的时候，type值错了 2.对端发送的结构体不对
			cout << "type2 : " << type << endl;
		}
	}else{
		//1.声明的结构体packType没有第一个声明 2.对端发送的时候offset没有清零
		cout << "type1 : " << type << endl;
	}
	//处理完数据之后回收空间，不然该线程没有用完空间就清零了
	delete[]data;
}

void CKernel::dealRegisterRq(char* data, int len, unsigned long from)
{
	cout << __func__ << endl;
	//1.拆包
	_STRU_REGISTER_RQ* rq = (_STRU_REGISTER_RQ*)data;
	 
	//2.校验电话号码是否被注册 
	//根据电话号码查询电话号码
	//判断查询结果是否为空
	char sql[1024] = "";
	list<string> lstStr;
	sprintf_s(sql, "select tel from t_user where tel = '%s';", rq->tel);
	if (!mysql.SelectMySql(sql , 1 , lstStr)) {
		//1.没有连接数据库		2.sql语句有语法错误 3.列名和列表对不上（把日志中的sql语句拷贝到workbench执行一下）
		cout << "查询数据失败" << endl;
		return;
	}else {
		cout << "电话号码未被注册" << endl;
	}
	//3.校验昵称是否被注册

	if (0 != lstStr.size()) {
		//判断查询结果是否为空
		_STRU_REGISTER_RS rs;
		rs.result = _def_login_tel_notbe_register;
		m_pTcpServerMediator->sendData((char*)&rs, sizeof(rs), from);
		return;
	}
	//根据昵称查询昵称
	sprintf_s(sql, "select name from t_user where name = '%s';", rq->name);
	if (!mysql.SelectMySql(sql, 1, lstStr)) {
		//1.没有连接数据库		2.sql语句有语法错误 3.列名和列表对不上（把日志中的sql语句拷贝到workbench执行一下）
		cout << "查询数据失败" << endl;
		return;
	}
	if (0 != lstStr.size()) {
		//判断查询结果是否为空
		_STRU_REGISTER_RS rs;
		rs.result = _def_reginster_name_exits;
		m_pTcpServerMediator->sendData((char*)&rs, sizeof(rs), from);
		return;
	}
	//4.把注册数据插入数据库
	sprintf_s(sql, "insert into t_user (tel,passwd,name,feeling,iconid) values ('%s','%s', '%s' , '学习太累了',8);"
		, rq->tel, rq->password, rq->name);
	if (!mysql.UpdateMySql(sql)) {
		cout << "更新数据库失败 " << endl;
		return;
	}

	//5.注册成功
	_STRU_REGISTER_RS rs;
	rs.result = _def_register_success;
	m_pTcpServerMediator->sendData((char*)&rs, sizeof(rs), from);
}

void CKernel::dealLoginRq(char* data, int len, unsigned long from)
{
	cout << __func__ << endl;
	//1.拆包
	_STRU_LOGIN_RQ* rq = (_STRU_LOGIN_RQ*)data;
	//2.查询电话号码
	//从数据库查询，根据电话号码查密码
	char sql[1024] = "";
	list<string> lstStr;
	sprintf_s(sql, "select passwd,id from t_user where tel = '%s';", rq->tel);
	//3.判断查询结果是否为空
	if (!mysql.SelectMySql(sql, 2, lstStr)) {
		//1.没有连接数据库		2.sql语句有语法错误 3.列名和列表对不上（把日志中的sql语句拷贝到workbench执行一下）
		cout << "查询数据失败" << endl;
		return;
	}
	_STRU_LOGIN_RS rs;
	//4.查询结果为空，说明电话号码未注册，登陆失败
	if (0 == lstStr.size()) {
		rs.result = _def_login_tel_notbe_register;
	}else {
		//5.比较查询的密码和输入的密码
		string pass = lstStr.front();
		lstStr.pop_front();

		int userId = stoi(lstStr.front());
		lstStr.pop_front();
		if (pass == rq->password) {
			//相等，就是登陆成功
			rs.result = _def_login_success;
			//存入自己的socket
			m_mapIdSocket[userId] = from;
			//获取userId对应的信息
			getUserInfoAndFriendInfo(userId);
		}else {
			//不相等，登陆失败，密码错误
			rs.result = _def_login_password_error;
		}
	}
	//6.返回给客户端
	m_pTcpServerMediator->sendData((char*)&rs, sizeof(rs), from);
}
