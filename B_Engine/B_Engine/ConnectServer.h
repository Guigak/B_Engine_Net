#pragma once
#include <winsock2.h>

bool Connect_To_Server(char* sServer_IP);
void CreateKeyInputServerSocket(char* sServer_IP);

SOCKET GetKeyInputSocket();

void SetPlayerNumber(int pn);
int GetPlayerNumber();


struct KeyInput
{
	int key;
	bool keydown;
};

//int time = 500;				// 서버 시간 변수