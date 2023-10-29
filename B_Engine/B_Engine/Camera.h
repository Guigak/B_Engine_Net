#pragma once

#include "stdafx.h"

#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

#define FIRST_PERSON_CAMERA 0x01
#define SPACESHIP_CAMERA 0x02
#define THIRD_PERSON_CAMERA 0x03

class CPlayer;

struct VS_CB_CAMERA_INFO {
	DirectX::XMFLOAT4X4 m_xmf4x4_View;
	DirectX::XMFLOAT4X4 m_xmf4x4_Projection;
};

class CCamera {
protected :
	DirectX::XMFLOAT3 m_xmf3_Position;

	DirectX::XMFLOAT3 m_xmf3_Right;
	DirectX::XMFLOAT3 m_xmf3_Up;
	DirectX::XMFLOAT3 m_xmf3_Look;

	float m_fPitch;
	float m_fYaw;
	float m_fRoll;

	DWORD m_nMode;

	// third person
	DirectX::XMFLOAT3 m_xmf3_LookAt;
	DirectX::XMFLOAT3 m_xmf3_Offset;

	float m_fLag_Time;

	DirectX::XMFLOAT4X4 m_xmf4x4_View;
	DirectX::XMFLOAT4X4 m_xmf4x4_Projection;

	D3D12_VIEWPORT m_d3d_Viewport;
	D3D12_RECT m_d3d_ScissorRect;

	//
	CPlayer* m_pPlayer = NULL;

public :
	CCamera();
	CCamera(CCamera* pCamera);
	virtual ~CCamera();

	// Shader
	virtual void Crt_Shader_Variables(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List);
	virtual void Release_Shader_Variables();
	virtual void Udt_Shader_Variables(ID3D12GraphicsCommandList* pd3d_Command_List);

	// View matrix
	void Generate_View_Matrix();
	void Generate_View_Matrix(DirectX::XMFLOAT3 xmf3_Position, DirectX::XMFLOAT3 xmf3_LookAt, DirectX::XMFLOAT3 xmf3_Up);

	void Regenerate_View_Matrix();

	// Projection matrix
	void Gernerate_Projection_Matrix(float fFovAngle, float fAspect_Ratio, float fNear_Plane_Distance, float fFar_Plane_Distance);

	// viewport, scissor rect
	void Set_Viewport(int x_Top_Left, int y_Top_Left, int nWidth, int nHeight, float fMin_Z = 0.0f, float fMax_Z = 1.0f);
	void Set_ScissorRect(LONG x_Left, LONG y_top, LONG x_Right, LONG y_Bottom);

	virtual void Set_Viewports_N_ScissorRects(ID3D12GraphicsCommandList* pd3d_Command_List);

	//
	void Set_Player(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	CPlayer* Get_Player() { return m_pPlayer; }

	void Set_Mode(DWORD nMode) { m_nMode = nMode; }
	DWORD Get_Mode() { return m_nMode; }

	void Set_Position(DirectX::XMFLOAT3 xmf3_Position) { m_xmf3_Position = xmf3_Position; }
	DirectX::XMFLOAT3& Get_Position() { return m_xmf3_Position; }

	void Set_LookAt_Position(DirectX::XMFLOAT3 xmf3_LookAt) { m_xmf3_LookAt = xmf3_LookAt; }
	DirectX::XMFLOAT3& Get_LookAt_Position() { return m_xmf3_LookAt; }

	DirectX::XMFLOAT3& Get_Right_Vector() { return m_xmf3_Right; }
	DirectX::XMFLOAT3& Get_Up_Vector() { return m_xmf3_Up; }
	DirectX::XMFLOAT3& Get_Look_Vector() { return m_xmf3_Look; }

	void Set_Offset(DirectX::XMFLOAT3 xmf3_Offset) { m_xmf3_Offset = xmf3_Offset; }
	DirectX::XMFLOAT3& Get_Offset() { return m_xmf3_Offset; }

	void Set_Lag_Time(float fLag_Time) { m_fLag_Time = fLag_Time; }
	float Get_Lag_Time() { return m_fLag_Time; }

	DirectX::XMFLOAT4X4 Get_View_Matrix() { return m_xmf4x4_View; }
	DirectX::XMFLOAT4X4 Get_Projection_Matrix() { return m_xmf4x4_Projection; }
	D3D12_VIEWPORT Get_ViewPort() { return m_d3d_Viewport; }
	D3D12_RECT Get_ScissorRect() { return m_d3d_ScissorRect; }

	//
	virtual void Move(DirectX::XMFLOAT3& xmf3_Shift) {
		m_xmf3_Position.x += xmf3_Shift.x;
		m_xmf3_Position.y += xmf3_Shift.y;
		m_xmf3_Position.z += xmf3_Shift.z;
	}
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f) {}
	virtual void Update(DirectX::XMFLOAT3& xmf3_LookAt, float fElapsed_Time) {}
	virtual void Set_LookAt(DirectX::XMFLOAT3& xmf3_LookAt) {}
};


//
class CSpaceShip_Camera : public CCamera {
public :
	CSpaceShip_Camera(CCamera* pCamera);
	virtual ~CSpaceShip_Camera() {}

	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
};


//
class CFirst_Person_Camera : public CCamera {
public :
	CFirst_Person_Camera(CCamera* pCamera);
	virtual ~CFirst_Person_Camera() {}

	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
};


//
class CThird_Person_Camera : public CCamera {
public :
	CThird_Person_Camera(CCamera* pCamera);
	virtual ~CThird_Person_Camera() {}

	virtual void Update(DirectX::XMFLOAT3& xmf3_LookAt, float fElapsed_Time);
	virtual void Set_LookAt(DirectX::XMFLOAT3& xmf3_LookAt);
};