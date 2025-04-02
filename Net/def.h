#pragma once
//Net�µĺ�
//���ؿ�İ汾��
#define _DEF_VERSION_LOW			(2)
#define _DEF_VERSION_HIGH			(2)
//UDP�˿ں�
#define _DEF_UDP_PORT				(12345)
//TCPClient�˿ں�
#define _DEF_TCP_CLIENT_PORT		(23241)
//TCP�������е���󳤶�
#define _DEF_TCP_LISTEN_MAX			(100)

//�ṹ���ĸ���
#define _DEF_PROTOCOL_COUNT			(10)

//�����ṹ�����ͺ�
#define _DEF_PROTOCOL_BASE			(1000)
//ע������
#define _DEF_REGISTER_RQ			(_DEF_PROTOCOL_BASE + 1)
//ע��ظ�
#define _DEF_REGISTER_RS			(_DEF_PROTOCOL_BASE + 2)
//��¼����
#define _DEF_LOGIN_RQ				(_DEF_PROTOCOL_BASE + 3)
//��¼�ظ�
#define _DEF_LOGIN_RS				(_DEF_PROTOCOL_BASE + 4)
//��Ӻ�������
#define _DEF_ADD_FRIEND_RQ			(_DEF_PROTOCOL_BASE + 5)

//��Ӻ��ѻظ�
#define _DEF_ADD_FRIEND_RS			(_DEF_PROTOCOL_BASE + 6)
//��������
#define _DEF_CHAT_RQ				(_DEF_PROTOCOL_BASE + 7)
//����ظ�
#define _DEF_CHAT_RS				(_DEF_PROTOCOL_BASE + 8)
//��������
#define _DEF_OFFLINE_RQ				(_DEF_PROTOCOL_BASE + 9)

//�ṹ�����ͱ���
typedef int packType;

//����ṹ��
//ע������
//�ǳ��ֻ��ų���
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

//ע��ظ�
//��������ĺ�
//ע����
#define _def_register_success			(0)
#define _def_register_tel_exits			(1)
#define _def_reginster_name_exits		(2)

typedef struct _STRU_REGISTER_RS {
	_STRU_REGISTER_RS() :result(_def_reginster_name_exits), type(_DEF_REGISTER_RS) {

	}
	packType type;
	int result;
}_STRU_REGISTER_RS;

//��¼����:�ֻ��ź�����
typedef struct _STRU_LOGIN_RQ {
	_STRU_LOGIN_RQ():type(_DEF_LOGIN_RQ) {
		memset(tel, 0, _DEF_MAX_LENGTH);
		memset(password, 0, _DEF_MAX_LENGTH);
	}
	packType type;
	char tel[_DEF_MAX_LENGTH];
	char password[_DEF_MAX_LENGTH];
}_STRU_LOGIN_RQ;
//��¼�ظ�
//��������绰����δע�� 1   �绰�Ŵ��� 2
#define _def_login_success				(0)
#define _def_login_tel_notbe_register	(1)
#define _def_login_password_error		(2)

typedef struct _STRU_LOGIN_RS {
	_STRU_LOGIN_RS() :result(_def_login_password_error), type(_DEF_LOGIN_RS) {

	}
	packType type;
	int result;
}_STRU_LOGIN_RS;

//��Ӻ�������
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
//��Ӻ��ѻظ�:�������ӳɹ������Ѳ����ڡ������Ѿܾ������Ѳ����ߣ����Լ���ID���Լ����ǳơ����ѵ�ID

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

//��������
//�������ݡ��Լ���ID�ͺ��ѵ�ID
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

//����ظ�
#define _def_send_success			(0)
#define _def_send_error				(1)

typedef struct _STRU_CHAT_RS {
	_STRU_CHAT_RS() :result(_def_send_error), type(_DEF_CHAT_RS) {}
	packType type;
	int result;
}_STRU_CHAT_RS;

//��������
typedef struct _STRU_OFFLINE_RQ {
	_STRU_OFFLINE_RQ() :result(0), type(_DEF_OFFLINE_RQ) {}
	packType type;
	int result;
}_STRU_OFFLINE_RQ;
