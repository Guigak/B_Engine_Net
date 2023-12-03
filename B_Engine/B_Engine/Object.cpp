#include "Object.h"
#include "Shader.h"

//
CTexture::CTexture(UINT nResource_Type, int nRootParameter_Index, int nRows, int nCols) {
	m_nTexture_Type = nResource_Type;

	m_nRootParameter_Index = nRootParameter_Index;

	m_pd3d_Texture = NULL;
	m_pd3d_Texture_Upload_Buffer = NULL;

	//
	m_nRow = 0;
	m_nCol = 0;

	m_nRows = nRows;
	m_nCols = nCols;
}

CTexture::~CTexture() {
	if (m_pd3d_Texture) {
		delete m_pd3d_Texture;
	}
}

void CTexture::Udt_Shader_Variable(ID3D12GraphicsCommandList* pd3d_Command_List) {
	if (m_pd3d_Texture) {
		pd3d_Command_List->SetDescriptorHeaps(1, &m_pd3d_Descriptor_Heap);

		pd3d_Command_List->SetGraphicsRootShaderResourceView(m_nRootParameter_Index, m_pd3d_Texture->GetGPUVirtualAddress());
	}
}

void CTexture::Release_Shader_Variable() {
}

void CTexture::Load_Texture_From_DDS_File(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List, wchar_t* pszFileName) {
	m_pd3d_Texture = Crt_Texture_Resource_From_DDS_File(pd3d_Device, pd3d_Command_List, pszFileName, &m_pd3d_Texture_Upload_Buffer, D3D12_RESOURCE_STATE_GENERIC_READ);
}

D3D12_SHADER_RESOURCE_VIEW_DESC CTexture::Get_Shader_Resource_View_Desc() {
	D3D12_RESOURCE_DESC d3d_Resource_Desc = m_pd3d_Texture->GetDesc();

	D3D12_SHADER_RESOURCE_VIEW_DESC d3d_Shader_Resource_View_Desc;
	d3d_Shader_Resource_View_Desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	switch (m_nTexture_Type)
	{
	case RESOURCE_TEXTURE2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
	case RESOURCE_TEXTURE2D_ARRAY: //[]
		d3d_Shader_Resource_View_Desc.Format = d3d_Resource_Desc.Format;
		d3d_Shader_Resource_View_Desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		d3d_Shader_Resource_View_Desc.Texture2D.MipLevels = -1;
		d3d_Shader_Resource_View_Desc.Texture2D.MostDetailedMip = 0;
		d3d_Shader_Resource_View_Desc.Texture2D.PlaneSlice = 0;
		d3d_Shader_Resource_View_Desc.Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_TEXTURE2DARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		d3d_Shader_Resource_View_Desc.Format = d3d_Resource_Desc.Format;
		d3d_Shader_Resource_View_Desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		d3d_Shader_Resource_View_Desc.Texture2DArray.MipLevels = -1;
		d3d_Shader_Resource_View_Desc.Texture2DArray.MostDetailedMip = 0;
		d3d_Shader_Resource_View_Desc.Texture2DArray.PlaneSlice = 0;
		d3d_Shader_Resource_View_Desc.Texture2DArray.ResourceMinLODClamp = 0.0f;
		d3d_Shader_Resource_View_Desc.Texture2DArray.FirstArraySlice = 0;
		d3d_Shader_Resource_View_Desc.Texture2DArray.ArraySize = d3d_Resource_Desc.DepthOrArraySize;
		break;
	case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
		d3d_Shader_Resource_View_Desc.Format = d3d_Resource_Desc.Format;
		d3d_Shader_Resource_View_Desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		d3d_Shader_Resource_View_Desc.TextureCube.MipLevels = 1;
		d3d_Shader_Resource_View_Desc.TextureCube.MostDetailedMip = 0;
		d3d_Shader_Resource_View_Desc.TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		d3d_Shader_Resource_View_Desc.Format = m_d3d_Buffer_Format;
		d3d_Shader_Resource_View_Desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		d3d_Shader_Resource_View_Desc.Buffer.FirstElement = 0;
		d3d_Shader_Resource_View_Desc.Buffer.NumElements = m_nBuffer_Elements;
		d3d_Shader_Resource_View_Desc.Buffer.StructureByteStride = 0;
		d3d_Shader_Resource_View_Desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	}
	return(d3d_Shader_Resource_View_Desc);
}

void CTexture::Release_Upload_Buffers() {
	if (m_pd3d_Texture_Upload_Buffer) {
		m_pd3d_Texture_Upload_Buffer->Release();
		m_pd3d_Texture_Upload_Buffer = NULL;
	}
}

void CTexture::Crt_Shader_Resource_View(ID3D12Device* pd3d_Device) {
	D3D12_DESCRIPTOR_HEAP_DESC d3d_Descriptor_Heap_Desc;
	d3d_Descriptor_Heap_Desc.NumDescriptors = 1;
	d3d_Descriptor_Heap_Desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	d3d_Descriptor_Heap_Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	d3d_Descriptor_Heap_Desc.NodeMask = 0;
	pd3d_Device->CreateDescriptorHeap(&d3d_Descriptor_Heap_Desc, __uuidof(ID3D12DescriptorHeap), (void**)&m_pd3d_Descriptor_Heap);

	pd3d_Device->CreateShaderResourceView(m_pd3d_Texture, &(Get_Shader_Resource_View_Desc()), m_pd3d_Descriptor_Heap->GetCPUDescriptorHandleForHeapStart());

	m_d3d_GPU_Descriptor_Handle = m_pd3d_Descriptor_Heap->GetGPUDescriptorHandleForHeapStart();
}



//
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

	// color
	pd3d_Command_List->SetGraphicsRoot32BitConstants(3, 1, &m_xmf4_Color, 0);
}

void CObject::Release_Shader_Variables() {
}

DirectX::XMFLOAT3 CObject::Get_Position() {
	return DirectX::XMFLOAT3(m_xmf4x4_World._41, m_xmf4x4_World._42, m_xmf4x4_World._43);
}

DirectX::XMFLOAT3 CObject::Get_Look() {
	return Vector3::Normalize(DirectX::XMFLOAT3(m_xmf4x4_World._31, m_xmf4x4_World._32, m_xmf4x4_World._33));
}

void CObject::Set_Look_xz(float x, float z)
{
	m_xmf4x4_World._31 = x;
	m_xmf4x4_World._33 = z;

	DirectX::XMFLOAT3 xmf3_Look = Get_Look();
	DirectX::XMFLOAT3 xmf3_Up = Get_Up();
	DirectX::XMFLOAT3 xmf3_Right = Vector3::Cross_Product(xmf3_Up, xmf3_Look, true);
	
	m_xmf4x4_World._11 = xmf3_Right.x;
	m_xmf4x4_World._12 = xmf3_Right.y;
	m_xmf4x4_World._13 = xmf3_Right.z;
}

DirectX::XMFLOAT3 CObject::Get_Up() {
	return Vector3::Normalize(DirectX::XMFLOAT3(m_xmf4x4_World._21, m_xmf4x4_World._22, m_xmf4x4_World._23));
}

DirectX::XMFLOAT3 CObject::Get_Right() {
	return Vector3::Normalize(DirectX::XMFLOAT3(m_xmf4x4_World._11, m_xmf4x4_World._12, m_xmf4x4_World._13));
}

DirectX::BoundingOrientedBox CObject::Get_OBB() {
	DirectX::BoundingOrientedBox d3d_OBB;
	DirectX::XMFLOAT4X4 xmf4x4_Transform = Matrix4x4::Identity();

	xmf4x4_Transform._41 = m_xmf4x4_World._41;
	xmf4x4_Transform._42 = m_xmf4x4_World._42;
	xmf4x4_Transform._43 = m_xmf4x4_World._43;

	if (m_pMesh) {
		m_pMesh->Get_OBB().Transform(d3d_OBB, DirectX::XMLoadFloat4x4(&xmf4x4_Transform));
	}
	else {
		d3d_OBB = DirectX::BoundingOrientedBox(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(CUBE_WIDTH * 0.5f, CUBE_WIDTH * 0.5f, CUBE_WIDTH * 0.5f), DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
		d3d_OBB.Transform(d3d_OBB, DirectX::XMLoadFloat4x4(&xmf4x4_Transform));
	}

	return d3d_OBB;
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



//
CNumber_Object::CNumber_Object() {
	DirectX::XMFLOAT4X4 m_xmf4x4_Texture_UV = Matrix4x4::Identity();
}

CNumber_Object::~CNumber_Object() {
}

void CNumber_Object::Udt_Number(int nNumber) {
	int nCol = nNumber % 5;
	int nRow = nNumber / 5;

	m_xmf4x4_Texture_UV._11 = 1.0f / float(5);
	m_xmf4x4_Texture_UV._22 = 1.0f / float(2);
	m_xmf4x4_Texture_UV._31 = float(nRow) / float(2);
	m_xmf4x4_Texture_UV._32 = float(nCol) / float(5);
}

void CNumber_Object::Render(ID3D12GraphicsCommandList* pd3d_Command_List, CCamera* pCamera) {
	//pd3d_Command_List->SetGraphicsRoot32BitConstants(4, 16, &m_xmf4x4_Texture_UV, 0);

	CObject::Render(pd3d_Command_List, pCamera);
}
