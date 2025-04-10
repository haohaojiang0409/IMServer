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
	m_protocol[_DEF_REGISTER_RQ		- _DEF_PROTOCOL_BASE - 1] = &CKernel::dealRegisterRq;
	m_protocol[_DEF_LOGIN_RQ		- _DEF_PROTOCOL_BASE - 1] = &CKernel::dealLoginRq;
	m_protocol[_DEF_CHAT_RQ			- _DEF_PROTOCOL_BASE - 1] = &CKernel::dealChatRq;
	m_protocol[_DEF_OFFLINE_RQ		- _DEF_PROTOCOL_BASE - 1] = &CKernel::dealOfflineRq;
	m_protocol[_DEF_ADD_FRIEND_RQ	- _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRq;
	m_protocol[_DEF_ADD_FRIEND_RS - _DEF_PROTOCOL_BASE - 1] = &CKernel::dealAddFriendRs;
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
	}
	else {
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
		}
		else {
			cout << "m_mapIdToSocket 中没有Id " << userId << endl;
			return;
		}
	}
	if (m_mapIdSocket.count(friendId) > 0){
		m_pTcpServerMediator->sendData((char*)&friendInfo, sizeof(friendInfo), m_mapIdSocket[friendId]);
	}
}

//通过Id获取用户信息，传入结构体中
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

//处理注册请求
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

//处理登录请求
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
			rs.userId = userId;
			//存入自己的socket
			m_mapIdSocket[userId] = from;
			//6.返回给客户端
			m_pTcpServerMediator->sendData((char*)&rs, sizeof(rs), from);
			//获取userId对应的信息
			getUserInfoAndFriendInfo(userId);
			return;
		}else {
			//不相等，登陆失败，密码错误
			rs.result = _def_login_password_error;
		}
	}
	//6.返回给客户端
	m_pTcpServerMediator->sendData((char*)&rs, sizeof(rs), from);
}

//处理消息发送的函数
void CKernel::dealChatRq(char* data, int len, unsigned long from)
{
	cout << __func__ << endl;
	_STRU_CHAT_RQ* rq = (_STRU_CHAT_RQ*)data;
	//判断好友是否在线
	if (m_mapIdSocket.count(rq->friendId) > 0) {
		//把聊天请求转发给好友
		m_pTcpServerMediator->sendData(data, len, m_mapIdSocket[rq->friendId]);
	}else {
		//好友不在线给客户端回复发送失败
		//正常软件，应该给不在线的好友发的消息存入数据库里，等到好友上线的时候
		//再把数据库里面的聊天内容转发给好友，把数据库中保存的数据删掉
		_STRU_CHAT_RS rs;
		rs.friendId = rq->friendId;
		m_pTcpServerMediator->sendData((char*)&rs, sizeof(rs), from);
	}
}

void CKernel::dealOfflineRq(char* data, int len, unsigned long from)
{
	cout << __func__ << endl;
	//1.拆包
	_STRU_OFFLINE_RQ* rq = (_STRU_OFFLINE_RQ*)data;
	//2.查询好友ID列表
	char sql[1024] = "";
	list<string> lstStr;
	sprintf_s(sql, "select idB from t_friend where idA = '%d';", rq->userId);
	if (!mysql.SelectMySql(sql, 1, lstStr)) {
		cout << "查询好友ID失败" << endl;
		return;
	}
	int friendId = 0;
	//3.遍历好友ID列表
	while (lstStr.size() > 0) {
		//4.取出好友id
		friendId = stoi(lstStr.front());
		lstStr.pop_front();
		//5.判断好友是否在线，在线就转发下线请求给好友
		if (m_mapIdSocket.count(friendId) > 0) {
			m_pTcpServerMediator->sendData(data, len, m_mapIdSocket[friendId]);
		}
	}
	//6.关闭下线用户的socket，并且回收map的空间
	auto ite = m_mapIdSocket.find(rq->userId);
	if (ite != m_mapIdSocket.end()) {
		closesocket(ite->second);
		m_mapIdSocket.erase(ite);
	}
}

//处理添加好友请求
void CKernel::dealAddFriendRq(char* data, int len, unsigned long from)
{
	cout << __func__ << endl;
	//1.拆包
	_STRU_ADD_FRIEND_RQ* rq = (_STRU_ADD_FRIEND_RQ*)data;
	//2.根据好友昵称查询好友id
	char sql[1024] = "";
	list<string> lstStr;
	sprintf_s(sql, "select id from t_user where name = '%s';", rq->friendName);
	if (!mysql.SelectMySql(sql, 1, lstStr)) {
		cout << "查询好友id错误" << sql << endl;
	}
	int friendId = 0;
	//3.判断查询结果是否为空
	if (0 == lstStr.size()) {
		_STRU_ADD_FRIEND_RS rs;
		//4.查询结果是否为空，说明昵称不存在，添加失败_def_add_friend_not_exists
		rs.result = _def_add_friend_not_exists;
		strcpy_s(rs.friendName, sizeof(rs.friendName), rq->friendName);
		//把添加结果返回给A客户端
		m_pTcpServerMediator->sendData((char*)&rs, sizeof(rs), from);
	}else {
		//5.判断好友是否在线
		if (m_mapIdSocket.count(rq->userId) > 0 ) {
			//6.好友在线，把添加好友的请求转发给B客户端
			friendId = stoi(lstStr.front());
			lstStr.pop_front();
			m_pTcpServerMediator->sendData(data, len, m_mapIdSocket[friendId]);
		}
		else {
			//8.把添加结果返回给A客户端
			_STRU_ADD_FRIEND_RS rs;
			//7.好友不在线，添加失败_def_add_friend_offline
			rs.result = _def_add_friend_offline;
			strcpy_s(rs.friendName, sizeof(rs.friendName), rq->friendName);
			m_pTcpServerMediator->sendData((char*)&rs, sizeof(rs), from);
		}
	}
}

//处理添加好友回复
void CKernel::dealAddFriendRs(char* data, int len, unsigned long from)
{
	cout << __func__ << endl;
	//1.拆包
	_STRU_ADD_FRIEND_RS* rs = (_STRU_ADD_FRIEND_RS*)data;
	//2.判断结果是否同意
	if (_def_add_friend_success == rs->result) {
		char sql[1024] = "";
		list<string> lstStr;
		sprintf_s(sql, "insert into t_friend values ( %d , %d );", rs->friendId, rs->userId);
		if (!mysql.UpdateMySql(sql)) {
			cout << "插入数据库错误" << sql << endl;
			return;
		}
		sprintf_s(sql, "insert into t_friend values ( %d , %d );", rs->userId, rs->friendId);
		if (!mysql.UpdateMySql(sql)) {
			cout << "插入数据库错误" << sql << endl;
			return;
		}
		//4.更新双端好友列表
		getUserInfoAndFriendInfo(rs->friendId);
	}

	//5.不管结果如何，都要把结果转发给A客户端
	if (m_mapIdSocket.count(rs->friendId)) {
		m_pTcpServerMediator->sendData(data, len, m_mapIdSocket[rs->userId]);
	}
}
