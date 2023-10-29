#include "Mesh.h"

CMesh::CMesh(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List) {
}

CMesh::~CMesh() {
	if (m_pd3d_Vertex_Buffer) {
		m_pd3d_Vertex_Buffer->Release();
	}
	if (m_pd3d_Vertex_Upload_Buffer) {
		m_pd3d_Vertex_Upload_Buffer->Release();
	}

	//
	if (m_pd3d_Index_Buffer) {
		m_pd3d_Index_Buffer->Release();
	}
	if (m_pd3d_Index_Upload_Buffer) {
		m_pd3d_Index_Upload_Buffer->Release();
	}

	//
	if (m_pVertices) {
		delete[] m_pVertices;
	}
	if (m_pnIndices) {
		delete[] m_pnIndices;
	}
}

void CMesh::Release_Upload_Buffers() {
	if (m_pd3d_Vertex_Upload_Buffer) {
		m_pd3d_Vertex_Upload_Buffer->Release();
	}
	m_pd3d_Vertex_Upload_Buffer = NULL;

	if (m_pd3d_Index_Upload_Buffer) {
		m_pd3d_Index_Upload_Buffer->Release();
	}
	m_pd3d_Index_Upload_Buffer = NULL;
}

//void CMesh::Render(ID3D12GraphicsCommandList* pd3d_Command_List) {
//	pd3d_Command_List->IASetPrimitiveTopology(m_d3d_Primitive_Topology);
//	pd3d_Command_List->IASetVertexBuffers(m_nSlot, 1, &m_d3d_Vertex_Buffer_View);
//
//	if (m_pd3d_Index_Buffer) {
//		pd3d_Command_List->IASetIndexBuffer(&m_d3d_Index_Buffer_View);
//		pd3d_Command_List->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
//	}
//	else {
//		pd3d_Command_List->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
//	}
//}

void CMesh::Render(ID3D12GraphicsCommandList* pd3d_Command_List, UINT nInstances) {
	pd3d_Command_List->IASetVertexBuffers(m_nSlot, 1, &m_d3d_Vertex_Buffer_View);
	pd3d_Command_List->IASetPrimitiveTopology(m_d3d_Primitive_Topology);

	if (m_pd3d_Index_Buffer) {
		pd3d_Command_List->IASetIndexBuffer(&m_d3d_Index_Buffer_View);
		pd3d_Command_List->DrawIndexedInstanced(m_nIndices, nInstances, 0, 0, 0);
	}
	else {
		pd3d_Command_List->DrawInstanced(m_nVertices, nInstances, m_nOffset, 0);
	}
}

int CMesh::Chk_Ray_Intersection(DirectX::XMFLOAT3& xmf3_Ray_Position, DirectX::XMFLOAT3& xmf3_Ray_Direction, float* pfNear_Hit_Distance, int* nPrimitive_Number) {
	int nIntersections = 0;
	BYTE* pbPositions = (BYTE*)m_pVertices;

	int nOffset = (m_d3d_Primitive_Topology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? 3 : 1;
	int nPrimitives = (m_d3d_Primitive_Topology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nVertices / 3) : (m_nVertices - 2);
	if (m_nIndices > 0) {
		nPrimitives = (m_d3d_Primitive_Topology == D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST) ? (m_nIndices / 3) : (m_nIndices - 2);
	}

	DirectX::XMVECTOR xmv_Ray_Position = DirectX::XMLoadFloat3(&xmf3_Ray_Position);
	DirectX::XMVECTOR xmv_Ray_Direction = DirectX::XMLoadFloat3(&xmf3_Ray_Direction);

	bool bIntersected = m_xmOOBB.Intersects(xmv_Ray_Position, xmv_Ray_Direction, *pfNear_Hit_Distance);

	if (bIntersected) {
		float fNear_Hit_Distance = FLT_MAX;

		for (int i = 0; i < nPrimitives; ++i) {
			DirectX::XMVECTOR v0 = DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i * nOffset) + 0]) : ((i * nOffset) + 0)) * m_nStride));
			DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i * nOffset) + 1]) : ((i * nOffset) + 1)) * m_nStride));
			DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)(pbPositions + ((m_pnIndices) ? (m_pnIndices[(i * nOffset) + 2]) : ((i * nOffset) + 2)) * m_nStride));

			float fHit_Distance;
			BOOL bIntersected = DirectX::TriangleTests::Intersects(xmv_Ray_Position, xmv_Ray_Direction, v0, v1, v2, fHit_Distance);

			if (bIntersected) {
				if (fHit_Distance < fNear_Hit_Distance) {
					*pfNear_Hit_Distance = fNear_Hit_Distance = fHit_Distance;

					if (nPrimitive_Number != NULL) {
						*nPrimitive_Number = i / 2;
					}
				}

				++nIntersections;
			}
		}
	}

	return nIntersections;
}

CTriangle_Mesh::CTriangle_Mesh(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List) {
	m_nVertices = 3;
	m_nStride = sizeof(CDiffused_Vertex);
	m_d3d_Primitive_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CDiffused_Vertex pVertices[3];
	pVertices[0] = CDiffused_Vertex(DirectX::XMFLOAT3(0.0f, 0.5f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	pVertices[1] = CDiffused_Vertex(DirectX::XMFLOAT3(0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f));
	pVertices[2] = CDiffused_Vertex(DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));

	m_pd3d_Vertex_Buffer = Crt_Buffer_Resource(pd3d_Device, pd3d_Command_List, pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, & m_pd3d_Vertex_Upload_Buffer);

	m_d3d_Vertex_Buffer_View.BufferLocation = m_pd3d_Vertex_Buffer->GetGPUVirtualAddress();
	m_d3d_Vertex_Buffer_View.StrideInBytes = m_nStride;
	m_d3d_Vertex_Buffer_View.SizeInBytes = m_nStride * m_nVertices;
}

CCube_Mesh::CCube_Mesh(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List, float fWidth, float fHeight, float fDepth) {
	m_nVertices = 8;
	m_nStride = sizeof(CDiffused_Vertex);
	m_d3d_Primitive_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f;
	float fy = fHeight * 0.5f;
	float fz = fDepth * 0.5f;

	m_pVertices = new CDiffused_Vertex[m_nVertices];
	m_pVertices[0] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	m_pVertices[1] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	m_pVertices[2] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	m_pVertices[3] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	m_pVertices[4] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	m_pVertices[5] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	m_pVertices[6] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	m_pVertices[7] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);

	m_pd3d_Vertex_Buffer = Crt_Buffer_Resource(pd3d_Device, pd3d_Command_List, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3d_Vertex_Upload_Buffer);

	m_d3d_Vertex_Buffer_View.BufferLocation = m_pd3d_Vertex_Buffer->GetGPUVirtualAddress();
	m_d3d_Vertex_Buffer_View.StrideInBytes = m_nStride;
	m_d3d_Vertex_Buffer_View.SizeInBytes = m_nStride * m_nVertices;

	//
	m_nIndices = 36;
	m_pnIndices = new UINT[m_nIndices];

	// top
	m_pnIndices[0] = 3;
	m_pnIndices[1] = 1;
	m_pnIndices[2] = 0;
	m_pnIndices[3] = 2;
	m_pnIndices[4] = 1;
	m_pnIndices[5] = 3;

	// front
	m_pnIndices[6] = 0;
	m_pnIndices[7] = 5;
	m_pnIndices[8] = 4;
	m_pnIndices[9] = 1;
	m_pnIndices[10] = 5;
	m_pnIndices[11] = 0;

	// left
	m_pnIndices[12] = 3;
	m_pnIndices[13] = 4;
	m_pnIndices[14] = 7;
	m_pnIndices[15] = 0;
	m_pnIndices[16] = 4;
	m_pnIndices[17] = 3;

	// right
	m_pnIndices[18] = 1;
	m_pnIndices[19] = 6;
	m_pnIndices[20] = 5;
	m_pnIndices[21] = 2;
	m_pnIndices[22] = 6;
	m_pnIndices[23] = 1;

	// back
	m_pnIndices[24] = 2;
	m_pnIndices[25] = 7;
	m_pnIndices[26] = 6;
	m_pnIndices[27] = 3;
	m_pnIndices[28] = 7;
	m_pnIndices[29] = 2;

	// bottom
	m_pnIndices[30] = 6;
	m_pnIndices[31] = 4;
	m_pnIndices[32] = 5;
	m_pnIndices[33] = 7;
	m_pnIndices[34] = 4;
	m_pnIndices[35] = 6;

	m_pd3d_Index_Buffer = Crt_Buffer_Resource(pd3d_Device, pd3d_Command_List, m_pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3d_Index_Upload_Buffer);

	m_d3d_Index_Buffer_View.BufferLocation = m_pd3d_Index_Buffer->GetGPUVirtualAddress();
	m_d3d_Index_Buffer_View.Format = DXGI_FORMAT_R32_UINT;
	m_d3d_Index_Buffer_View.SizeInBytes = sizeof(UINT) * m_nIndices;

	//
	m_xmOOBB = DirectX::BoundingOrientedBox(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(fx, fy, fz), DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CCube_Mesh::~CCube_Mesh()
{
}


//
CAirPlane_Mesh::CAirPlane_Mesh(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List, float fWidth, float fHeight, float fDepth, DirectX::XMFLOAT4 xmf4_Color) : CMesh(pd3d_Device, pd3d_Command_List){
	m_nVertices = 24 * 3;
	m_nStride = sizeof(CDiffused_Vertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3d_Primitive_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f;
	float fy = fHeight * 0.5f;
	float fz = fDepth * 0.5f;

	m_pVertices = new CDiffused_Vertex[m_nVertices];

	float x1 = fx * 0.2f;
	float y1 = fy * 0.2f;
	float x2 = fx * 0.1f;
	float y3 = fy * 0.3f;
	float y2 = ((y1 - (fy - y3)) / x1) * x2 + (fy - y3);

	int i = 0;

	// top
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(0.0f, +(fy + y3), -fz),	Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));

	// bottom
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));

	// right
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+x2, +y2, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+x2, +y2, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));

	// back right
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, -y3, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, -y3, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+x1, -y1, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(+x1, -y1, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));

	// left
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(0.0f, +(fy + y3), -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(0.0f, +(fy + y3), +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-x2, +y2, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-x2, +y2, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));

	// back left
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(0.0f, 0.0f, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(0.0f, 0.0f, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-x1, -y1, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-x1, -y1, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, -y3, +fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));
	m_pVertices[i++] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, -y3, -fz), Vector4::Add(xmf4_Color, RANDOM_COLOR));

	m_pd3d_Vertex_Buffer = Crt_Buffer_Resource(pd3d_Device, pd3d_Command_List, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3d_Vertex_Upload_Buffer);

	m_d3d_Vertex_Buffer_View.BufferLocation = m_pd3d_Vertex_Buffer->GetGPUVirtualAddress();
	m_d3d_Vertex_Buffer_View.StrideInBytes = m_nStride;
	m_d3d_Vertex_Buffer_View.SizeInBytes = m_nStride * m_nVertices;

	//
	m_xmOOBB = DirectX::BoundingOrientedBox(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(fx, fy, fz), DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CAirPlane_Mesh::~CAirPlane_Mesh() {
}

CSphere_Mesh::CSphere_Mesh(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List, float fRadius, int nSlices, int nStacks) : CMesh(pd3d_Device, pd3d_Command_List) {
	m_nStride = sizeof(CDiffused_Vertex);
	m_d3d_Primitive_Topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_nVertices = 2 + (nSlices * (nStacks - 1));
	m_pVertices = new CDiffused_Vertex[m_nVertices];

	float fDelta_Phi = float(DirectX::XM_PI / nStacks);
	float fDelta_Theta = float((2.0f * DirectX::XM_PI) / nSlices);

	int k = 0;
	m_pVertices[k++] = CDiffused_Vertex(0.0f, fRadius, 0.0f, RANDOM_COLOR);

	float i_theta, j_phi;
	for (int j = 1; j < nStacks; ++j) {
		j_phi = fDelta_Phi * j;

		for (int i = 0; i < nSlices; ++i) {
			i_theta = fDelta_Theta * i;

			m_pVertices[k++] = CDiffused_Vertex(fRadius * sinf(j_phi) * cosf(i_theta), fRadius * cosf(j_phi), fRadius * sinf(j_phi) * sinf(i_theta), RANDOM_COLOR);
		}
	}

	m_pVertices[k] = CDiffused_Vertex(0.0f, -fRadius, 0.0f, RANDOM_COLOR);

	m_pd3d_Vertex_Buffer = Crt_Buffer_Resource(pd3d_Device, pd3d_Command_List, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3d_Vertex_Upload_Buffer);

	m_d3d_Vertex_Buffer_View.BufferLocation = m_pd3d_Vertex_Buffer->GetGPUVirtualAddress();
	m_d3d_Vertex_Buffer_View.StrideInBytes = m_nStride;
	m_d3d_Vertex_Buffer_View.SizeInBytes = m_nStride * m_nVertices;

	//
	m_nIndices = (nSlices * 3) * 2 + (nSlices * (nStacks - 2) * 3 * 2);
	m_pnIndices = new UINT[m_nIndices];

	k = 0;
	for (int i = 0; i < nSlices; ++i) {
		m_pnIndices[k++] = 0;
		m_pnIndices[k++] = 1 + ((i + 1) % nSlices);
		m_pnIndices[k++] = 1 + i;
	}

	for (int j = 0; j < nStacks; ++j) {
		for (int i = 0; i < nSlices; ++i) {
			m_pnIndices[k++] = 1 + (i + (j * nSlices));
			m_pnIndices[k++] = 1 + ((i + 1) % nSlices) + (j * nSlices);
			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));

			m_pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			m_pnIndices[k++] = 1 + (((i + 1) % nSlices) + ((j + 1) * nSlices));
		}
	}

	for (int i = 0; i < nSlices; ++i) {
		m_pnIndices[k++] = m_nVertices - 1;
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + i;
		m_pnIndices[k++] = ((m_nVertices - 1) - nSlices) + ((i + 1) % nSlices);
	}

	m_pd3d_Index_Buffer = Crt_Buffer_Resource(pd3d_Device, pd3d_Command_List, m_pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3d_Index_Upload_Buffer);

	m_d3d_Index_Buffer_View.BufferLocation = m_pd3d_Index_Buffer->GetGPUVirtualAddress();
	m_d3d_Index_Buffer_View.Format = DXGI_FORMAT_R32_UINT;
	m_d3d_Index_Buffer_View.SizeInBytes = sizeof(UINT) * m_nIndices;

	//
	m_xmOOBB = DirectX::BoundingOrientedBox(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(fRadius, fRadius, fRadius), DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
}

CSphere_Mesh::~CSphere_Mesh() {
}

//
CBounding_Box_Mesh::CBounding_Box_Mesh(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List, float fWidth, float fHeight, float fDepth) {
	m_nVertices = 12 * 2;
	m_nStride = sizeof(CDiffused_Vertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_d3d_Primitive_Topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

	float fx = fWidth * 0.5f;
	float fy = fHeight * 0.5f;
	float fz = fDepth * 0.5f;

	DirectX::XMFLOAT4 xmf4_Color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	m_pVertices = new CDiffused_Vertex[m_nVertices];
	m_pVertices[0] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, +fy, -fz), xmf4_Color);
	m_pVertices[1] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, +fy, +fz), xmf4_Color);

	m_pVertices[2] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, +fy, +fz), xmf4_Color);
	m_pVertices[3] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, +fy, +fz), xmf4_Color);

	m_pVertices[4] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, +fy, +fz), xmf4_Color);
	m_pVertices[5] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, +fy, -fz), xmf4_Color);

	m_pVertices[6] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, +fy, -fz), xmf4_Color);
	m_pVertices[7] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, +fy, -fz), xmf4_Color);

	//
	m_pVertices[8] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, -fy, -fz), xmf4_Color);
	m_pVertices[9] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, -fy, +fz), xmf4_Color);

	m_pVertices[10] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, -fy, +fz), xmf4_Color);
	m_pVertices[11] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, -fy, +fz), xmf4_Color);

	m_pVertices[12] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, -fy, +fz), xmf4_Color);
	m_pVertices[13] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, -fy, -fz), xmf4_Color);

	m_pVertices[14] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, -fy, -fz), xmf4_Color);
	m_pVertices[15] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, -fy, -fz), xmf4_Color);

	//
	m_pVertices[16] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, -fy, -fz), xmf4_Color);
	m_pVertices[17] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, +fy, -fz), xmf4_Color);

	m_pVertices[18] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, -fy, +fz), xmf4_Color);
	m_pVertices[19] = CDiffused_Vertex(DirectX::XMFLOAT3(-fx, +fy, +fz), xmf4_Color);

	m_pVertices[20] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, -fy, +fz), xmf4_Color);
	m_pVertices[21] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, +fy, +fz), xmf4_Color);

	m_pVertices[22] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, -fy, -fz), xmf4_Color);
	m_pVertices[23] = CDiffused_Vertex(DirectX::XMFLOAT3(+fx, +fy, -fz), xmf4_Color);

	m_pd3d_Vertex_Buffer = Crt_Buffer_Resource(pd3d_Device, pd3d_Command_List, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3d_Vertex_Upload_Buffer);

	m_d3d_Vertex_Buffer_View.BufferLocation = m_pd3d_Vertex_Buffer->GetGPUVirtualAddress();
	m_d3d_Vertex_Buffer_View.StrideInBytes = m_nStride;
	m_d3d_Vertex_Buffer_View.SizeInBytes = m_nStride * m_nVertices;
}

CBounding_Box_Mesh::~CBounding_Box_Mesh() {
}

//void CBounding_Box_Mesh::Render(ID3D12GraphicsCommandList* pd3d_Command_List, UINT nInstances) {
//	pd3d_Command_List->IASetVertexBuffers(m_nSlot, 1, &m_d3d_Vertex_Buffer_View);
//	pd3d_Command_List->IASetPrimitiveTopology(m_d3d_Primitive_Topology);
//
//	if (m_pd3d_Index_Buffer) {
//		pd3d_Command_List->IASetIndexBuffer(&m_d3d_Index_Buffer_View);
//		pd3d_Command_List->DrawIndexedInstanced(m_nIndices, nInstances, 0, 0, 0);
//	}
//	else {
//		pd3d_Command_List->DrawInstanced(m_nVertices, nInstances, m_nOffset, 0);
//	}
//}

//
CUI_Box_Mesh::CUI_Box_Mesh(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List,float fPosition_x, float fPosition_y, float fWidth, float fHeight, DirectX::XMFLOAT4 xmf4_Color) {
	m_nVertices = 4;
	m_nStride = sizeof(CDiffused_Vertex);
	m_d3d_Primitive_Topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fleft = fPosition_x - fWidth * 0.5f;
	float ftop = fPosition_y + fHeight * 0.5f;
	float fright = fPosition_x + fWidth * 0.5f;
	float fbottom = fPosition_y - fHeight * 0.5f;
	float fz = CAMERA_NEAR_DISTANCE;

	m_pVertices = new CDiffused_Vertex[m_nVertices];
	m_pVertices[0] = CDiffused_Vertex(DirectX::XMFLOAT3(fleft, ftop, fz), xmf4_Color);
	m_pVertices[1] = CDiffused_Vertex(DirectX::XMFLOAT3(fright, ftop, fz), xmf4_Color);
	m_pVertices[2] = CDiffused_Vertex(DirectX::XMFLOAT3(fright, fbottom, fz), xmf4_Color);
	m_pVertices[3] = CDiffused_Vertex(DirectX::XMFLOAT3(fleft, fbottom, fz), xmf4_Color);

	m_pd3d_Vertex_Buffer = Crt_Buffer_Resource(pd3d_Device, pd3d_Command_List, m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3d_Vertex_Upload_Buffer);

	m_d3d_Vertex_Buffer_View.BufferLocation = m_pd3d_Vertex_Buffer->GetGPUVirtualAddress();
	m_d3d_Vertex_Buffer_View.StrideInBytes = m_nStride;
	m_d3d_Vertex_Buffer_View.SizeInBytes = m_nStride * m_nVertices;

	//
	m_nIndices = 6;
	m_pnIndices = new UINT[m_nIndices];

	m_pnIndices[0] = 0;
	m_pnIndices[1] = 1;
	m_pnIndices[2] = 2;
	m_pnIndices[3] = 0;
	m_pnIndices[4] = 2;
	m_pnIndices[5] = 3;

	m_pd3d_Index_Buffer = Crt_Buffer_Resource(pd3d_Device, pd3d_Command_List, m_pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pd3d_Index_Upload_Buffer);

	m_d3d_Index_Buffer_View.BufferLocation = m_pd3d_Index_Buffer->GetGPUVirtualAddress();
	m_d3d_Index_Buffer_View.Format = DXGI_FORMAT_R32_UINT;
	m_d3d_Index_Buffer_View.SizeInBytes = sizeof(UINT) * m_nIndices;
}

CUI_Box_Mesh::~CUI_Box_Mesh() {
}
