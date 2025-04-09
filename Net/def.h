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
//�û���Ϣ
#define _DEF_FRIEND_INFO			(_DEF_PROTOCOL_BASE + 10)

//�û�״̬
#define _def_status_online          (0)
#define _def_status_offline         (1)

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
	_STRU_LOGIN_RS() :type(_DEF_LOGIN_RS), userId(0) , result(_def_login_password_error) {

	}
	packType type;
	int userId;
	int result;
}_STRU_LOGIN_RS;

#define _def_add_friend_success			(0)
#define _def_add_friend_offline			(1)
#define _def_add_friend_refuse			(2)
#define _def_add_friend_not_exists		(3)
//��Ӻ�������
typedef struct _STRU_ADD_FRIEND_RS {
	_STRU_ADD_FRIEND_RS() :type(_DEF_ADD_FRIEND_RS), result(_def_add_friend_offline), userId(0), friendId(0)
	{
		memset(friendName, 0, _DEF_MAX_LENGTH);
		memset(userName, 0, _DEF_MAX_LENGTH);
	}
	packType type;
	int userId;
	int friendId;
	char userName[_DEF_MAX_LENGTH];
	char friendName[_DEF_MAX_LENGTH];
	int result;
}_STRU_ADD_FRIEND_RS;
//��Ӻ��ѻظ�:�������ӳɹ������Ѳ����ڡ������Ѿܾ������Ѳ����ߣ����Լ���ID���Լ����ǳơ����ѵ�ID



typedef struct _STRU_ADD_FRIEND_RQ {
	_STRU_ADD_FRIEND_RQ() :type(_DEF_ADD_FRIEND_RQ), userId(0)
	{
		memset(userName, 0, _DEF_MAX_LENGTH);
		memset(friendName, 0, _DEF_MAX_LENGTH);
	}
	packType type;
	int userId;
	char userName[_DEF_MAX_LENGTH];
	char friendName[_DEF_MAX_LENGTH];
}_STRU_ADD_FRIEND_RQ;



//��������
//�������ݡ��Լ���ID�ͺ��ѵ�ID
#define _DEF_CONTENT_LENGTH			(8*1024)

typedef struct _STRU_CHAT_RQ {
	_STRU_CHAT_RQ() :userId(0), friendId(0) , type(_DEF_CHAT_RQ) {
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
	_STRU_CHAT_RS() :result(_def_send_error), type(_DEF_CHAT_RS) ,friendId(0){}
	packType type;
	int friendId;
	int result;
}_STRU_CHAT_RS;

//��������
typedef struct _STRU_OFFLINE_RQ {
	_STRU_OFFLINE_RQ() :type(_DEF_OFFLINE_RQ), result(0), userId(0) {}
	packType type;
	int userId;
	int result;
}_STRU_OFFLINE_RQ;


//�û���Ϣ�ṹ��
typedef struct _STRU_FRIEND_INFO {
	_STRU_FRIEND_INFO():type(_DEF_FRIEND_INFO),id(0),iconId(0),status(_def_status_offline) {
		memset(name, 0, _DEF_MAX_LENGTH);
		memset(feeling, 0, _DEF_MAX_LENGTH);
	}
	packType type;
	int id;
	int iconId;
	int status;
	char name[_DEF_MAX_LENGTH];
	char feeling[_DEF_MAX_LENGTH];
}_STRU_FRIEND_INFO;