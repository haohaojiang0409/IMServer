#include <iostream>
#include "INetMediator/UdpMediator.h"
#include "INetMediator/TcpClientMediator.h"
#include "INetMediator/TcpServerMediator.h"
#include <Windows.h>
#include "CKernel.h"
using namespace std;
int main() {
	CKernel kernel;
	if (!kernel.startServer()) {
		cout << "开启服务器失败" << endl;
		return 1;
	}
	while (1) {
		Sleep(50000);
		cout << "server is running........" << endl;
	}
	return 0;
}

//创建数据库和表
//用户信息表：用户id，手机号，密码，昵称，签名，头像id
//好友关系表：idA、idB（要双向存储）