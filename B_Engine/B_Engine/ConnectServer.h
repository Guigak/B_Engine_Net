#pragma once
#include <winsock2.h>

bool Connect_To_Server(char* sServer_IP);
void CreateKeyInputServerSocket(char* sServer_IP);

SOCKET GetKeyInputSocket();

struct KeyInput
{
	int key;
	bool keydown;
};

//int time = 500;				// ���� �ð� ����