#include "Player.h"
#include "Shader.h"

CPlayer::CPlayer() {
	m_pCamera = NULL;

	m_xmf3_Position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3_Right = DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_xmf3_Up = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_xmf3_Look = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);

	m_xmf3_Velocity = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_xmf3_Gravity = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_fMax_Velocity = 0.0f;
	m_fMax_Gravity = 0.0f;
	m_fFriction = 0.0f;

	m_fPitch = 0.0f;
	m_fYaw = 0.0f;
	m_fRoll = 0.0f;

	m_pPlayer_Udt_Context = NULL;
	m_pCamera_Udt_Context = NULL;
}

CPlayer::~CPlayer() {
	Release_Shader_Variables();

	if (m_pCamera) {
		delete m_pCamera;
	}
}

void CPlayer::Set_Position(float x, float y, float z) {
	m_xmf3_Position.x = x;
	m_xmf3_Position.y = x;
	m_xmf3_Position.z = x;

	m_xmf4x4_World._41 = x;
	m_xmf4x4_World._42 = y;
	m_xmf4x4_World._43 = z;
}

void CPlayer::Move(ULONG dwDirection, float fDistance, bool bVelocity) {
	if (dwDirection) {
		DirectX::XMFLOAT3 xmf3_Shift = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

		if (dwDirection & DIRECT_FORWARD) {
			xmf3_Shift = Vector3::Add(xmf3_Shift, m_xmf3_Look, fDistance);
		}
		if (dwDirection & DIRECT_BACKWARD) {
			xmf3_Shift = Vector3::Add(xmf3_Shift, m_xmf3_Look, -fDistance);
		}
		if (dwDirection & DIRECT_RIGHT) {
			xmf3_Shift = Vector3::Add(xmf3_Shift, m_xmf3_Right, fDistance);
		}
		if (dwDirection & DIRECT_LEFT) {
			xmf3_Shift = Vector3::Add(xmf3_Shift, m_xmf3_Right, -fDistance);
		}

		if (m_bAble_2_Jump) {
			if (dwDirection & DIRECT_UP) {
				m_xmf3_Velocity.y = 0.0f;
				xmf3_Shift = Vector3::Add(xmf3_Shift, m_xmf3_Up, fDistance * CUBE_WIDTH * 1.5f);
			}

			m_bAble_2_Jump = false;
		}
		//if (dwDirection & DIRECT_DOWN) {
		//	xmf3_Shift = Vector3::Add(xmf3_Shift, m_xmf3_Up, -fDistance);
		//}

		Move(xmf3_Shift, bVelocity);
	}
}

void CPlayer::Move(DirectX::XMFLOAT3& xmf3_Shift, bool bVelocity) {
	if (bVelocity) {
		m_xmf3_Velocity = Vector3::Add(m_xmf3_Velocity, xmf3_Shift);
	}
	else {
		m_xmf3_Position = Vector3::Add(m_xmf3_Position, xmf3_Shift);

		if (m_pCamera) {
			//m_pCamera->Move(xmf3_Shift);
		}
	}
}

void CPlayer::Move(float fOffset_x, float fOffset_y, float fOffset_z) {
	DirectX::XMFLOAT3 xmf3_Offset = DirectX::XMFLOAT3(fOffset_x, fOffset_y, fOffset_z);

	Move(xmf3_Offset, false);
}

void CPlayer::Rotate(float x, float y, float z) {
	DWORD nCamera_Mode = m_pCamera->Get_Mode();

	if ((nCamera_Mode == FIRST_PERSON_CAMERA) || (nCamera_Mode == THIRD_PERSON_CAMERA)) {
		if (x != 0.0f) {
			m_fPitch += x;

			if (m_fPitch > 89.0f) {
				x -= (m_fPitch - 89.0f);
				m_fPitch = 89.0f;
			}

			if (m_fPitch < -89.0f) {
				x -= (m_fPitch + 89.0f);
				m_fPitch = -89.0f;
			}
		}

		if (y != 0.0f) {
			m_fYaw += y;

			if (m_fYaw > 360.0f) {
				m_fYaw -= 360.0f;
			}

			if (m_fYaw < 0.0f) {
				m_fYaw += 360.0f;
			}
		}

		if (z != 0.0f) {
			m_fRoll += z;

			if (m_fRoll > 20.0f) {
				z -= (m_fRoll - 20.0f);
				m_fRoll = 20.0f;
			}

			if (m_fRoll < -20.0f) {
				z -= (m_fRoll + 20.0f);
				m_fRoll = -20.0f;
			}
		}

		m_pCamera->Rotate(x, y, z);

		if (y != 0.0f) {
			DirectX::XMMATRIX xmmtx_Rotate = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&m_xmf3_Up), DirectX::XMConvertToRadians(y));

			m_xmf3_Look = Vector3::Transform_Normal(m_xmf3_Look, xmmtx_Rotate);
			m_xmf3_Right = Vector3::Transform_Normal(m_xmf3_Right, xmmtx_Rotate);
		}
	}
	else if (nCamera_Mode == SPACESHIP_CAMERA) {
		m_pCamera->Rotate(x, y, z);

		if (x != 0.0f) {
			DirectX::XMMATRIX xmmtx_Rotate = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&m_xmf3_Right), DirectX::XMConvertToRadians(x));

			m_xmf3_Look = Vector3::Transform_Normal(m_xmf3_Look, xmmtx_Rotate);
			m_xmf3_Up = Vector3::Transform_Normal(m_xmf3_Up, xmmtx_Rotate);
		}

		if (y != 0.0f) {
			DirectX::XMMATRIX xmmtx_Rotate = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&m_xmf3_Up), DirectX::XMConvertToRadians(y));

			m_xmf3_Look = Vector3::Transform_Normal(m_xmf3_Look, xmmtx_Rotate);
			m_xmf3_Right = Vector3::Transform_Normal(m_xmf3_Right, xmmtx_Rotate);
		}

		if (z != 0.0f) {
			DirectX::XMMATRIX xmmtx_Rotate = DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&m_xmf3_Look), DirectX::XMConvertToRadians(z));

			m_xmf3_Up = Vector3::Transform_Normal(m_xmf3_Up, xmmtx_Rotate);
			m_xmf3_Right = Vector3::Transform_Normal(m_xmf3_Right, xmmtx_Rotate);
		}
	}

	m_xmf3_Look = Vector3::Normalize(m_xmf3_Look);
	m_xmf3_Right = Vector3::Cross_Product(m_xmf3_Up, m_xmf3_Look, true);
	m_xmf3_Up = Vector3::Cross_Product(m_xmf3_Look, m_xmf3_Right, true);
}

//void CPlayer::Update(float fElapsed_Time) {
//	m_xmf3_Velocity = Vector3::Add(m_xmf3_Velocity, Vector3::Multiply(m_xmf3_Gravity, fElapsed_Time, false));
//
//	float fLength = sqrtf(m_xmf3_Velocity.x * m_xmf3_Velocity.x + m_xmf3_Velocity.z * m_xmf3_Velocity.z);
//	float fMax_Velocity = m_fMax_Velocity * fElapsed_Time;
//
//	if (fLength > m_fMax_Velocity) {
//		m_xmf3_Velocity.x *= (m_fMax_Velocity / fLength);
//		m_xmf3_Velocity.z *= (m_fMax_Velocity / fLength);
//	}
//
//	float fMax_Gravity = m_fMax_Gravity * fElapsed_Time;
//	fLength = sqrtf(m_xmf3_Velocity.y * m_xmf3_Velocity.y);
//
//	if (fLength > m_fMax_Gravity) {
//		m_xmf3_Velocity.y *= (m_fMax_Gravity / fLength);
//	}
//
//	DirectX::XMFLOAT3 xmf3_Velocity = Vector3::Multiply(m_xmf3_Velocity, fElapsed_Time, false);
//	Move(xmf3_Velocity, false);
//
//	if (m_pPlayer_Udt_Context) {
//		Player_Udt_Callback(fElapsed_Time);
//	}
//
//	DWORD nCamera_Mode = m_pCamera->Get_Mode();
//	if (nCamera_Mode == THIRD_PERSON_CAMERA) {
//		m_pCamera->Update(m_xmf3_Position, fElapsed_Time);
//	}
//
//	m_pCamera->Update(m_xmf3_Position, fElapsed_Time);
//
//	if (m_pCamera_Udt_Context) {
//		Camera_Udt_Callback(fElapsed_Time);
//	}
//	if (nCamera_Mode == THIRD_PERSON_CAMERA) {
//		m_pCamera->Set_LookAt(m_xmf3_Position);
//	}
//
//	m_pCamera->Regenerate_View_Matrix();
//
//	fLength = Vector3::Length(m_xmf3_Velocity);
//	float fDeceleration = m_fFriction * fElapsed_Time;
//
//	if (fDeceleration > fLength) {
//		fDeceleration = fLength;
//	}
//
//	//
//	m_xmf3_Velocity = Vector3::Add(m_xmf3_Velocity, Vector3::Multiply(m_xmf3_Velocity, -fDeceleration, true));
//
//	Prepare_Render();
//}

void CPlayer::Update(float fElapsed_Time) {
	m_xmf3_Velocity = Vector3::Add(m_xmf3_Velocity, Vector3::Multiply(m_xmf3_Gravity, fElapsed_Time, false));

	float fLength = sqrtf(m_xmf3_Velocity.x * m_xmf3_Velocity.x + m_xmf3_Velocity.z * m_xmf3_Velocity.z);
	float fMax_Velocity = m_fMax_Velocity * fElapsed_Time;

	if (fLength > m_fMax_Velocity) {
		m_xmf3_Velocity.x *= (m_fMax_Velocity / fLength);
		m_xmf3_Velocity.z *= (m_fMax_Velocity / fLength);
	}

	float fMax_Gravity = m_fMax_Gravity * fElapsed_Time;
	fLength = sqrtf(m_xmf3_Velocity.y * m_xmf3_Velocity.y);

	if (fLength > m_fMax_Gravity && m_xmf3_Velocity.y < 0.0f) {
		m_xmf3_Velocity.y *= (m_fMax_Gravity / fLength);
	}

	m_xmf3_Calculated_Vel = Vector3::Multiply(m_xmf3_Velocity, fElapsed_Time, false);
	//Move(xmf3_Velocity, false);

	//
	//if (!m_bAble_2_Jump) {
	//	char str[100] = "";
	//	sprintf(str, "%.5f\t%.5f\t%.5f\n", m_xmf3_Calculated_Vel.x, m_xmf3_Calculated_Vel.y, m_xmf3_Calculated_Vel.z);
	//	OutputDebugStringA(str);
	//}

	if (m_pPlayer_Udt_Context) {
		Player_Udt_Callback(fElapsed_Time);
	}

	DWORD nCamera_Mode = m_pCamera->Get_Mode();
	if (nCamera_Mode == THIRD_PERSON_CAMERA) {
		m_pCamera->Update(m_xmf3_Position, fElapsed_Time);
	}

	m_pCamera->Update(m_xmf3_Position, fElapsed_Time);

	if (m_pCamera_Udt_Context) {
		Camera_Udt_Callback(fElapsed_Time);
	}
	if (nCamera_Mode == THIRD_PERSON_CAMERA) {
		m_pCamera->Set_LookAt(m_xmf3_Position);
	}

	m_pCamera->Regenerate_View_Matrix();

	//fLength = Vector3::Length(DirectX::XMFLOAT3(m_xmf3_Velocity.x, 0.0f, m_xmf3_Velocity.z));
	fLength = Vector3::Length(m_xmf3_Velocity);
	float fDeceleration = m_fFriction * fElapsed_Time;

	if (fDeceleration > fLength) {
		fDeceleration = fLength;
	}

	//
	DirectX::XMFLOAT3 xmf3_Friction = Vector3::Add(m_xmf3_Velocity, Vector3::Multiply(m_xmf3_Velocity, -fDeceleration, true));

	//if (!m_bAble_2_Jump) {
	//	char str[100] = "";
	//	sprintf(str, "%.5f\t%.5f\t%.5f\n", xmf3_Friction.x, xmf3_Friction.y, xmf3_Friction.z);
	//	OutputDebugStringA(str);
	//}

	if (m_bAble_2_Jump) {
		m_xmf3_Velocity.x = xmf3_Friction.x;
		m_xmf3_Velocity.z = xmf3_Friction.z;
	}

	Prepare_Render();
}

void CPlayer::Crt_Shader_Variables(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List) {
	CObject::Crt_Shader_Variables(pd3d_Device, pd3d_Command_List);

	if (m_pCamera) {
		m_pCamera->Crt_Shader_Variables(pd3d_Device, pd3d_Command_List);
	}
}

void CPlayer::Release_Shader_Variables() {
	CObject::Release_Shader_Variables();

	if (m_pCamera) {
		m_pCamera->Release_Shader_Variables();
	}
}

void CPlayer::Udt_Shader_Variables(ID3D12GraphicsCommandList* pd3d_Command_List) {
	CObject::Udt_Shader_Variables(pd3d_Command_List);
}

CCamera* CPlayer::Chg_Camera(DWORD nNew_Camera_Mode, DWORD nCurrent_Camera_Mode) {
	CCamera* pNew_Camera = NULL;

	switch (nNew_Camera_Mode) {
	case FIRST_PERSON_CAMERA:
		pNew_Camera = new CFirst_Person_Camera(m_pCamera);
		break;
	case THIRD_PERSON_CAMERA:
		pNew_Camera = new CThird_Person_Camera(m_pCamera);
		break;
	case SPACESHIP_CAMERA:
		pNew_Camera = new CSpaceShip_Camera(m_pCamera);
		break;
	default:
		break;
	}

	if (nCurrent_Camera_Mode == SPACESHIP_CAMERA) {
		m_xmf3_Right = Vector3::Normalize(DirectX::XMFLOAT3(m_xmf3_Right.x, 0.0f, m_xmf3_Right.z));
		m_xmf3_Up = Vector3::Normalize(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_xmf3_Look = Vector3::Normalize(DirectX::XMFLOAT3(m_xmf3_Look.x, 0.0f, m_xmf3_Look.z));

		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		m_fYaw = Vector3::Angle(DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f), m_xmf3_Look);

		if (m_xmf3_Look.x < 0.0f) {
			m_fYaw = -m_fYaw;
		}
	}
	else if (nNew_Camera_Mode == SPACESHIP_CAMERA && m_pCamera) {
		m_xmf3_Right = m_pCamera->Get_Right_Vector();
		m_xmf3_Up = m_pCamera->Get_Up_Vector();
		m_xmf3_Look = m_pCamera->Get_Look_Vector();
	}

	if (pNew_Camera) {
		pNew_Camera->Set_Mode(nNew_Camera_Mode);
		pNew_Camera->Set_Player(this);
	}

	if (m_pCamera) {
		delete m_pCamera;
	}

	return pNew_Camera;
}

void CPlayer::Prepare_Render() {
	m_xmf4x4_World._11 = m_xmf3_Right.x;
	m_xmf4x4_World._12 = m_xmf3_Right.y;
	m_xmf4x4_World._13 = m_xmf3_Right.z;

	m_xmf4x4_World._21 = m_xmf3_Up.x;
	m_xmf4x4_World._22 = m_xmf3_Up.y;
	m_xmf4x4_World._23 = m_xmf3_Up.z;

	m_xmf4x4_World._31 = m_xmf3_Look.x;
	m_xmf4x4_World._32 = m_xmf3_Look.y;
	m_xmf4x4_World._33 = m_xmf3_Look.z;

	m_xmf4x4_World._41 = m_xmf3_Position.x;
	m_xmf4x4_World._42 = m_xmf3_Position.y;
	m_xmf4x4_World._43 = m_xmf3_Position.z;
}

void CPlayer::Render(ID3D12GraphicsCommandList* pd3d_Command_List, CCamera* pCamera) {
	DWORD nCamera_Mode = (pCamera) ? pCamera->Get_Mode() : 0x00;

	m_pCamera->Regenerate_View_Matrix();
	m_pCamera->Udt_Shader_Variables(pd3d_Command_List);

/*	if (nCamera_Mode == THIRD_PERSON_CAMERA)*/ {
		if (m_pShader) {
			m_pShader->Render(pd3d_Command_List, pCamera);
			CObject::Render(pd3d_Command_List, pCamera);
		}
	}
}

void CPlayer::Prcs_Collision(CObject* pObject) {
	// asix Y
	DirectX::BoundingOrientedBox d3d_OBB_Player = Get_OBB();
	DirectX::BoundingOrientedBox d3d_OBB_Object = pObject->Get_OBB();

	if (d3d_OBB_Player.Intersects(d3d_OBB_Object)) {
		DirectX::XMFLOAT3 xmf3_Object_Position = pObject->Get_Position();
		float fNew_Position_Y = 0.0f;

		if (m_xmf3_Velocity.y < 0) {
			m_xmf4x4_World._42 = m_xmf3_Position.y = xmf3_Object_Position.y + CUBE_WIDTH / 2 + PLAYER_HEIGHT / 2 + PLAYER_COLLISION_OFFSET;
		}
		else {
			m_xmf4x4_World._42 = m_xmf3_Position.y = xmf3_Object_Position.y - CUBE_WIDTH / 2 - PLAYER_HEIGHT / 2 - PLAYER_COLLISION_OFFSET;
		}

		//OutputDebugStringA("충돌함\n");
	}

	// asix X
	d3d_OBB_Player = Get_OBB();

	if (d3d_OBB_Player.Intersects(d3d_OBB_Object)) {
		DirectX::XMFLOAT3 xmf3_Object_Position = pObject->Get_Position();
		float fNew_Position_X = 0.0f;

		if (m_xmf3_Velocity.x < 0) {
			m_xmf4x4_World._41 = m_xmf3_Position.x = xmf3_Object_Position.x + CUBE_WIDTH / 2 + PLAYER_WIDTH / 2 + PLAYER_COLLISION_OFFSET;
		}
		else {
			m_xmf4x4_World._41 = m_xmf3_Position.x = xmf3_Object_Position.x - CUBE_WIDTH / 2 - PLAYER_WIDTH / 2 - PLAYER_COLLISION_OFFSET;
		}

		//OutputDebugStringA("충돌함\n");
	}
}

void CPlayer::Udt_N_Prcs_Collision(CObject** ppObject, int nObjects) {
	DirectX::XMFLOAT3 xmf3_Player_Position = Get_Position();
	DirectX::XMFLOAT3 xmf3_Cube_Position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	float fLength = 0.0f;

	std::vector<int> vCrashed_Objs;

	for (int i = 0; i < nObjects; ++i) {
		xmf3_Cube_Position = ppObject[i]->Get_Position();

		fLength = Vector3::Length(Vector3::Subtract(xmf3_Player_Position, xmf3_Cube_Position));

		if (fLength < PLAYER_COLLISION_LENGTH) {
			vCrashed_Objs.push_back(i);
		}
	}

	DirectX::BoundingOrientedBox d3d_OBB_Player;
	DirectX::BoundingOrientedBox d3d_OBB_Object;

	// axis Y
	Move(0.0f, m_xmf3_Calculated_Vel.y, 0.0f);
	Prepare_Render();

	//
	for (int& num : vCrashed_Objs) {
		d3d_OBB_Player = Get_OBB();
		d3d_OBB_Object = ppObject[num]->Get_OBB();

		if (d3d_OBB_Player.Intersects(d3d_OBB_Object)) {
			DirectX::XMFLOAT3 xmf3_Object_Position = ppObject[num]->Get_Position();
			float fNew_Position_Y = 0.0f;

			if (m_xmf3_Calculated_Vel.y < 0) {
				m_xmf4x4_World._42 = m_xmf3_Position.y = xmf3_Object_Position.y + CUBE_WIDTH / 2 + PLAYER_HEIGHT / 2 + PLAYER_COLLISION_OFFSET;

				m_bAble_2_Jump = true;
			}
			else {
				m_xmf4x4_World._42 = m_xmf3_Position.y = xmf3_Object_Position.y - CUBE_WIDTH / 2 - PLAYER_HEIGHT / 2 - PLAYER_COLLISION_OFFSET;

				m_xmf3_Velocity.y = 0.0f;
			}
		}
	}

	// 투명 벽 설치
	// **axis x**
	if (m_xmf3_Position.x < CUBE_WIDTH * CUBE_INIT_RING_NUMBER && m_xmf3_Position.x > -(CUBE_WIDTH * CUBE_INIT_RING_NUMBER)) {
		// axis x
		Move(m_xmf3_Calculated_Vel.x, 0.0f, 0.0f);
	}
	else
	{
		if (m_xmf3_Position.x > CUBE_WIDTH * CUBE_INIT_RING_NUMBER) {
			// axis x
			Move(-0.05f, 0.0f, 0.0f);
		}
		else {
			// axis x
			Move(0.05f, 0.0f, 0.0f);
		}
	}
	Prepare_Render();

	//
	for (int& num : vCrashed_Objs) {
		d3d_OBB_Player = Get_OBB();
		d3d_OBB_Object = ppObject[num]->Get_OBB();

		if (d3d_OBB_Player.Intersects(d3d_OBB_Object)) {
			DirectX::XMFLOAT3 xmf3_Object_Position = ppObject[num]->Get_Position();
			float fNew_Position_X = 0.0f;

			if (m_xmf3_Calculated_Vel.x < 0) {
				m_xmf4x4_World._41 = m_xmf3_Position.x = xmf3_Object_Position.x + CUBE_WIDTH / 2 + PLAYER_WIDTH / 2 + PLAYER_COLLISION_OFFSET;
			}
			else {
				m_xmf4x4_World._41 = m_xmf3_Position.x = xmf3_Object_Position.x - CUBE_WIDTH / 2 - PLAYER_WIDTH / 2 - PLAYER_COLLISION_OFFSET;
			}
		}
	}

	// 투명 벽 설치
	// **axis z**
	if (m_xmf3_Position.z < CUBE_WIDTH * CUBE_INIT_RING_NUMBER && m_xmf3_Position.z > -(CUBE_WIDTH * CUBE_INIT_RING_NUMBER)) {
		// axis z
		Move(0.0f, 0.0f, m_xmf3_Calculated_Vel.z);
	}
	else
	{
		if (m_xmf3_Position.z > CUBE_WIDTH * CUBE_INIT_RING_NUMBER) {
			// axis x
			Move(0.0f, 0.0f, -0.05f);
		}
		else {
			// axis x
			Move(0.0f, 0.0f, 0.05f);
		}
	}
	Prepare_Render();

	//
	for (int& num : vCrashed_Objs) {
		d3d_OBB_Player = Get_OBB();
		d3d_OBB_Object = ppObject[num]->Get_OBB();

		if (d3d_OBB_Player.Intersects(d3d_OBB_Object)) {
			DirectX::XMFLOAT3 xmf3_Object_Position = ppObject[num]->Get_Position();
			float fNew_Position_Z = 0.0f;

			if (m_xmf3_Calculated_Vel.z < 0) {
				m_xmf4x4_World._43 = m_xmf3_Position.z = xmf3_Object_Position.z + CUBE_WIDTH / 2 + PLAYER_WIDTH / 2 + PLAYER_COLLISION_OFFSET;
			}
			else {
				m_xmf4x4_World._43 = m_xmf3_Position.z = xmf3_Object_Position.z - CUBE_WIDTH / 2 - PLAYER_WIDTH / 2 - PLAYER_COLLISION_OFFSET;
			}
		}
	}

	// drop
	if (m_xmf4x4_World._42 < -100.0f) {
		Set_Position(0.0f, 50.0f, 0.0f);
	}
}


CAirplane_Player::CAirplane_Player(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List, ID3D12RootSignature* pd3d_Graphics_RootSignature) {
	CMesh* pAirplane_Mesh = new CAirPlane_Mesh(pd3d_Device, pd3d_Command_List, 20.0f, 20.0f, 4.0f, DirectX::XMFLOAT4(0.0f, 0.5f, 0.0f, 0.0f));

	Set_Mesh(pAirplane_Mesh);

	m_pCamera = Chg_Camera(SPACESHIP_CAMERA, 0.0f);

	Crt_Shader_Variables(pd3d_Device, pd3d_Command_List);

	Set_Position(DirectX::XMFLOAT3(0.0f, 0.0f, -50.0f));

	CObjects_Shader* pShader = new CObjects_Shader();
	pShader->Crt_Shader(pd3d_Device, pd3d_Graphics_RootSignature);
	Set_Shader(pShader);
}

CAirplane_Player::~CAirplane_Player() {
}

CCamera* CAirplane_Player::Chg_Camera(DWORD nNew_Camera_Mode, float fElapsed_Time) {
	DWORD nCurrent_Camera_Mode = (m_pCamera) ? m_pCamera->Get_Mode() : 0x00;

	if (nCurrent_Camera_Mode == nNew_Camera_Mode) {
		return m_pCamera;
	}

	switch (nNew_Camera_Mode) {
	case FIRST_PERSON_CAMERA:
		Set_Friction(200.0f);
		Set_Gravity(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
		Set_Max_Velocity(125.0f);
		Set_Max_Gravity(400.0f);
		m_pCamera = CPlayer::Chg_Camera(FIRST_PERSON_CAMERA, nCurrent_Camera_Mode);
		m_pCamera->Set_Lag_Time(0.0f);
		m_pCamera->Set_Offset(DirectX::XMFLOAT3(0.0f, 20.0f, 0.0f));
		m_pCamera->Gernerate_Projection_Matrix(60.0f, ASPECT_RATIO, CAMERA_NEAR_DISTANCE, CAMERA_FAR_DISTANCE);
		m_pCamera->Set_Viewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->Set_ScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case SPACESHIP_CAMERA:
		Set_Friction(125.0f);
		Set_Gravity(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
		Set_Max_Velocity(400.0f);
		Set_Max_Gravity(400.0f);
		m_pCamera = CPlayer::Chg_Camera(SPACESHIP_CAMERA, nCurrent_Camera_Mode);
		m_pCamera->Set_Lag_Time(0.0f);
		m_pCamera->Set_Offset(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pCamera->Gernerate_Projection_Matrix(60.0f, ASPECT_RATIO, CAMERA_NEAR_DISTANCE, CAMERA_FAR_DISTANCE);
		m_pCamera->Set_Viewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->Set_ScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case THIRD_PERSON_CAMERA:
		Set_Friction(250.0f);
		Set_Gravity(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
		Set_Max_Velocity(125.0f);
		Set_Max_Gravity(400.0f);
		m_pCamera = CPlayer::Chg_Camera(THIRD_PERSON_CAMERA, nCurrent_Camera_Mode);
		m_pCamera->Set_Lag_Time(0.25f);
		m_pCamera->Set_Offset(DirectX::XMFLOAT3(0.0f, 20.0f, -50.0f));
		m_pCamera->Gernerate_Projection_Matrix(60.0f, ASPECT_RATIO, CAMERA_NEAR_DISTANCE, CAMERA_FAR_DISTANCE);
		m_pCamera->Set_Viewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->Set_ScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	default:
		break;
	}

	m_pCamera->Set_Position(Vector3::Add(m_xmf3_Position, m_pCamera->Get_Offset()));

	Update(fElapsed_Time);

	return m_pCamera;
}

void CAirplane_Player::Prepare_Render() {
	CPlayer::Prepare_Render();

	DirectX::XMMATRIX xmmtx_Rotate = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(90.0f), 0.0f, 0.0f);
	m_xmf4x4_World = Matrix4x4::Multiply(xmmtx_Rotate, m_xmf4x4_World);
}

CCube_Player::CCube_Player(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List, ID3D12RootSignature* pd3d_Graphics_RootSignature) {
	CMesh* pCube_Mesh = new CCube_Mesh(pd3d_Device, pd3d_Command_List, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_WIDTH);

	Set_Mesh(pCube_Mesh);

	m_pCamera = Chg_Camera(FIRST_PERSON_CAMERA, 0.0f);

	Crt_Shader_Variables(pd3d_Device, pd3d_Command_List);

	Set_Position(DirectX::XMFLOAT3(0.0f, 50.0f, 0.0f));

	CObjects_Shader* pShader = new CObjects_Shader();
	pShader->Crt_Shader(pd3d_Device, pd3d_Graphics_RootSignature);
	Set_Shader(pShader);
}

CCube_Player::~CCube_Player() {
}

CCamera* CCube_Player::Chg_Camera(DWORD nNew_Camera_Mode, float fElapsed_Time) {
	DWORD nCurrent_Camera_Mode = (m_pCamera) ? m_pCamera->Get_Mode() : 0x00;

	if (nCurrent_Camera_Mode == nNew_Camera_Mode) {
		return m_pCamera;
	}

	switch (nNew_Camera_Mode) {
	case FIRST_PERSON_CAMERA:
		Set_Friction(PLAYER_FRICTION);
		Set_Gravity(DirectX::XMFLOAT3(0.0f, -PLAYER_GRAVITY, 0.0f));
		Set_Max_Velocity(PLAYER_MAX_VELOCITY);
		Set_Max_Gravity(PLAYER_MAX_GRAVITY);
		m_pCamera = CPlayer::Chg_Camera(FIRST_PERSON_CAMERA, nCurrent_Camera_Mode);
		m_pCamera->Set_Lag_Time(0.0f);
		m_pCamera->Set_Offset(DirectX::XMFLOAT3(0.0f, CUBE_WIDTH * 3 / 5, 0.0f));
		m_pCamera->Gernerate_Projection_Matrix(60.0f, ASPECT_RATIO, CAMERA_NEAR_DISTANCE, CAMERA_FAR_DISTANCE);
		m_pCamera->Set_Viewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->Set_ScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	case SPACESHIP_CAMERA:
		break;
	case THIRD_PERSON_CAMERA:
		Set_Friction(PLAYER_FRICTION);
		//Set_Gravity(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
		Set_Max_Velocity(PLAYER_MAX_VELOCITY);
		Set_Max_Gravity(PLAYER_MAX_GRAVITY);
		m_pCamera = CPlayer::Chg_Camera(THIRD_PERSON_CAMERA, nCurrent_Camera_Mode);
		m_pCamera->Set_Lag_Time(0.0f);
		m_pCamera->Set_Offset(DirectX::XMFLOAT3(0.0f, 40.0f, -50.0f));
		m_pCamera->Gernerate_Projection_Matrix(60.0f, ASPECT_RATIO, CAMERA_NEAR_DISTANCE, CAMERA_FAR_DISTANCE);
		m_pCamera->Set_Viewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
		m_pCamera->Set_ScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);
		break;
	default:
		break;
	}

	m_pCamera->Set_Position(Vector3::Add(m_xmf3_Position, m_pCamera->Get_Offset()));

	Update(fElapsed_Time);

	return m_pCamera;
}
