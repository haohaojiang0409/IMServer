#pragma once
//Net下的宏
//加载库的版本号
#define _DEF_VERSION_LOW			(2)
#define _DEF_VERSION_HIGH			(2)
//UDP端口号
#define _DEF_UDP_PORT				(12345)
//TCPClient端口号
#define _DEF_TCP_CLIENT_PORT		(23241)
//TCP监听队列的最大长度
#define _DEF_TCP_LISTEN_MAX			(100)

//结构体宏的个数
#define _DEF_PROTOCOL_COUNT			(10)

//声明结构体类型宏
#define _DEF_PROTOCOL_BASE			(1000)
//注册请求
#define _DEF_REGISTER_RQ			(_DEF_PROTOCOL_BASE + 1)
//注册回复
#define _DEF_REGISTER_RS			(_DEF_PROTOCOL_BASE + 2)
//登录请求
#define _DEF_LOGIN_RQ				(_DEF_PROTOCOL_BASE + 3)
//登录回复
#define _DEF_LOGIN_RS				(_DEF_PROTOCOL_BASE + 4)
//添加好友请求
#define _DEF_ADD_FRIEND_RQ			(_DEF_PROTOCOL_BASE + 5)

//添加好友回复
#define _DEF_ADD_FRIEND_RS			(_DEF_PROTOCOL_BASE + 6)
//聊天请求
#define _DEF_CHAT_RQ				(_DEF_PROTOCOL_BASE + 7)
//聊天回复
#define _DEF_CHAT_RS				(_DEF_PROTOCOL_BASE + 8)
//下线请求
#define _DEF_OFFLINE_RQ				(_DEF_PROTOCOL_BASE + 9)

//结构体类型变量
typedef int packType;

//请求结构体
//注册请求
//昵称手机号长度
#define _DEF_MAX_LENGTH			15
typedef struct _STRU_REGISTER_RQ {
	_STRU_REGISTER_RQ():type(_DEF_REGISTER_RQ) {
		memset(tel, 0, _DEF_MAX_LENGTH);
		memset(password, 0, _DEF_MAX_LENGTH);
		memset(name, 0, _DEF_MAX_LENGTH);
	}
	packType type;
	char tel[_DEF_MAX_LENGTH];
	char password[_DEF_MAX_LENGTH];
	char name[_DEF_MAX_LENGTH];
}_STRU_REGISTER_RQ;

//注册回复
//声明结果的宏
//注册结果
#define _def_register_success			(0)
#define _def_register_tel_exits			(1)
#define _def_reginster_name_exits		(2)

typedef struct _STRU_REGISTER_RS {
	_STRU_REGISTER_RS() :result(_def_reginster_name_exits), type(_DEF_REGISTER_RS) {

	}
	packType type;
	int result;
}_STRU_REGISTER_RS;

//登录请求:手机号和密码
typedef struct _STRU_LOGIN_RQ {
	_STRU_LOGIN_RQ():type(_DEF_LOGIN_RQ) {
		memset(tel, 0, _DEF_MAX_LENGTH);
		memset(password, 0, _DEF_MAX_LENGTH);
	}
	packType type;
	char tel[_DEF_MAX_LENGTH];
	char password[_DEF_MAX_LENGTH];
}_STRU_LOGIN_RQ;
//登录回复
//结果集：电话号码未注册 1   电话号错误 2
#define _def_login_success				(0)
#define _def_login_tel_notbe_register	(1)
#define _def_login_password_error		(2)

typedef struct _STRU_LOGIN_RS {
	_STRU_LOGIN_RS() :result(_def_login_password_error), type(_DEF_LOGIN_RS) {

	}
	packType type;
	int result;
}_STRU_LOGIN_RS;

//添加好友请求
typedef struct _STRU_ADD_FRIEND_RQ {
	_STRU_ADD_FRIEND_RQ():userId(0) , type(_DEF_ADD_FRIEND_RQ) {
		memset(userName, 0, _DEF_MAX_LENGTH);
		memset(friendName, 0, _DEF_MAX_LENGTH);
	}
	packType type;
	int userId;
	char userName[_DEF_MAX_LENGTH];
	char friendName[_DEF_MAX_LENGTH];
}_STRU_ADD_FRIEND_RQ;
//添加好友回复:结果（添加成功、好友不存在、好友已拒绝、好友不在线），自己的ID、自己的昵称、好友的ID

#define _def_add_friend_success			(0)
#define _def_add_friend_offline			(1)
#define _def_add_friend_refuse			(2)
#define _def_add_friend_not_exists		(3)

typedef struct _STRU_ADD_FRIEND_RS {
	_STRU_ADD_FRIEND_RS() :result(0), userId(0), friendId(0), type(_DEF_ADD_FRIEND_RS) {
		memset(userName, 0, _DEF_MAX_LENGTH);
	}
	packType type;
	int result;
	int userId;
	char userName[_DEF_MAX_LENGTH];
	int friendId;
}_STRU_ADD_FRIEND_RS;

//聊天请求
//聊天内容、自己的ID和好友的ID
#define _DEF_CONTENT_LENGTH			(8*1024)

typedef struct _STRU_CHAT_RQ {
	_STRU_CHAT_RQ() :userId(0), friendId(0) , type(_DEF_CHAR_RQ) {
		memset(content, 0, _DEF_MAX_LENGTH);
	}
	packType type;
	char content[_DEF_CONTENT_LENGTH];
	int userId;
	int friendId;
}_STRU_CHAT_RQ;

//聊天回复
#define _def_send_success			(0)
#define _def_send_error				(1)

typedef struct _STRU_CHAT_RS {
	_STRU_CHAT_RS() :result(_def_send_error), type(_DEF_CHAT_RS) {}
	packType type;
	int result;
}_STRU_CHAT_RS;

//下线请求
typedef struct _STRU_OFFLINE_RQ {
	_STRU_OFFLINE_RQ() :result(0), type(_DEF_OFFLINE_RQ) {}
	packType type;
	int result;
}_STRU_OFFLINE_RQ;
