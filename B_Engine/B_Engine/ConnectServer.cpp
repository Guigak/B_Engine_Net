#include "ConnectServer.h"
#include "Common.h"

//#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#define SERVERPORT			9000
#define KEYINPUTSERVERPORT	9001
#define CUBESERVERPORT		9002
#define RECVPLAYERDATAPORT	9003
#define SENDLOOKVECTORPORT  9004

int now_time = 500;
SOCKET KeyInputSocket;
SOCKET CubeSocket;
SOCKET sock;
SOCKET RecvPlayerDataSocket;
SOCKET SendLookVectorSocket;
int PlayerNumber{};

CObject* m_pSeverObjects = NULL;
bool AddorDelete = NULL;

CObject* Get_m_pServerObjects() { return m_pSeverObjects; }
void Release_m_pServerObjects() { m_pSeverObjects = NULL; }
bool Get_AddorDelete_Cube() { return AddorDelete; }
void Release_AddorDelete_Cube() { AddorDelete = NULL; }


//
bool bConnected = false;

bool Get_Con() {
	return bConnected;
}

void Set_Con(bool bNew_Con) {
	bConnected = bNew_Con;
}

bool Connect_To_Server(char* sServer_IP)
{
	// 소켓 설정
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return false;

	sock = socket(AF_INET, SOCK_STREAM, 0);
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

	// PlayerNumber 받기
	int playerNumber{};
	recv(sock, (char*)&playerNumber, sizeof(playerNumber), 0);
	SetPlayerNumber(playerNumber);

}

SOCKET GetKeyInputSocket() { return KeyInputSocket; }

void CreateKeyInputServerSocket(char* sServer_IP)
{
	// 소켓 설정
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	KeyInputSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (KeyInputSocket == INVALID_SOCKET) {
		printf("Socket Fail - CreateKeyInputServerSocket(char)");
		err_quit("Socket Fail - CreateKeyInputServerSocket(char)");
		return;
	}

	//서버에 연결
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
	// 소켓 설정
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	CubeSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (CubeSocket == INVALID_SOCKET) {
		printf("Socket Fail - CreateCubetServerSocket(char)");
		err_quit("Socket Fail - CreateCubetServerSocket(char)");
		return;
	}

	//서버에 연결
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

void CreateRecvPlayerDataSocket(char* sServer_IP)
{
	// 소켓 설정
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	RecvPlayerDataSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (RecvPlayerDataSocket == INVALID_SOCKET) {
		printf("Socket Fail - CreateRecvPlayerDataSocket(char)");
		err_quit("Socket Fail - CreateRecvPlayerDataSocket(char)");
		return;
	}

	//서버에 연결
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	inet_pton(AF_INET, sServer_IP, &serverAddr.sin_addr);
	serverAddr.sin_port = htons(RECVPLAYERDATAPORT);
	int retval = connect(RecvPlayerDataSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR) {
		printf("connect Fail - CreateRecvPlayerDataSocket(char)");
		err_quit("connect Fail - CreateRecvPlayerDataSocket(char)");
		return;
	}

}

SOCKET GetRecvPlayerSocket() { return RecvPlayerDataSocket; }

void CreateSendLookVectorSocket(char* sServer_IP)
{
	// 소켓 설정
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	SendLookVectorSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (SendLookVectorSocket == INVALID_SOCKET) {
		printf("Socket Fail - CreateSendLookVectorSocket(char)");
		err_quit("Socket Fail - CreateSendLookVectorSocket(char)");
		return;
	}

	//서버에 연결
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	inet_pton(AF_INET, sServer_IP, &serverAddr.sin_addr);
	serverAddr.sin_port = htons(SENDLOOKVECTORPORT);
	int retval = connect(SendLookVectorSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR) {
		printf("connect Fail - CreateSendLookVectorSocket(char)");
		err_quit("connect Fail - CreateSendLookVectorSocket(char)");
		return;
	}
}

SOCKET GetSendLookVectorSocket()
{
	return SendLookVectorSocket;
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

DWORD WINAPI Get_Time(LPVOID arg)
{
	int retval = 0;
	while (1) {
		retval = recv(sock, (char*)&now_time, sizeof(int), MSG_WAITALL);
		if (retval == SOCKET_ERROR) {
			err_display("Time recv()");
			break;
		}
		else if (retval == 0)
			break;
	}
	
	// 소켓 닫기
	closesocket(sock);
	// 윈속 종료
	WSACleanup();
	return 0;
}


DWORD WINAPI Add_Cube_Object_From_Server(LPVOID arg)
{
	int retval = 0;
	struct Cube_Info CubeInput;
	while (1) {
		retval = recv(CubeSocket, (char*)&CubeInput, sizeof(Cube_Info), MSG_WAITALL);
		if (retval == SOCKET_ERROR) {
			err_display("Cube_Infor recv()");
			break;
		}
		else if (retval == 0)
			break;

		m_pSeverObjects = new CObject();
		m_pSeverObjects->Set_Position(CubeInput.fPosition_x, CubeInput.fPosition_y, CubeInput.fPosition_z);
		m_pSeverObjects->Set_Color(CubeInput.fColor_r, CubeInput.fColor_g, CubeInput.fColor_b, 0.0f);
		AddorDelete = CubeInput.AddorDelete;
		printf("입력받은 큐브 정보 위치 : %.2f, %.2f, %.2f\n", CubeInput.fPosition_x, CubeInput.fPosition_y, CubeInput.fPosition_z);
		printf("**큐브 %s**\n", AddorDelete ? "설치" : "삭제");
	}
	

	// 소켓 닫기
	closesocket(CubeSocket);
	// 윈속 종료
	WSACleanup();
	return 0;
}