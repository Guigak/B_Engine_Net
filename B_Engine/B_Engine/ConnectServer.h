#pragma once
#include <winsock2.h>
#include "stdafx.h"

bool Connect_To_Server(char* sServer_IP);
void CreateKeyInputServerSocket(char* sServer_IP);
void CreateCubeServerSocket(char* sServer_IP);
void CreateRecvPlayerDataSocket(char* sServer_IP);
void CreateSendLookVectorSocket(char* sServer_IP);

//void Send_Cube_Info(DirectX::XMFLOAT3 xmf3_Position, DirectX::XMFLOAT4 m_xmf4_Color);

// 소켓 Getter
SOCKET GetKeyInputSocket();
SOCKET GetCubeSocket();
SOCKET GetRecvPlayerSocket();
SOCKET GetSendLookVectorSocket();

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

struct Player_Info
{
	float fPosition_x, fPosition_y, fPosition_z;
	float fLook_x, fLook_z;
};

struct Cube_Info {
	float fPosition_x, fPosition_y, fPosition_z;
	float fColor_r, fColor_g, fColor_b;
};

struct Look_Data
{
	int PlayerNumber;
	float fLook_x, fLook_z;
};