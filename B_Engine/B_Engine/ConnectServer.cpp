#include "ConnectServer.h"
#include "Common.h"

#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#define SERVERPORT 9000
#define KEYINPUTSERVERPORT 9001
int time = 500;
SOCKET KeyInputSocket;
int PlayerNumber{};

bool Connect_To_Server(char* sServer_IP)
{
	// ���� ����
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return false;

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		err_quit("Socket Fail - Connect_To_Server(char)");
		return false;
	}

	//������ ����
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

	// PlayerNumber �ޱ�
	int playerNumber{};
	recv(sock, (char*)&playerNumber, sizeof(playerNumber), 0);
	SetPlayerNumber(playerNumber);

	//==============================
	// ������ �ޱ�(���� �̸� ���� ����)
	//==============================
	//while (1) {
	//
	//	
	//	retval = recv(sock, (char*)&time, sizeof(int), MSG_WAITALL);
	//	if (retval == SOCKET_ERROR) {
	//		err_display("recv()");
	//		break;
	//	}
	//	else if (retval == 0)
	//		break;
	//}
	//
	//
	//// ���� �ݱ�
	//closesocket(sock);
	//
	//// ���� ����
	//WSACleanup();
}

SOCKET GetKeyInputSocket() { return KeyInputSocket; }

void CreateKeyInputServerSocket(char* sServer_IP)
{
	// ���� ����
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	KeyInputSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (KeyInputSocket == INVALID_SOCKET) {
		printf("Socket Fail - CreateKeyInputServerSocket(char)");
		err_quit("Socket Fail - CreateKeyInputServerSocket(char)");
		return;
	}

	//������ ����
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	inet_pton(AF_INET, sServer_IP, &serverAddr.sin_addr);
	serverAddr.sin_port = htons(KEYINPUTSERVERPORT);
	int retval = connect(KeyInputSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR) {
		printf("connect Fail - CreateKeyInputServerSocket(char)");
		err_quit("connect Fail - CreateKeyInputServerSocket(char)");
		return;
	}
}

void SetPlayerNumber(int pn)
{
	PlayerNumber = pn;
}
int GetPlayerNumber()
{
	return PlayerNumber;
}

bool PlayerKeyBuffer[256];
bool GetPlayerBuffer(DWORD key)
{
	return PlayerKeyBuffer[key];
}
void SetPlayerBuffer(DWORD key, bool bSet)
{
	PlayerKeyBuffer[key] = bSet;
}