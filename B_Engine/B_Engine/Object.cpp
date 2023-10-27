#include "Object.h"
#include "Shader.h"

CObject::CObject() {
	DirectX::XMStoreFloat4x4(&m_xmf4x4_World, DirectX::XMMatrixIdentity());
}

CObject::~CObject() {
	if (m_pMesh) {
		m_pMesh->Release();

		if (m_pBounding_Box_Mesh) {
			m_pBounding_Box_Mesh->Release();
		}
	}
	if (m_pShader) {
		m_pShader->Release_Shader_Variables();
		m_pShader->Release();
	}
}

void CObject::Release_Upload_Buffers() {
	if (m_pMesh) {
		m_pMesh->Release_Upload_Buffers();
	}
}

void CObject::Set_Mesh(CMesh* pMesh) {
	if (m_pMesh) {
		m_pMesh->Release();
	}

	m_pMesh = pMesh;

	if (m_pMesh) {
		m_pMesh->Add_Ref();
	}
}

void CObject::Set_Bounding_Box_Mesh(CBounding_Box_Mesh* pBounding_Box_Mesh) {
	if (m_pBounding_Box_Mesh) {
		m_pBounding_Box_Mesh->Release();
	}

	m_pBounding_Box_Mesh = pBounding_Box_Mesh;

	if (m_pBounding_Box_Mesh) {
		m_pBounding_Box_Mesh->Add_Ref();
	}
}

void CObject::Set_Shader(CShader* pShader) {
	if (m_pShader) {
		m_pShader->Release();
	}

	m_pShader = pShader;

	if (m_pShader) {
		m_pShader->Add_Ref();
	}
}

void CObject::Anim(float fElapsed_Time) {
}

void CObject::Prepare_Render() {
}

void CObject::Render(ID3D12GraphicsCommandList* pd3d_Command_List, CCamera* pCamera) {
	Prepare_Render();

	Udt_Shader_Variables(pd3d_Command_List);

	if (m_pShader) {
		m_pShader->Udt_Shader_Variable(pd3d_Command_List, &m_xmf4x4_World);
		m_pShader->Render(pd3d_Command_List, pCamera);
	}

	if (m_pMesh) {
		m_pMesh->Render(pd3d_Command_List);
	}
}

void CObject::Render(ID3D12GraphicsCommandList* pd3d_Command_List, CCamera* pCamera, UINT nInstances) {
	Prepare_Render();

	if (m_pMesh) {
		m_pMesh->Render(pd3d_Command_List, nInstances);
	}
}

void CObject::Render_Bounding_Box(ID3D12GraphicsCommandList* pd3d_Command_List, CCamera* pCamera, UINT nInstances) {
	Prepare_Render();

	if (m_pBounding_Box_Mesh) {
		m_pBounding_Box_Mesh->Render(pd3d_Command_List, nInstances);
	}
}

void CObject::Rotate(DirectX::XMFLOAT3* pxmf3_Axis, float fAngle) {
	DirectX::XMMATRIX xmmtx_Rotate = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(pxmf3_Axis), DirectX::XMConvertToRadians(fAngle));
	m_xmf4x4_World = Matrix4x4::Multiply(xmmtx_Rotate, m_xmf4x4_World);
}

void CObject::Crt_Shader_Variables(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List) {
}

void CObject::Udt_Shader_Variables(ID3D12GraphicsCommandList* pd3d_Command_List) {
	DirectX::XMFLOAT4X4 xmf4x4_World;
	DirectX::XMStoreFloat4x4(&xmf4x4_World, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_xmf4x4_World)));

	pd3d_Command_List->SetGraphicsRoot32BitConstants(0, 16, &xmf4x4_World, 0);
}

void CObject::Release_Shader_Variables() {
}

DirectX::XMFLOAT3 CObject::Get_Position() {
	return DirectX::XMFLOAT3(m_xmf4x4_World._41, m_xmf4x4_World._42, m_xmf4x4_World._43);
}

DirectX::XMFLOAT3 CObject::Get_Look() {
	return Vector3::Normalize(DirectX::XMFLOAT3(m_xmf4x4_World._31, m_xmf4x4_World._32, m_xmf4x4_World._33));
}

DirectX::XMFLOAT3 CObject::Get_Up() {
	return Vector3::Normalize(DirectX::XMFLOAT3(m_xmf4x4_World._21, m_xmf4x4_World._22, m_xmf4x4_World._23));
}

DirectX::XMFLOAT3 CObject::Get_Right() {
	return Vector3::Normalize(DirectX::XMFLOAT3(m_xmf4x4_World._11, m_xmf4x4_World._12, m_xmf4x4_World._13));
}

void CObject::Set_Position(float x, float y, float z) {
	m_xmf4x4_World._41 = x;
	m_xmf4x4_World._42 = y;
	m_xmf4x4_World._43 = z;
}

void CObject::Set_Position(DirectX::XMFLOAT3 xmf3_Position) {
	Set_Position(xmf3_Position.x, xmf3_Position.y, xmf3_Position.z);
}

void CObject::Move_Right(float fDistance) {
	DirectX::XMFLOAT3 xmf3_Position = Get_Position();
	DirectX::XMFLOAT3 xmf3_Right = Get_Right();
	xmf3_Position = Vector3::Add(xmf3_Position, xmf3_Right, fDistance);

	Set_Position(xmf3_Position);
}

void CObject::Move_Up(float fDistance) {
	DirectX::XMFLOAT3 xmf3_Position = Get_Position();
	DirectX::XMFLOAT3 xmf3_Up = Get_Up();
	xmf3_Position = Vector3::Add(xmf3_Position, xmf3_Up, fDistance);

	Set_Position(xmf3_Position);
}

void CObject::Move_Forward(float fDistance) {
	DirectX::XMFLOAT3 xmf3_Position = Get_Position();
	DirectX::XMFLOAT3 xmf3_Look = Get_Look();
	xmf3_Position = Vector3::Add(xmf3_Position, xmf3_Look, fDistance);

	Set_Position(xmf3_Position);
}

void CObject::Rotate(float fPitch, float fYaw, float fRoll) {
	DirectX::XMMATRIX xmmtx_Rotate = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(fPitch), DirectX::XMConvertToRadians(fYaw), DirectX::XMConvertToRadians(fRoll));
	m_xmf4x4_World = Matrix4x4::Multiply(xmmtx_Rotate, m_xmf4x4_World);
}

void CObject::Generate_Ray_4_Picking(DirectX::XMFLOAT3& xmf3_Pick_Position, DirectX::XMFLOAT4X4& xmf4x4_View, DirectX::XMFLOAT3* pxmf3_Pick_Ray_Position, DirectX::XMFLOAT3* pxmf3_Pick_Ray_Direction) {
	DirectX::XMFLOAT4X4 xmf4x4_World_View = Matrix4x4::Multiply(m_xmf4x4_World, xmf4x4_View);
	DirectX::XMFLOAT4X4 xmf4x4_Inverse = Matrix4x4::Inverse(xmf4x4_World_View);
	DirectX::XMFLOAT3 xmf3_Camera_Position(0.0f, 0.0f, 0.0f);

	*pxmf3_Pick_Ray_Position = Vector3::Transform_Coord(xmf3_Camera_Position, xmf4x4_Inverse);
	*pxmf3_Pick_Ray_Direction = Vector3::Transform_Coord(xmf3_Pick_Position, xmf4x4_Inverse);
	*pxmf3_Pick_Ray_Direction = Vector3::Normalize(Vector3::Subtract(*pxmf3_Pick_Ray_Direction, *pxmf3_Pick_Ray_Position));
}

int CObject::Pick_Object_By_Ray_Intersection(DirectX::XMFLOAT3& xmf3_Pick_Position, DirectX::XMFLOAT4X4& xmf4x4_View, float* pfHit_Distance, CMesh* pMesh, int* nPrimive_Number) {
	int nIntersected = 0;

	if (m_pMesh){
		DirectX::XMFLOAT3 xmf3_Pick_Ray_Position, xmf3_Pick_Ray_Direction;
		Generate_Ray_4_Picking(xmf3_Pick_Position, xmf4x4_View, &xmf3_Pick_Ray_Position, &xmf3_Pick_Ray_Direction);

		nIntersected = m_pMesh->Chk_Ray_Intersection(xmf3_Pick_Ray_Position, xmf3_Pick_Ray_Direction, pfHit_Distance, nPrimive_Number);
	}
	else {
		DirectX::XMFLOAT3 xmf3_Pick_Ray_Position, xmf3_Pick_Ray_Direction;
		Generate_Ray_4_Picking(xmf3_Pick_Position, xmf4x4_View, &xmf3_Pick_Ray_Position, &xmf3_Pick_Ray_Direction);

		nIntersected = pMesh->Chk_Ray_Intersection(xmf3_Pick_Ray_Position, xmf3_Pick_Ray_Direction, pfHit_Distance, nPrimive_Number);
	}

	return nIntersected;
}

void CObject::Set_Color(float fColor_r, float fColor_g, float fColor_b, float fColor_a) {
	m_xmf4_Color = DirectX::XMFLOAT4(fColor_r, fColor_g, fColor_b, fColor_a);
}

void CObject::Set_Color(DirectX::XMFLOAT4 xmf4_Color) {
	Set_Color(xmf4_Color.x, xmf4_Color.y, xmf4_Color.z, xmf4_Color.w);
}

//
CRotating_Object::CRotating_Object() {
	m_xmf3_Rotation_Axis = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_fRotation_Speed = 90.0f;
}

CRotating_Object::~CRotating_Object() {
}

void CRotating_Object::Anim(float fElapsed_Time) {
	CObject::Rotate(&m_xmf3_Rotation_Axis, m_fRotation_Speed * fElapsed_Time);
}
