#pragma once

#include "openvr/openvr.hpp"
#include "../d3d9/d3d9_device.h"

class ITexture;

struct SharedTextureHolder
{
	vr::VRVulkanTextureData_t m_VulkanData;
	vr::Texture_t			m_VRTexture;
	ITexture* pTexture;
};

class BaseVRSystem
{
public:
	BaseVRSystem();

	virtual void SetRenderTargetSize(uint32_t width, uint32_t height);
	virtual void NextCreateTextureIs(int index);
	virtual vr::Texture_t GetVRTexture(int index);

	virtual void SetMultiSampleEnabled(bool enabled);
	bool IsMultiSampleEnabled();

	void GetRenderTargetSize(uint32_t& width, uint32_t& height);

	int	GetNextCreateTextureIs() { return m_nextRTIs; };

	void PostPresent(dxvk::D3D9DeviceEx *pD3D9);

	void StoreSharedTexture(int index, vr::VRVulkanTextureData_t* pVRVulkanTextureData);

	SharedTextureHolder m_SharedTextureHolder[5];

private:
	uint32_t m_nRenderWidth;
	uint32_t m_nRenderHeight;

	bool m_initialised;
	int m_nextRTIs;

	bool m_multiSample;
};

BaseVRSystem* GetVRSystem();
