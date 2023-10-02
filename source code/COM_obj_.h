#pragma once

#include <d3d11.h> 
#include <windows.h>
#include <tchar.h>
#include <sstream>
#include <vector>
#include <wrl/client.h>

using namespace  Microsoft::WRL;//Comオブジェ版スマートポインターをテンプレートで略してる

CONST LONG SCREEN_WIDTH{ 1280 };
CONST LONG SCREEN_HEIGHT{ 720 };
CONST float SHADOWMAP_DRAWRECT{ 60 };

class Graphics
{
public:
	static Graphics& Instance()
	{
		static Graphics instance;
		return instance;
	}

	ComPtr<ID3D11Device> device; //デバイスの変わり
	ComPtr<ID3D11DeviceContext> immediate_context;//レンダリング コマンド(描画命令)
	ComPtr<IDXGISwapChain> swap_chain; //レンタリングの保管場所
	ComPtr<ID3D11RenderTargetView> render_target_view;//レンダー ターゲット インターフェース
	ComPtr<ID3D11DepthStencilView> depth_stencil_view;//深度/*ステンシル*/ ビュー インターフェイス


};

class States
{
public:

	static States& Instance()
	{
		static States instance;
		return instance;
	}

	enum class SAMPLER_STATE { POINT, LINEAR, ANISOTROPIC, LINEAR_BORDER_BLACK/*UNIT.32*/, LINEAR_BORDER_WHITE/*UNIT.32*/, COMPARISON_LINEAR_BORDER_WHITE/*SHADOW*/ };

	std::vector<ComPtr<ID3D11SamplerState>> sampler_states; //サンプリング方法の指定

	enum class DEPTH_STATE { ZT_ON_ZW_ON, ZT_ON_ZW_OFF, ZT_OFF_ZW_ON, ZT_OFF_ZW_OFF };
	std::vector<ComPtr<ID3D11DepthStencilState>> depth_stencil_states;

	std::vector<ComPtr<ID3D11BlendState>> blend_states; //ブレンドステート

	std::vector<ComPtr<ID3D11RasterizerState>> rasterizer_states; //ラスタライザー

	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowmap_sampler_state;

	States(){}

	virtual ~States() = default;

	// 初期化
	void Initialize(ID3D11Device* device, ID3D11DeviceContext* immediate_context);

};

struct Shadowmap
{
	Microsoft::WRL::ComPtr<ID3D11Buffer> shadowmap_constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowmap_depth_stencil_view;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowmap_shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> shadowmap_sampler_state;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shadowmap_caster_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> shadowmap_caster_input_layout;
};

struct ComOBJ
{

	ComPtr<ID3D11Device> device; //デバイスの変わり
	ComPtr<ID3D11DeviceContext> immediate_context;//レンダリング コマンド(描画命令)

};
