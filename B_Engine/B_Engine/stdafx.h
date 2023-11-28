// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable : 6031)

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string.h>

// io
#include <iostream>

//
#include <vector>

// Timer 관련
#include <Mmsystem.h>

#pragma comment(lib, "winmm.lib")

// Direct3D 관련 headers
#include <string>
#include <wrl.h>
#include <shellapi.h>

#include <d3d12.h>
#include <dxgi1_4.h>

#include <D3Dcompiler.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#include <DXGIDebug.h>

//using namespace DirectX;
//using namespace DirectX::PackedVector;

using Microsoft::WRL::ComPtr;

// import library 추가
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

#pragma comment(lib, "dxguid.lib")

// client rect size
#define FRAME_BUFFER_WIDTH	800
#define FRAME_BUFFER_HEIGHT	600

// random color
#define RANDOM_COLOR DirectX::XMFLOAT4(rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX), rand() / float(RAND_MAX))

// Cube data
#define CUBE_WIDTH 10.0f
#define CUBE_INIT_RING_NUMBER 10
#define CUBE_MAX_NUMBER 10000
#define CUBE_DEFAULT_COLOR 0.75f

#define BOUNDING_BOX_OFFSET 0.03f

// player data
#define PLAYER_HEIGHT CUBE_WIDTH * 8 / 5
#define PLAYER_WIDTH CUBE_WIDTH * 2 / 3

#define PLAYER_MAX_NUMBER 3
#define PLAYER_DEFAULT_COLOR DirectX::XMFLOAT4(0.0f, 0.5f, 0.5f, 0.0f)

#define PLAYER_MAX_VELOCITY 75.0f
#define PLAYER_MAX_GRAVITY 75.0f
#define PLAYER_GRAVITY 100.0f
#define PLAYER_FRICTION 500.0f
#define PLAYER_MOVE_DISTANCE 200.0f
#define PLAYER_PICKING_DISTANCE 100.0f

#define PLAYER_COLLISION_LENGTH 20.0f
#define PLAYER_COLLISION_OFFSET 0.001f

// camera data
#define CAMERA_NEAR_DISTANCE 1.0f
#define CAMERA_FAR_DISTANCE 5000.0f

// VK data
#define VK_W 0x57
#define VK_A 0x41
#define VK_S 0x53
#define VK_D 0x44

// IP data
#define IP_ADDRESS "25.55.51.203"

// 임시로 여기에다 두었다.
struct Cube_Info {
	float fPosition_x, fPosition_y, fPosition_z;
	float fColor_r, fColor_g, fColor_b;
	bool AddorDelete;
};


// fullscreen default
//#define _WITH_SWAPCHAIN_FULLSCREEN_STATE

// Create buffer resource
extern ID3D12Resource* Crt_Buffer_Resource(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List, void* pData, UINT nBytes, D3D12_HEAP_TYPE d3d_HeapType = D3D12_HEAP_TYPE_UPLOAD,
	D3D12_RESOURCE_STATES d3d_Resource_States = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, ID3D12Resource** ppd3d_UploadBuffer = NULL);

// about calculate
namespace Vector3 {

	inline DirectX::XMFLOAT3 XMVector_2_Float3(DirectX::XMVECTOR& xmv_Vector) {	// vector -> float3
		DirectX::XMFLOAT3 xmf3_Result;
		DirectX::XMStoreFloat3(&xmf3_Result, xmv_Vector);

		return xmf3_Result;
	}

	inline DirectX::XMFLOAT3 Multiply(DirectX::XMFLOAT3& xmf3_Vector, float fScalar, bool bNormalize = true) {	// float3 * float
		DirectX::XMFLOAT3 xmf3_Result;

		if (bNormalize) {
			DirectX::XMStoreFloat3(&xmf3_Result, DirectX::XMVectorScale(DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&xmf3_Vector)), fScalar));
		}
		else {
			DirectX::XMStoreFloat3(&xmf3_Result, DirectX::XMVectorScale(DirectX::XMLoadFloat3(&xmf3_Vector), fScalar));
		}

		return xmf3_Result;
	}

	inline DirectX::XMFLOAT3 Add(const DirectX::XMFLOAT3& xmf3_Vector1, const DirectX::XMFLOAT3& xmf3_Vector2) {	// float3 + float3
		DirectX::XMFLOAT3 xmf3_Result;
		DirectX::XMStoreFloat3(&xmf3_Result, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&xmf3_Vector1), DirectX::XMLoadFloat3(&xmf3_Vector2)));

		return xmf3_Result;
	}

	inline DirectX::XMFLOAT3 Add(DirectX::XMFLOAT3& xmf3_Vector1, DirectX::XMFLOAT3& xmf3_Vector2, float fScalar) {	// float3 + float3 * float
		DirectX::XMFLOAT3 xmf3_Result;
		DirectX::XMStoreFloat3(&xmf3_Result, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&xmf3_Vector1), DirectX::XMVectorScale(DirectX::XMLoadFloat3(&xmf3_Vector2), fScalar)));

		return xmf3_Result;
	}

	inline DirectX::XMFLOAT3 Subtract(DirectX::XMFLOAT3& xmf3_Vector1, DirectX::XMFLOAT3& xmf3_Vector2) {	// float3 - float3
		DirectX::XMFLOAT3 xmf3_Result;
		DirectX::XMStoreFloat3(&xmf3_Result, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&xmf3_Vector1), DirectX::XMLoadFloat3(&xmf3_Vector2)));

		return xmf3_Result;
	}

	inline float Dot_Product(DirectX::XMFLOAT3& xmf3_Vector1, DirectX::XMFLOAT3& xmf3_Vector2) {	// float3 dot float3
		DirectX::XMFLOAT3 xmf3_Result;
		DirectX::XMStoreFloat3(&xmf3_Result, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&xmf3_Vector1), DirectX::XMLoadFloat3(&xmf3_Vector2)));

		return xmf3_Result.x;
	}

	inline DirectX::XMFLOAT3 Cross_Product(DirectX::XMFLOAT3& xmf3_Vector1, DirectX::XMFLOAT3& xmf3_Vector2, bool bNormalize = true) {	// float3 cross float3
		DirectX::XMFLOAT3 xmf3_Result;

		if (bNormalize) {
			DirectX::XMStoreFloat3(&xmf3_Result, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&xmf3_Vector1), DirectX::XMLoadFloat3(&xmf3_Vector2))));
		}
		else {
			DirectX::XMStoreFloat3(&xmf3_Result, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&xmf3_Vector1), DirectX::XMLoadFloat3(&xmf3_Vector2)));
		}

		return xmf3_Result;
	}

	inline DirectX::XMFLOAT3 Normalize(DirectX::XMFLOAT3& xmf3_Vector) {	// normalize float3
		DirectX::XMFLOAT3 xmf3_Result;
		DirectX::XMStoreFloat3(&xmf3_Result, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&xmf3_Vector)));

		return xmf3_Result;
	}

	inline float Length(DirectX::XMFLOAT3& xmf3_Vector) {	// length of float3
		DirectX::XMFLOAT3 xmf3_Result;
		DirectX::XMStoreFloat3(&xmf3_Result, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&xmf3_Vector)));

		return xmf3_Result.x;
	}

	inline float Angle(DirectX::XMVECTOR& xmv_Vector1, DirectX::XMVECTOR& xmv_Vector2) {	// angle btw vector & vector
		DirectX::XMVECTOR xmvAngle = DirectX::XMVector3AngleBetweenNormals(xmv_Vector1, xmv_Vector2);
		return DirectX::XMConvertToDegrees(acosf(DirectX::XMVectorGetX(xmvAngle)));
	}

	inline float Angle(DirectX::XMFLOAT3& xmf3_Vector1, DirectX::XMFLOAT3& xmf3_Vector2) {	// angle btw float3 & float3
		return Angle(DirectX::XMLoadFloat3(&xmf3_Vector1), DirectX::XMLoadFloat3(&xmf3_Vector2));
	}

	inline DirectX::XMFLOAT3 Transform_Normal(DirectX::XMFLOAT3& xmf3_Vector, DirectX::XMMATRIX& xmmtx_Transform) {
		DirectX::XMFLOAT3 xmf3_Result;
		DirectX::XMStoreFloat3(&xmf3_Result, DirectX::XMVector3TransformNormal(DirectX::XMLoadFloat3(&xmf3_Vector), xmmtx_Transform));

		return xmf3_Result;
	}

	inline DirectX::XMFLOAT3 Transform_Coord(DirectX::XMFLOAT3& xmf3_Vector, DirectX::XMMATRIX& xmmtx_Transform) {
		DirectX::XMFLOAT3 xmf3_Result;
		DirectX::XMStoreFloat3(&xmf3_Result, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&xmf3_Vector), xmmtx_Transform));

		return xmf3_Result;
	}

	inline DirectX::XMFLOAT3 Transform_Coord(DirectX::XMFLOAT3& xmf3_Vector, DirectX::XMFLOAT4X4& xmmtx4x4_Matrix) {
		return Transform_Coord(xmf3_Vector, DirectX::XMLoadFloat4x4(&xmmtx4x4_Matrix));
	}
}

namespace Vector4 {
	inline DirectX::XMFLOAT4 Add(DirectX::XMFLOAT4& xmf4_Vector1, DirectX::XMFLOAT4& xmf4_Vector2) {	// float4 + float4
		DirectX::XMFLOAT4 xmf4_Result;
		DirectX::XMStoreFloat4(&xmf4_Result, DirectX::XMVectorAdd(DirectX::XMLoadFloat4(&xmf4_Vector1), DirectX::XMLoadFloat4(&xmf4_Vector2)));

		return xmf4_Result;
	}

	inline DirectX::XMFLOAT4 Multiply(DirectX::XMFLOAT4& xmf4_Vector1, DirectX::XMFLOAT4& xmf4_Vector2) {	// float4 * float4
		DirectX::XMFLOAT4 xmf4_Result;
		DirectX::XMStoreFloat4(&xmf4_Result, DirectX::XMVectorMultiply(DirectX::XMLoadFloat4(&xmf4_Vector1), DirectX::XMLoadFloat4(&xmf4_Vector2)));

		return xmf4_Result;
	}

	inline DirectX::XMFLOAT4 Multiply(DirectX::XMFLOAT4& xmf4_Vector, float fScalar) {	// float4 * float
		DirectX::XMFLOAT4 xmf4_Result;
		DirectX::XMStoreFloat4(&xmf4_Result, DirectX::XMVectorScale(DirectX::XMLoadFloat4(&xmf4_Vector), fScalar));
		
		return xmf4_Result;
	}
}

namespace Matrix4x4 {
	inline DirectX::XMFLOAT4X4 Identity() {
		DirectX::XMFLOAT4X4 xmf4x4Result;
		DirectX::XMStoreFloat4x4(&xmf4x4Result, DirectX::XMMatrixIdentity());

		return xmf4x4Result;
	}

	inline DirectX::XMFLOAT4X4 Multiply(DirectX::XMFLOAT4X4& xmmtx4x4_Matrix1, DirectX::XMFLOAT4X4& xmmtx4x4_Matrix2) {
		DirectX::XMFLOAT4X4 xmf4x4Result;
		DirectX::XMStoreFloat4x4(&xmf4x4Result, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&xmmtx4x4_Matrix1), DirectX::XMLoadFloat4x4(&xmmtx4x4_Matrix2)));

		return xmf4x4Result;
	}

	inline DirectX::XMFLOAT4X4 Multiply(DirectX::XMFLOAT4X4& xmmtx4x4_Matrix1, DirectX::XMMATRIX& xmmtx_Matrix2) {
		DirectX::XMFLOAT4X4 xmf4x4Result;
		DirectX::XMStoreFloat4x4(&xmf4x4Result, DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&xmmtx4x4_Matrix1), xmmtx_Matrix2));

		return xmf4x4Result;
	}

	inline DirectX::XMFLOAT4X4 Multiply(DirectX::XMMATRIX& xmmtx_Matrix1, DirectX::XMFLOAT4X4& xmmtx4x4_Matrix2) {
		DirectX::XMFLOAT4X4 xmf4x4Result;
		DirectX::XMStoreFloat4x4(&xmf4x4Result, DirectX::XMMatrixMultiply(xmmtx_Matrix1, DirectX::XMLoadFloat4x4(&xmmtx4x4_Matrix2)));

		return xmf4x4Result;
	}

	inline DirectX::XMFLOAT4X4 Inverse(DirectX::XMFLOAT4X4& xmmtx4x4_Matrix) {	// Inverse
		DirectX::XMFLOAT4X4 xmf4x4Result;
		DirectX::XMStoreFloat4x4(&xmf4x4Result, DirectX::XMMatrixInverse(NULL, DirectX::XMLoadFloat4x4(&xmmtx4x4_Matrix)));

		return xmf4x4Result;
	}

	inline DirectX::XMFLOAT4X4 Transpose(DirectX::XMFLOAT4X4& xmmtx4x4_Matrix) {	// Transpose
		DirectX::XMFLOAT4X4 xmf4x4Result;
		DirectX::XMStoreFloat4x4(&xmf4x4Result, DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(&xmmtx4x4_Matrix)));

		return xmf4x4Result;
	}

	inline DirectX::XMFLOAT4X4 PerspectiveFov_LH(float FovAngle_Y, float Aspect_Ratio, float Near_Z, float Far_Z) {	// Projection Matrix
		DirectX::XMFLOAT4X4 xmf4x4Result;
		DirectX::XMStoreFloat4x4(&xmf4x4Result, DirectX::XMMatrixPerspectiveFovLH(FovAngle_Y, Aspect_Ratio, Near_Z, Far_Z));

		return xmf4x4Result;
	}

	inline DirectX::XMFLOAT4X4 LookAt_LH(DirectX::XMFLOAT3& xmf3_Eye_Position, DirectX::XMFLOAT3& xmf3_LookAt_Position, DirectX::XMFLOAT3& xmf3_Up_Direction) {	// View Matrix
		DirectX::XMFLOAT4X4 xmf4x4Result;
		DirectX::XMStoreFloat4x4(&xmf4x4Result, DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&xmf3_Eye_Position), DirectX::XMLoadFloat3(&xmf3_LookAt_Position), DirectX::XMLoadFloat3(&xmf3_Up_Direction)));

		return xmf4x4Result;
	}
}
