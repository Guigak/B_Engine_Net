#pragma once

#include "stdafx.h"
#include "Object.h"
#include "Camera.h"

//

struct CB_GAMEOBJECT_INFO {
	DirectX::XMFLOAT4X4 m_xmf4x4_World;
};

struct VS_VB_INSTANCE {
	DirectX::XMFLOAT4X4 m_xmf4x4_Transform;
	DirectX::XMFLOAT4 m_xmf4_Color;
};

//

class CShader {
private:
	int m_nReferences = 0;

protected:
	CObject** m_ppObjects = NULL;
	int m_nObjects = 0;

	ID3D12PipelineState** m_ppd3d_Pipeline_States = NULL;
	int m_nPipeline_States = 0;

public:
	CShader();
	virtual ~CShader();

	//
	void Add_Ref() {
		++m_nReferences;
	}
	void Release() {
		if (--m_nReferences <= 0) {
			delete this;
		}
	}

	//
	virtual D3D12_INPUT_LAYOUT_DESC Crt_Input_Layout();
	virtual D3D12_RASTERIZER_DESC Crt_Rasterizer_State();
	virtual D3D12_BLEND_DESC Crt_Blend_State();
	virtual D3D12_DEPTH_STENCIL_DESC Crt_Depth_Stencil_State();

	virtual D3D12_SHADER_BYTECODE Crt_Vertex_Shader(ID3DBlob** ppd3d_Shader_Blob);
	virtual D3D12_SHADER_BYTECODE Crt_Pixel_Shader(ID3DBlob** ppd3d_Shader_Blob);

	D3D12_SHADER_BYTECODE Compile_Shader_From_File(WCHAR* pszFile_Name, LPCSTR pszShader_Name, LPCSTR pszShader_Profile, ID3DBlob** ppd3d_Shader_Blob);

	virtual void Crt_Shader(ID3D12Device* pd3d_Device, ID3D12RootSignature* pd3d_RootSignature);

	virtual void Crt_Shader_Variables(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List);
	virtual void Udt_Shader_Variables(ID3D12GraphicsCommandList* pd3d_Command_List);
	virtual void Release_Shader_Variables();

	virtual void Udt_Shader_Variable(ID3D12GraphicsCommandList* pd3d_Command_List, DirectX::XMFLOAT4X4* pxmf4x4_World);

	virtual void Release_Upload_Buffers();

	virtual void Build_Objects(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List, void* pContext = NULL);
	virtual void Anim_Objects(float fElapsed_Time);
	virtual void Release_Objects();

	virtual void Prepare_Render(ID3D12GraphicsCommandList* pd3d_Command_List);
	virtual void Render(ID3D12GraphicsCommandList* pd3d_Command_List, CCamera* pCamera);
};

class CDiffused_Shader : public CShader {
public :
	CDiffused_Shader();
	virtual ~CDiffused_Shader();

	virtual D3D12_INPUT_LAYOUT_DESC Crt_Input_Layout();
	
	virtual D3D12_SHADER_BYTECODE Crt_Vertex_Shader(ID3DBlob** ppd3d_Shader_Blob);
	virtual D3D12_SHADER_BYTECODE Crt_Pixel_Shader(ID3DBlob** ppd3d_Shader_Blob);

	virtual void Crt_Shader(ID3D12Device* pd3d_Device, ID3D12RootSignature* pd3d_Graphics_RootSignature);
};

//
class CObjects_Shader : public CShader {
protected :
	CObject** m_ppObjects = NULL;
	int m_nObjects = 0;

public :
	CObjects_Shader();
	virtual ~CObjects_Shader();

	virtual void Build_Objects(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List);
	virtual void Release_Objects();

	virtual D3D12_INPUT_LAYOUT_DESC Crt_Input_Layout();
	virtual D3D12_SHADER_BYTECODE Crt_Vertex_Shader(ID3DBlob** ppd3d_Shader_Blob);
	virtual D3D12_SHADER_BYTECODE Crt_Pixel_Shader(ID3DBlob** ppd3d_Shader_Blob);

	virtual void Crt_Shader(ID3D12Device* pd3d_Device, ID3D12RootSignature* pd3d_Graphics_RootSignature);

	virtual void Release_Upload_Buffers();

	virtual void Anim_Objects(float fElapsed_Time);
	virtual void Render(ID3D12GraphicsCommandList* pd3d_Command_List, CCamera* pCamera);

	//
	virtual CObject* Pick_Object_By_Ray_Intersection(DirectX::XMFLOAT3& xmf3_Pick_Position, DirectX::XMFLOAT4X4& xmf4x4_View, float* pfNear_Hit_Distance);
};

//
class CInstancing_Shader : public CObjects_Shader {
protected :
	ID3D12Resource* m_pd3d_CB_Objects = NULL;
	VS_VB_INSTANCE* m_pCB_Mapped_Objects = NULL;

public :
	CInstancing_Shader();
	virtual ~CInstancing_Shader();

	virtual D3D12_INPUT_LAYOUT_DESC Crt_Input_Layout();
	virtual D3D12_SHADER_BYTECODE Crt_Vertex_Shader(ID3DBlob** ppd3d_Shader_Blob);
	virtual D3D12_SHADER_BYTECODE Crt_Pixel_Shader(ID3DBlob** ppd3d_Shader_Blob);
	virtual void Crt_Shader(ID3D12Device* pd3d_Device, ID3D12RootSignature* pd3d_Graphics_RootSignature);

	virtual void Crt_Shader_Variables(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List);
	virtual void Udt_Shader_Variables(ID3D12GraphicsCommandList* pd3d_Command_List);
	virtual void Release_Shader_Variables();

	virtual void Build_Objects(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List);

	virtual void Render(ID3D12GraphicsCommandList* pd3d_Command_List, CCamera* pCamera);
};