#pragma once
#include <winsock2.h>

bool Connect_To_Server(char* sServer_IP);
void CreateKeyInputServerSocket(char* sServer_IP);

SOCKET GetKeyInputSocket();

void SetPlayerNumber(int pn);
int GetPlayerNumber();

bool GetPlayerBuffer(DWORD key);
void SetPlayerBuffer(DWORD key, bool bSet);

struct KeyInput
{
	int key;
	bool keydown;
	int PlayerNumber;
};

//int time = 500;				// 서버 시간 변수