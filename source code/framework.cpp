#include "framework.h"

#include <iostream>

#include "SceneManager.h"
#include "vertex.h"
#include "texture.h"
#include "VertexTypes.h"
#include "SceneGame.h"


Framework::Framework(HWND hwnd) : hwnd(hwnd)
{

}

void acquire_high_performance_adapter(IDXGIFactory6* dxgi_factory6, IDXGIAdapter3** dxgi_adapter3)
{
	HRESULT hr{ S_OK };

	Microsoft::WRL::ComPtr<IDXGIAdapter3> enumerated_adapter;
	for (UINT adapter_index = 0; DXGI_ERROR_NOT_FOUND != dxgi_factory6->EnumAdapterByGpuPreference(adapter_index, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(enumerated_adapter.ReleaseAndGetAddressOf())); ++adapter_index)
	{
		DXGI_ADAPTER_DESC1 adapter_desc;
		hr = enumerated_adapter->GetDesc1(&adapter_desc);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		if (adapter_desc.VendorId == 0x1002/*AMD*/ || adapter_desc.VendorId == 0x10DE/*NVIDIA*/)
		{
			OutputDebugStringW((std::wstring(adapter_desc.Description) + L" has been selected.\n").c_str());
			OutputDebugStringA(std::string("\tVendorId:" + std::to_string(adapter_desc.VendorId) + '\n').c_str());
			OutputDebugStringA(std::string("\tDeviceId:" + std::to_string(adapter_desc.DeviceId) + '\n').c_str());
			OutputDebugStringA(std::string("\tSubSysId:" + std::to_string(adapter_desc.SubSysId) + '\n').c_str());
			OutputDebugStringA(std::string("\tRevision:" + std::to_string(adapter_desc.Revision) + '\n').c_str());
			OutputDebugStringA(std::string("\tDedicatedVideoMemory:" + std::to_string(adapter_desc.DedicatedVideoMemory) + '\n').c_str());
			OutputDebugStringA(std::string("\tDedicatedSystemMemory:" + std::to_string(adapter_desc.DedicatedSystemMemory) + '\n').c_str());
			OutputDebugStringA(std::string("\tSharedSystemMemory:" + std::to_string(adapter_desc.SharedSystemMemory) + '\n').c_str());
			OutputDebugStringA(std::string("\tAdapterLuid.HighPart:" + std::to_string(adapter_desc.AdapterLuid.HighPart) + '\n').c_str());
			OutputDebugStringA(std::string("\tAdapterLuid.LowPart:" + std::to_string(adapter_desc.AdapterLuid.LowPart) + '\n').c_str());
			OutputDebugStringA(std::string("\tFlags:" + std::to_string(adapter_desc.Flags) + '\n').c_str());
			break;
		}
	}
	*dxgi_adapter3 = enumerated_adapter.Detach();
}

//バインドとは連結のようなもの
bool Framework::initialize()
{
	 HRESULT hr{ S_OK }; //エラー内容？ 現在は成功

	 UINT create_factory_flags{};
#ifdef _DEBUG
	 create_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
	 //Microsoft::WRL::ComPtr<IDXGIFactory6> dxgi_factory6;
	 //hr = CreateDXGIFactory2(create_factory_flags, IID_PPV_ARGS(dxgi_factory6.GetAddressOf()));
	 //_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	 //acquire_high_performance_adapter(dxgi_factory6.Get(), adapter.GetAddressOf());
	 UINT create_device_flags{ 0 };
#ifdef _DEBUG
	 create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
#ifdef ENABLE_DIRECT2D
	 create_device_flags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#endif


	 //UINT create_device_flags{ 0 };
	 //#ifdef _DEBUG
		// create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
	 //#endif

	 {
		 D3D_FEATURE_LEVEL feature_levels{ D3D_FEATURE_LEVEL_11_0 };
		 //画面描写？スワップチェーンという名前
		 DXGI_SWAP_CHAIN_DESC swap_chain_desc{}; //初期化

		 swap_chain_desc.BufferCount = 1; //スワップ チェーン内のバッファーの数を示す値
		 swap_chain_desc.BufferDesc.Width = SCREEN_WIDTH;    //ウィンドウ範囲幅
		 swap_chain_desc.BufferDesc.Height = SCREEN_HEIGHT; //ウィンドウ範囲高さ
		 swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //フォーマット番号？
		 swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60; //リフレッシュレート 有理数の先頭を表す符号なし整数値()
		 swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1; //リフレッシュレート 有理数の底を表す符号なし整数値（）
		 swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; //使用領域バッファー？
		 swap_chain_desc.OutputWindow = hwnd; //ウィンドウハンドル
		 swap_chain_desc.SampleDesc.Count = 1; //ピクセルあたりのマルチサンプル数
		 swap_chain_desc.SampleDesc.Quality = 0; //画質レベル(数値が大きいほど画質が良い)
		 swap_chain_desc.Windowed = !FULLSCREEN;        //設定フルスクリーン
		 hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, create_device_flags,
			 &feature_levels, 1, D3D11_SDK_VERSION, &swap_chain_desc,
			 swap_chain.ReleaseAndGetAddressOf(), immediate_obj.device.ReleaseAndGetAddressOf(), NULL, immediate_obj.immediate_context.ReleaseAndGetAddressOf()); //スワップチェーンの作成
		 _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));//(SUCCEEDED)COMエラー処理 //(hr_trace)トレース ログ //falseの場合エラー

	 }
		 //IDXGIDevice1* pDXGI = NULL;
		 //IDXGIAdapter* pAdapter = NULL;
		 //IDXGIFactory* pFactory = NULL;

		 //D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE,
			// NULL, 0, NULL, 0, D3D11_SDK_VERSION,
			// &p3Ddevice, NULL, &immediate_context);
		 //p3Ddevice->QueryInterface(__uuidof(IDXGIDevice1), (void**)&pDXGI);
		 //pDXGI->GetAdapter(&pAdapter);
		 //pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

		 //swap_chain_desc.OutputWindow = hWindow[0];
		 //pFactory->CreateSwapChain(p3Ddevice, &swap_chain_desc, &swap_chain);


	 {
		 //レンダーターゲットビューの作成
		 ID3D11Texture2D* back_buffer{};
		 hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&back_buffer)); //スワップ チェーンのバック バッファーの 1 つにアクセス
		 _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr)); //falseの場合エラー

		 hr = immediate_obj.device->CreateRenderTargetView(back_buffer, NULL, render_target_view.ReleaseAndGetAddressOf()); //リソース データにアクセスするためのレンダー　ターゲット ビューの作成
		 _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));  //falseの場合エラー

		 back_buffer->Release();//スマートポインターと同じ原理で他が見ている
	 }
	//深度ステンシルビューの作成

	 ID3D11Texture2D * depth_stencil_buffer{}; //2D テクスチャのインターフェース（）
	 D3D11_TEXTURE2D_DESC texture2d_desc{}; //2D テクスチャメイン部分？
	 texture2d_desc.Width = SCREEN_WIDTH; //テクスチャ範囲幅
	 texture2d_desc.Height = SCREEN_HEIGHT;//テクスチャ範囲高さ
	 texture2d_desc.MipLevels = 1; //ミップマップレベル最大数
	 texture2d_desc.ArraySize = 1; //　テクスチャ配列の数
	 texture2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //リソース データ形式
	//DXGI_FORMAT_D24_UNORM_S8_UINT(深度に 24 ビット、ステンシルに 8 ビットをサポートする 32 ビットの z バッファー形式)
	 texture2d_desc.SampleDesc.Count = 1; //ピクセルあたりのマルチサンプルの数
	 texture2d_desc.SampleDesc.Quality = 0; //画質レベル
	 texture2d_desc.Usage = D3D11_USAGE_DEFAULT; //テクスチャの読み取りと書き込み方法を識別する値
	 //D3D11_USAGE_DEFAULT(GPU含めて使用許可)
	 texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL; //パイプライン 種類指定の値
	//パイプラインとは一連の流れ

	 //D3D11_BIND_DEPTH_STENCIL テクスチャをOM(Out Merger)ステージの深度ステンシル ターゲットとしてバインドします
	 texture2d_desc.CPUAccessFlags = 0; //CPU アクセスの種類を指定するため値 
	 //(なし)
	 texture2d_desc.MiscFlags = 0; //他のあまり一般的でないリソース オプションを識別する
	 //(なし)
	 hr = immediate_obj.device->CreateTexture2D(&texture2d_desc, NULL, &depth_stencil_buffer); //テクスチャの配列を作成します
	 _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr)); //falseの場合エラー

	 {
		 //深度ステンシル ビューからアクセスできるテクスチャのサブリソースを指定？一部分のアクセス？
		 D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
		 depth_stencil_view_desc.Format = texture2d_desc.Format; //フォーマットリソース データ形式
		 depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; //２Dテクスチャ
		 depth_stencil_view_desc.Texture2D.MipSlice = 0; //ミップマップ(画素数の枠組みの操作値？)
		 hr = immediate_obj.device->CreateDepthStencilView(depth_stencil_buffer, &depth_stencil_view_desc, depth_stencil_view.ReleaseAndGetAddressOf());//リソース データにアクセスするための深度ステンシル ビュー(深度ステンシルバッファを深度ステンシル ビューに変換)
		 _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr)); //falseの場合エラー


		 depth_stencil_buffer->Release(); //バッファーが消えてもスワップチェーンとrefcantにみられるためビューは消えない(スマートポインターと同じ原理)
	 }

	  {
		  //ビューポートの設定

		  D3D11_VIEWPORT viewport{};
		  viewport.TopLeftX = 0; //左側のX座標
		  viewport.TopLeftY = 0; //左側のY座標
		  viewport.Width = static_cast<float>(SCREEN_WIDTH); //幅
		  viewport.Height = static_cast<float>(SCREEN_HEIGHT);//高さ
		  viewport.MinDepth = 0.0f; //最小深度
		  viewport.MaxDepth = 1.0f; //最大深度
		  immediate_obj.immediate_context->RSSetViewports(1, &viewport);//作成したビューポートを設定（描画場所の指定）
	  }

	States::Instance().Initialize(immediate_obj.device.Get(),immediate_obj.immediate_context.Get());

	  //sprite_batches[BACK] = std::make_unique<Sprite_Batch>(device.Get(), L".\\resources\\screenshot.jpg", 1);


#if 1
	  // ライトから見たシーンの深度描画用のバッファ生成
	  //{
		 // Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_buffer{};
		 // D3D11_TEXTURE2D_DESC texture2d_desc{};
		 // texture2d_desc.Width = SCREEN_WIDTH;
		 // texture2d_desc.Height = SCREEN_HEIGHT;
		 // texture2d_desc.MipLevels = 1;
		 // texture2d_desc.ArraySize = 1;
		 // texture2d_desc.Format = DXGI_FORMAT_R32_TYPELESS;
		 // texture2d_desc.SampleDesc.Count = 1;
		 // texture2d_desc.SampleDesc.Quality = 0;
		 // texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		 // texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		 // texture2d_desc.CPUAccessFlags = 0;
		 // texture2d_desc.MiscFlags = 0;
		 // hr = device->CreateTexture2D(&texture2d_desc, NULL, depth_buffer.GetAddressOf());
		 // _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		 // //	深度ステンシルビュー生成
		 // D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
		 // depth_stencil_view_desc.Format = DXGI_FORMAT_D32_FLOAT;
		 // depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		 // depth_stencil_view_desc.Texture2D.MipSlice = 0;
		 // hr = device->CreateDepthStencilView(depth_buffer.Get(),
			//  &depth_stencil_view_desc,
			//  shadowmap.shadowmap_depth_stencil_view.GetAddressOf());
		 // _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		 // //	シェーダーリソースビュー生成
		 // D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
		 // shader_resource_view_desc.Format = DXGI_FORMAT_R32_FLOAT;
		 // shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		 // shader_resource_view_desc.Texture2D.MostDetailedMip = 0;
		 // shader_resource_view_desc.Texture2D.MipLevels = 1;
		 // hr = device->CreateShaderResourceView(depth_buffer.Get(),
			//  &shader_resource_view_desc,
			//  shadowmap.shadowmap_shader_resource_view.GetAddressOf());
		 // _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	  //}


	  //{
		 // buffer_desc.ByteWidth = sizeof(shadowmap_constants);
		 // hr = device->CreateBuffer(&buffer_desc, nullptr, shadowmap.shadowmap_constant_buffer.GetAddressOf());
		 // _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	  //}

#endif

	  //sprite_batches[PLAYER] = new Sprite_Batch(device.Get(), L"resources/player-sprites.png", 1);

	  //spritr[0] = new CircleSprite(device.Get());
	  //geometric_primitive オブジェクトを生成する
	  //geometric_primitive[0] = std::make_unique<Geometric_Primitive>(device.Get());

	  //static_meshes[0] = std::make_unique<Static_Mesh>(device.Get(), L"./resources/Rock/Rock.obj");

	 
	  //const char* cude_02 = ".\\resources\\cube.000.fbx";
	  //Skinned_Meshes[0] = std::make_unique<Skinned_Mesh>(device.Get(), cude_02,false,0);


	  framebuffers[0] = std::make_unique<framebuffer>(immediate_obj.device.Get(), 1280, 720);

	  framebuffers[1] = std::make_unique<framebuffer>(immediate_obj.device.Get(), 1280 / 2, 720 / 2);


	  //Skinned_Meshes[0] = std::make_unique<Skinned_Mesh>(device.Get(),".\\resources\\plane\\planestage.fbx",false,0);

	  bit_block_transfer = std::make_unique<fullscreen_quad>(immediate_obj.device.Get());

#if 0 // SKYMAP
	  //bit_block_transfer = std::make_unique<fullscreen_quad>(device.Get());
	  Texture::create_ps_from_cso(immediate_obj.device.Get(), "skymap_ps.cso", pixel_shaders[0].GetAddressOf());
	  Texture::load_texture_from_file(immediate_obj.device.Get(), L"./skymap/空.png", skymap.GetAddressOf(), &texture2d_desc);
#endif
	  //※Aim_Space.fbx はアニメーションデータだけを持ったファイルである
	  //Skinned_Meshes[0]->append_animations(".\\resources\\AimTest\\Aim_Space.fbx", 0);

	  //Skinned_Meshes[0] = std::make_unique<Skinned_Mesh>(device.Get(), "./resources/cube.000.fbx");

	  //geometric_primitive[1] = std::make_unique<Geometric_Primitive>(device.Get());

	  //for (int i = 0; i < 36; ++i)
	  //{
		 // geometric_primitive[1].get()->vertices[i].position.x += 1.0f;
	  //}
	  //geometric_primitive[1]->create_com_buffers(device.Get(), geometric_primitive[1]->vertices, 24, geometric_primitive[1]->indices, (sizeof(geometric_primitive[1]->indices) / sizeof(uint32_t))); //バッファの生成

	  //UNIT33

	  SceneManager::Instance().ChangeScene(new SceneGame);

	  Texture::create_ps_from_cso(immediate_obj.device.Get(), "luminance_extraction_ps.cso", pixel_shaders[0].GetAddressOf());

	  Texture::create_ps_from_cso(immediate_obj.device.Get(), "blur_ps.cso", pixel_shaders[1].GetAddressOf());

	  //Texture::load_texture_from_file(device.Get(), L"./environments/environments/sunset_jhbcentral_4k/sunset_jhbcentral_4k.dds",
		 // shader_resource_views[0].GetAddressOf(), &texture2d_desc);
	  //Texture::load_texture_from_file(device.Get(), L"./environments/environments/sunset_jhbcentral_4k/diffuse_iem.dds",
		 // shader_resource_views[1].GetAddressOf(), &texture2d_desc);
	  //Texture::load_texture_from_file(device.Get(), L"./environments/environments/sunset_jhbcentral_4k/specular_pmrem.dds",
		 // shader_resource_views[2].GetAddressOf(), &texture2d_desc);
	  //Texture::load_texture_from_file(device.Get(), L"./environments/environments/lut_ggx.dds",
		 // shader_resource_views[3].GetAddressOf(), &texture2d_desc);


	return true;
}


void Framework::update(float elapsed_time/*Elapsed seconds from last frame*/)
{
#ifdef USE_IMGUI
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif

	// シーン更新処理
	SceneManager::Instance().Update(immediate_obj.device.Get(), elapsed_time);

	
#ifdef USE_IMGUI


	ImGui::Begin("ImGUI");
	int i = 0;

	//if (ImGui::CollapsingHeader("Position", ImGuiTreeNodeFlags_DefaultOpen))
	//	if(Imguiprimitivevertex(device.Get(), &geometric_primitive[0]->vertices[i],nullptr))
	//		geometric_primitive[0]->create_com_buffers(device.Get(), geometric_primitive[0]->vertices, 
	//				24, geometric_primitive[0]->indices, 36); //バッファの生成

	//if (ImGui::CollapsingHeader("Indices", ImGuiTreeNodeFlags_DefaultOpen))
	//	if (Imguiprimitivevertex(device.Get(), nullptr, &geometric_primitive[0]->indices[i]))
	//		geometric_primitive[0]->create_com_buffers(device.Get(), geometric_primitive[0]->vertices, 24, geometric_primitive[0]->indices, 36); //バッファの生成




	ImGui::End();
#endif

#ifdef USE_IMGUI
	ImGui::Begin("ImGUI");

	ImGui::SliderFloat("Roll ", &RollPitchYaw.x, -180.0f, 180.0f);
	ImGui::SliderFloat("Pitch ", &RollPitchYaw.y, -180.0f, 180.0f);
	ImGui::SliderFloat("Yaw", &RollPitchYaw.z, -180.0f, 180.0f);


	ImGui::End();
#endif

	
#ifdef USE_IMGUI
	ImGui::Begin("ImGUI");

	//ImGui::SliderFloat("Collr Red", &Collr.x, 0.0f, 1.0f);
	//ImGui::SliderFloat("Collr Gren", &Collr.y, 0.0f, 1.0f);
	//ImGui::SliderFloat("Collr Bur", &Collr.z, 0.0f, 1.0f);
	//ImGui::SliderFloat("Collr W", &Collr.w, 0.0f, 1.0f);

	ImGui::End();
#endif

#ifdef USE_IMGUI
	ImGui::Begin("ImGUI");

	int clip_index{ 0 };
	int frame_index{ 0 };
	static float animation_tick{ 0 };

	//animation& animation{ Skinned_Meshes[0]->animation_clips.at(clip_index) };
	//frame_index = static_cast<int>(animation_tick * animation.sampling_rate);

	//ImGui::SliderFloat("sampling_rate", &animation.sampling_rate, 0.0f, 100.0f);

	ImGui::End();
#endif
	
#ifdef USE_IMGUI
	ImGui::Begin("ImGUI");

	ImGui::SliderFloat("translation", &Animetranslation.x, 0.0f, 300.0f);
	ImGui::SliderFloat("Angle", &animeAngle, 0.0f, 6.0f);
	ImGui::SliderFloat("factor", &animefactor, 0.0f, 10.0f);


	//ImGui::Image(gltf_models[0]->texture_resource_views[0].Get(), { 200, 200 });
	//ImGui::Image(gltf_models[0]->texture_resource_views[1].Get(), { 200, 200 });
	//ImGui::Image(gltf_models[0]->texture_resource_views[2].Get(), { 200, 200 });

	ImGui::End();

#endif

}

//深度ステンシルとは描写する際の順番決めるためのやつっぽい？   rsour smart phone resource view
//カメラと描写の距離を測る機能？
void Framework::render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	HRESULT hr{ S_OK };

	States& states = States::Instance();

	 //UNIT32※フレームバッファは同時にレンダーターゲットビューおよびシェーダーリソースビューとしてシェーダーにバインドすることはできない
	 //※下記コードを framework クラスの render メンバ関数の先頭に挿入する
	 ID3D11RenderTargetView* null_render_target_views[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT]{};
	 immediate_obj.immediate_context->OMSetRenderTargets(_countof(null_render_target_views), null_render_target_views, 0);
	 ID3D11ShaderResourceView* null_shader_resource_views[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT]{};
	 immediate_obj.immediate_context->VSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);
	 immediate_obj.immediate_context->PSSetShaderResources(0, _countof(null_shader_resource_views), null_shader_resource_views);
	 FLOAT colorback[]{ 0.2f, 0.2f, 0.2f, 1.0f };
	 immediate_obj.immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get()); //描画する画面を指定する（レンダーターゲットビューの配列化）
	 immediate_obj.immediate_context->ClearRenderTargetView(render_target_view.Get(), colorback); //画面の塗りつぶし
	 immediate_obj.immediate_context->ClearDepthStencilView(depth_stencil_view.Get(), //描画の奥行の情報をリセット（この値を使用して深度バッファーをクリア）
		 D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0); //	クリアする深度ステンシルへのポインター（表示範囲の設定？） （この値は 0 から 1 の間でクランプ。）


		//③ サンプラーステートオブジェクトをバインドする
	 immediate_obj.immediate_context->PSSetSamplers(0, 1, states.sampler_states[0].GetAddressOf());
	 immediate_obj.immediate_context->PSSetSamplers(1, 1, states.sampler_states[1].GetAddressOf());
	 immediate_obj.immediate_context->PSSetSamplers(2, 1, states. sampler_states[2].GetAddressOf());
	 // SHADOW
	 immediate_obj.immediate_context->PSSetSamplers(5, 1, states.sampler_states[3].GetAddressOf());


		//UNIT32
	 framebuffers[0]->clear(immediate_obj.immediate_context.Get(),0,0,1);
	 framebuffers[0]->activate(immediate_obj.immediate_context.Get());


	 {

		 SceneManager::Instance().Render(immediate_obj.immediate_context.Get(), elapsed_time);


	 }
	 //①framework クラスの initialize メンバ関数で framebuffer オブジェクトを生成する


	 // それぞれのウィンドウに反映されるようにSwapChainのPresent



	 immediate_obj.immediate_context->RSSetState(states.rasterizer_states[0].Get()); //ラスタライザー
	 immediate_obj.immediate_context->OMSetDepthStencilState(states.depth_stencil_states[3].Get(), 1);//デプスステンシル

		 //UNIT32
	 framebuffers[0]->deactivate(immediate_obj.immediate_context.Get());
	 bit_block_transfer->blit(immediate_obj.immediate_context.Get(), skymap.GetAddressOf(), 0, 1, pixel_shaders[0].Get());

	 framebuffers[1]->clear(immediate_obj.immediate_context.Get());
	  framebuffers[1]->activate(immediate_obj.immediate_context.Get());
	  bit_block_transfer->blit(immediate_obj.immediate_context.Get(),
		 framebuffers[0]->shader_resource_views[0].GetAddressOf(), 0, 1, pixel_shaders[0].Get());
	  framebuffers[1]->deactivate(immediate_obj.immediate_context.Get());
	  ID3D11ShaderResourceView* shader_resource_views[2]
		   { framebuffers[0]->shader_resource_views[0].Get(), framebuffers[1]->shader_resource_views[0].Get() };
	   bit_block_transfer->blit(immediate_obj.immediate_context.Get(), shader_resource_views, 0, 2, pixel_shaders[1].Get());

	   //ImGui::Begin("ImGUI");

	   //ImGui::Image(skymap.Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });


	   //ImGui::End();
		 // 不要になったのでシャドウマップの設定を外しておく。
 // そうしないと次回シャドウマップ生成時に警告が出てしまうため。
	 ID3D11ShaderResourceView* clear_shader_resource_view[]{ nullptr };
	 immediate_obj.immediate_context->PSSetShaderResources(4, 1, clear_shader_resource_view);



#ifdef USE_IMGUI
	 ImGui::Render();
	 ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

	 UINT sync_interval{ 0 };
	 swap_chain->Present(sync_interval, 0);//スワップチェーンを入れ替え


}

//auto safedelete = [](auto& p)
//{
//	if (p != nullptr)
//		p->Release();
//};

bool Framework::uninitialize()
{
	 //device->Release();
	 //immediate_context->Release();
	 //swap_chain->Release();
	 //render_target_view->Release();
	 //depth_stencil_view->Release();

	 //for (SpritrHLSI* p : sprites) delete p;

	 //for (Sprite_Batch* p : sprite_batches) delete p;

	 //for (auto p : sampler_states) safedelete(p);

	 //for (auto p : depth_stencil_states) safedelete(p);

	 //for (ID3D11BlendState* p : blend_states) safedelete(p);
	 
	//for (auto p : sprite_batches) delete p;

	for (auto p : spritr) delete p;

	SceneManager::Instance().Clear();

	return true;
}

Framework::~Framework()
{

}