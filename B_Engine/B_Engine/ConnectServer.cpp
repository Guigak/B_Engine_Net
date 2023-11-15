#include "ConnectServer.h"
#include "Common.h"

#define SERVERPORT 9000
int time = 500;

bool Connect_To_Server(char* sServer_IP)
{
	// 소켓 설정
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return false;

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		err_quit("Socket Fail - Connect_To_Server(char)");
		return false;
	}

	//서버에 연결
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	inet_pton(AF_INET, sServer_IP, &serverAddr.sin_addr);
	serverAddr.sin_port = htons(SERVERPORT);
	int retval = connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR) {
		err_quit("connect Fail - Connect_To_ServeR(char)");
		return false;
	}


	//==============================
	// 데이터 받기(파일 이름 고정 길이)
	//==============================
	while (1) {

		
		retval = recv(sock, (char*)&time, sizeof(int), MSG_WAITALL);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;
	}


	// 소켓 닫기
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
}