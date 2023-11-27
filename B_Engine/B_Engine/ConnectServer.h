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


// ���� Getter
SOCKET GetKeyInputSocket();
SOCKET GetCubeSocket();
SOCKET GetRecvPlayerSocket();
SOCKET GetSendLookVectorSocket();

void SetPlayerNumber(int pn);
int GetPlayerNumber();

bool GetPlayerBuffer(DWORD key);
void SetPlayerBuffer(DWORD key, bool bSet);

DWORD WINAPI Get_Time(LPVOID arg);		// �ð��� �޴� ����
DWORD WINAPI Add_Cube_Object_From_Server(LPVOID arg); // ť�� ���� ������

CObject* Get_m_pServerObjects();		// ������ ���� ���� ������Ʈ Geter

void Release_m_pServerObjects();		// ������ ���� ���� ������Ʈ ����

bool Get_AddorDelete_Cube();			// ��ġ ���� bool getter

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