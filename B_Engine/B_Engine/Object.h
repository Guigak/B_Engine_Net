#pragma once

#include "stdafx.h"
#include "Mesh.h"
#include "Camera.h"

class CShader;

class CObject {
private :
	int m_nReferences = 0;

protected :
	DirectX::XMFLOAT4X4 m_xmf4x4_World;
	CMesh* m_pMesh = NULL;

	CShader* m_pShader = NULL;

	//
	DirectX::XMFLOAT4 m_xmf4_Color;

public :
	CObject();
	virtual ~CObject();

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
	void Release_Upload_Buffers();

	virtual void Set_Mesh(CMesh* pMesh);
	virtual void Set_Shader(CShader* pShader);

	//
	virtual CMesh* Get_Mesh() { return m_pMesh; }

	virtual void Anim(float fElapsed_Time);

	virtual void Prepare_Render();
	virtual void Render(ID3D12GraphicsCommandList* pd3d_Command_List, CCamera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList* pd3d_Command_List, CCamera* pCamera, UINT nInstances);

	void Rotate(DirectX::XMFLOAT3* pxmf3_Axis, float fAngle);

	//
	virtual void Crt_Shader_Variables(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List);
	virtual void Udt_Shader_Variables(ID3D12GraphicsCommandList* pd3d_Command_List);
	virtual void Release_Shader_Variables();

	DirectX::XMFLOAT3 Get_Position();
	DirectX::XMFLOAT3 Get_Look();
	DirectX::XMFLOAT3 Get_Up();
	DirectX::XMFLOAT3 Get_Right();

	void Set_Position(float x, float y, float z);
	void Set_Position(DirectX::XMFLOAT3 xmf3_Position);

	void Move_Right(float fDistance = 1.0f);
	void Move_Up(float fDistance = 1.0f);
	void Move_Forward(float fDistance = 1.0f);

	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	//
	virtual DirectX::XMFLOAT4X4 Get_World_Matrix() { return m_xmf4x4_World; };

	//
	void Generate_Ray_4_Picking(DirectX::XMFLOAT3& xmf3_Pick_Position, DirectX::XMFLOAT4X4& xmf4x4_View, DirectX::XMFLOAT3* pxmf3_Pick_Ray_Position, DirectX::XMFLOAT3* pxmf3_Pick_Ray_Direction);
	int Pick_Object_By_Ray_Intersection(DirectX::XMFLOAT3& xmf3_Pick_Position, DirectX::XMFLOAT4X4& xmf4x4_View, float* pfHit_Distance, CMesh* pMesh = NULL, int* nPrimive_Number = NULL);

	//
	void Set_Color(float fColor_r, float fColor_g, float fColor_b, float fColor_a);
	void Set_Color(DirectX::XMFLOAT4 xmf4_Color);

	DirectX::XMFLOAT4 Get_Color() { return m_xmf4_Color; };
};

class CRotating_Object : public CObject {
private :
	DirectX::XMFLOAT3 m_xmf3_Rotation_Axis;
	float m_fRotation_Speed;

public :
	CRotating_Object();
	virtual ~CRotating_Object();

	void Set_Rotation_Axis(DirectX::XMFLOAT3 xmf3_Rotation_Axis) { m_xmf3_Rotation_Axis = xmf3_Rotation_Axis; }
	void Set_Rotation_Speed(float fRotation_Speed) { m_fRotation_Speed = fRotation_Speed; }

	virtual void Anim(float fElapsed_Time);
};