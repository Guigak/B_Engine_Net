#include "Framework.h"
#include "ConnectServer.h"

CFramework::CFramework() {
	m_pdxgi_Factory = NULL;
	m_pdxgi_SwapChain = NULL;
	m_pd3d_Device = NULL;

	m_pd3d_Command_Queue = NULL;
	m_pd3d_Command_Allocator = NULL;
	m_pd3d_Command_List = NULL;

	m_pd3d_PipelineState = NULL;

	for (int i = 0; i < m_nSwapChainBuffers; ++i) {
		m_ppd3d_RenderTargetBuffers[i] = NULL;
	}
	m_pd3d_Rtv_DescriptorHeap = NULL;
	m_nRtv_Descriptor_IncrementSize = 0;

	m_pd3d_DepthStencilBuffer = NULL;
	m_pd3d_Dsv_DescriptorHeap = NULL;
	m_nDsv_Descriptor_IncrementSize = 0;

	m_nSwapChainBuffer_Index = 0;

	m_hFence_Event = NULL;
	m_pd3d_Fence = NULL;
	for (int i = 0; i < m_nSwapChainBuffers; ++i) {
		m_pnFence_Value[i] = 0;
	}

	m_pScene = NULL;

	//m_d3d_Viewport = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	//m_d3d_ScissoerRect = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };

	m_nWndClient_Width = FRAME_BUFFER_WIDTH;
	m_nWndClient_Height = FRAME_BUFFER_HEIGHT;

	_tcscpy_s(m_pcFrameRate, _T("B_Engine ("));
}

CFramework::~CFramework() {

}

bool CFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd) {
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	Crt_D3D_Device();
	Crt_Command_Queue_N_List();
	Crt_Rtv_N_Dsv_DescriptorHeaps();
	Crt_SwapChain();
	Crt_Dsv();

	Build_Objects();

	return true;
}

void CFramework::OnDestroy() {
	Wait_4_GPU_Complete();

	Release_Objects();

	CloseHandle(m_hFence_Event);

	for (int i = 0; i < m_nSwapChainBuffers; ++i) {
		if (m_ppd3d_RenderTargetBuffers[i]) {
			m_ppd3d_RenderTargetBuffers[i]->Release();
		}
	}
	if (m_pd3d_Rtv_DescriptorHeap) {
		m_pd3d_Rtv_DescriptorHeap->Release();
	}

	if (m_pd3d_DepthStencilBuffer) {
		m_pd3d_DepthStencilBuffer->Release();
	}
	if (m_pd3d_Dsv_DescriptorHeap) {
		m_pd3d_Dsv_DescriptorHeap->Release();
	}

	if (m_pd3d_Command_Allocator) {
		m_pd3d_Command_Allocator->Release();
	}
	if (m_pd3d_Command_Queue) {
		m_pd3d_Command_Queue->Release();
	}
	if (m_pd3d_Command_List) {
		m_pd3d_Command_List->Release();
	}

	if (m_pd3d_PipelineState) {
		m_pd3d_PipelineState->Release();
	}

	if (m_pd3d_Fence) {
		m_pd3d_Fence->Release();
	}

	m_pdxgi_SwapChain->SetFullscreenState(FALSE, NULL);
	if (m_pdxgi_SwapChain) {
		m_pdxgi_SwapChain->Release();
	}
	if (m_pd3d_Device) {
		m_pd3d_Device->Release();
	}
	if (m_pdxgi_Factory) {
		m_pdxgi_Factory->Release();
	}

//#if defined(_DEBUG)
//	IDXGIDebug1* pdxgi_Debug = NULL;
//	DXGIGetDebugInterface1(0, __uuidof(IDXGIDebug1), (void**)&pdxgi_Debug);
//	HRESULT hResult = pdxgi_Debug->ReportLiveObjects/(DXGI_DEBUG_ALL, /DXGI_DEBUG_RLO_DETAIL);
//	pdxgi_Debug->Release();
//#endif
}

void CFramework::Crt_SwapChain() {
	RECT rcClient;
	GetClientRect(m_hWnd, &rcClient);
	m_nWndClient_Width = rcClient.right - rcClient.left;
	m_nWndClient_Height = rcClient.bottom - rcClient.top;

	DXGI_SWAP_CHAIN_DESC dxgi_SwapChain_Desc;
	ZeroMemory(&dxgi_SwapChain_Desc, sizeof(DXGI_SWAP_CHAIN_DESC));
	dxgi_SwapChain_Desc.BufferCount = m_nSwapChainBuffers;
	dxgi_SwapChain_Desc.BufferDesc.Width = m_nWndClient_Width;
	dxgi_SwapChain_Desc.BufferDesc.Height = m_nWndClient_Height;
	dxgi_SwapChain_Desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgi_SwapChain_Desc.BufferDesc.RefreshRate.Numerator = 60;
	dxgi_SwapChain_Desc.BufferDesc.RefreshRate.Denominator = 1;
	dxgi_SwapChain_Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgi_SwapChain_Desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgi_SwapChain_Desc.OutputWindow = m_hWnd;
	dxgi_SwapChain_Desc.SampleDesc.Count = (m_bMSAA4x_Enable) ? 4 : 1;
	dxgi_SwapChain_Desc.SampleDesc.Quality = (m_bMSAA4x_Enable) ? (m_nMSAA4x_QualityLevels - 1) : 0;
	dxgi_SwapChain_Desc.Windowed = TRUE;
	dxgi_SwapChain_Desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hResult = m_pdxgi_Factory->CreateSwapChain(m_pd3d_Command_Queue, &dxgi_SwapChain_Desc, (IDXGISwapChain**)&m_pdxgi_SwapChain);
	m_nSwapChainBuffer_Index = m_pdxgi_SwapChain->GetCurrentBackBufferIndex();

	if (FAILED(hResult)) {
		OutputDebugStringA("스왑체인 에러\n");
	}

	hResult = m_pdxgi_Factory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);

#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE
	Crt_Rtv();
#endif

	//DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgi_SwapChain_FullScreen_Desc;
	//ZeroMemory(&dxgi_SwapChain_FullScreen_Desc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	//dxgi_SwapChain_FullScreen_Desc.RefreshRate.Numerator = 60;
	//dxgi_SwapChain_FullScreen_Desc.RefreshRate.Denominator = 1;
	//dxgi_SwapChain_FullScreen_Desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//dxgi_SwapChain_FullScreen_Desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	//dxgi_SwapChain_FullScreen_Desc.Windowed = TRUE;

	//m_pdxgi_Factory->CreateSwapChainForHwnd(m_pd3d_Command_Queue, m_hWnd, &dxgi_SwapChain_Desc, &dxgi_SwapChain_FullScreen_Desc, NULL, (IDXGISwapChain1**)&m_pdxgi_SwapChain);

	//m_pdxgi_Factory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);

	//m_nSwapChainBuffer_Index = m_pdxgi_SwapChain->GetCurrentBackBufferIndex();
}

void CFramework::Crt_D3D_Device() {
	HRESULT hResult;

	UINT ndxgi_Factory_Flags = 0;



	hResult = CreateDXGIFactory2(ndxgi_Factory_Flags, __uuidof(IDXGIFactory4), (void**)&m_pdxgi_Factory);

	if (FAILED(hResult)) {
		OutputDebugStringA("팩토리 에러\n");
	}

	IDXGIAdapter1* pd3d_Adapter = NULL;

	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgi_Factory->EnumAdapters1(i, &pd3d_Adapter); ++i) {
		DXGI_ADAPTER_DESC1 dxgi_Adapter_Desc;
		pd3d_Adapter->GetDesc1(&dxgi_Adapter_Desc);

		if (dxgi_Adapter_Desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
			continue;
		}
		if (SUCCEEDED(D3D12CreateDevice(pd3d_Adapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&m_pd3d_Device))) {
			break;
		}
	}

	if (!pd3d_Adapter) {
		m_pdxgi_Factory->EnumWarpAdapter(__uuidof(IDXGIAdapter1), (void**)&pd3d_Adapter);
		D3D12CreateDevice(pd3d_Adapter, D3D_FEATURE_LEVEL_12_0, __uuidof(ID3D12Device), (void**)&m_pd3d_Device);
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3d_MSAA_Quality_Levels;
	d3d_MSAA_Quality_Levels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3d_MSAA_Quality_Levels.SampleCount = 4;
	d3d_MSAA_Quality_Levels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3d_MSAA_Quality_Levels.NumQualityLevels = 0;
	m_pd3d_Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &d3d_MSAA_Quality_Levels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMSAA4x_QualityLevels = d3d_MSAA_Quality_Levels.NumQualityLevels;
	m_bMSAA4x_Enable = (m_nMSAA4x_QualityLevels > 1) ? true : false;

	hResult = m_pd3d_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&m_pd3d_Fence);
	for (int i = 0; i < m_nSwapChainBuffers; ++i) {
		m_pnFence_Value[i] = 0;
	}

	if (FAILED(hResult)) {
		OutputDebugStringA("펜스 생성 에러\n");
	}

	m_hFence_Event = CreateEvent(NULL, FALSE, FALSE, NULL);

	//m_d3d_Viewport.TopLeftX = 0;
	//m_d3d_Viewport.TopLeftY = 0;
	//m_d3d_Viewport.Width = static_cast<float>(m_nWndClient_Width);
	//m_d3d_Viewport.Height = static_cast<float>(m_nWndClient_Height);
	//m_d3d_Viewport.MinDepth = 0.0f;
	//m_d3d_Viewport.MaxDepth = 0.0f;

	//m_d3d_ScissoerRect = { 0, 0, m_nWndClient_Width, m_nWndClient_Height };

	if (pd3d_Adapter) {
		pd3d_Adapter->Release();
	}
}

void CFramework::Crt_Command_Queue_N_List() {
	D3D12_COMMAND_QUEUE_DESC d3d_Command_Queue_Desc;
	ZeroMemory(&d3d_Command_Queue_Desc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	d3d_Command_Queue_Desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3d_Command_Queue_Desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	HRESULT hResult = m_pd3d_Device->CreateCommandQueue(&d3d_Command_Queue_Desc, __uuidof(ID3D12CommandQueue), (void**)&m_pd3d_Command_Queue);

	if (FAILED(hResult)) {
		OutputDebugStringA("커맨드 큐 생성 에러\n");
	}

	hResult = m_pd3d_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&m_pd3d_Command_Allocator);

	if (FAILED(hResult)) {
		OutputDebugStringA("커맨드 알로케이터 생성 에러\n");
	}

	hResult = m_pd3d_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_pd3d_Command_Allocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&m_pd3d_Command_List);

	if (FAILED(hResult)) {
		OutputDebugStringA("커맨드 리스트 생성 에러\n");
	}

	hResult = m_pd3d_Command_List->Close();

	if (FAILED(hResult)) {
		OutputDebugStringA("커맨드 리스트 클로즈 에러\n");
	}
}

void CFramework::Crt_Rtv_N_Dsv_DescriptorHeaps() {
	D3D12_DESCRIPTOR_HEAP_DESC d3d_DescriptorHeap_Desc;
	ZeroMemory(&d3d_DescriptorHeap_Desc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3d_DescriptorHeap_Desc.NumDescriptors = m_nSwapChainBuffers;
	d3d_DescriptorHeap_Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3d_DescriptorHeap_Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3d_DescriptorHeap_Desc.NodeMask = 0;
	HRESULT hResult = m_pd3d_Device->CreateDescriptorHeap(&d3d_DescriptorHeap_Desc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3d_Rtv_DescriptorHeap);
	m_nRtv_Descriptor_IncrementSize = m_pd3d_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	if (FAILED(hResult)) {
		OutputDebugStringA("디스크립터 힙 생성 에러(RTV)\n");
	}

	d3d_DescriptorHeap_Desc.NumDescriptors = 1;
	d3d_DescriptorHeap_Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3d_Device->CreateDescriptorHeap(&d3d_DescriptorHeap_Desc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3d_Dsv_DescriptorHeap);
	m_nDsv_Descriptor_IncrementSize = m_pd3d_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	if (FAILED(hResult)) {
		OutputDebugStringA("디스크립터 힙 생성 에러(DSV)\n");
	}
}

void CFramework::Crt_Rtv() {
	D3D12_CPU_DESCRIPTOR_HANDLE d3d_Rtv_CPU_Descriptor_Handle =	m_pd3d_Rtv_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	for (UINT i = 0; i < m_nSwapChainBuffers; ++i) {
		m_pdxgi_SwapChain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&m_ppd3d_RenderTargetBuffers[i]);
		m_pd3d_Device->CreateRenderTargetView(m_ppd3d_RenderTargetBuffers[i], NULL, d3d_Rtv_CPU_Descriptor_Handle);
		d3d_Rtv_CPU_Descriptor_Handle.ptr += m_nRtv_Descriptor_IncrementSize;
	}
}

void CFramework::Crt_Dsv() {
	D3D12_RESOURCE_DESC d3d_Resource_Desc;
	d3d_Resource_Desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3d_Resource_Desc.Alignment = 0;
	d3d_Resource_Desc.Width = m_nWndClient_Width;
	d3d_Resource_Desc.Height = m_nWndClient_Height;
	d3d_Resource_Desc.DepthOrArraySize = 1;
	d3d_Resource_Desc.MipLevels = 1;
	d3d_Resource_Desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3d_Resource_Desc.SampleDesc.Count = (m_bMSAA4x_Enable) ? 4 : 1;
	d3d_Resource_Desc.SampleDesc.Quality = (m_bMSAA4x_Enable) ? (m_nMSAA4x_QualityLevels - 1) : 0;
	d3d_Resource_Desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3d_Resource_Desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3d_Heap_Properties;
	ZeroMemory(&d3d_Heap_Properties, sizeof(D3D12_HEAP_PROPERTIES));
	d3d_Heap_Properties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3d_Heap_Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3d_Heap_Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3d_Heap_Properties.CreationNodeMask = 1;
	d3d_Heap_Properties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3d_Clear_Value;
	d3d_Clear_Value.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3d_Clear_Value.DepthStencil.Depth = 1.0f;
	d3d_Clear_Value.DepthStencil.Stencil = 0;
	m_pd3d_Device->CreateCommittedResource(&d3d_Heap_Properties, D3D12_HEAP_FLAG_NONE, &d3d_Resource_Desc
		, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3d_Clear_Value, __uuidof(ID3D12Resource), (void**)&m_pd3d_DepthStencilBuffer);

	D3D12_CPU_DESCRIPTOR_HANDLE d3d_Dsv_CPU_Descriptor_Handle = m_pd3d_Dsv_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3d_Device->CreateDepthStencilView(m_pd3d_DepthStencilBuffer, NULL, d3d_Dsv_CPU_Descriptor_Handle);
}

void CFramework::Build_Objects() {
	m_pd3d_Command_List->Reset(m_pd3d_Command_Allocator, NULL);

	m_pScene = new CScene();
	m_pScene->Build_Objects(m_pd3d_Device, m_pd3d_Command_List);

	CCube_Player* pCube_Player = new CCube_Player(m_pd3d_Device, m_pd3d_Command_List, m_pScene->Get_Graphics_RootSignature());
	m_pPlayer = pCube_Player;
	m_pCamera = m_pPlayer->Get_Camera();

	m_pd3d_Command_List->Close();
	ID3D12CommandList* ppd3d_Command_Lists[] = { m_pd3d_Command_List };
	m_pd3d_Command_Queue->ExecuteCommandLists(1, ppd3d_Command_Lists);

	Wait_4_GPU_Complete();

	if (m_pScene) {
		m_pScene->Release_Upload_Buffers();
	}

	m_Timer.Reset();
}

void CFramework::Release_Objects() {
	if (m_pScene) {
		m_pScene->Release_Objects();
		delete m_pScene;
	}
	
	if (m_pPlayer) {
		m_pPlayer->Release();
	}
}

void CFramework::Prcs_Input() {
	static UCHAR pKey_Buffer[256];
	DWORD dwDirection = 0;

	if (GetKeyboardState(pKey_Buffer)) {
		if (pKey_Buffer[VK_W] & 0xF0) {
			dwDirection |= DIRECT_FORWARD;
		}
		if (pKey_Buffer[VK_S] & 0xF0) {
			dwDirection |= DIRECT_BACKWARD;
		}
		if (pKey_Buffer[VK_A] & 0xF0) {
			dwDirection |= DIRECT_LEFT;
		}
		if (pKey_Buffer[VK_D] & 0xF0) {
			dwDirection |= DIRECT_RIGHT;
		}
		if (pKey_Buffer[VK_SPACE] & 0xF0) {
			dwDirection |= DIRECT_UP;
		}
		if (pKey_Buffer[VK_LSHIFT] & 0xF0) {
			dwDirection |= DIRECT_DOWN;
		}
	}

	float cxDelta = 0.0f;
	float cyDelta = 0.0f;
	POINT ptCursor_Pos;

	if (GetCapture() == m_hWnd) {
		SetCursor(NULL);
		GetCursorPos(&ptCursor_Pos);

		cxDelta = (float)(ptCursor_Pos.x - m_ptOld_Cursor_Pos.x) / 3.0f;
		cyDelta = (float)(ptCursor_Pos.y - m_ptOld_Cursor_Pos.y) / 3.0f;

		SetCursorPos(m_ptOld_Cursor_Pos.x, m_ptOld_Cursor_Pos.y);
	}

	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f)) {
		if (cxDelta || cyDelta) {
			if (pKey_Buffer[VK_RBUTTON] & 0xF0) {
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
			}
			else {
				m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
			}
		}
		if (dwDirection) {
			if (!Get_Con() && !GetShowChatBox()) {
				m_pPlayer->Move(dwDirection, PLAYER_MOVE_DISTANCE * m_Timer.Get_Elapsed_Time(), true);
			}
		}
	}

	m_pPlayer->Update(m_Timer.Get_Elapsed_Time());
}

void CFramework::Anim_Objects() {
	if (m_pScene) {
		m_pScene->Anim_Objects(m_Timer.Get_Elapsed_Time());
	}

	//
	Chk_Collision_Player_N_Cube();

	m_pCamera->Udt_Shader_Variables(m_pd3d_Command_List);
}


//#define _WITH_PLAYER_TOP
void CFramework::Adavance_Frame() {
	m_Timer.Tick(100.0f);

	if (Get_Con()) {
		// RecvMyLookVectorToServer
		RecvMyLookVectorToServer();

		// GetAllPlayerData
		// 플레이어 위치 동기화
		GetAllPlayerData(m_pPlayer);

		// 받은 박스 정보 설치 및 삭제
		m_pScene->Check_Cube_Object_4_Server(Get_m_vServerObjects());
	}else
	{
		Prcs_Input();
	}


	
	Anim_Objects();

	HRESULT hResult = m_pd3d_Command_Allocator->Reset();
	hResult = m_pd3d_Command_List->Reset(m_pd3d_Command_Allocator, NULL);

	if (FAILED(hResult)) {
		OutputDebugStringA("커맨드 리스트 리셋 에러\n");
	}

	D3D12_RESOURCE_BARRIER d3d_ResourceBarrier;
	ZeroMemory(&d3d_ResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3d_ResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3d_ResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3d_ResourceBarrier.Transition.pResource = m_ppd3d_RenderTargetBuffers[m_nSwapChainBuffer_Index];
	d3d_ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	d3d_ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3d_ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3d_Command_List->ResourceBarrier(1, &d3d_ResourceBarrier);

	D3D12_CPU_DESCRIPTOR_HANDLE d3d_Rtv_CPU_Descriptor_Handle = m_pd3d_Rtv_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3d_Rtv_CPU_Descriptor_Handle.ptr += (m_nSwapChainBuffer_Index * m_nRtv_Descriptor_IncrementSize);

	float pfClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	m_pd3d_Command_List->ClearRenderTargetView(d3d_Rtv_CPU_Descriptor_Handle, pfClearColor, 0, NULL);

	D3D12_CPU_DESCRIPTOR_HANDLE d3d_Dsv_CPU_Descriptor_Handle = m_pd3d_Dsv_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3d_Command_List->ClearDepthStencilView(d3d_Dsv_CPU_Descriptor_Handle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);

	m_pd3d_Command_List->OMSetRenderTargets(1, &d3d_Rtv_CPU_Descriptor_Handle, TRUE, &d3d_Dsv_CPU_Descriptor_Handle);

	// render
	if (m_pScene) {
		m_pScene->Render(m_pd3d_Command_List, m_pCamera);
	}

#ifdef _WITH_PLAYER_TOP
	m_pd3d_Command_List->ClearDepthStencilView(d3d_Dsv_CPU_Descriptor_Handle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif

	if (m_pPlayer) {
		m_pPlayer->Render(m_pd3d_Command_List, m_pCamera);
	}

	d3d_ResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3d_ResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3d_ResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3d_Command_List->ResourceBarrier(1, &d3d_ResourceBarrier);

	hResult = m_pd3d_Command_List->Close();

	if (FAILED(hResult)) {
		OutputDebugStringA("커맨드 리스트 클로즈 에러\n");
	}

	ID3D12CommandList* ppd3d_Command_Lists[] = { m_pd3d_Command_List };
	m_pd3d_Command_Queue->ExecuteCommandLists(1, ppd3d_Command_Lists);

	Wait_4_GPU_Complete();

	m_pdxgi_SwapChain->Present(0, 0);

	Move_2_Next_Frame();

	m_Timer.Get_FrameRate(m_pcFrameRate + 10, 37);
	SetWindowText(m_hWnd, m_pcFrameRate);
}

void CFramework::Wait_4_GPU_Complete() {
	UINT64 nFence_Value = ++m_pnFence_Value[m_nSwapChainBuffer_Index];
	HRESULT hResult = m_pd3d_Command_Queue->Signal(m_pd3d_Fence, nFence_Value);

	if (m_pd3d_Fence->GetCompletedValue() < nFence_Value) {
		hResult = m_pd3d_Fence->SetEventOnCompletion(nFence_Value, m_hFence_Event);
		WaitForSingleObject(m_hFence_Event, INFINITE);
	}

	if (FAILED(hResult)) {
		OutputDebugStringA("펜스 에러\n");
	}
}

void CFramework::Prcs_Msg_Mouse(HWND hWnd, UINT nMsg_ID, WPARAM wParam, LPARAM lParam) {
	switch (nMsg_ID) {
	case WM_LBUTTONDOWN :
		if (m_bCaptured) {
			m_pScene->Delete_Cube_Object(FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 2, m_pCamera);
		}
		else {
			SetCapture(hWnd);
			GetCursorPos(&m_ptOld_Cursor_Pos);

			m_bCaptured = true;
		}
		break;
	case WM_RBUTTONDOWN :
		if (m_bCaptured) {
			m_pScene->Add_Cube_Object(FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 2, m_pCamera);
		}
		break;
	case WM_LBUTTONUP :
	case WM_RBUTTONUP :
		break;
	case WM_MOUSEMOVE :
		m_pSelected_Object = m_pScene->Pick_Object_Pointed_By_Cursor(FRAME_BUFFER_WIDTH / 2, FRAME_BUFFER_HEIGHT / 2, m_pCamera);
		break;
	default :
		break;
	}
}

void CFramework::Prcs_Msg_Keyboard(HWND hWnd, UINT nMsg_ID, WPARAM wParam, LPARAM lParam) {
	if(((nMsg_ID==WM_KEYDOWN&&!GetPlayerBuffer(wParam)) || (nMsg_ID==WM_KEYUP&&GetPlayerBuffer(wParam))) && !GetShowChatBox())
	{
		KeyInput senddata;
		if (nMsg_ID == WM_KEYDOWN)
		{
			senddata.keydown = true;
			SetPlayerBuffer(wParam, true);
		}
		else
		{
			senddata.keydown = false;
			SetPlayerBuffer(wParam, false);
		}
		senddata.key = (int)wParam;
		senddata.PlayerNumber = GetPlayerNumber();
		send(GetKeyInputSocket(), (const char*)&senddata, sizeof(senddata), 0);
	}
	switch (nMsg_ID) {
	case WM_KEYUP :
		switch (wParam) {
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
		case VK_F1:
		//case VK_F2:
		//case VK_F3:
			if (m_pPlayer) {
				m_pCamera = m_pPlayer->Chg_Camera(((DWORD)wParam - VK_F1 + 1), m_Timer.Get_Elapsed_Time());
			}

			break;
		case VK_F8:
			break;
		case VK_F9:
			Chg_SwapChain_State();
			m_bCaptured = false;
			break;
		case VK_TAB:
			if (m_bCaptured) {
				ReleaseCapture();

				m_bCaptured = false;
			}
			break;
		default:
			break;
		}
		break;
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_F8:
			m_bCaptured = false;
			break;
		default:
			break;
		}
		break;
	default :
		break;
	}
}

LRESULT CALLBACK CFramework::Prcs_Msg_Wnd(HWND hWnd, UINT nMsg_ID, WPARAM wParam, LPARAM lParam) {
	switch (nMsg_ID) {
	case WM_SIZE :
		m_nWndClient_Width = LOWORD(lParam);
		m_nWndClient_Height = HIWORD(lParam);
		break;
	case WM_LBUTTONDOWN :
	case WM_RBUTTONDOWN :
	case WM_LBUTTONUP :
	case WM_RBUTTONUP :
	case WM_MOUSEMOVE :
		Prcs_Msg_Mouse(hWnd, nMsg_ID, wParam, lParam);
		break;
	case WM_KEYDOWN :
	case WM_KEYUP :
		Prcs_Msg_Keyboard(hWnd, nMsg_ID, wParam, lParam);
		break;
	}

	return 0;
}

void CFramework::Chg_SwapChain_State() {
	Wait_4_GPU_Complete();

	BOOL bFullScreen_State = FALSE;
	m_pdxgi_SwapChain->GetFullscreenState(&bFullScreen_State, NULL);
	m_pdxgi_SwapChain->SetFullscreenState(!bFullScreen_State, NULL);

	DXGI_MODE_DESC dxgi_Target_Parameters;
	dxgi_Target_Parameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgi_Target_Parameters.Width = m_nWndClient_Width;
	dxgi_Target_Parameters.Height = m_nWndClient_Height;
	dxgi_Target_Parameters.RefreshRate.Numerator = 60;
	dxgi_Target_Parameters.RefreshRate.Denominator = 1;
	dxgi_Target_Parameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgi_Target_Parameters.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_pdxgi_SwapChain->ResizeTarget(&dxgi_Target_Parameters);

	for (int i = 0; i < m_nSwapChainBuffers; ++i) {
		if (m_ppd3d_RenderTargetBuffers[i]) {
			m_ppd3d_RenderTargetBuffers[i]->Release();
		}
	}

	DXGI_SWAP_CHAIN_DESC dxgi_SwapChain_Desc;
	m_pdxgi_SwapChain->GetDesc(&dxgi_SwapChain_Desc);
	m_pdxgi_SwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClient_Width, m_nWndClient_Height, dxgi_SwapChain_Desc.BufferDesc.Format, dxgi_SwapChain_Desc.Flags);

	m_nSwapChainBuffer_Index = m_pdxgi_SwapChain->GetCurrentBackBufferIndex();

	Crt_Rtv();
}

void CFramework::Move_2_Next_Frame() {
	m_nSwapChainBuffer_Index = m_pdxgi_SwapChain->GetCurrentBackBufferIndex();

	Wait_4_GPU_Complete();
}

void CFramework::Prcs_Selected_Object(DWORD dwDirection, float fDelta_x, float fDelta_y) {
	if (dwDirection != 0) {
		if (dwDirection & DIRECT_FORWARD) {
			m_pSelected_Object->Move_Forward(1.0f);
		}
		if (dwDirection & DIRECT_BACKWARD) {
			m_pSelected_Object->Move_Forward(-1.0f);
		}
		if (dwDirection & DIRECT_LEFT) {
			m_pSelected_Object->Move_Right(-1.0f);
		}
		if (dwDirection & DIRECT_RIGHT) {
			m_pSelected_Object->Move_Right(1.0f);
		}
		if (dwDirection & DIRECT_UP) {
			m_pSelected_Object->Move_Up(1.0f);
		}
		if (dwDirection & DIRECT_DOWN) {
			m_pSelected_Object->Move_Up(-1.0f);
		}
	}
	else if ((fDelta_x != 0.0f) || (fDelta_y != 0.0f)) {
		m_pSelected_Object->Rotate(fDelta_y, fDelta_x, 0.0f);
	}
}

void CFramework::Chk_Collision_Player_N_Cube() {
	if (!m_pPlayer) {
		return;
	}

	CObject** ppCube_Objects = m_pScene->Get_Objects_From_Shader(0);	// 0 : Cube Shader Index
	int nObjects = m_pScene->Get_Object_Num_From_Shader(0);

	if (ppCube_Objects == NULL) {
		return;
	}
	
	m_pPlayer->Udt_N_Prcs_Collision(ppCube_Objects, nObjects);
}

void CFramework::Clr_Cube_Objects() {
	m_pScene->Clr_Cube_Objects();
}

void CFramework::RecvMyLookVectorToServer()
{
	if(m_pCamera)
	{
		struct Look_Data data;
		data.PlayerNumber = GetPlayerNumber();
		data.fLook_x = m_pPlayer->Get_Look().x;
		data.fLook_z = m_pPlayer->Get_Look().z;
		send(GetSendLookVectorSocket(), (char*)&data, sizeof(struct Look_Data), 0);
	}
	
}

void CFramework::GetAllPlayerData(CPlayer* m_pPlayer)
{
	if(m_pScene)
	{
		m_pScene->GetAllPlayerData(m_pPlayer);
	}
}
