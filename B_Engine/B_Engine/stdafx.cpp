#include "stdafx.h"
#include "DDSTextureLoader12.h"

ID3D12Resource* Crt_Buffer_Resource(ID3D12Device* pd3d_Device, ID3D12GraphicsCommandList* pd3d_Command_List, void* pData, UINT nBytes,
	D3D12_HEAP_TYPE d3d_HeapType, D3D12_RESOURCE_STATES d3d_Resource_States, ID3D12Resource** ppd3d_UploadBuffer) {
	ID3D12Resource* pd3d_Buffer = NULL;

	D3D12_HEAP_PROPERTIES d3d_Heap_Properties_Desc;
	ZeroMemory(&d3d_Heap_Properties_Desc, sizeof(D3D12_HEAP_PROPERTIES));
	d3d_Heap_Properties_Desc.Type = d3d_HeapType;
	d3d_Heap_Properties_Desc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3d_Heap_Properties_Desc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3d_Heap_Properties_Desc.CreationNodeMask = 1;
	d3d_Heap_Properties_Desc.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC d3d_Resource_Desc;
	ZeroMemory(&d3d_Resource_Desc, sizeof(D3D12_RESOURCE_DESC));
	d3d_Resource_Desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	d3d_Resource_Desc.Alignment = 0;
	d3d_Resource_Desc.Width = nBytes;
	d3d_Resource_Desc.Height = 1;
	d3d_Resource_Desc.DepthOrArraySize = 1;
	d3d_Resource_Desc.MipLevels = 1;
	d3d_Resource_Desc.Format = DXGI_FORMAT_UNKNOWN;
	d3d_Resource_Desc.SampleDesc.Count = 1;
	d3d_Resource_Desc.SampleDesc.Quality = 0;
	d3d_Resource_Desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d3d_Resource_Desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12_RESOURCE_STATES d3d_Resource_Initial_States = D3D12_RESOURCE_STATE_COMMON;
	if (d3d_HeapType == D3D12_HEAP_TYPE_UPLOAD) {
		d3d_Resource_Initial_States = D3D12_RESOURCE_STATE_GENERIC_READ;
	}
	else if (d3d_HeapType == D3D12_HEAP_TYPE_READBACK) {
		d3d_Resource_Initial_States = D3D12_RESOURCE_STATE_COPY_DEST;
	}

	HRESULT hResult = pd3d_Device->CreateCommittedResource(&d3d_Heap_Properties_Desc, D3D12_HEAP_FLAG_NONE, &d3d_Resource_Desc, d3d_Resource_Initial_States, NULL, __uuidof(ID3D12Resource), (void**)&pd3d_Buffer);

	if (pData) {
		switch (d3d_HeapType) {
		case D3D12_HEAP_TYPE_DEFAULT :
		{
			d3d_Heap_Properties_Desc.Type = D3D12_HEAP_TYPE_UPLOAD;
			pd3d_Device->CreateCommittedResource(&d3d_Heap_Properties_Desc, D3D12_HEAP_FLAG_NONE, &d3d_Resource_Desc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)ppd3d_UploadBuffer);

			D3D12_RANGE d3d_Read_Range = { 0, 0 };
			UINT8* pBuffer_Data_Begin = NULL;
			(*ppd3d_UploadBuffer)->Map(0, &d3d_Read_Range, (void**)&pBuffer_Data_Begin);
			memcpy(pBuffer_Data_Begin, pData, nBytes);
			(*ppd3d_UploadBuffer)->Unmap(0, NULL);

			pd3d_Command_List->CopyResource(pd3d_Buffer, *ppd3d_UploadBuffer);

			D3D12_RESOURCE_BARRIER d3d_Resource_Barrier;
			ZeroMemory(&d3d_Resource_Barrier, sizeof(D3D12_RESOURCE_BARRIER));
			d3d_Resource_Barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			d3d_Resource_Barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			d3d_Resource_Barrier.Transition.pResource = pd3d_Buffer;
			d3d_Resource_Barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
			d3d_Resource_Barrier.Transition.StateAfter = d3d_Resource_States;
			d3d_Resource_Barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			pd3d_Command_List->ResourceBarrier(1, &d3d_Resource_Barrier);
		}
			break;
		case D3D12_HEAP_TYPE_UPLOAD :
		{
			D3D12_RANGE d3d_Read_Range = { 0, 0 };
			UINT8* pBuffer_Data_Begin = NULL;
			pd3d_Buffer->Map(0, &d3d_Read_Range, (void**)&pBuffer_Data_Begin);
			memcpy(pBuffer_Data_Begin, pData, nBytes);
			pd3d_Buffer->Unmap(0, NULL);
		}
			break;
		case D3D12_HEAP_TYPE_READBACK :
			break;
		default :
			break;
		}
	}

	return pd3d_Buffer;
}

ID3D12Resource* Crt_Texture_Resource_From_DDS_File(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, wchar_t* pszFileName, ID3D12Resource** ppd3dUploadBuffer, D3D12_RESOURCE_STATES d3dResourceStates) {
	ID3D12Resource* pd3dTexture = NULL;
	std::unique_ptr<uint8_t[]> ddsData;
	std::vector<D3D12_SUBRESOURCE_DATA> vSubresources;
	DirectX::DDS_ALPHA_MODE ddsAlphaMode = DirectX::DDS_ALPHA_MODE::DDS_ALPHA_MODE_UNKNOWN;
	bool bIsCubeMap = false;

	HRESULT hResult = DirectX::LoadDDSTextureFromFileEx(pd3dDevice, pszFileName, 0, D3D12_RESOURCE_FLAG_NONE, DirectX::DDS_LOADER_FLAGS::DDS_LOADER_DEFAULT, &pd3dTexture, ddsData, vSubresources, &ddsAlphaMode, &bIsCubeMap);

	D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc;
	::ZeroMemory(&d3dHeapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
	d3dHeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapPropertiesDesc.CreationNodeMask = 1;
	d3dHeapPropertiesDesc.VisibleNodeMask = 1;

	//	D3D12_RESOURCE_DESC d3dResourceDesc = pd3dTexture->GetDesc();
	//	UINT nSubResources = d3dResourceDesc.DepthOrArraySize * d3dResourceDesc.MipLevels;
	UINT nSubResources = (UINT)vSubresources.size();
	//	UINT64 nBytes = 0;
	//	pd3dDevice->GetCopyableFootprints(&d3dResourceDesc, 0, nSubResources, 0, NULL, NULL, NULL, &nBytes);
	UINT64 nBytes = GetRequiredIntermediateSize(pd3dTexture, 0, nSubResources);

	D3D12_RESOURCE_DESC d3dResourceDesc;
	::ZeroMemory(&d3dResourceDesc, sizeof(D3D12_RESOURCE_DESC));
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER; //Upload Heap에는 텍스쳐를 생성할 수 없음
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = nBytes;
	d3dResourceDesc.Height = 1;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	d3dResourceDesc.SampleDesc.Count = 1;
	d3dResourceDesc.SampleDesc.Quality = 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, &d3dResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL, __uuidof(ID3D12Resource), (void**)ppd3dUploadBuffer);

	//UINT nSubResources = (UINT)vSubresources.size();
	//D3D12_SUBRESOURCE_DATA *pd3dSubResourceData = new D3D12_SUBRESOURCE_DATA[nSubResources];
	//for (UINT i = 0; i < nSubResources; i++) pd3dSubResourceData[i] = vSubresources.at(i);

	//	std::vector<D3D12_SUBRESOURCE_DATA>::pointer ptr = &vSubresources[0];
	::UpdateSubresources(pd3dCommandList, pd3dTexture, *ppd3dUploadBuffer, 0, 0, nSubResources, &vSubresources[0]);

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = pd3dTexture;
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	d3dResourceBarrier.Transition.StateAfter = d3dResourceStates;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	//	delete[] pd3dSubResourceData;

	return(pd3dTexture);
}
