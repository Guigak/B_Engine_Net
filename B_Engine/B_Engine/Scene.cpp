#include "Scene.h"

#include "ConnectServer.h"

CScene::CScene() {
}

CScene::~CScene() {
}

bool CScene::Prcs_Msg_Mouse(HWND hWnd, UINT nMsg_ID, WPARAM wParam, LPARAM lParam) {
	return false;
}

bool CScene::Prcs_Msg_Keyboard(HWND hWnd, UINT nMsg_ID, WPARAM wParam, LPARAM lParam) {
	return false;
}

ID3D12RootSignature* CScene::Crt_Graphics_RootSignature(ID3D12Device* pd3d_Device) {
	ID3D12RootSignature* pd3d_Graphics_RootSignature = NULL;

	D3D12_ROOT_PARAMETER pd3d_RootParameters[4];
	pd3d_RootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3d_RootParameters[0].Constants.Num32BitValues = 16;
	pd3d_RootParameters[0].Constants.ShaderRegister = 0;
	pd3d_RootParameters[0].Constants.RegisterSpace = 0;
	pd3d_RootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3d_RootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3d_RootParameters[1].Constants.Num32BitValues = 32;
	pd3d_RootParameters[1].Constants.ShaderRegister = 1;
	pd3d_RootParameters[1].Constants.RegisterSpace = 0;
	pd3d_RootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3d_RootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pd3d_RootParameters[2].Descriptor.ShaderRegister = 0;
	pd3d_RootParameters[2].Descriptor.RegisterSpace = 0;
	pd3d_RootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	//
	pd3d_RootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3d_RootParameters[3].Constants.Num32BitValues = 1;
	pd3d_RootParameters[3].Constants.ShaderRegister = 2;
	pd3d_RootParameters[3].Constants.RegisterSpace = 0;
	pd3d_RootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	D3D12_ROOT_SIGNATURE_FLAGS d3d_RootSignature_Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC d3d_RootSignature_Desc;
	ZeroMemory(&d3d_RootSignature_Desc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3d_RootSignature_Desc.NumParameters = _countof(pd3d_RootParameters);
	d3d_RootSignature_Desc.pParameters = pd3d_RootParameters;
	d3d_RootSignature_Desc.NumStaticSamplers = 0;
	d3d_RootSignature_Desc.pStaticSamplers = NULL;
	d3d_RootSignature_Desc.Flags = d3d_RootSignature_Flags;

	HRESULT hResult;
	ID3DBlob* pd3d_Signature_Blob = NULL;
	ID3DBlob* pd3d_Error_Blob = NULL;

	hResult = D3D12SerializeRootSignature(&d3d_RootSignature_Desc, D3D_ROOT_SIGNATURE_VERSION_1, &pd3d_Signature_Blob, &pd3d_Error_Blob);
	pd3d_Device->CreateRootSignature(0, pd3d_Signature_Blob->GetBufferPointer(), pd3d_Signature_Blob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void**)&pd3d_Graphics_RootSignature);

	// print error
	if (FAILED(hResult)) {
		OutputDebugStringA("루트 시그니처 에러\n");
		OutputDebugStringA(static_cast<const char*>(pd3d_Error_Blob->GetBufferPointer()));
	}

	if (pd3d_Signature_Blob) {
		pd3d_Signature_Blob->Release();
	}
	if (pd3d_Error_Blob) {
		pd3d_Error_Blob->Release();
	}

	return pd3d_Graphics_RootSignature;
}

ID3D12RootSignature* CScene::Get_Graphics_RootSignature() {	// unused
	return m_pd3d_Graphics_RootSignature;
}

CObject* CScene::Pick_Object_Pointed_By_Cursor(int Client_x, int Client_y, CCamera* pCamera) {
	if (!pCamera) {
		return NULL;
	}

	DirectX::XMFLOAT4X4 xmf4x4_View = pCamera->Get_View_Matrix();
	DirectX::XMFLOAT4X4 xmf4x4_Projection = pCamera->Get_Projection_Matrix();
	D3D12_VIEWPORT d3d_Viewport = pCamera->Get_ViewPort();

	DirectX::XMFLOAT3 xmf3_Pick_Position;
	xmf3_Pick_Position.x = (((2.0f * Client_x) / d3d_Viewport.Width) - 1) / xmf4x4_Projection._11;
	xmf3_Pick_Position.y = -(((2.0f * Client_y) / d3d_Viewport.Height) - 1) / xmf4x4_Projection._22;
	xmf3_Pick_Position.z = 1.0f;

	int nIntersected = 0;
	float fHit_Distance = FLT_MAX;
	float fNearest_Hit_Distance = FLT_MAX;
	CObject* pIntersected_Object = NULL;
	CObject* pNearest_Object = NULL;

	pIntersected_Object = m_ppShaders[0]->Pick_Object_By_Ray_Intersection(xmf3_Pick_Position, xmf4x4_View, &fHit_Distance);	// 0 : cube

	if (pIntersected_Object && (fHit_Distance < fNearest_Hit_Distance)) {
		fNearest_Hit_Distance = fHit_Distance;
		pNearest_Object = pIntersected_Object;
	}

	return pNearest_Object;
}

void CScene::Delete_Cube_Object(int Client_x, int Client_y, CCamera* pCamera) {	// have to fix later
	if (!pCamera) {
		return;
	}

	DirectX::XMFLOAT4X4 xmf4x4_View = pCamera->Get_View_Matrix();
	DirectX::XMFLOAT4X4 xmf4x4_Projection = pCamera->Get_Projection_Matrix();
	D3D12_VIEWPORT d3d_Viewport = pCamera->Get_ViewPort();

	DirectX::XMFLOAT3 xmf3_Pick_Position;
	xmf3_Pick_Position.x = (((2.0f * Client_x) / d3d_Viewport.Width) - 1) / xmf4x4_Projection._11;
	xmf3_Pick_Position.y = -(((2.0f * Client_y) / d3d_Viewport.Height) - 1) / xmf4x4_Projection._22;
	xmf3_Pick_Position.z = 1.0f;

	int nIntersected = 0;
	float fHit_Distance = FLT_MAX;
	float fNearest_Hit_Distance = FLT_MAX;

	for (int i = 0; i < m_nShaders; ++i) {
		m_ppShaders[i]->Delete_Cube_Object(xmf3_Pick_Position, xmf4x4_View, &fHit_Distance);
	}
}

void CScene::Add_Cube_Object(int Client_x, int Client_y, CCamera* pCamera, CPlayer* pPlayer) {
	if (!pCamera) {
		return;
	}

	DirectX::XMFLOAT4X4 xmf4x4_View = pCamera->Get_View_Matrix();
	DirectX::XMFLOAT4X4 xmf4x4_Projection = pCamera->Get_Projection_Matrix();
	D3D12_VIEWPORT d3d_Viewport = pCamera->Get_ViewPort();

	DirectX::XMFLOAT3 xmf3_Pick_Position;
	xmf3_Pick_Position.x = (((2.0f * Client_x) / d3d_Viewport.Width) - 1) / xmf4x4_Projection._11;
	xmf3_Pick_Position.y = -(((2.0f * Client_y) / d3d_Viewport.Height) - 1) / xmf4x4_Projection._22;
	xmf3_Pick_Position.z = 1.0f;

	int nIntersected = 0;
	float fHit_Distance = FLT_MAX;
	float fNearest_Hit_Distance = FLT_MAX;

	m_ppShaders[0]->Add_Cube_Object(xmf3_Pick_Position, xmf4x4_View, &fHit_Distance, pPlayer);
}

void CScene::Clr_Cube_Objects() {
	((CInstancing_Shader*)m_ppShaders[0])->Clr_Cube_Objects();
}

void CScene::Check_Cube_Object_4_Server(std::vector<Cube_Info> pObject)
{
	if (!pObject.empty())
	{
		for (auto p : pObject) {
			if (p.AddorDelete)
			{
				m_ppShaders[0]->Add_Cube_Object_Server(p);
			}

			else
			{
				for (int i = 0; i < m_nShaders; ++i)
				{
					m_ppShaders[i]->Delete_Cube_Object_Server(p);
				}
			}
		}
		Release_m_vServerObjects();
	}
}

void CScene::Check_Cube_Object_4_Server_test(CObject* pObject)
{
	if (pObject)
	{
		if (Get_AddorDelete_Cube())
		{
			m_ppShaders[0]->Add_Cube_Object_Server_test(pObject);
		}

		else
		{
			for (int i = 0; i < m_nShaders; ++i)
			{
				m_ppShaders[i]->Delete_Cube_Object_Server_test(pObject);
			}
		}
	}
}


CObject** CScene::Get_Objects_From_Shader(int nShader_Index) {
	if (m_nShaders > nShader_Index) {
		if (m_ppShaders[nShader_Index]) {
			return m_ppShaders[nShader_Index]->Get_Objects();
		}
	}

	return NULL;
}

int CScene::Get_Object_Num_From_Shader(int nShader_Index) {
	if (m_nShaders > nShader_Index) {
		if (m_ppShaders[nShader_Index]) {
			return m_ppShaders[nShader_Index]->Get_Object_Num();
		}
	}

	return 0;
}

void CScene::Build_Objects(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List) {
	m_pd3d_Graphics_RootSignature = Crt_Graphics_RootSignature(pd3d_Device);

	m_nShaders = 3;
	m_ppShaders = new CObjects_Shader * [m_nShaders];

	CObjects_Shader* pObject_Shader = new CInstancing_Shader;	// cube
	m_ppShaders[0] = pObject_Shader;
	m_ppShaders[0]->Crt_Shader(pd3d_Device, m_pd3d_Graphics_RootSignature);
	m_ppShaders[0]->Build_Objects(pd3d_Device, pd3d_Command_List);

	pObject_Shader = new CPlayers_Shader;	// player
	m_ppShaders[1] = pObject_Shader;
	m_ppShaders[1]->Crt_Shader(pd3d_Device, m_pd3d_Graphics_RootSignature);
	m_ppShaders[1]->Build_Objects(pd3d_Device, pd3d_Command_List);

	pObject_Shader = new CUI_Shader;	// ui
	m_ppShaders[2] = pObject_Shader;
	m_ppShaders[2]->Crt_Shader(pd3d_Device, m_pd3d_Graphics_RootSignature);
	m_ppShaders[2]->Build_Objects(pd3d_Device, pd3d_Command_List);
}

void CScene::Release_Objects() {
	if (m_pd3d_Graphics_RootSignature) {
		m_pd3d_Graphics_RootSignature->Release();
	}

	for (int i = 0; i < m_nShaders; ++i) {
		m_ppShaders[i]->Release_Shader_Variables();
		m_ppShaders[i]->Release_Objects();

		delete m_ppShaders[i];
	}

	if (m_ppShaders) {
		delete[] m_ppShaders;
	}
}

bool CScene::Prcs_Input() {
	return false;
}

void CScene::Anim_Objects(float fElapsed_Time) {
	for (int i = 0; i < m_nShaders; ++i) {
		m_ppShaders[i]->Anim_Objects(fElapsed_Time);
	}
}

//void CScene::Prepare_Render(ID3D12GraphicsCommandList* pd3d_Command_List) {	// unused
//	pd3d_Command_List->SetGraphicsRootSignature(m_pd3d_Graphics_RootSignature);
//	pd3d_Command_List->SetPipelineState(m_pd3d_Graphics_Pipeline_State);
//	pd3d_Command_List->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//}

void CScene::Render(ID3D12GraphicsCommandList* pd3d_Command_List, CCamera* pCamera) {
	pCamera->Set_Viewports_N_ScissorRects(pd3d_Command_List);
	pd3d_Command_List->SetGraphicsRootSignature(m_pd3d_Graphics_RootSignature);

	if (pCamera) {
		pCamera->Udt_Shader_Variables(pd3d_Command_List);
	}

	for (int i = 0; i < m_nShaders; ++i) {
		m_ppShaders[i]->Render(pd3d_Command_List, pCamera);
	}
}

void CScene::Release_Upload_Buffers() {
	for (int i = 0; i < m_nShaders; ++i) {
		m_ppShaders[i]->Release_Upload_Buffers();
	}
}



void CScene::GetAllPlayerData(CPlayer* m_pPlayer)
{
	
	if(m_ppShaders[1])
	{
		((CPlayers_Shader*)m_ppShaders[1])->GetAllPlayerData(m_pPlayer);
	}
}