#include "SceneGame.h"
#include <fstream>

#include "misc.h"
#include "texture.h"
#include "SceneManager.h"
#include "framework.h"
#include "../Mathf.h"
ComOBJ Framework::immediate_obj;

void SceneGame::Initialize(ID3D11Device* device)
{
	HRESULT hr{ S_OK };


	
	//②シーン定数バッファオブジェクトを生成する
	D3D11_BUFFER_DESC buffer_desc{};//上にほとんど同じことが書いてある
	buffer_desc.ByteWidth = sizeof(scene_constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT; //ノートに書いてある
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffers[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	// UNIT.32
	buffer_desc.ByteWidth = sizeof(parametric);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffers[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_buffer{};
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		texture2d_desc.Width = SCREEN_WIDTH;
		texture2d_desc.Height = SCREEN_HEIGHT;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = DXGI_FORMAT_R32_TYPELESS;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.SampleDesc.Quality = 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = 0;
		hr = device->CreateTexture2D(&texture2d_desc, NULL, depth_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		////	深度ステンシルビュー生成
		//D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc{};
		//depth_stencil_view_desc.Format = DXGI_FORMAT_D32_FLOAT;
		//depth_stencil_view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		//depth_stencil_view_desc.Texture2D.MipSlice = 0;
		//hr = device->CreateDepthStencilView(depth_buffer.Get(),
		//	&depth_stencil_view_desc,
		//	shadowmap.shadowmap_depth_stencil_view.GetAddressOf());
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		////	シェーダーリソースビュー生成
		//D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
		//shader_resource_view_desc.Format = DXGI_FORMAT_R32_FLOAT;
		//shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		//shader_resource_view_desc.Texture2D.MostDetailedMip = 0;
		//shader_resource_view_desc.Texture2D.MipLevels = 1;
		//hr = device->CreateShaderResourceView(depth_buffer.Get(),
		//	&shader_resource_view_desc,
		//	shadowmap.shadowmap_shader_resource_view.GetAddressOf());
		//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}


	//{
	//	buffer_desc.ByteWidth = sizeof(shadowmap_constants);
	//	hr = device->CreateBuffer(&buffer_desc, nullptr, shadowmap.shadowmap_constant_buffer.GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//}

	//D3D11_TEXTURE2D_DESC texture2d_desc{}; //2D テクスチャメイン部分？
	//texture2d_desc.Width = SCREEN_WIDTH; //テクスチャ範囲幅
	//texture2d_desc.Height = SCREEN_HEIGHT;//テクスチャ範囲高さ
	//texture2d_desc.MipLevels = 1; //ミップマップレベル最大数
	//texture2d_desc.ArraySize = 1; //　テクスチャ配列の数
	//texture2d_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //リソース データ形式
	////DXGI_FORMAT_D24_UNORM_S8_UINT(深度に 24 ビット、ステンシルに 8 ビットをサポートする 32 ビットの z バッファー形式)
	//texture2d_desc.SampleDesc.Count = 1; //ピクセルあたりのマルチサンプルの数
	//texture2d_desc.SampleDesc.Quality = 0; //画質レベル
	//texture2d_desc.Usage = D3D11_USAGE_DEFAULT; //テクスチャの読み取りと書き込み方法を識別する値
	////D3D11_USAGE_DEFAULT(GPU含めて使用許可)
	//texture2d_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL; //パイプライン 種類指定の値
	////パイプラインとは一連の流れ

	// //D3D11_BIND_DEPTH_STENCIL テクスチャをOM(Out Merger)ステージの深度ステンシル ターゲットとしてバインドします
	//texture2d_desc.CPUAccessFlags = 0; //CPU アクセスの種類を指定するため値 
	////(なし)
	//texture2d_desc.MiscFlags = 0; //他のあまり一般的でないリソース オプションを識別する

	//Texture::load_texture_from_file(device, L"./environments/environments/sunset_jhbcentral_4k/sunset_jhbcentral_4k.dds",
	//	shader_resource_views[0].GetAddressOf(), &texture2d_desc);
	//Texture::load_texture_from_file(device, L"./environments/environments/sunset_jhbcentral_4k/diffuse_iem.dds",
	//	shader_resource_views[1].GetAddressOf(), &texture2d_desc);
	//Texture::load_texture_from_file(device, L"./environments/environments/sunset_jhbcentral_4k/specular_pmrem.dds",
	//	shader_resource_views[2].GetAddressOf(), &texture2d_desc);
	//Texture::load_texture_from_file(device, L"./environments/environments/lut_ggx.dds",
	//	shader_resource_views[3].GetAddressOf(), &texture2d_desc);


	const int MAX = 1000;
	const char* cude_02 = ".\\resources\\cube.000.fbx";
	static_mesh_instncings[0] = std::make_unique<Static_mesh_instncing>(device, cude_02, false, MAX);

	//Skinned_Meshes[0] = std::make_unique<Skinned_Mesh>(device, ".\\resources\\plane\\planestage.fbx", false, 0);

	Stage = std::make_unique<GlTF_Character>(".\\resources\\stage\\ExampleStage.glb");

	Stage->position = {0,-2,0};
	Stage->scale_factor = 1.0f;
	Stage->scale = { 30,30,30 };
	//Shadow[0] = std::make_unique<Shadow_model>(".\\resources\\nico.fbx",MAX);

	//framebuffers[0] = std::make_unique<framebuffer>(device, 1280, 720);

	//framebuffers[1] = std::make_unique<framebuffer>(device, 1280 / 2, 720 / 2);

	sprite_batches[0] = std::make_unique<Sprite_Batch>(device,L".\\resources\\screenshot.jpg",1);

	player = std::make_unique<Player>(device, ".\\resources\\nico.glb",false);

	for (int i = 0; i < 5; ++i)
	{

		players.emplace_back() = std::make_unique<Character>();
		players.at(i)->position = { -(10 ) + (float)i ,0,0};

	}
	//Boxs.model = std::make_unique<Static_mesh_instncing>(device, cude_02, false, MAX);
	collision_mesh = std::make_unique<decltype(collision_mesh)::element_type>(device, ".\\resources\\stage\\ExampleStage.glb");

	//Boxs.characters.resize(MAX);

	camrea_count = 1;
	for (int i = 0; i < camrea_count; ++i)
	{
		auto& camera = cameras.emplace_back() = std::make_unique<Camera>();

		camera->camerasize.camera_number = i;
		camera->camerasize.Width = (SCREEN_WIDTH);
		camera->camerasize.Height = SCREEN_HEIGHT;
		camera->camerasize.TopLeftX = {};
		camera->camerasize.TopLeftY = {};
	}




	// SHADOW
	double_speed_z = std::make_unique<Shadow>(device, shadowmap_width, shadowmap_height);


	//for (int i = -50; i < 50; ++i)
	//{

	//	//for (int b = 0; b < 1; ++b)
	//	//{
	//		if (MAX < a)
	//			break;

	//	Boxs.characters.at(a).position = XMFLOAT3(i, 0, 0);
	//	Boxs.characters.at(a).value = 0.1f * 0.5f + (i * 0.1f);

	//		a++;

	//	//}
	//}
}

void SceneGame::Update(ID3D11Device* device,float elapsedTime)
{

	gamepad& pad = SceneManager::Instance().pad;

	pad.acquire();

    //biking->Render(dc, elapsedTime);


	player->Update(elapsedTime);


	Stage->Update(elapsedTime);


	animated_nodes = { player->model->nodes };

	player->model->animate(0, player->timer += elapsedTime, animated_nodes, true);

	for(auto& player : players)
	{
		player->Update(elapsedTime);


		//地面のレイキャスト
		using namespace DirectX;
		{
			const float radius = 1.0f; //キャラの半径
			const float liftup = 1.0f; //始点補正後ろに置くため？
			XMVECTOR L = XMVector3Normalize(XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));//下に法線
			XMVECTOR L0 = XMLoadFloat3(&player->position) - liftup * L;//始点

			XMFLOAT3 l0;
			XMFLOAT3 l;
			XMStoreFloat3(&l0, L0);
			XMStoreFloat3(&l, L);

			XMFLOAT3 intersection_point;
			XMFLOAT3 intersected_normal;
			std::string intersected_mesh;
			std::string intersected_material;


			if (collision_mesh->raycast(l0, l, Stage->world, intersection_point, intersected_normal, intersected_mesh, intersected_material))
			{
				XMVECTOR X = XMLoadFloat3(&intersection_point);//当たった場所のベクトル
				float d = XMVectorGetX(XMVector3Length(X - L0));//キャラクターと当たった場所の距離
				if (d < radius)//距離が範囲以下なら
				{
					XMStoreFloat3(&player->position, XMLoadFloat3(&player->position) - L * (radius - d));//法線は修正する向き？radius - dで距離が求まる？
					player->velocity.y = 0;

					player->acceleration.y = 0;

					material_name = intersected_material;

					float ax = player->angle.x;
					float az = player->angle.z;

					player->angle.x = atan2f(intersected_normal.z, intersected_normal.y);
					player->angle.z = -atan2f(intersected_normal.x, intersected_normal.y);

					// 線形補完で滑らかに回転する
					player->angle.x = Mathf::Lerp(ax, player->angle.x, 0.1f);
					player->angle.z = Mathf::Lerp(az, player->angle.z, 0.1f);


				}
			}
		}

		{
			const float radius = 1.0f;//キャラの半径
			const float liftup = 0.5f;//始点補正(上に上げる)
			const float stepback = 0.5f;//後ろからするため？
			//XMVECTOR L = XMVector3Normalize(XMLoadFloat3(&k.velocity));
			XMVECTOR L = XMVector3Normalize(XMVectorSet(player->velocity.x, 0, player->velocity.z, 0));//移動法線
			XMVECTOR L0 = XMLoadFloat3(&player->position) + XMVectorSet(0.0f, liftup, 0.0f, 0.0f) - stepback * L;//法線で方向を決めて後ろに線を出す？

			XMFLOAT3 l0;
			XMFLOAT3 l;
			XMStoreFloat3(&l0, L0);
			XMStoreFloat3(&l, L);

			XMFLOAT3 intersection_point;//当たった場所(交差判定)
			XMFLOAT3 intersected_normal;//壁の法線(交差法線)
			std::string intersected_mesh;//当たったメッシュ
			std::string intersected_material;//当たったマテリアル

			crashed = false;
			if (collision_mesh->raycast(l0, l, Stage->world, intersection_point, intersected_normal, intersected_mesh, intersected_material))
			{

				XMVECTOR X = XMLoadFloat3(&intersection_point);//当たった場所のベクトル
				float d = XMVectorGetX(XMVector3Length(X - L0));//キャラクターと当たった場所の距離
				if (d < radius)
				{
					XMVECTOR V = XMLoadFloat3(&player->velocity);//車のベクトル
					//XMVECTOR N = XMVector3Normalize(XMLoadFloat3(&intersected_normal));//モデル？のせいで少し飛ぶためY軸は使わない
					XMVECTOR N = XMVector3Normalize(XMVectorSet(intersected_normal.x, 0, intersected_normal.z, 0));//壁の法線
					XMVECTOR P = XMVector3Dot(N, V) * N;//反射の長さを求める壁の方向に作るため
					XMVECTOR R = V - P;//Rejoctionの略Rは反転してはいけないため分ける
					const float coefficient_of_restitution = 0.5f; //反発値(バウンドしてるやつ)
					XMStoreFloat3(&player->velocity, R - P * coefficient_of_restitution);//PはマイナスでRはそのまま
					XMStoreFloat3(&player->position, XMLoadFloat3(&player->position) - L * (radius - d));//ポジション作成

					crashed = true;
				}
			}
		}

	}

	//地面のレイキャスト
	using namespace DirectX;
	{
		const float radius = 1.0f; //キャラの半径
		const float liftup = 1.0f; //始点補正後ろに置くため？
		XMVECTOR L = XMVector3Normalize(XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));//下に法線
		XMVECTOR L0 = XMLoadFloat3(&player->position) - liftup * L;//始点

		XMFLOAT3 l0;
		XMFLOAT3 l;
		XMStoreFloat3(&l0, L0);
		XMStoreFloat3(&l, L);

		XMFLOAT3 intersection_point;
		XMFLOAT3 intersected_normal;
		std::string intersected_mesh;
		std::string intersected_material;


		if (collision_mesh->raycast(l0, l, Stage->world, intersection_point, intersected_normal, intersected_mesh, intersected_material))
		{
			XMVECTOR X = XMLoadFloat3(&intersection_point);//当たった場所のベクトル
			float d = XMVectorGetX(XMVector3Length(X - L0));//キャラクターと当たった場所の距離
			if (d < radius)//距離が範囲以下なら
			{
				XMStoreFloat3(&player->position, XMLoadFloat3(&player->position) - L * (radius - d));//法線は修正する向き？radius - dで距離が求まる？
				player->velocity.y = 0;

				player->acceleration.y = 0;

				material_name = intersected_material;

				float ax = player->angle.x;
				float az = player->angle.z;

				player->angle.x = atan2f(intersected_normal.z, intersected_normal.y);
				player->angle.z = -atan2f(intersected_normal.x, intersected_normal.y);

				// 線形補完で滑らかに回転する
				player->angle.x = Mathf::Lerp(ax, player->angle.x, 0.1f);
				player->angle.z = Mathf::Lerp(az, player->angle.z, 0.1f);


			}
		}
	}

	{
		const float radius = 1.0f;//キャラの半径
		const float liftup = 0.5f;//始点補正(上に上げる)
		const float stepback = 0.5f;//後ろからするため？
		//XMVECTOR L = XMVector3Normalize(XMLoadFloat3(&k.velocity));
		XMVECTOR L = XMVector3Normalize(XMVectorSet(player->velocity.x, 0, player->velocity.z, 0));//移動法線
		XMVECTOR L0 = XMLoadFloat3(&player->position) + XMVectorSet(0.0f, liftup, 0.0f, 0.0f) - stepback * L;//法線で方向を決めて後ろに線を出す？

		XMFLOAT3 l0;
		XMFLOAT3 l;
		XMStoreFloat3(&l0, L0);
		XMStoreFloat3(&l, L);

		XMFLOAT3 intersection_point;//当たった場所(交差判定)
		XMFLOAT3 intersected_normal;//壁の法線(交差法線)
		std::string intersected_mesh;//当たったメッシュ
		std::string intersected_material;//当たったマテリアル

		crashed = false;
		if (collision_mesh->raycast(l0, l, Stage->world, intersection_point, intersected_normal, intersected_mesh, intersected_material))
		{

			XMVECTOR X = XMLoadFloat3(&intersection_point);//当たった場所のベクトル
			float d = XMVectorGetX(XMVector3Length(X - L0));//キャラクターと当たった場所の距離
			if (d < radius)
			{
				XMVECTOR V = XMLoadFloat3(&player->velocity);//車のベクトル
				//XMVECTOR N = XMVector3Normalize(XMLoadFloat3(&intersected_normal));//モデル？のせいで少し飛ぶためY軸は使わない
				XMVECTOR N = XMVector3Normalize(XMVectorSet(intersected_normal.x, 0, intersected_normal.z, 0));//壁の法線
				XMVECTOR P = XMVector3Dot(N, V) * N;//反射の長さを求める壁の方向に作るため
				XMVECTOR R = V - P;//Rejoctionの略Rは反転してはいけないため分ける
				const float coefficient_of_restitution = 0.5f; //反発値(バウンドしてるやつ)
				XMStoreFloat3(&player->velocity, R - P * coefficient_of_restitution);//PはマイナスでRはそのまま
				XMStoreFloat3(&player->position, XMLoadFloat3(&player->position) - L * (radius - d));//ポジション作成

				crashed = true;
			}
		}

	}

	IMGUIRender();

}

//void SceneGame::Render(ID3D11DeviceContext* immediate_context, float elapsedTime)
//{
//
//
//	//D3D11_VIEWPORT viewport;
//	//UINT num_viewports{ 1 };
//	//immediate_context->RSGetViewports(&num_viewports, &viewport);
//
//	//float aspect_ratio{ viewport.Width / viewport.Height };
//
//	//DirectX::XMMATRIX P{ DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30), aspect_ratio, 0.1f, 100.0f) };
//
//	//DirectX::XMVECTOR eye{ DirectX::XMVectorSet(camrea.pos.x, camrea.pos.y,camrea.pos.z - 10.0f, 1.0f) };
//	//DirectX::XMVECTOR focus{ DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f) };//ゲープロ３を参照
//	//DirectX::XMVECTOR up{ DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };
//	//DirectX::XMMATRIX V{ DirectX::XMMatrixLookAtLH(eye, focus, up) };
//
//	//scene_constants data{};
//	//DirectX::XMStoreFloat4x4(&data.view_projection, V * P);
//	//data.light_direction = { DirectX::XMConvertToRadians(camrea.light.x), DirectX::XMConvertToRadians(camrea.light.y), DirectX::XMConvertToRadians(camrea.light.z), DirectX::XMConvertToRadians(camrea.light.w) };
//	////UNIT.16
//	//data.camera_position = { camrea.pos.x,camrea.pos.y,camrea.pos.z,1 };
//
//	//immediate_context->UpdateSubresource(constant_buffers[0].Get(), 0, 0, &data, 0, 0);//CPU が、マップできないメモリにサブリソースからデータをコピー
//	//immediate_context->VSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());//頂点シェーダー パイプライン の設定で使用される定数バッファーを設定
//
//    static_mesh_instncings[0]->Begin(immediate_context);
//
//    for (auto& character : characters)
//    {
//        //GPUエラー吐いてるのこいつが原因
//        static_mesh_instncings[0]->render(immediate_context, character.world, { 1, 1, 1, 1 });
//
//    }
//
//    static_mesh_instncings[0]->End(immediate_context);
//
//	IMGUIRender();
//}
//
void SceneGame::Render(ID3D11DeviceContext* immediate_context, float elapsedTime)
{
	States& states = States::Instance();

	camrea_count = 0;



	for (auto& camera : cameras)
	{

		//if(camera->camerasize.camera_number == 2)
		{


			//ビューポートの設定

			D3D11_VIEWPORT viewport{};
			viewport.TopLeftX = camera->camerasize.TopLeftX; //左側のX座標
			viewport.TopLeftY = camera->camerasize.TopLeftY; //左側のY座標
			viewport.Width = static_cast<float>(camera->camerasize.Width); //幅
			viewport.Height = static_cast<float>(camera->camerasize.Height);//高さ
			viewport.MinDepth = 0.0f; //最小深度
			viewport.MaxDepth = 1.0f; //最大深度
			immediate_context->RSSetViewports(1, &viewport);//作成したビューポートを設定（描画場所の指定）


			//float aspect_ratio{ viewport.Width / viewport.Height };
			//camera->P = { DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30), aspect_ratio, 0.1f, 100.0f) };


			//camera->camera_position = { 0,2,0,1 };

			//camera->eye = camera_position;

			//camera->SetLookAt(camera->eye, { player->position.x,player->position.y + 0.5f,player->position.z }, DirectX::XMFLOAT3(0, 1, 0));

		}
		camera->Updata(immediate_context, elapsedTime, player.get());


		D3D11_VIEWPORT viewport;
		UINT num_viewports{ 1 };
		immediate_context->RSGetViewports(&num_viewports, &viewport);

		float aspect_ratio{ viewport.Width / viewport.Height };
		DirectX::XMMATRIX P{ DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30), aspect_ratio, 0.1f, 100.0f) };

		//DirectX::XMVECTOR eye{ DirectX::XMVectorSet(camrea.pos.x, camrea.pos.y,camrea.pos.z - 10.0f, 1.0f) };
		//DirectX::XMVECTOR focus{ DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f) };//ゲープロ３を参照
		//DirectX::XMVECTOR up{ DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };
		//DirectX::XMMATRIX V{ DirectX::XMMatrixLookAtLH(eye, focus, up) };
		data.light_direction = camera->light_direction;
		data.camera_position = camera->camera_position;

		

		//immediate_context->UpdateSubresource(constant_buffers[0].Get(), 0, 0, &data, 0, 0);//CPU が、マップできないメモリにサブリソースからデータをコピー
		//immediate_context->VSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());//頂点シェーダー パイプライン の設定で使用される定数バッファーを設定

		//// UNIT.16
		//immediate_context->PSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());//ピクセルシェーダー パイプライン の設定で使用される定数バッファーを設定

		//UNIT32
		immediate_context->UpdateSubresource(constant_buffers[1].Get(), 0, 0, &parametric, 0, 0);//CPU が、マップできないメモリにサブリソースからデータをコピー
		immediate_context->PSSetConstantBuffers(2, 1, constant_buffers[1].GetAddressOf());


		//immediate_context->PSSetShaderResources(32, 1, shader_resource_views[0].GetAddressOf());
		//immediate_context->PSSetShaderResources(33, 1, shader_resource_views[1].GetAddressOf());
		//immediate_context->PSSetShaderResources(34, 1, shader_resource_views[2].GetAddressOf());
		//immediate_context->PSSetShaderResources(35, 1, shader_resource_views[3].GetAddressOf());

		//※深度テスト：オフ、深度ライト：オフで生成したオブジェクトをセットする
		immediate_context->OMSetDepthStencilState(states.depth_stencil_states[0].Get(), 1);//デプスステンシル

		immediate_context->RSSetState(states.rasterizer_states[0].Get()); //ラスタライザー


		////UNIT10
		//sprite_batches[0]->Begin(immediate_context);

		//sprite_batches[0]->render(immediate_context,
		//	0, 0, 1280, 720);

		//sprite_batches[0]->End(immediate_context);


		// SHADOW : make shadow map
		{
			using namespace DirectX;

			const float aspect_ratio = double_speed_z->viewport.Width / double_speed_z->viewport.Height;

			const XMFLOAT4 light_view_focus = { camera->focus.x,camera->focus.y,camera->focus.z,1.0f };
			XMVECTOR F{ XMLoadFloat4(&light_view_focus) };
			XMVECTOR E{ F - XMVector3Normalize(XMLoadFloat4(&data.light_direction)) * camera->cameradistance };
			XMVECTOR U{ XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };
			XMMATRIX V{ XMMatrixLookAtLH(E, F, U) };
			XMMATRIX P{ XMMatrixOrthographicLH(camera->light_view_size * aspect_ratio, camera->light_view_size, camera->light_view_near_z, camera->light_view_far_z) };


			DirectX::XMStoreFloat4x4(&data.view_projection, V * P);
			data.light_view_projection = data.view_projection;

			immediate_context->UpdateSubresource(constant_buffers[0].Get(), 0, 0, &data, 0, 0);
			immediate_context->VSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());

			double_speed_z->clear(immediate_context, 1.0f);
			double_speed_z->activate(immediate_context);


			ID3D11PixelShader* null_pixel_shader{ NULL };

			static std::vector<gltf_model::node> animated_nodes = { player->model->nodes };

			player->model->render(immediate_context, player->world, animated_nodes, &null_pixel_shader);

			//immediate_context->RSSetState(states.rasterizer_states[1].Get()); //ラスタライザー

			//immediate_context->OMSetDepthStencilState(states.depth_stencil_states[0].Get(), 1);//デプスステンシル

			for (auto& pl : players)
			{
				pl->Render(player->model.get(), immediate_context, elapsedTime);
			}


			//immediate_context->PSSetShaderResources(4, 1, shadowmap.shadowmap_shader_resource_view.GetAddressOf());
			//immediate_context->PSSetSamplers(4, 1, shadowmap.shadowmap_sampler_state.GetAddressOf());

			////※深度テスト：オフ、深度ライト：オフで生成したオブジェクトをセットする
			//immediate_context->OMSetDepthStencilState(states.depth_stencil_states[0].Get(), 1);//デプスステンシル

			//immediate_context->RSSetState(states.rasterizer_states[0].Get()); //ラスタライザー

			static std::vector<gltf_model::node> animated_nodesstage = { Stage->model->nodes };

			Stage->model->render(immediate_context, Stage->world, animated_nodesstage, &null_pixel_shader);


			double_speed_z->deactivate(immediate_context);
		}

		immediate_context->RSGetViewports(&num_viewports, &viewport);



		//camera->Updata(immediate_context, elapsedTime, player.get());

		//data = {};
		DirectX::XMStoreFloat4x4(&data.view_projection, XMLoadFloat4x4(&camera->view_projection));
		data.light_direction = { DirectX::XMConvertToRadians(light_.light.x), DirectX::XMConvertToRadians(light_.light.y), DirectX::XMConvertToRadians(light_.light.z), DirectX::XMConvertToRadians(light_.light.w) };
		//UNIT.16
		data.camera_position = { camera->camera_position.x,camera->camera_position.y,camera->camera_position.z,1 };
		DirectX::XMStoreFloat4x4(&data.view_projection, XMLoadFloat4x4(&camera->view_projection));

		immediate_context->UpdateSubresource(constant_buffers[0].Get(), 0, 0, &data, 0, 0);
		immediate_context->VSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());
		immediate_context->PSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());



		// SHADOW : bind shadow map at slot 8
		immediate_context->PSSetShaderResources(8, 1, double_speed_z->shader_resource_view.GetAddressOf());

		immediate_context->RSSetState(states.rasterizer_states[0].Get()); //ラスタライザー

		immediate_context->OMSetDepthStencilState(states.depth_stencil_states[0].Get(), 1);//デプスステンシル


		player->model->render(immediate_context, player->world, animated_nodes, nullptr);

		immediate_context->RSSetState(states.rasterizer_states[1].Get()); //ラスタライザー

		immediate_context->OMSetDepthStencilState(states.depth_stencil_states[0].Get(), 1);//デプスステンシル

		for (auto& pl : players)
		{
			pl->Render(player->model.get(), immediate_context, elapsedTime);
		}


		//immediate_context->PSSetShaderResources(4, 1, shadowmap.shadowmap_shader_resource_view.GetAddressOf());
		//immediate_context->PSSetSamplers(4, 1, shadowmap.shadowmap_sampler_state.GetAddressOf());

		//※深度テスト：オフ、深度ライト：オフで生成したオブジェクトをセットする
		immediate_context->OMSetDepthStencilState(states.depth_stencil_states[0].Get(), 1);//デプスステンシル

		immediate_context->RSSetState(states.rasterizer_states[0].Get()); //ラスタライザー

		Stage->Render(immediate_context, elapsedTime);


		//animation::keyframe& keyframe{ anime.update_animation(*player->model, elapsedTime) };

		immediate_context->OMSetDepthStencilState(states.depth_stencil_states[0].Get(), 1);//デプスステンシル

		immediate_context->RSSetState(states.rasterizer_states[0].Get()); //ラスタライザー

	}
	//// シャドウマップ生成処理
	//{

	//	//これまた再設定しなおしても意味ないからくそ
	//	// シャドウマップ用の深度バッファに設定
	//	immediate_context->ClearDepthStencilView(shadowmap.shadowmap_depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	//	immediate_context->OMSetRenderTargets(0, nullptr, shadowmap.shadowmap_depth_stencil_view.Get());
	//	// ビューポートの設定
	//	D3D11_VIEWPORT viewport{};
	//	viewport.TopLeftX = 0;
	//	viewport.TopLeftY = 0;
	//	viewport.Width = static_cast<float>(SCREEN_WIDTH);
	//	viewport.Height = static_cast<float>(SCREEN_HEIGHT);
	//	viewport.MinDepth = 0.0f;
	//	viewport.MaxDepth = 1.0f;
	//	immediate_context->RSSetViewports(1, &viewport);



	//	//// ブレンドステートの設定
	//	//immediate_context->OMSetBlendState(blend_state.Get(), nullptr, 0xFFFFFFFF);
	//	//// 深度ステンシルステートの設定
	//	//immediate_context->OMSetDepthStencilState(depth_stencil_state.Get(), 0);
	//	//// ラスタライザーステートの設定
	//	//immediate_context->RSSetState(rasterizer_state.Get());
	//	// シェーダー設定
	//	immediate_context->PSSetShader(nullptr, nullptr, 0);

	//	immediate_context->RSSetState(states.rasterizer_states[1].Get()); //ラスタライザー

	//	immediate_context->OMSetDepthStencilState(states.depth_stencil_states[1].Get(), 1);//デプスステンシル


	//	{

	//		// ライトの位置から見た視線行列を生成
	//		DirectX::XMVECTOR LightPosition = DirectX::XMLoadFloat4(&light_.light);
	//		LightPosition = DirectX::XMVectorScale(LightPosition, -50);
	//		DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(LightPosition,
	//			DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
	//			DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

	//		// シャドウマップに描画したい範囲の射影行列を生成
	//		DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(SHADOWMAP_DRAWRECT, SHADOWMAP_DRAWRECT,
	//			0.1f, 200.0f);
	//		// ライトビュー行列を保存
	//		DirectX::XMStoreFloat4x4(&light_view_projection, V * P);

	//		// 定数バッファの更新
	//		{
	//			// 0番はメッシュ側で更新している
	//			scene_constants scene{};
	//			//scene.options.x = cursor_position.x;
	//			//scene.options.y = cursor_position.y;
	//			//scene.options.z = timer;
	//			//scene.options.w = flag;
	//			scene.view_projection = light_view_projection;
	//			immediate_context->UpdateSubresource(constant_buffers[0].Get(), 0, 0, &scene, 0, 0);
	//			immediate_context->VSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());
	//			immediate_context->PSSetConstantBuffers(1, 1, constant_buffers[0].GetAddressOf());

	//		}
	//		//player->ShadowRender(immediate_context);
	//		//Shadow[0]->Begin(immediate_context);

	//		//for (auto& character : Boxs.characters)
	//		//{
	//		//	Shadow[0]->render(immediate_context, character.world, { 1, 1, 1, 1 });
	//		//}
	//		//Shadow[0]->End(immediate_context);

	//	}
	//}

	////UNIT10
	//shadowmap_constants shadowmap_constants{};
	//shadowmap_constants.light_view_projection = light_view_projection;
	//shadowmap_constants.shadow_color = shadow_color;
	//shadowmap_constants.shadow_bias = shadow_bias;
	//immediate_context->UpdateSubresource(shadowmap.shadowmap_constant_buffer.Get(), 0, 0, &shadowmap_constants, 0, 0);
	//immediate_context->VSSetConstantBuffers(6, 1, shadowmap.shadowmap_constant_buffer.GetAddressOf());
	//immediate_context->PSSetConstantBuffers(6, 1, shadowmap.shadowmap_constant_buffer.GetAddressOf());

}


void SceneGame::Finalize()
{
	
}

void SceneGame::IMGUIRender()
{
#ifdef USE_IMGUI
	ImGui::Begin("ImGUI");

	ImGui::BulletText("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
#if 0
	ImGui::BulletText("Video memory usage %d MB", video_memory_usage());
#endif


	ImGui::SliderFloat("LightX ", &light_.light.x, -1.0f, 1.0f);
	ImGui::SliderFloat("LightY ", &light_.light.y, -1.0f, 1.0f);
	ImGui::SliderFloat("LightZ ", &light_.light.z, -1.0f, 1.0f);

	ImGui::SliderFloat("Camera_positionX ", &camera_position.x, -10.0f, 10.0f);
	ImGui::SliderFloat("Camera_positionY ", &camera_position.y, -10.0f, 10.0f);
	ImGui::SliderFloat("Camera_positionZ ", &camera_position.z, -10.0f, 10.0f);

	//if (ImGui::InputFloat4("Light", &camrea.light.x))
	//{

	//}
	for (auto& camera : cameras)
	camera->DrawDebugGUI();

	ImGui::SliderFloat("TranslationX", &Stage->position.x, -50.0f, 50.0f);
	ImGui::SliderFloat("TranslationY", &Stage->position.y, -50.0f, 50.0f);
	ImGui::SliderFloat("TranslationZ", &Stage->position.z, -100.0f, 100.0f);

	player->IMGUI();

	//ImGui::ColorEdit3("color", &color.x);

	// UNIT.32
	ImGui::SliderFloat("extraction_threshold", &parametric.extraction_threshold, +0.0f, +5.0f);
	ImGui::SliderFloat("gaussian_sigma", &parametric.gaussian_sigma, +0.0f, +10.0f);
	ImGui::SliderFloat("bloom_intensity", &parametric.bloom_intensity, +0.0f, +10.0f);
	ImGui::SliderFloat("exposure", &parametric.exposure, +0.0f, +10.0f);


	for (auto& camera : cameras)
	{
		ImGui::SliderFloat("light_directionX ", &camera->light_direction.x, -1.0f, 1.0f);
		ImGui::SliderFloat("light_directionY ", &camera->light_direction.y, -1.0f, 1.0f);
		ImGui::SliderFloat("light_directionZ ", &camera->light_direction.z, -1.0f, 1.0f);


		ImGui::Text("shadow_map");
		ImGui::SliderFloat("light_view_size", &camera->light_view_size, 1.0f, +100.0f);
	}

	ImGui::Image(reinterpret_cast<void*>(double_speed_z->shader_resource_view.Get()), ImVec2(shadowmap_width / 5.0f, shadowmap_height / 5.0f));
	//ImGui::Image(skymap.Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });

	ImGui::ColorEdit3("shadow_color", &shadow_color.x);
	ImGui::SliderFloat("shadow_bias", &shadow_bias, 0.00f, +0.01f);


	ImGui::End();
#endif


}
