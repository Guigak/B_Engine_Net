#pragma once

#include "Timer.h"
#include "Shader.h"
#include "Object.h"

class CScene {
protected :
	//CShader** m_ppShaders = NULL;
	//int m_nShaders = 0;

	//CObject** m_ppObjects = NULL;
	//int m_nObjects = 0;

	CObjects_Shader* m_pShaders = NULL;
	int m_nShaders = 0;

	ID3D12RootSignature* m_pd3d_Graphics_RootSignature = NULL;

public :
	CScene();
	~CScene();

	bool Prcs_Msg_Mouse(HWND hWnd, UINT nMsg_ID, WPARAM wParam, LPARAM lParam);
	bool Prcs_Msg_Keyboard(HWND hWnd, UINT nMsg_ID, WPARAM wParam, LPARAM lParam);

	void Build_Objects(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List);
	void Release_Objects();

	bool Prcs_Input();
	void Anim_Objects(float fElapsed_Time);

	//void Prepare_Render(ID3D12GraphicsCommandList* pd3d_Command_List);	// unused
	void Render(ID3D12GraphicsCommandList* pd3d_Command_List, CCamera* pCamera);

	void Release_Upload_Buffers();

	ID3D12RootSignature* Crt_Graphics_RootSignature(ID3D12Device* pd3d_Device);
	ID3D12RootSignature* Get_Graphics_RootSignature();

	//void Crt_Graphics_Pipeline_State(ID3D12Device* pd3d_Device);	// unused
	//ID3D12PipelineState* m_pd3d_Graphics_Pipeline_State = NULL;	//unused

	//
	CObject* Pick_Object_Pointed_By_Cursor(int Client_x, int Client_y, CCamera* pCamera);

	void Delete_Cube_Object(int Client_x, int Client_y, CCamera* pCamera);
	void Add_Cube_Object(int Client_x, int Client_y, CCamera* pCamera);
};

