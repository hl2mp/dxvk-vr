#pragma once

#include "openvr/openvr.hpp"
#include "../d3d9/d3d9_device.h"

class ITexture;

extern IDirect3DSurface9* m_pRenderTarget;

struct SharedTextureHolder
{
	IDirect3DSurface9* m_pRenderTarget;
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
	virtual IDirect3DSurface9* GetRenderTarget() { return m_pRenderTarget; }

	bool IsMultiSampleEnabled();
	void GetRenderTargetSize(uint32_t& width, uint32_t& height);
	int	GetNextCreateTextureIs() { return m_nextRTIs; };
	void PostPresent(dxvk::D3D9DeviceEx *pD3D9);
	void StoreSharedTexture(int index, vr::VRVulkanTextureData_t* pVRVulkanTextureData);

	SharedTextureHolder m_SharedTextureHolder[6];

private:
	uint32_t m_nRenderWidth;
	uint32_t m_nRenderHeight;

	bool m_initialised;
	int m_nextRTIs;

	bool m_multiSample;
};

BaseVRSystem* GetVRSystem();
