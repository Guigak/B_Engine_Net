#include "Camera.h"
#include "Player.h"

CCamera::CCamera() {
	m_xmf4x4_View = Matrix4x4::Identity();
	m_xmf4x4_Projection = Matrix4x4::Identity();
	m_d3d_Viewport = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	m_d3d_ScissorRect = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };

	m_xmf3_Position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3_Right = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3_Up = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3_Look = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_fPitch = 0.0f;
	m_fRoll = 0.0f;
	m_fYaw = 0.0f;

	m_xmf3_Offset = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fLag_Time = 0.0f;
	m_xmf3_LookAt = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_nMode = 0x00;
	m_pPlayer = NULL;
}

CCamera::CCamera(CCamera* pCamera) {
	if (pCamera) {
		*this = *pCamera;
	}
	else {
		m_xmf4x4_View = Matrix4x4::Identity();
		m_xmf4x4_Projection = Matrix4x4::Identity();
		m_d3d_Viewport = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
		m_d3d_ScissorRect = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };

		m_xmf3_Position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_xmf3_Right = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
		m_xmf3_Up = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
		m_xmf3_Look = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);

		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		m_fYaw = 0.0f;

		m_xmf3_Offset = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_fLag_Time = 0.0f;
		m_xmf3_LookAt = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		m_nMode = 0x00;
		m_pPlayer = NULL;
	}
}

CCamera::~CCamera() {
}

void CCamera::Crt_Shader_Variables(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List) {
}

void CCamera::Release_Shader_Variables() {
}

void CCamera::Udt_Shader_Variables(ID3D12GraphicsCommandList* pd3d_Command_List) {
	Regenerate_View_Matrix();

	DirectX::XMFLOAT4X4 xmf4x4_View;
	DirectX::XMStoreFloat4x4(&xmf4x4_View, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_xmf4x4_View)));
	pd3d_Command_List->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4_View, 0);

	DirectX::XMFLOAT4X4 xmf4x4_Projection;
	DirectX::XMStoreFloat4x4(&xmf4x4_Projection, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&m_xmf4x4_Projection)));
	pd3d_Command_List->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4_Projection, 16);
}

void CCamera::Generate_View_Matrix() {
	m_xmf4x4_View = Matrix4x4::LookAt_LH(m_xmf3_Position, m_xmf3_LookAt, m_xmf3_Up);
}

void CCamera::Generate_View_Matrix(DirectX::XMFLOAT3 xmf3_Position, DirectX::XMFLOAT3 xmf3_LookAt, DirectX::XMFLOAT3 xmf3_Up) {
	m_xmf3_Position = xmf3_Position;
	m_xmf3_LookAt = xmf3_LookAt;
	m_xmf3_Up = xmf3_Up;

	Generate_View_Matrix();
}

void CCamera::Regenerate_View_Matrix() {
	Update(m_pPlayer->Get_Position(), 0.0f);

	m_xmf3_Look = Vector3::Normalize(m_xmf3_Look);
	m_xmf3_Right = Vector3::Cross_Product(m_xmf3_Up, m_xmf3_Look, true);
	m_xmf3_Up = Vector3::Cross_Product(m_xmf3_Look, m_xmf3_Right, true);
	m_xmf4x4_View._11 = m_xmf3_Right.x;
	m_xmf4x4_View._12 = m_xmf3_Up.x;
	m_xmf4x4_View._13 = m_xmf3_Look.x;

	m_xmf4x4_View._21 = m_xmf3_Right.y;
	m_xmf4x4_View._22 = m_xmf3_Up.y;
	m_xmf4x4_View._23 = m_xmf3_Look.y;

	m_xmf4x4_View._31 = m_xmf3_Right.z;
	m_xmf4x4_View._32 = m_xmf3_Up.z;
	m_xmf4x4_View._33 = m_xmf3_Look.z;

	m_xmf4x4_View._41 = -Vector3::Dot_Product(m_xmf3_Position, m_xmf3_Right);
	m_xmf4x4_View._42 = -Vector3::Dot_Product(m_xmf3_Position, m_xmf3_Up);
	m_xmf4x4_View._43 = -Vector3::Dot_Product(m_xmf3_Position, m_xmf3_Look);
}

void CCamera::Gernerate_Projection_Matrix(float fFovAngle, float fAspect_Ratio, float fNear_Plane_Distance, float fFar_Plane_Distance) {
	m_xmf4x4_Projection = Matrix4x4::PerspectiveFov_LH(DirectX::XMConvertToRadians(fFovAngle), fAspect_Ratio, fNear_Plane_Distance, fFar_Plane_Distance);
}

void CCamera::Set_Viewport(int x_Top_Left, int y_Top_Left, int nWidth, int nHeight, float fMin_Z, float fMax_Z) {
	m_d3d_Viewport.TopLeftX = float(x_Top_Left);
	m_d3d_Viewport.TopLeftY = float(y_Top_Left);
	m_d3d_Viewport.Width = float(nWidth);
	m_d3d_Viewport.Height = float(nHeight);
	m_d3d_Viewport.MinDepth = fMin_Z;
	m_d3d_Viewport.MaxDepth = fMax_Z;
}

void CCamera::Set_ScissorRect(LONG x_Left, LONG y_top, LONG x_Right, LONG y_Bottom) {
	m_d3d_ScissorRect.left = x_Left;
	m_d3d_ScissorRect.top = y_top;
	m_d3d_ScissorRect.right = x_Right;
	m_d3d_ScissorRect.bottom = y_Bottom;
}

void CCamera::Set_Viewports_N_ScissorRects(ID3D12GraphicsCommandList* pd3d_Command_List) {
	pd3d_Command_List->RSSetViewports(1, &m_d3d_Viewport);
	pd3d_Command_List->RSSetScissorRects(1, &m_d3d_ScissorRect);
}

CSpaceShip_Camera::CSpaceShip_Camera(CCamera* pCamera) {
	m_nMode = SPACESHIP_CAMERA;
}

void CSpaceShip_Camera::Rotate(float fPitch, float fYaw, float fRoll) {
	if (m_pPlayer && (fPitch != 0.0f)) {
		DirectX::XMFLOAT3 xmf3_Right = m_pPlayer->Get_Right_Vector();
		DirectX::XMMATRIX xmmtx_Rotate = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&xmf3_Right), DirectX::XMConvertToRadians(fPitch));

		m_xmf3_Right = Vector3::Transform_Normal(m_xmf3_Right, xmmtx_Rotate);
		m_xmf3_Up = Vector3::Transform_Normal(m_xmf3_Up, xmmtx_Rotate);
		m_xmf3_Look = Vector3::Transform_Normal(m_xmf3_Look, xmmtx_Rotate);

		m_xmf3_Position = Vector3::Subtract(m_xmf3_Position, m_pPlayer->Get_Position());
		m_xmf3_Position = Vector3::Transform_Coord(m_xmf3_Position, xmmtx_Rotate);
		m_xmf3_Position = Vector3::Add(m_xmf3_Position, m_pPlayer->Get_Position());
	}

	if (m_pPlayer && (fYaw != 0.0f)) {
		DirectX::XMFLOAT3 xmf3_Up = m_pPlayer->Get_Up_Vector();
		DirectX::XMMATRIX xmmtx_Rotate = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&xmf3_Up), DirectX::XMConvertToRadians(fYaw));

		m_xmf3_Right = Vector3::Transform_Normal(m_xmf3_Right, xmmtx_Rotate);
		m_xmf3_Up = Vector3::Transform_Normal(m_xmf3_Up, xmmtx_Rotate);
		m_xmf3_Look = Vector3::Transform_Normal(m_xmf3_Look, xmmtx_Rotate);

		m_xmf3_Position = Vector3::Subtract(m_xmf3_Position, m_pPlayer->Get_Position());
		m_xmf3_Position = Vector3::Transform_Coord(m_xmf3_Position, xmmtx_Rotate);
		m_xmf3_Position = Vector3::Add(m_xmf3_Position, m_pPlayer->Get_Position());
	}

	if (m_pPlayer && (fRoll != 0.0f)) {
		DirectX::XMFLOAT3 xmf3_Look = m_pPlayer->Get_Up_Vector();
		DirectX::XMMATRIX xmmtx_Rotate = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&xmf3_Look), DirectX::XMConvertToRadians(fRoll));

		m_xmf3_Right = Vector3::Transform_Normal(m_xmf3_Right, xmmtx_Rotate);
		m_xmf3_Up = Vector3::Transform_Normal(m_xmf3_Up, xmmtx_Rotate);
		m_xmf3_Look = Vector3::Transform_Normal(m_xmf3_Look, xmmtx_Rotate);

		m_xmf3_Position = Vector3::Subtract(m_xmf3_Position, m_pPlayer->Get_Position());
		m_xmf3_Position = Vector3::Transform_Coord(m_xmf3_Position, xmmtx_Rotate);
		m_xmf3_Position = Vector3::Add(m_xmf3_Position, m_pPlayer->Get_Position());
	}
}

CFirst_Person_Camera::CFirst_Person_Camera(CCamera* pCamera) {
	m_nMode = FIRST_PERSON_CAMERA;

	if (pCamera) {
		if (pCamera->Get_Mode() == SPACESHIP_CAMERA) {
			m_xmf3_Up = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
			m_xmf3_Right.y = 0.0f;
			m_xmf3_Look.y = 0.0f;
			m_xmf3_Right = Vector3::Normalize(m_xmf3_Right);
			m_xmf3_Look = Vector3::Normalize(m_xmf3_Look);
		}
	}
}

void CFirst_Person_Camera::Update(DirectX::XMFLOAT3& xmf3_LookAt, float fElapsed_Time) {
	if (!m_pPlayer) {
		return;
	}

	DirectX::XMFLOAT4X4 xmf4x4_Rotate = Matrix4x4::Identity();
	DirectX::XMFLOAT3 xmf3_Right = m_pPlayer->Get_Right_Vector();
	DirectX::XMFLOAT3 xmf3_Up = m_pPlayer->Get_Up_Vector();
	DirectX::XMFLOAT3 xmf3_Look = m_pPlayer->Get_Look_Vector();

	xmf4x4_Rotate._11 = xmf3_Right.x;
	xmf4x4_Rotate._21 = xmf3_Up.x;
	xmf4x4_Rotate._31 = xmf3_Look.x;

	xmf4x4_Rotate._12 = xmf3_Right.y;
	xmf4x4_Rotate._22 = xmf3_Up.y;
	xmf4x4_Rotate._32 = xmf3_Look.y;

	xmf4x4_Rotate._13 = xmf3_Right.z;
	xmf4x4_Rotate._23 = xmf3_Up.z;
	xmf4x4_Rotate._33 = xmf3_Look.z;

	DirectX::XMFLOAT3 xmf3_Offset = Vector3::Transform_Coord(m_xmf3_Offset, xmf4x4_Rotate);
	DirectX::XMFLOAT3 xmf3_Position = Vector3::Add(m_pPlayer->Get_Position(), xmf3_Offset);

	Set_Position(xmf3_Position);
}

void CFirst_Person_Camera::Rotate(float fPitch, float fYaw, float fRoll) {
	if (fPitch != 0.0f)
	{
		DirectX::XMMATRIX xmmtx_Rotate = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&m_xmf3_Right), DirectX::XMConvertToRadians(fPitch));

		m_xmf3_Look = Vector3::Transform_Normal(m_xmf3_Look, xmmtx_Rotate);
		m_xmf3_Up = Vector3::Transform_Normal(m_xmf3_Up, xmmtx_Rotate);
		m_xmf3_Right = Vector3::Transform_Normal(m_xmf3_Right, xmmtx_Rotate);
	}

	if (m_pPlayer && (fYaw != 0.0f))
	{
		DirectX::XMFLOAT3 xmf3_Up = m_pPlayer->Get_Up_Vector();
		DirectX::XMMATRIX xmmtx_Rotate = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&xmf3_Up), DirectX::XMConvertToRadians(fYaw));

		m_xmf3_Look = Vector3::Transform_Normal(m_xmf3_Look, xmmtx_Rotate);
		m_xmf3_Up = Vector3::Transform_Normal(m_xmf3_Up, xmmtx_Rotate);
		m_xmf3_Right = Vector3::Transform_Normal(m_xmf3_Right, xmmtx_Rotate);
	}

	if (m_pPlayer && (fRoll != 0.0f))
	{
		DirectX::XMFLOAT3 xmf3_Look = m_pPlayer->Get_Look_Vector();
		DirectX::XMMATRIX xmmtxRotate = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&xmf3_Look), DirectX::XMConvertToRadians(fRoll));

		//m_xmf3_Position = Vector3::Subtract(m_xmf3_Position, m_pPlayer->Get_Position());
		//m_xmf3_Position = Vector3::Transform_Coord(m_xmf3_Position, xmmtxRotate);
		//m_xmf3_Position = Vector3::Add(m_xmf3_Position, m_pPlayer->Get_Position());

		m_xmf3_Look = Vector3::Transform_Normal(m_xmf3_Look, xmmtxRotate);
		m_xmf3_Up = Vector3::Transform_Normal(m_xmf3_Up, xmmtxRotate);
		m_xmf3_Right = Vector3::Transform_Normal(m_xmf3_Right, xmmtxRotate);
	}
}

CThird_Person_Camera::CThird_Person_Camera(CCamera* pCamera) {
	m_nMode = THIRD_PERSON_CAMERA;

	if (pCamera) {
		if (pCamera->Get_Mode() == SPACESHIP_CAMERA) {
			m_xmf3_Up = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
			m_xmf3_Right.y = 0.0f;
			m_xmf3_Look.y = 0.0f;
			m_xmf3_Right = Vector3::Normalize(m_xmf3_Right);
			m_xmf3_Look = Vector3::Normalize(m_xmf3_Look);
		}
	}
}

void CThird_Person_Camera::Update(DirectX::XMFLOAT3& xmf3_LookAt, float fElapsed_Time) {
	if (m_pPlayer) {
		DirectX::XMFLOAT4X4 xmf4x4_Rotate = Matrix4x4::Identity();
		DirectX::XMFLOAT3 xmf3_Right = m_pPlayer->Get_Right_Vector();
		DirectX::XMFLOAT3 xmf3_Up = m_pPlayer->Get_Up_Vector();
		DirectX::XMFLOAT3 xmf3_Look = m_pPlayer->Get_Look_Vector();

		xmf4x4_Rotate._11 = xmf3_Right.x;
		xmf4x4_Rotate._21 = xmf3_Up.x;
		xmf4x4_Rotate._31 = xmf3_Look.x;

		xmf4x4_Rotate._12 = xmf3_Right.y;
		xmf4x4_Rotate._22 = xmf3_Up.y;
		xmf4x4_Rotate._32 = xmf3_Look.y;

		xmf4x4_Rotate._13 = xmf3_Right.z;
		xmf4x4_Rotate._23 = xmf3_Up.z;
		xmf4x4_Rotate._33 = xmf3_Look.z;

		DirectX::XMFLOAT3 xmf3_Offset = Vector3::Transform_Coord(m_xmf3_Offset, xmf4x4_Rotate);
		DirectX::XMFLOAT3 xmf3_Position = Vector3::Add(m_pPlayer->Get_Position(), xmf3_Offset);
		DirectX::XMFLOAT3 xmf3_Direction = Vector3::Subtract(xmf3_Position, m_xmf3_Position);

		float fLength = Vector3::Length(xmf3_Direction);
		xmf3_Direction = Vector3::Normalize(xmf3_Direction);

		float fLag_Time_Scale = (m_fLag_Time) ? fElapsed_Time * (1.0f / m_fLag_Time) : 1.0f;
		float fDistance = fLength * fLag_Time_Scale;

		if (fDistance > fLength) {
			fDistance = fLength;
		}

		if (fLength < 0.01f) {
			fDistance = fLength;
		}

		if (fDistance > 0) {
			m_xmf3_Position = Vector3::Add(m_xmf3_Position, xmf3_Direction, fDistance);

			Set_LookAt(xmf3_LookAt);
		}
	}
}

void CThird_Person_Camera::Set_LookAt(DirectX::XMFLOAT3& xmf3_LookAt) {
	DirectX::XMFLOAT4X4 xmmtx_LookAt = Matrix4x4::LookAt_LH(m_xmf3_Position, xmf3_LookAt, m_pPlayer->Get_Up_Vector());

	m_xmf3_Right = DirectX::XMFLOAT3(xmmtx_LookAt._11, xmmtx_LookAt._21, xmmtx_LookAt._31);
	m_xmf3_Up = DirectX::XMFLOAT3(xmmtx_LookAt._12, xmmtx_LookAt._22, xmmtx_LookAt._32);
	m_xmf3_Look = DirectX::XMFLOAT3(xmmtx_LookAt._13, xmmtx_LookAt._23, xmmtx_LookAt._33);
}
