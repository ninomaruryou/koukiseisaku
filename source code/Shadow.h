#pragma once

// UNIT.99
#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>

#include <cstdint>
#include <functional>

#include "fullscreen_quad.h"
#include "framebuffer.h"

class Shadow
{
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view;

public:
	Shadow(ID3D11Device* device, uint32_t width, uint32_t height);
	virtual ~Shadow() = default;
	Shadow(const Shadow&) = delete;
	Shadow& operator =(const Shadow&) = delete;
	Shadow(Shadow&&) noexcept = delete;
	Shadow& operator =(Shadow&&) noexcept = delete;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
	D3D11_VIEWPORT viewport;

	void clear(ID3D11DeviceContext * immediate_context, float depth = 1);
	void activate(ID3D11DeviceContext * immediate_context);
	void deactivate(ID3D11DeviceContext * immediate_context);

private:
	UINT viewport_count{ D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE };
	D3D11_VIEWPORT cached_viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cached_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cached_depth_stencil_view;
};
