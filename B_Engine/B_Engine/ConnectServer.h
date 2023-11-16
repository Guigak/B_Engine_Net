#pragma once
#include <winsock2.h>

bool Connect_To_Server(char* sServer_IP);
void CreateKeyInputServerSocket(char* sServer_IP);

SOCKET GetKeyInputSocket();

void SetPlayerNumber(int pn);
int GetPlayerNumber();

bool GetPlayerBuffer(DWORD key);
void SetPlayerBuffer(DWORD key, bool bSet);

DWORD WINAPI Get_Time(LPVOID arg);		// �ð��� �޴� ����

struct KeyInput
{
	int key;
	bool keydown;
	int PlayerNumber;
};
