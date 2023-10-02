#pragma once

#include <d3d11.h> //インクルード
#include <windows.h>
#include <tchar.h>
#include <sstream>

#include "misc.h"
#include "high_resolution_timer.h"

#ifdef USE_IMGUI
#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"
#include "../imgui/imgui_impl_dx11.h"
#include "../imgui/imgui_impl_win32.h"
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern ImWchar glyphRangesJapanese[];
#endif

#include <d3d11_1.h>
#include <wrl.h>
#include <dxgi1_6.h>

#define ENABLE_DIRECT2D
#ifdef ENABLE_DIRECT2D
#include <d2d1_1.h>
#include <dwrite.h>
#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")
#endif


#include "circle_sprite.h"
#include "sprite.h"
#include "sprite_batch.h"
#include "Geometric_primitive.h"
#include "static_mesh.h"
#include  <wrl/client.h>
#include "Skinned_Mesh.h"
#include "framebuffer.h"
#include "fullscreen_quad.h"
#include "gltf_model.h"
#include "Static_mesh_instncing.h"
#include "Shadow.h"
#include "COM_obj_.h"
using namespace  Microsoft::WRL;//Comオブジェ版スマートポインターをテンプレートで略してる

CONST BOOL FULLSCREEN{ FALSE };
CONST LPCWSTR APPLICATION_NAME{ L"X3DGP" };



class Framework
{
public:

	CONST HWND hwnd;

	//HWND hWindow[2];

	//static Framework& Instance()
	//{
	//	static Framework framework;
	//	return framework;
	//}

	ComPtr<IDXGIAdapter3> adapter;
	size_t video_memory_usage()
	{
		DXGI_QUERY_VIDEO_MEMORY_INFO video_memory_info;
		adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &video_memory_info);
		return video_memory_info.CurrentUsage / 1024 / 1024;
	}


	Framework(HWND hwnd);
	~Framework();

	Framework(const Framework&) = delete;
	Framework& operator=(const Framework&) = delete;
	Framework(Framework&&) noexcept = delete;
	Framework& operator=(Framework&&) noexcept = delete;

	//インターフェース　＝ コマンド？。
			/*COMオブジェ*/
	static  ComOBJ immediate_obj;

	ComPtr<IDXGISwapChain> swap_chain; //レンタリングの保管場所
	ComPtr<ID3D11RenderTargetView> render_target_view;//レンダー ターゲット インターフェース
	ComPtr<ID3D11DepthStencilView> depth_stencil_view;//深度/*ステンシル*/ ビュー インターフェイス
	//レンダリングとはシーン内のポリゴンとライトを変換するプロセス。カメラの元？


	//IDXGISwapChain* swap_chain2; //レンタリングの保管場所
	//ID3D11RenderTargetView* render_target_view2;//レンダー ターゲット インターフェース
	//ID3D11DepthStencilView* depth_stencil_view2;//深度/*ステンシル*/ ビュー インターフェイス

	//SpritrHLSI* sprites[8]; //スプライトの頂点

	//std::vector<ComPtr<ID3D11SamplerState>> sampler_states; //サンプリング方法の指定

	////デプスステンシル描画の方法
	//ComPtr<ID3D11DepthStencilState> depth_stencil_states[4];

	////①framework クラスに以下のメンバ変数を追加する
	//ComPtr<ID3D11BlendState> blend_states[4]; //ブレンドステート

	//ComPtr<ID3D11RasterizerState> rasterizer_states[4]; //ラスタライザー

	ComPtr<ID3D11PixelShader> pixel_shaders[8]; //ピクセルシェーダー

	//std::unique_ptr <Sprite_Batch> sprite_batches[8];
	CircleSprite* spritr[4];
	//SpritrHLSI* spritr[4];
	std::unique_ptr<Geometric_Primitive> geometric_primitive[8];

	std::unique_ptr<Static_Mesh> static_meshes[8];

	std::unique_ptr<Skinned_Mesh> Skinned_Meshes[8];

	//std::unique_ptr<Static_mesh_instncing> static_mesh_instncings[8];

	std::unique_ptr<framebuffer> framebuffers[8];

	//std::unique_ptr<gltf_model> gltf_models[8];

	//std::unique_ptr<Shadow_map> Shadow[8];

	std::unique_ptr<fullscreen_quad> bit_block_transfer;
	// SKYMAP
	ComPtr<ID3D11ShaderResourceView> skymap;

	//std::unique_ptr<Skinned_Mesh> Skinned_Meshes[8];
	//サンプラーステートとはサンプリング方法を指定するもの

	//Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[8];
	//UNIT33
	std::vector<std::unique_ptr<gltf_model>> gltf_models;

	DirectX::XMFLOAT3 Animetranslation = {};

	float animeAngle = 0;
	float animefactor = 0;


	DirectX::XMFLOAT3 RollPitchYaw = {};
	//DirectX::XMFLOAT3 Translation = {};
	//DirectX::XMFLOAT4 Collr = {1,1,1,1};
	struct scene_constants//定数バッファ構造体()主にカメラ
	{
		DirectX::XMFLOAT4X4 view_projection; //ビュー・プロジェクション変換行列
		DirectX::XMFLOAT4 light_direction; //ライトの向き
		DirectX::XMFLOAT4 camera_position;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffers[8];

	//struct shadowmap_constants
	//{
	//	DirectX::XMFLOAT4X4 light_view_projection;	// ライトの位置から見た射影行列
	//	DirectX::XMFLOAT3	shadow_color;			// 影色
	//	float				shadow_bias;			// 深度バイアス
	//};

	//DirectX::XMFLOAT4X4 light_view_projection;




	//struct Camrea
	//{
	//	DirectX::XMFLOAT4 light = { 0,-1,1,1 };
	//	DirectX::XMFLOAT3 pos = { 0,0,0};

	//}camrea;

	//struct Parametric
	//{
	//	float extraction_threshold{ 0.8f };
	//	float gaussian_sigma{ 1.0f };
	//	float bloom_intensity{ 0.0f };
	//	float exposure{ 1.0f };
	//}parametric;

	//float				shadow_bias{ 0.008f };
	//DirectX::XMFLOAT3	shadow_color{ 0.3f, 0.3f, 0.3f };

	

	Microsoft::WRL::ComPtr<ID3D11Buffer> scene_constant_buffer;
	float timer{ 0.0f };
	bool flag{ false };

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> scene_render_target_view;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> scene_shader_resource_view;

	//Shadowmap shadowmap;

	int run()
	{
		MSG msg{};

		if (!initialize()) //ここで読み込み	
		{
			return 0;
		}

#ifdef USE_IMGUI
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::GetIO().Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 14.0f, nullptr, glyphRangesJapanese);
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(immediate_obj.device.Get(), immediate_obj.immediate_context.Get());
		ImGui::StyleColorsDark();
#endif

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				tictoc.tick();
				calculate_frame_stats();
				update(tictoc.time_interval());
				render(tictoc.time_interval());
			}
		}

#ifdef USE_IMGUI
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
#endif

#if 1
		BOOL fullscreen = {};
		swap_chain->GetFullscreenState(&fullscreen, 0);
		if (fullscreen)
		{
			swap_chain->SetFullscreenState(FALSE, 0);
		}
#endif

		return uninitialize() ? static_cast<int>(msg.wParam) : 0;
	}

	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
#ifdef USE_IMGUI
		if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) { return true; }
#endif
		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps{};
			BeginPaint(hwnd, &ps);

			EndPaint(hwnd, &ps);
		}
		break;

		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE)
			{
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			}
			break;
		case WM_ENTERSIZEMOVE:
			tictoc.stop();
			break;
		case WM_EXITSIZEMOVE:
			tictoc.start();
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}

private:
	bool initialize();                                                  //初期設定
	void update(float elapsed_time/*Elapsed seconds from last frame*/); //画面の更新
	void render(float elapsed_time/*Elapsed seconds from last frame*/); //描写
	bool uninitialize();


private:
	high_resolution_timer tictoc;
	uint32_t frames_per_second{ 0 };
	float count_by_seconds{ 0.0f };
	void calculate_frame_stats()
	{
		if (++frames_per_second, (tictoc.time_stamp() - count_by_seconds) >= 1.0f)
		{
			float fps = static_cast<float>(frames_per_second);									  
			std::wostringstream outs;                                                             //文字表示
			outs.precision(6);
			outs << APPLICATION_NAME << L" : FPS : " << fps << L" / " << L"Frame Time : " << 1000.0f / fps << L" (ms)"; //FPS表示
			SetWindowTextW(hwnd, outs.str().c_str());

			frames_per_second= 0;
			count_by_seconds += 1.0f;
		}
	}


};


enum TEXNO
{
	BACK = 0,
	PLAYER,
};


