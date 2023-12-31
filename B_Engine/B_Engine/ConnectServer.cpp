#include "ConnectServer.h"
#include "Common.h"
#include <array>
#include <algorithm>
#include <mutex>

//#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")

#define SERVERPORT			9000
#define KEYINPUTSERVERPORT	9001
#define CUBESERVERPORT		9002
#define RECVPLAYERDATAPORT	9003
#define CHATDATAPORT  9004

int now_time = 999;
SOCKET KeyInputSocket;
SOCKET CubeSocket;
SOCKET sock;
SOCKET RecvPlayerDataSocket;
SOCKET ChatDataSocket;
int PlayerNumber{};

//  큐브 벡터 크리티컬 섹션
CRITICAL_SECTION cs_Cube;
std::mutex mtx;

std::vector<Cube_Info> m_vServerObjects;
std::vector<Cube_Info>* Get_m_vServerObjects() { 
	return &m_vServerObjects; 
}



void Release_m_vServerObjects()
{
	EnterCriticalSection(&cs_Cube);
	try {
		
		if (m_vServerObjects.size() != 0) {
			m_vServerObjects.clear();
			//std::vector<Cube_Info>().clear(m_vServerObjects);
		}

	}
	catch(std::exception e){
		std::cout << e.what() << std::endl;
	}
	LeaveCriticalSection(&cs_Cube);
}


DirectX::XMFLOAT3 Player_Cube_Color;
DirectX::XMFLOAT3 Get_Player_Cube_Color() { return Player_Cube_Color; }

//
int Get_Now_Time() {
	return now_time;
}

//
bool bConnected = false;

bool Get_Con() {
	return bConnected;
}

void Set_Con(bool bNew_Con) {
	bConnected = bNew_Con;
}

bool Connect_To_Server(const char* sServer_IP)
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
		//적절치 않은 서버, 연결하지 못하게 해야함
		return false;
	}

	// PlayerNumber 받기
	int playerNumber{};
	retval = recv(sock, (char*)&playerNumber, sizeof(playerNumber), 0);
	if (retval == INVALID_SOCKET)
	{
		//DisconnectServer();
		return false;
	}
	SetPlayerNumberAndColor(playerNumber);

}

SOCKET GetKeyInputSocket() { return KeyInputSocket; }

void CreateKeyInputServerSocket(const char* sServer_IP)
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

void CreateCubeServerSocket(const char* sServer_IP)
{
	// cs_for_Cube
	InitializeCriticalSection(&cs_Cube);
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

void CreateRecvPlayerDataSocket(const char* sServer_IP)
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


void SetPlayerNumberAndColor(int pn)
{
	PlayerNumber = pn;
	// 여기에 clear 넣어주세용
	switch (pn % 3)
	{
	case 0:
		Player_Cube_Color = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);	// player 1 cube - red
		return;
	case 1:
		Player_Cube_Color = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);	// player 2 cube - green
		return;
	case 2:
		Player_Cube_Color = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);	// player 3 cube - blue
		return;

	}
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
	bool check_tmp = true;
	while (check_tmp) {
		retval = recv(sock, (char*)&now_time, sizeof(int), MSG_WAITALL);
		if (retval == INVALID_SOCKET)
		{
			//DisconnectServer();
			return -1;
		}
		else if (retval == 0)
			break;
		printf("%d 초라고 시간 받았음\n", now_time);

		std::string s;
		switch (now_time)
		{
		case -1:
			s = "승리자는 0번 입니다.";
			MessageBoxA(hWnd, s.c_str(), "결과", MB_OK);
			check_tmp = false;
			break;
		case -2:
			s = "승리자는 1번 입니다.";
			MessageBoxA(hWnd, s.c_str(), "결과", MB_OK);
			check_tmp = false;
			break;
		case -3:
			s = "승리자는 2번 입니다.";
			MessageBoxA(hWnd, s.c_str(), "결과", MB_OK);
			check_tmp = false;
			break;
		default:
			break;
		}

	}

	// 소켓 닫기
	DisconnectServer();
	// 윈속 종료
	WSACleanup();
	return 0;
}


DWORD WINAPI Get_Cube_Object_From_Server(LPVOID arg)
{
	int retval = 0;
	struct Cube_Info CubeInput;
	while (1) {
		retval = recv(CubeSocket, (char*)&CubeInput, sizeof(Cube_Info), MSG_WAITALL);
		if (retval == INVALID_SOCKET)
		{
			//DisconnectServer();
			return -1;
		}
		else if (retval == 0)
			break;
		
		EnterCriticalSection(&cs_Cube);
		m_vServerObjects.push_back(CubeInput);
		LeaveCriticalSection(&cs_Cube);
	}
	
	DeleteCriticalSection(&cs_Cube);
	// 소켓 닫기
	// 윈속 종료
	WSACleanup();
	return 0;
}

SOCKET GetChatDataSocket() { return ChatDataSocket; }

void CreateChatDataSocket(const char* sServer_IP)
{
	// 소켓 설정
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	ChatDataSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (ChatDataSocket == INVALID_SOCKET) {
		printf("Socket Fail - CreateChatDataSocket(char*)");
		err_quit("Socket Fail - CreateChatDataSocket(char*)");
		return;
	}

	//서버에 연결
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	inet_pton(AF_INET, sServer_IP, &serverAddr.sin_addr);
	serverAddr.sin_port = htons(CHATDATAPORT);
	int retval = connect(ChatDataSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (retval == SOCKET_ERROR) {
		printf("connect Fail - CreateChatDataSocket(char)");
		err_quit("connect Fail - CreateChatDataSocket(char)");
		return;
	}
}




bool ShowChatBox = false;
bool GetShowChatBox() { return ShowChatBox; }
void SetShowChatBox(bool bSet) { ShowChatBox = bSet; }
void AddLastChatData(int playerNumber, std::string);
void Reset_Game();
bool checkCRITICAL;
void DisconnectServer()
{
	mtx.lock();
	//EnterCriticalSection(&cs_Cube);
	if(Get_Con() &&!checkCRITICAL)
	{
		checkCRITICAL = true;
		if (ChatDataSocket != INVALID_SOCKET) closesocket(ChatDataSocket);
		if (KeyInputSocket != INVALID_SOCKET) closesocket(KeyInputSocket);
		if (CubeSocket != INVALID_SOCKET) closesocket(CubeSocket);
		if (sock != INVALID_SOCKET) closesocket(sock);
		if (RecvPlayerDataSocket != INVALID_SOCKET) closesocket(RecvPlayerDataSocket);
		ChatDataSocket = INVALID_SOCKET;
		KeyInputSocket = INVALID_SOCKET;
		CubeSocket = INVALID_SOCKET;
		sock = INVALID_SOCKET;
		RecvPlayerDataSocket = INVALID_SOCKET;
		Set_Con(false);

		Reset_Game();
		AddLastChatData(-1, std::string{"[시스템] 서버와의 연결이 끊어졌습니다."});
		checkCRITICAL = false;
		
	}
	//EnterCriticalSection(&cs_Cube);
	mtx.unlock();
}