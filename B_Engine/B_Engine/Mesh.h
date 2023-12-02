#pragma once

#include "stdafx.h"

class CVertex {
protected :
	DirectX::XMFLOAT3 m_xmf3_Position;

public :
	CVertex() {
		m_xmf3_Position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	}
	CVertex(DirectX::XMFLOAT3 xmf3_Position) {
		m_xmf3_Position = xmf3_Position;
	}
	~CVertex() {};
};

class CDiffused_Vertex : public CVertex {
protected :
	DirectX::XMFLOAT4 m_xmf4_Diffuse;

public :
	CDiffused_Vertex() {
		m_xmf3_Position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf4_Diffuse = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	CDiffused_Vertex(float x, float y, float z, DirectX::XMFLOAT4 xmf4_Diffuse) {
		m_xmf3_Position = DirectX::XMFLOAT3(x, y, z);
		m_xmf4_Diffuse = xmf4_Diffuse;
	}
	CDiffused_Vertex(DirectX::XMFLOAT3 xmf3_Position, DirectX::XMFLOAT4 xmf4_Diffuse) {
		m_xmf3_Position = xmf3_Position;
		m_xmf4_Diffuse = xmf4_Diffuse;
	}
	~CDiffused_Vertex() {};
};

//
class CTextured_Vertex : public CVertex {
protected :
	DirectX::XMFLOAT2 m_xmf2_UV;

public :
	CTextured_Vertex() {
		m_xmf3_Position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf2_UV = DirectX::XMFLOAT2(0.0f, 0.0f);
	}
	CTextured_Vertex(float x, float y, float z, DirectX::XMFLOAT2(xmf2_UV)) {
		m_xmf3_Position = DirectX::XMFLOAT3(x, y, z);
		m_xmf2_UV = xmf2_UV;
	}
	CTextured_Vertex(DirectX::XMFLOAT3 xmf3_Position, DirectX::XMFLOAT2 xmf2_UV) {
		m_xmf3_Position = xmf3_Position;
		m_xmf2_UV = xmf2_UV;
	}
	~CTextured_Vertex() {}
};

class CMesh {
private :
	int m_nReferences = 0;

protected :
	ID3D12Resource* m_pd3d_Vertex_Buffer = NULL;
	ID3D12Resource* m_pd3d_Vertex_Upload_Buffer = NULL;

	D3D12_VERTEX_BUFFER_VIEW m_d3d_Vertex_Buffer_View;

	D3D12_PRIMITIVE_TOPOLOGY m_d3d_Primitive_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT m_nSlot = 0;
	UINT m_nVertices = 0;
	UINT m_nStride = 0;
	UINT m_nOffset = 0;

	//
	ID3D12Resource* m_pd3d_Index_Buffer = NULL;
	ID3D12Resource* m_pd3d_Index_Upload_Buffer = NULL;

	D3D12_INDEX_BUFFER_VIEW m_d3d_Index_Buffer_View;

	UINT m_nIndices = 0;
	UINT m_nStart_Index = 0;
	UINT m_nBase_Vertex = 0;

	//
	DirectX::BoundingOrientedBox m_xmOOBB;

	//
	CDiffused_Vertex* m_pVertices = NULL;
	UINT* m_pnIndices = NULL;

public :
	CMesh() = default;
	CMesh(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List);
	virtual ~CMesh();

	void Add_Ref() {
		++m_nReferences;
	}
	void Release() {
		if (--m_nReferences <= 0) {
			delete this;
		}
	}

	void Release_Upload_Buffers();

public :
	//virtual void Render(ID3D12GraphicsCommandList* pd3d_Command_List);
	virtual void Render(ID3D12GraphicsCommandList* pd3d_Command_List, UINT nInstances = 1);

	//
	virtual DirectX::BoundingOrientedBox Get_OBB() { return m_xmOOBB; }

	//
	int Chk_Ray_Intersection(DirectX::XMFLOAT3& xmf3_Ray_Position, DirectX::XMFLOAT3& xmf3_Ray_Direction, float* pfNear_Hit_Distance, int* nPrimitive_Number = NULL);
};

class CTriangle_Mesh : public CMesh {
public :
	CTriangle_Mesh(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List);
	virtual ~CTriangle_Mesh() {};
};

class CCube_Mesh : public CMesh {
public :
	CCube_Mesh(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCube_Mesh();

	//
	virtual DirectX::BoundingOrientedBox Get_OBB();
};

//
class CAirPlane_Mesh : public CMesh {
public:
	CAirPlane_Mesh(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f, DirectX::XMFLOAT4 xmf4_Color = DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CAirPlane_Mesh();
};

//
class CSphere_Mesh : public CMesh {
public :
	CSphere_Mesh(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List, float fRadius = 2.0f, int nSlices = 20, int nStacks = 20);
	virtual ~CSphere_Mesh();
};

//
class CBounding_Box_Mesh : public CMesh {
public :
	CBounding_Box_Mesh(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List, float fWidth, float fHeight, float fDepth);
	virtual ~CBounding_Box_Mesh();
};

//
class CUI_Box_Mesh : public CMesh {
public :
	CUI_Box_Mesh(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List, float fPosition_x, float fPosition_y, float fWidth, float fHeight, DirectX::XMFLOAT4 xmf4_Color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f));
	virtual ~CUI_Box_Mesh();
};

//
class CNumber_Mesh : public CMesh {
private :
	CTextured_Vertex* m_pTextured_Vertices = NULL;

public:
	CNumber_Mesh(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List, int nIndex);
	virtual ~CNumber_Mesh();
};