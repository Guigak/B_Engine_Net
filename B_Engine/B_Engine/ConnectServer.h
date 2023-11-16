#pragma once
#include <winsock2.h>

bool Connect_To_Server(char* sServer_IP);
void CreateKeyInputServerSocket(char* sServer_IP);

SOCKET GetKeyInputSocket();

void SetPlayerNumber(int pn);
int GetPlayerNumber();

bool GetPlayerBuffer(DWORD key);
void SetPlayerBuffer(DWORD key, bool bSet);

DWORD WINAPI Get_Time(LPVOID arg);		// 시간값 받는 루프

struct KeyInput
{
	int key;
	bool keydown;
	int PlayerNumber;
};
