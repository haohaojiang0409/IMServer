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
		cout << "¿ªÆô·þÎñÆ÷Ê§°Ü" << endl;
		return 1;
	}
	while (1) {
		Sleep(50000);
		cout << "server is running........" << endl;
	}
	return 0;
}