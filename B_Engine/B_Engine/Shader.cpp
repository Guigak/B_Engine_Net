#include "Shader.h"

CShader::CShader() {
}

CShader::~CShader() {
	if (m_ppd3d_Pipeline_States) {
		for (int i = 0; i < m_nPipeline_States; ++i) {
			if (m_ppd3d_Pipeline_States[i]) {
				m_ppd3d_Pipeline_States[i]->Release();
			}
		}

		delete[] m_ppd3d_Pipeline_States;
	}
}

D3D12_INPUT_LAYOUT_DESC CShader::Crt_Input_Layout() {
	//UINT nInput_Element_Descs = 2;
	//D3D12_INPUT_ELEMENT_DESC* pd3d_Input_Element_Descs = new D3D12_INPUT_ELEMENT_DESC[nInput_Element_Descs];

	//pd3d_Input_Element_Descs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	//pd3d_Input_Element_Descs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	//D3D12_INPUT_LAYOUT_DESC d3d_Input_Layout_Desc;
	//d3d_Input_Layout_Desc.pInputElementDescs = pd3d_Input_Element_Descs;
	//d3d_Input_Layout_Desc.NumElements = nInput_Element_Descs;

	D3D12_INPUT_LAYOUT_DESC d3d_Input_Layout_Desc;
	d3d_Input_Layout_Desc.pInputElementDescs = NULL;
	d3d_Input_Layout_Desc.NumElements = 0;

	return d3d_Input_Layout_Desc;
}

D3D12_RASTERIZER_DESC CShader::Crt_Rasterizer_State(bool bSolid) {
	D3D12_RASTERIZER_DESC d3d_Rasterizer_Desc;
	ZeroMemory(&d3d_Rasterizer_Desc, sizeof(D3D12_RASTERIZER_DESC));
	if (bSolid) {
		d3d_Rasterizer_Desc.FillMode = D3D12_FILL_MODE_SOLID;
	}
	else {
		d3d_Rasterizer_Desc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	}
	d3d_Rasterizer_Desc.CullMode = D3D12_CULL_MODE_BACK;
	d3d_Rasterizer_Desc.FrontCounterClockwise = FALSE;
	d3d_Rasterizer_Desc.DepthBias = 0;
	d3d_Rasterizer_Desc.DepthBiasClamp = 0.0f;
	d3d_Rasterizer_Desc.SlopeScaledDepthBias = 0.0f;
	d3d_Rasterizer_Desc.DepthClipEnable = TRUE;
	d3d_Rasterizer_Desc.MultisampleEnable = FALSE;
	d3d_Rasterizer_Desc.AntialiasedLineEnable = FALSE;
	d3d_Rasterizer_Desc.ForcedSampleCount = 0;
	d3d_Rasterizer_Desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return d3d_Rasterizer_Desc;
}

D3D12_BLEND_DESC CShader::Crt_Blend_State() {
	D3D12_BLEND_DESC d3d_Blend_Desc;
	ZeroMemory(&d3d_Blend_Desc, sizeof(D3D12_BLEND_DESC));
	d3d_Blend_Desc.AlphaToCoverageEnable = FALSE;
	d3d_Blend_Desc.IndependentBlendEnable = FALSE;
	d3d_Blend_Desc.RenderTarget[0].BlendEnable = FALSE;
	d3d_Blend_Desc.RenderTarget[0].LogicOpEnable = FALSE;
	d3d_Blend_Desc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	d3d_Blend_Desc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	d3d_Blend_Desc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	d3d_Blend_Desc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	d3d_Blend_Desc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	d3d_Blend_Desc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	d3d_Blend_Desc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	d3d_Blend_Desc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return d3d_Blend_Desc;
}

D3D12_DEPTH_STENCIL_DESC CShader::Crt_Depth_Stencil_State() {
	D3D12_DEPTH_STENCIL_DESC d3d_Depth_Stencil_Desc;
	ZeroMemory(&d3d_Depth_Stencil_Desc, sizeof(D3D12_DEPTH_STENCIL_DESC));
	d3d_Depth_Stencil_Desc.DepthEnable = TRUE;
	d3d_Depth_Stencil_Desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	d3d_Depth_Stencil_Desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	d3d_Depth_Stencil_Desc.StencilEnable = FALSE;
	d3d_Depth_Stencil_Desc.StencilReadMask = 0x00;
	d3d_Depth_Stencil_Desc.StencilWriteMask = 0x00;
	d3d_Depth_Stencil_Desc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3d_Depth_Stencil_Desc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3d_Depth_Stencil_Desc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3d_Depth_Stencil_Desc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	d3d_Depth_Stencil_Desc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	d3d_Depth_Stencil_Desc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	d3d_Depth_Stencil_Desc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	d3d_Depth_Stencil_Desc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return d3d_Depth_Stencil_Desc;
}

D3D12_SHADER_BYTECODE CShader::Crt_Vertex_Shader(ID3DBlob** ppd3d_Shader_Blob) {
	D3D12_SHADER_BYTECODE d3d_Shader_ByteCode;
	d3d_Shader_ByteCode.BytecodeLength = 0;
	d3d_Shader_ByteCode.pShaderBytecode = NULL;

	return d3d_Shader_ByteCode;

	//return (Compile_Shader_From_File(L"Shaders.hlsl", "VSMain", "vs_5_1", ppd3d_Shader_Blob));
}

D3D12_SHADER_BYTECODE CShader::Crt_Pixel_Shader(ID3DBlob** ppd3d_Shader_Blob) {
	D3D12_SHADER_BYTECODE d3d_Shader_ByteCode;
	d3d_Shader_ByteCode.BytecodeLength = 0;
	d3d_Shader_ByteCode.pShaderBytecode = NULL;

	return d3d_Shader_ByteCode;

	//return (Compile_Shader_From_File(L"Shaders.hlsl", "PSMain", "ps_5_1", ppd3d_Shader_Blob));
}

D3D12_SHADER_BYTECODE CShader::Compile_Shader_From_File(WCHAR* pszFile_Name, LPCSTR pszShader_Name, LPCSTR pszShader_Profile, ID3DBlob** ppd3d_Shader_Blob) {
	UINT nCompile_Flags = 0;
#if defined(_DEBUG)
	nCompile_Flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hResult;
	ID3DBlob* pd3d_Error_Blob;

	hResult = D3DCompileFromFile(pszFile_Name, NULL, NULL, pszShader_Name, pszShader_Profile, nCompile_Flags, 0, ppd3d_Shader_Blob, &pd3d_Error_Blob);

	// print error
	if (FAILED(hResult)) {
		OutputDebugStringA("쉐이더 컴파일 실패\n");
		OutputDebugStringA(static_cast<const char*>(pd3d_Error_Blob->GetBufferPointer()));
	}

	if (pd3d_Error_Blob) {
		pd3d_Error_Blob->Release();
	}

	//
	D3D12_SHADER_BYTECODE d3d_Shader_ByteCode;
	d3d_Shader_ByteCode.BytecodeLength = (*ppd3d_Shader_Blob)->GetBufferSize();
	d3d_Shader_ByteCode.pShaderBytecode = (*ppd3d_Shader_Blob)->GetBufferPointer();

	return d3d_Shader_ByteCode;
}

void CShader::Crt_Shader(ID3D12Device* pd3d_Device, ID3D12RootSignature* pd3d_RootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE d3d_Primitive_Topology_Type) {
	//m_nPipeline_States = 1;
	//m_ppd3d_Pipeline_States = new ID3D12PipelineState * [m_nPipeline_States];

	ID3DBlob* pd3d_Vertex_Shader_Blob = NULL;
	ID3DBlob* pd3d_Pixel_Shader_Blob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3d_Pipeline_State_Desc;
	ZeroMemory(&d3d_Pipeline_State_Desc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3d_Pipeline_State_Desc.pRootSignature = pd3d_RootSignature;
	d3d_Pipeline_State_Desc.VS = Crt_Vertex_Shader(&pd3d_Vertex_Shader_Blob);
	d3d_Pipeline_State_Desc.PS = Crt_Pixel_Shader(&pd3d_Pixel_Shader_Blob);
	d3d_Pipeline_State_Desc.RasterizerState = Crt_Rasterizer_State();
	d3d_Pipeline_State_Desc.BlendState = Crt_Blend_State();
	d3d_Pipeline_State_Desc.DepthStencilState = Crt_Depth_Stencil_State();
	d3d_Pipeline_State_Desc.InputLayout = Crt_Input_Layout();
	d3d_Pipeline_State_Desc.SampleMask = UINT_MAX;
	d3d_Pipeline_State_Desc.PrimitiveTopologyType = d3d_Primitive_Topology_Type;
	d3d_Pipeline_State_Desc.NumRenderTargets = 1;
	d3d_Pipeline_State_Desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3d_Pipeline_State_Desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3d_Pipeline_State_Desc.SampleDesc.Count = 1;
	d3d_Pipeline_State_Desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	pd3d_Device->CreateGraphicsPipelineState(&d3d_Pipeline_State_Desc, __uuidof(ID3D12PipelineState), (void**)&m_ppd3d_Pipeline_States[0]);

	if (pd3d_Vertex_Shader_Blob) {
		pd3d_Vertex_Shader_Blob->Release();
	}
	if (pd3d_Pixel_Shader_Blob) {
		pd3d_Pixel_Shader_Blob->Release();
	}

	if (d3d_Pipeline_State_Desc.InputLayout.pInputElementDescs) {
		delete[] d3d_Pipeline_State_Desc.InputLayout.pInputElementDescs;
	}
}

void CShader::Crt_Shader_Variables(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List) {
}

void CShader::Udt_Shader_Variables(ID3D12GraphicsCommandList* pd3d_Command_List) {
}

void CShader::Release_Shader_Variables() {
}

void CShader::Udt_Shader_Variable(ID3D12GraphicsCommandList* pd3d_Command_List, DirectX::XMFLOAT4X4* pxmf4x4_World) {
	DirectX::XMFLOAT4X4 xmf4x4_World;
	DirectX::XMStoreFloat4x4(&xmf4x4_World, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(pxmf4x4_World)));
	pd3d_Command_List->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4_World, 0);
}

void CShader::Release_Upload_Buffers() {
	if (m_ppObjects) {
		for (int i = 0; i < m_nObjects; ++i) {
			if (m_ppObjects[i]) {
				m_ppObjects[i]->Release_Upload_Buffers();
			}
		}
	}
}

void CShader::Build_Objects(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List, void* pContext) {
	CTriangle_Mesh* pTriangle_Mesh = new CTriangle_Mesh(pd3d_Device, pd3d_Command_List);

	m_nObjects = 1;
	m_ppObjects = new CObject * [m_nObjects];

	m_ppObjects[0] = new CObject();
	m_ppObjects[0]->Set_Mesh(pTriangle_Mesh);
}

void CShader::Anim_Objects(float fElapsed_Time) {
	for (int i = 0; i < m_nObjects; ++i) {
		m_ppObjects[i]->Anim(fElapsed_Time);
	}
}

void CShader::Release_Objects() {
	if (m_ppObjects) {
		for (int i = 0; i < m_nObjects; ++i) {
			if (m_ppObjects[i]) {
				delete m_ppObjects[i];
			}
		}

		delete[] m_ppObjects;
	}
}

void CShader::Prepare_Render(ID3D12GraphicsCommandList* pd3d_Command_List) {
	pd3d_Command_List->SetPipelineState(m_ppd3d_Pipeline_States[0]);
}

void CShader::Render(ID3D12GraphicsCommandList* pd3d_Command_List, CCamera* pCamera) {
	Prepare_Render(pd3d_Command_List);

	//for (int i = 0; i < m_nObjects; ++i) {
	//	if (m_ppObjects[i]) {
	//		m_ppObjects[i]->Render(pd3d_Command_List);
	//	}
	//}
}

CDiffused_Shader::CDiffused_Shader() {
}

CDiffused_Shader::~CDiffused_Shader() {
}

D3D12_INPUT_LAYOUT_DESC CDiffused_Shader::Crt_Input_Layout() {
	UINT nInput_Element_Descs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3d_Input_Element_Descs = new D3D12_INPUT_ELEMENT_DESC[nInput_Element_Descs];

	pd3d_Input_Element_Descs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0 , D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3d_Input_Element_Descs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3d_Input_Layout_Desc;
	d3d_Input_Layout_Desc.pInputElementDescs = pd3d_Input_Element_Descs;
	d3d_Input_Layout_Desc.NumElements = nInput_Element_Descs;

	return d3d_Input_Layout_Desc;
}

D3D12_SHADER_BYTECODE CDiffused_Shader::Crt_Vertex_Shader(ID3DBlob** ppd3d_Shader_Blob) {
	return CShader::Compile_Shader_From_File(L"Shaders.hlsl", "VSDiffused", "vs_5_1", ppd3d_Shader_Blob);
}

D3D12_SHADER_BYTECODE CDiffused_Shader::Crt_Pixel_Shader(ID3DBlob** ppd3d_Shader_Blob) {
	return CShader::Compile_Shader_From_File(L"Shaders.hlsl", "PSDiffused", "ps_5_1", ppd3d_Shader_Blob);
}

void CDiffused_Shader::Crt_Shader(ID3D12Device* pd3d_Device, ID3D12RootSignature* pd3d_RootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE d3d_Primitive_Topology_Type) {
	m_nPipeline_States = 1;
	m_ppd3d_Pipeline_States = new ID3D12PipelineState * [m_nPipeline_States];

	CShader::Crt_Shader(pd3d_Device, pd3d_RootSignature);
}


//
CObjects_Shader::CObjects_Shader() {
}

CObjects_Shader::~CObjects_Shader() {
}

void CObjects_Shader::Build_Objects(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List) {
}

void CObjects_Shader::Release_Objects() {
	if (m_ppObjects) {
		for (int i = 0; i < m_nObjects; ++i) {
			if (m_ppObjects[i]) {
				delete m_ppObjects[i];
			}
		}

		delete[] m_ppObjects;
	}
}

D3D12_INPUT_LAYOUT_DESC CObjects_Shader::Crt_Input_Layout() {
	UINT uInput_Element_Descs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3d_Input_Element_Descs = new D3D12_INPUT_ELEMENT_DESC[uInput_Element_Descs];

	pd3d_Input_Element_Descs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3d_Input_Element_Descs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3d_Input_Layout_Desc;
	d3d_Input_Layout_Desc.pInputElementDescs = pd3d_Input_Element_Descs;
	d3d_Input_Layout_Desc.NumElements = 2;

	return d3d_Input_Layout_Desc;
}

D3D12_SHADER_BYTECODE CObjects_Shader::Crt_Vertex_Shader(ID3DBlob** ppd3d_Shader_Blob) {
	return CShader::Compile_Shader_From_File(L"Shaders.hlsl", "VSDiffused", "vs_5_1", ppd3d_Shader_Blob);
}

D3D12_SHADER_BYTECODE CObjects_Shader::Crt_Pixel_Shader(ID3DBlob** ppd3d_Shader_Blob) {
	return CShader::Compile_Shader_From_File(L"Shaders.hlsl", "PSDiffused", "ps_5_1", ppd3d_Shader_Blob);
}

void CObjects_Shader::Crt_Shader(ID3D12Device* pd3d_Device, ID3D12RootSignature* pd3d_RootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE d3d_Primitive_Topology_Type) {
	m_nPipeline_States = 1;
	m_ppd3d_Pipeline_States = new ID3D12PipelineState * [m_nPipeline_States];

	CShader::Crt_Shader(pd3d_Device, pd3d_RootSignature);
}

void CObjects_Shader::Release_Upload_Buffers() {
	if (m_ppObjects) {
		for (int i = 0; i < m_nObjects; ++i) {
			m_ppObjects[i]->Release_Upload_Buffers();
		}
	}
}

void CObjects_Shader::Anim_Objects(float fElapsed_Time) {
	for (int i = 0; i < m_nObjects; ++i) {
		m_ppObjects[i]->Anim(fElapsed_Time);
	}
}

void CObjects_Shader::Render(ID3D12GraphicsCommandList* pd3d_Command_List, CCamera* pCamera) {
	CShader::Render(pd3d_Command_List, pCamera);

	for (int i = 0; i < m_nObjects; ++i) {
		if (m_ppObjects[i]) {
			m_ppObjects[i]->Render(pd3d_Command_List, pCamera);
		}
	}
}

CObject* CObjects_Shader::Pick_Object_By_Ray_Intersection(DirectX::XMFLOAT3& xmf3_Pick_Position, DirectX::XMFLOAT4X4& xmf4x4_View, float* pfNear_Hit_Distance) {
	int nIntersected = 0;
	*pfNear_Hit_Distance = FLT_MAX;
	float fHit_Distance = FLT_MAX;
	CObject* pSelected_Object = NULL;

	for (int i = 0; i < m_nObjects; ++i) {
		nIntersected = m_ppObjects[i]->Pick_Object_By_Ray_Intersection(xmf3_Pick_Position, xmf4x4_View, &fHit_Distance, m_ppObjects[0]->Get_Mesh());

		if ((nIntersected > 0) && (fHit_Distance < *pfNear_Hit_Distance)) {
			*pfNear_Hit_Distance = fHit_Distance;
			pSelected_Object = m_ppObjects[i];
		}
	}

	return pSelected_Object;
}

//
CInstancing_Shader::CInstancing_Shader() {
}

CInstancing_Shader::~CInstancing_Shader() {
}

D3D12_INPUT_LAYOUT_DESC CInstancing_Shader::Crt_Input_Layout() {
	UINT nInput_Element_Descs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3d_Input_Element_Descs = new D3D12_INPUT_ELEMENT_DESC[nInput_Element_Descs];

	pd3d_Input_Element_Descs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3d_Input_Element_Descs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3d_Input_Layout_Desc;
	d3d_Input_Layout_Desc.pInputElementDescs = pd3d_Input_Element_Descs;
	d3d_Input_Layout_Desc.NumElements = nInput_Element_Descs;

	return d3d_Input_Layout_Desc;
}

D3D12_SHADER_BYTECODE CInstancing_Shader::Crt_Vertex_Shader(ID3DBlob** ppd3d_Shader_Blob) {
	return (CShader::Compile_Shader_From_File(L"Shaders.hlsl", "VSInstancing", "vs_5_1", ppd3d_Shader_Blob));
}

D3D12_SHADER_BYTECODE CInstancing_Shader::Crt_Pixel_Shader(ID3DBlob** ppd3d_Shader_Blob) {
	return (CShader::Compile_Shader_From_File(L"Shaders.hlsl", "PSInstancing", "ps_5_1", ppd3d_Shader_Blob));
}

void CInstancing_Shader::Crt_Shader(ID3D12Device* pd3d_Device, ID3D12RootSignature* pd3d_RootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE d3d_Primitive_Topology_Type) {
	m_nPipeline_States = 2;
	m_ppd3d_Pipeline_States = new ID3D12PipelineState * [m_nPipeline_States];

	CShader::Crt_Shader(pd3d_Device, pd3d_RootSignature);
	Crt_Shader_4_Bounding_Box(pd3d_Device, pd3d_RootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
}

void CInstancing_Shader::Crt_Shader_Variables(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List) {
	m_pd3d_CB_Objects = Crt_Buffer_Resource(pd3d_Device, pd3d_Command_List, NULL, sizeof(VS_VB_INSTANCE) * CUBE_MAX_NUMBER, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, NULL);

	m_pd3d_CB_Objects->Map(0, NULL, (void**)&m_pCB_Mapped_Objects);
}

void CInstancing_Shader::Udt_Shader_Variables(ID3D12GraphicsCommandList* pd3d_Command_List) {
	pd3d_Command_List->SetGraphicsRootShaderResourceView(2, m_pd3d_CB_Objects->GetGPUVirtualAddress());

	for (int i = 0; i < m_nObjects; ++i) {
		m_pCB_Mapped_Objects[i].m_xmf4_Color = m_ppObjects[i]->Get_Color();

		DirectX::XMStoreFloat4x4(&m_pCB_Mapped_Objects[i].m_xmf4x4_Transform, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_ppObjects[i]->Get_World_Matrix())));
	}
}

void CInstancing_Shader::Release_Shader_Variables() {
	if (m_pd3d_CB_Objects) {
		m_pd3d_CB_Objects->Unmap(0, NULL);
	}

	if (m_pd3d_CB_Objects) {
		m_pd3d_CB_Objects->Release();
	}
}

void CInstancing_Shader::Build_Objects(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List) {
	int i = 0;

	m_nObjects = (CUBE_INIT_RING_NUMBER * 2 + 1) * (CUBE_INIT_RING_NUMBER * 2 + 1);

	m_ppObjects = new CObject * [CUBE_MAX_NUMBER] { NULL };

	CObject* pObject = NULL;

	for (int x = -CUBE_INIT_RING_NUMBER; x <= CUBE_INIT_RING_NUMBER; ++x) {
		for (int z = -CUBE_INIT_RING_NUMBER; z <= CUBE_INIT_RING_NUMBER; ++z) {
			pObject = new CObject();
			pObject->Set_Position(CUBE_WIDTH * x, 0.0f, CUBE_WIDTH * z);
			pObject->Set_Color(CUBE_DEFAULT_COLOR, CUBE_DEFAULT_COLOR, CUBE_DEFAULT_COLOR, 0.0f);
			m_ppObjects[i++] = pObject;
		}
	}

	CCube_Mesh* pCube_Mesh = new CCube_Mesh(pd3d_Device, pd3d_Command_List, CUBE_WIDTH, CUBE_WIDTH, CUBE_WIDTH);
	m_ppObjects[0]->Set_Mesh(pCube_Mesh);

	CBounding_Box_Mesh* pBounding_Box_Mesh = new CBounding_Box_Mesh(pd3d_Device, pd3d_Command_List, CUBE_WIDTH + BOUNDING_BOX_OFFSET, CUBE_WIDTH + BOUNDING_BOX_OFFSET, CUBE_WIDTH + BOUNDING_BOX_OFFSET);
	m_ppObjects[0]->Set_Bounding_Box_Mesh(pBounding_Box_Mesh);

	Crt_Shader_Variables(pd3d_Device, pd3d_Command_List);
}

void CInstancing_Shader::Render(ID3D12GraphicsCommandList* pd3d_Command_List, CCamera* pCamera) {
	CShader::Render(pd3d_Command_List, pCamera);

	Udt_Shader_Variables(pd3d_Command_List);

	m_ppObjects[0]->Render(pd3d_Command_List, pCamera, m_nObjects);

	Prepare_Render_4_Bounding_Box(pd3d_Command_List);

	m_ppObjects[0]->Render_Bounding_Box(pd3d_Command_List, pCamera, m_nObjects);
}

void CInstancing_Shader::Add_Cube_Object(DirectX::XMFLOAT3& xmf3_Pick_Position, DirectX::XMFLOAT4X4& xmf4x4_View, float* pfNear_Hit_Distance) {
	int nIntersected = 0;
	*pfNear_Hit_Distance = PLAYER_PICKING_DISTANCE;
	float fHit_Distance = FLT_MAX;
	CObject* pSelected_Object = NULL;
	int nSelected_Index = 0;
	int nPrimitive_Number = 0;
	int nSelected_Primitive_Number = 0;

	for (int i = 0; i < m_nObjects; ++i) {
		nIntersected = m_ppObjects[i]->Pick_Object_By_Ray_Intersection(xmf3_Pick_Position, xmf4x4_View, &fHit_Distance, m_ppObjects[0]->Get_Mesh(), &nPrimitive_Number);

		if ((nIntersected > 0) && (fHit_Distance < *pfNear_Hit_Distance)) {
			*pfNear_Hit_Distance = fHit_Distance;
			pSelected_Object = m_ppObjects[i];
			nSelected_Index = i;
			nSelected_Primitive_Number = nPrimitive_Number;
		}
	}

	if (pSelected_Object) {
		DirectX::XMFLOAT3 xmf3_Position = m_ppObjects[nSelected_Index]->Get_Position();

		switch (nSelected_Primitive_Number) {
		case 0:	// top
			xmf3_Position = Vector3::Add(xmf3_Position, Vector3::Multiply(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f), CUBE_WIDTH, false));
			break;
		case 1:	// front
			xmf3_Position = Vector3::Add(xmf3_Position, Vector3::Multiply(DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f), CUBE_WIDTH, false));
			break;
		case 2:	// left
			xmf3_Position = Vector3::Add(xmf3_Position, Vector3::Multiply(DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f), CUBE_WIDTH, false));
			break;
		case 3:	// right
			xmf3_Position = Vector3::Add(xmf3_Position, Vector3::Multiply(DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f), CUBE_WIDTH, false));
			break;
		case 4:	// back
			xmf3_Position = Vector3::Add(xmf3_Position, Vector3::Multiply(DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), CUBE_WIDTH, false));
			break;
		case 5:	// bottom
			xmf3_Position = Vector3::Add(xmf3_Position, Vector3::Multiply(DirectX::XMFLOAT3(0.0f, -1.0f, 0.0f), CUBE_WIDTH, false));
			break;
		default:
			break;
		}

		CObject* pObject = NULL;
		pObject = new CObject();
		pObject->Set_Position(xmf3_Position);
		pObject->Set_Color(0.5f, 0.0f, 0.0f, 0.0f);
		m_ppObjects[m_nObjects++] = pObject;
	}
}

void CInstancing_Shader::Delete_Cube_Object(DirectX::XMFLOAT3& xmf3_Pick_Position, DirectX::XMFLOAT4X4& xmf4x4_View, float* pfNear_Hit_Distance) {
	int nIntersected = 0;
	*pfNear_Hit_Distance = PLAYER_PICKING_DISTANCE;
	float fHit_Distance = FLT_MAX;
	CObject* pSelected_Object = NULL;
	int nSelected_Index = 0;

	for (int i = 0; i < m_nObjects; ++i) {
		nIntersected = m_ppObjects[i]->Pick_Object_By_Ray_Intersection(xmf3_Pick_Position, xmf4x4_View, &fHit_Distance, m_ppObjects[0]->Get_Mesh());

		if ((nIntersected > 0) && (fHit_Distance < *pfNear_Hit_Distance)) {
			*pfNear_Hit_Distance = fHit_Distance;
			pSelected_Object = m_ppObjects[i];
			nSelected_Index = i;
		}
	}

	if (pSelected_Object) {
		if (nSelected_Index < (CUBE_INIT_RING_NUMBER * 2 + 1) * (CUBE_INIT_RING_NUMBER * 2 + 1)) {
			return;
		}

		m_ppObjects[nSelected_Index]->Release();
		m_ppObjects[nSelected_Index] = NULL;

		if (nSelected_Index != m_nObjects - 1) {
			m_ppObjects[nSelected_Index] = m_ppObjects[m_nObjects - 1];
		}

		--m_nObjects;
	}
}

void CInstancing_Shader::Prepare_Render_4_Bounding_Box(ID3D12GraphicsCommandList* pd3d_Command_List) {
	pd3d_Command_List->SetPipelineState(m_ppd3d_Pipeline_States[1]);
}

D3D12_SHADER_BYTECODE CInstancing_Shader::Crt_Vertex_Shader_4_Bounding_Box(ID3DBlob** ppd3d_Shader_Blob) {
	return (CShader::Compile_Shader_From_File(L"Shaders.hlsl", "VSInstancing4BoudingBox", "vs_5_1", ppd3d_Shader_Blob));
}

void CInstancing_Shader::Crt_Shader_4_Bounding_Box(ID3D12Device* pd3d_Device, ID3D12RootSignature* pd3d_RootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE d3d_Primitive_Topology_Type) {
	ID3DBlob* pd3d_Vertex_Shader_Blob = NULL;
	ID3DBlob* pd3d_Pixel_Shader_Blob = NULL;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC d3d_Pipeline_State_Desc;
	ZeroMemory(&d3d_Pipeline_State_Desc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	d3d_Pipeline_State_Desc.pRootSignature = pd3d_RootSignature;
	d3d_Pipeline_State_Desc.VS = Crt_Vertex_Shader_4_Bounding_Box(&pd3d_Vertex_Shader_Blob);
	d3d_Pipeline_State_Desc.PS = Crt_Pixel_Shader(&pd3d_Pixel_Shader_Blob);
	d3d_Pipeline_State_Desc.RasterizerState = Crt_Rasterizer_State();
	d3d_Pipeline_State_Desc.BlendState = Crt_Blend_State();
	d3d_Pipeline_State_Desc.DepthStencilState = Crt_Depth_Stencil_State();
	d3d_Pipeline_State_Desc.InputLayout = Crt_Input_Layout();
	d3d_Pipeline_State_Desc.SampleMask = UINT_MAX;
	d3d_Pipeline_State_Desc.PrimitiveTopologyType = d3d_Primitive_Topology_Type;
	d3d_Pipeline_State_Desc.NumRenderTargets = 1;
	d3d_Pipeline_State_Desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3d_Pipeline_State_Desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3d_Pipeline_State_Desc.SampleDesc.Count = 1;
	d3d_Pipeline_State_Desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	pd3d_Device->CreateGraphicsPipelineState(&d3d_Pipeline_State_Desc, __uuidof(ID3D12PipelineState), (void**)&m_ppd3d_Pipeline_States[1]);

	if (pd3d_Vertex_Shader_Blob) {
		pd3d_Vertex_Shader_Blob->Release();
	}
	if (pd3d_Pixel_Shader_Blob) {
		pd3d_Pixel_Shader_Blob->Release();
	}

	if (d3d_Pipeline_State_Desc.InputLayout.pInputElementDescs) {
		delete[] d3d_Pipeline_State_Desc.InputLayout.pInputElementDescs;
	}
}

//
CUI_Shader::CUI_Shader() {
}

CUI_Shader::~CUI_Shader() {
}

D3D12_INPUT_LAYOUT_DESC CUI_Shader::Crt_Input_Layout() {
	UINT nInput_Element_Descs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3d_Input_Element_Descs = new D3D12_INPUT_ELEMENT_DESC[nInput_Element_Descs];

	pd3d_Input_Element_Descs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3d_Input_Element_Descs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3d_Input_Layout_Desc;
	d3d_Input_Layout_Desc.pInputElementDescs = pd3d_Input_Element_Descs;
	d3d_Input_Layout_Desc.NumElements = nInput_Element_Descs;

	return d3d_Input_Layout_Desc;
}

D3D12_SHADER_BYTECODE CUI_Shader::Crt_Vertex_Shader(ID3DBlob** ppd3d_Shader_Blob) {
	return CShader::Compile_Shader_From_File(L"Shaders.hlsl", "VSUI", "vs_5_1", ppd3d_Shader_Blob);
}

D3D12_SHADER_BYTECODE CUI_Shader::Crt_Pixel_Shader(ID3DBlob** ppd3d_Shader_Blob) {
	return (CShader::Compile_Shader_From_File(L"Shaders.hlsl", "PSInstancing", "ps_5_1", ppd3d_Shader_Blob));
}

void CUI_Shader::Crt_Shader(ID3D12Device* pd3d_Device, ID3D12RootSignature* pd3d_RootSignature, D3D12_PRIMITIVE_TOPOLOGY_TYPE d3d_Primitive_Topology_Type) {
	m_nPipeline_States = 1;
	m_ppd3d_Pipeline_States = new ID3D12PipelineState * [m_nPipeline_States];

	CShader::Crt_Shader(pd3d_Device, pd3d_RootSignature);
}
