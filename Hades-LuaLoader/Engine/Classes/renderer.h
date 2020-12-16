#pragma once

namespace engine
{
	class Renderer
	{
	public:
		static Renderer*& Instance;
	private:
		char pad[0x18];
	public:
		ID3D11Device* device;
		ID3D11DeviceContext* context;
	};

	engine::Renderer*& engine::Renderer::Instance = *(engine::Renderer**)(engine::addresses::renderer::instance);
}