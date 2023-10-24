#pragma once

#include "stdafx.h"
#include "Timer.h"
#include "Scene.h"
#include "Camera.h"
#include "Player.h"

class CFramework {
private :
	// Window
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	int m_nWndClient_Width;
	int m_nWndClient_Height;

	// Factory, SwapChain, Device
	IDXGIFactory4* m_pdxgi_Factory;
	IDXGISwapChain3* m_pdxgi_SwapChain;
	ID3D12Device* m_pd3d_Device;

	// MSAA
	bool m_bMSAA4x_Enable = false;
	UINT m_nMSAA4x_QualityLevels = 0;

	// SwapChainBuffer
	static const UINT m_nSwapChainBuffers = 2;
	UINT m_nSwapChainBuffer_Index;

	// Rtv
	ID3D12Resource* m_ppd3d_RenderTargetBuffers[m_nSwapChainBuffers];
	ID3D12DescriptorHeap* m_pd3d_Rtv_DescriptorHeap;
	UINT m_nRtv_Descriptor_IncrementSize;

	// Dsv
	ID3D12Resource* m_pd3d_DepthStencilBuffer;
	ID3D12DescriptorHeap* m_pd3d_Dsv_DescriptorHeap;
	UINT m_nDsv_Descriptor_IncrementSize;

	// Command
	ID3D12CommandQueue* m_pd3d_Command_Queue;
	ID3D12CommandAllocator* m_pd3d_Command_Allocator;
	ID3D12GraphicsCommandList* m_pd3d_Command_List;

	// PipelineState
	ID3D12PipelineState* m_pd3d_PipelineState;

	// Fence
	ID3D12Fence* m_pd3d_Fence;
	UINT64 m_pnFence_Value[m_nSwapChainBuffers];
	HANDLE m_hFence_Event;

	// Viewport, ScissorRect	// unused
	//D3D12_VIEWPORT m_d3d_Viewport;
	//D3D12_RECT m_d3d_ScissoerRect;

	// Timer
	CTimer m_Timer;

	_TCHAR m_pcFrameRate[50];

	// Scene
	CScene* m_pScene;

	// Camera
	CCamera* m_pCamera = NULL;

	// picking
	CObject* m_pSelected_Object = NULL;

public :
	CFramework();
	~CFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void Crt_SwapChain();
	void Crt_Rtv_N_Dsv_DescriptorHeaps();
	void Crt_D3D_Device();
	void Crt_Command_Queue_N_List();

	void Crt_Rtv();
	void Crt_Dsv();

	void Build_Objects();
	void Release_Objects();

	void Prcs_Input();
	void Anim_Objects();
	void Adavance_Frame();

	void Wait_4_GPU_Complete();

	void Prcs_Msg_Mouse(HWND hWnd, UINT nMsg_ID, WPARAM wParam, LPARAM lParam);
	void Prcs_Msg_Keyboard(HWND hWnd, UINT nMsg_ID, WPARAM wParam, LPARAM lParam);

	LRESULT CALLBACK Prcs_Msg_Wnd(HWND hWnd, UINT nMsg_ID, WPARAM wParam, LPARAM lParam);

	// full screen
public :
	void Chg_SwapChain_State();

	// about Scene
public :
	void Move_2_Next_Frame();

	// player
public :
	CPlayer* m_pPlayer = NULL;

	// mouse point
	POINT m_ptOld_Cursor_Pos;

	// picking
	void Prcs_Selected_Object(DWORD dwDirection, float fDelta_x, float fDelta_y);
};

