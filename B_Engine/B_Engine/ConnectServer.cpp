#include "ConnectServer.h"
#include "Common.h"

#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#define SERVERPORT			9000
#define KEYINPUTSERVERPORT	9001
#define CUBESERVERPORT		9002

int now_time = 500;
SOCKET KeyInputSocket;
SOCKET CubeSocket;
SOCKET sock;
int PlayerNumber{};

bool Connect_To_Server(char* sServer_IP)
{
	// ���� ����
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return false;

	sock = socket(AF_INET, SOCK_STREAM, 0);
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

void CreateCubeServerSocket(char* sServer_IP)
{
	// ���� ����
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	CubeSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (CubeSocket == INVALID_SOCKET) {
		printf("Socket Fail - CreateCubetServerSocket(char)");
		err_quit("Socket Fail - CreateCubetServerSocket(char)");
		return;
	}

	//������ ����
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	inet_pton(AF_INET, sServer_IP, &serverAddr.sin_addr);
	serverAddr.sin_port = htons(CUBESERVERPORT);
	int retval = connect(CubeSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR) {
		printf("connect Fail - CreateCubetServerSocket(char)");
		err_quit("connect Fail - CreateCubetServerSocket(char)");
		return;
	}
}

SOCKET GetCubeSocket() { return CubeSocket; };

//void Send_Cube_Info(DirectX::XMFLOAT3 xmf3_Position, DirectX::XMFLOAT4 m_xmf4_Color)
//{
//	struct Cube_Info cube(xmf3_Position.x, xmf3_Position.y, xmf3_Position.z, m_xmf4_Color.x, m_xmf4_Color.y, m_xmf4_Color.z);
//	int retval = send(CubeSocket, (char*)&remainingSeconds, sizeof(int), 0);
//	if (retval == SOCKET_ERROR) {
//		err_display("send()");
//		break;
//	}
//}



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

DWORD WINAPI Get_Time(LPVOID arg)
{
	int retval = 0;
	while (1) {
		retval = recv(sock, (char*)&now_time, sizeof(int), MSG_WAITALL);
		if (retval == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}
		else if (retval == 0)
			break;
	}
	
	// ���� �ݱ�
	closesocket(sock);
	// ���� ����
	WSACleanup();
	return 0;
}