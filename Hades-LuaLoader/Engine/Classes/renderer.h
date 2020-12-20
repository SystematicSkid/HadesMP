#pragma once

namespace engine
{
	class Renderer
	{
	private:
	public:
		static Renderer*& Instance;
	public:
		DWORD64* pDXGIFactory;
		DWORD64* pDXGIFactory2;
		IDXGIAdapter1* pDxActiveGPU;
		ID3D11Device* pDxDevice;
		ID3D11DeviceContext* pDxContext;
		ID3D11BlendState* pDefaultBlendState;
		ID3D11DepthStencilState* pDefaultDepthState;
		ID3D11RasterizerState* pDefaultRasterizerState;
	};

	engine::Renderer*& engine::Renderer::Instance = *(engine::Renderer**)(engine::addresses::renderer::instance);

	// Swapchain is separate class

	class SwapChain
	{
	public:
		static SwapChain*& Instance;
	public:
		DWORD64** ppRenderTargets;
		IDXGISwapChain* pDxSwapChain;
		DWORD64* pDxSwapChain1; // No idea what this is for...
		unsigned __int32 mDxSyncInterval : 3;
		unsigned __int32 mFlags : 10;
		unsigned __int32 mImageCount : 3;
		unsigned __int32 mImageIndex : 3;
		unsigned __int32 mEnableVsync : 1;
		DXGI_SWAP_EFFECT mSwapEffect;
		unsigned __int64 mPadB[5];
	};

	engine::SwapChain*& engine::SwapChain::Instance = *(engine::SwapChain**)(engine::addresses::renderer::swapchain_instance);

}