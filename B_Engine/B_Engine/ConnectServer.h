#pragma once
#include <winsock2.h>
#include "stdafx.h"
#include "Shader.h"
#include <vector>

bool Connect_To_Server(char* sServer_IP);
void CreateKeyInputServerSocket(char* sServer_IP);
void CreateCubeServerSocket(char* sServer_IP);
void CreateRecvPlayerDataSocket(char* sServer_IP);
void CreateSendLookVectorSocket(char* sServer_IP);


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
DWORD WINAPI Add_Cube_Object_From_Server(LPVOID arg); // 큐브 생성 스레드

CObject* Get_m_pServerObjects();		// 서버로 부터 받은 오브젝트 Geter

void Release_m_pServerObjects();		// 생성후 전달 받은 오브젝트 제거

bool Get_AddorDelete_Cube();			// 설치 삭제 bool getter

void Release_AddorDelete_Cube();

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
	bool AddorDelete;
};

struct Look_Data
{
	int PlayerNumber;
	float fLook_x, fLook_z;
};