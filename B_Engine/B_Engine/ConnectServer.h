#pragma once
#include <winsock2.h>
#include "stdafx.h"
#include "Shader.h"
#include <vector>

#define CHATMAXBUFFER 30

extern HWND hWnd;

bool Connect_To_Server(const char* sServer_IP);
void CreateKeyInputServerSocket(const char* sServer_IP);
void CreateCubeServerSocket(const char* sServer_IP);
void CreateRecvPlayerDataSocket(const char* sServer_IP);
void CreateChatDataSocket(const char* sServer_IP);

// 소켓 Getter
SOCKET GetKeyInputSocket();
SOCKET GetCubeSocket();
SOCKET GetRecvPlayerSocket();
SOCKET GetChatDataSocket();

void SetPlayerNumberAndColor(int pn);
int GetPlayerNumber();

bool GetPlayerBuffer(DWORD key);
void SetPlayerBuffer(DWORD key, bool bSet);

DWORD WINAPI Get_Time(LPVOID arg);		// 시간값 받는 루프
DWORD WINAPI Get_Cube_Object_From_Server(LPVOID arg); // 큐브 생성 스레드

// 서버로 부터 받은 오브젝트 Getter
std::vector<Cube_Info> Get_m_vServerObjects();
// 서버로 부터 받은 오브젝트 Release
void Release_m_vServerObjects();

DirectX::XMFLOAT3 Get_Player_Cube_Color();	// send 하기위한 cube_color getter

// 채팅 박스 getter / setter
bool GetShowChatBox();
void SetShowChatBox(bool bSet);

//
bool Get_Con();
void Set_Con(bool bNew_Con);

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


struct Look_Data
{
	int PlayerNumber;
	float fLook_x, fLook_z;
};

