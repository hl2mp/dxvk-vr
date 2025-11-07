#include "vr_system.h"

BaseVRSystem* GetVRSystem()
{
	static BaseVRSystem instance;
	return &instance;
}

typedef void (*SourceVR)(BaseVRSystem* vrSystem);
SourceVR Present;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		HMODULE hDll = LoadLibrary("hl2mp.dll");
		if (!hDll) {
			OutputDebugString("Could not load DLL hl2mp.dll!\n");
			return TRUE;
		}

		// Получаем адрес функции Present() из DLL
		Present = (SourceVR)GetProcAddress(hDll, "Present");
		if (!Present) {
			FreeLibrary(hDll);
		}
	}
	return TRUE;
}

BaseVRSystem::BaseVRSystem() :
	m_initialised(false),
	m_nRenderWidth(-1),
	m_nRenderHeight(-1),
	m_multiSample(false),
	m_nextRTIs(-1),
	m_SharedTextureHolder{}
{
	if (Present)
		Present(this);
}

void BaseVRSystem::SetRenderTargetSize( uint32_t width, uint32_t height )
{
	m_initialised = true;
	m_nRenderWidth = width;
	m_nRenderHeight = height;
}

void BaseVRSystem::GetRenderTargetSize( uint32_t &width, uint32_t &height )
{
	width = m_nRenderWidth;
	height = m_nRenderHeight;
}

vr::Texture_t BaseVRSystem::GetVRTexture(int index)
{
	return m_SharedTextureHolder[index].m_VRTexture;
}

void BaseVRSystem::NextCreateTextureIs( int index )
{
	m_nextRTIs = index;
}

void BaseVRSystem::SetMultiSampleEnabled( bool enabled )
{
	m_multiSample = enabled;
}

bool BaseVRSystem::IsMultiSampleEnabled()
{
	return m_multiSample;
}


void BaseVRSystem::PostPresent(dxvk::D3D9DeviceEx* pD3D9)
{
	if (!m_initialised)
		return;

	//Wait for the work to finish
	//IDirect3DQuery9* pEventQuery = nullptr;
	//pD3D9->CreateQuery(D3DQUERYTYPE_EVENT, &pEventQuery);
	//if (pEventQuery != nullptr)
	//{
	//	pEventQuery->Issue(D3DISSUE_END);

	//	while (pEventQuery->GetData(nullptr, 0, D3DGETDATA_FLUSH) != S_OK);
	//		pEventQuery->Release();
	//}

	pD3D9->Flush();
	pD3D9->SynchronizeCsThread();
	pD3D9->GetDXVKDevice()->waitForIdle();
	
	if (Present)
		Present(this);
}

void BaseVRSystem::StoreSharedTexture(int index, vr::VRVulkanTextureData_t* pVRVulkanTextureData)
{
	if( index == -1 )
		return;

	memcpy(&m_SharedTextureHolder[index].m_VulkanData, pVRVulkanTextureData, sizeof(vr::VRVulkanTextureData_t));

	m_SharedTextureHolder[index].m_VRTexture.handle = &m_SharedTextureHolder[index].m_VulkanData;
	m_SharedTextureHolder[index].m_VRTexture.eColorSpace = vr::ColorSpace_Auto;
	m_SharedTextureHolder[index].m_VRTexture.eType = vr::TextureType_Vulkan;

	//Reset next create texture
	m_nextRTIs = -1;
}