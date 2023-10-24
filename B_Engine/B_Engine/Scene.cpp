#include "Scene.h"

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

	D3D12_ROOT_PARAMETER pd3d_RootParameters[3];
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
		OutputDebugStringA("��Ʈ �ñ״�ó ����\n");
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

	for (int i = 0; i < m_nShaders; ++i) {
		pIntersected_Object = m_pShaders[i].Pick_Object_By_Ray_Intersection(xmf3_Pick_Position, xmf4x4_View, &fHit_Distance);

		if (pIntersected_Object && (fHit_Distance < fNearest_Hit_Distance)) {
			fNearest_Hit_Distance = fHit_Distance;
			pNearest_Object = pIntersected_Object;
		}
	}

	return pNearest_Object;
}

void CScene::Build_Objects(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List) {
	m_pd3d_Graphics_RootSignature = Crt_Graphics_RootSignature(pd3d_Device);

	m_nShaders = 1;
	m_pShaders = new CInstancing_Shader[m_nShaders];
	m_pShaders[0].Crt_Shader(pd3d_Device, m_pd3d_Graphics_RootSignature);
	m_pShaders[0].Build_Objects(pd3d_Device, pd3d_Command_List);
}

void CScene::Release_Objects() {
	if (m_pd3d_Graphics_RootSignature) {
		m_pd3d_Graphics_RootSignature->Release();
	}

	for (int i = 0; i < m_nShaders; ++i) {
		m_pShaders[i].Release_Shader_Variables();
		m_pShaders[i].Release_Objects();
	}

	if (m_pShaders) {
		delete[] m_pShaders;
	}
}

bool CScene::Prcs_Input() {
	return false;
}

void CScene::Anim_Objects(float fElapsed_Time) {
	for (int i = 0; i < m_nShaders; ++i) {
		m_pShaders[i].Anim_Objects(fElapsed_Time);
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
		m_pShaders[i].Render(pd3d_Command_List, pCamera);
	}
}

void CScene::Release_Upload_Buffers() {
	for (int i = 0; i < m_nShaders; ++i) {
		m_pShaders[i].Release_Upload_Buffers();
	}
}