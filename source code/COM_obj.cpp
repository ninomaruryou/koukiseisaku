#include "COM_obj_.h"
#include "misc.h"

void States::Initialize(ID3D11Device* device, ID3D11DeviceContext* immediate_context)
{
	HRESULT hr{ S_OK }; //エラー内容？ 現在は成功

	
	{
		// 下記コードは深度テスト：オン、深度ライト：オンの設定である
		// 深度テスト：オン・オフ、深度ライト：オン・オフの全４通りのオブジェクト

		D3D11_DEPTH_STENCIL_DESC depth_stencil_desc{}; //デプスステンシルビュー
		depth_stencil_desc.DepthEnable = TRUE; //深度テスト
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL; //深度ライト
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states.emplace_back().ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		depth_stencil_desc.DepthEnable = TRUE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states.emplace_back().ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		depth_stencil_desc.DepthEnable = FALSE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states.emplace_back().ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		depth_stencil_desc.DepthEnable = FALSE;
		depth_stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depth_stencil_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&depth_stencil_desc, depth_stencil_states.emplace_back().ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}


	{
		//② framework クラスの initialize メンバ関数でサンプラーステートオブジェクトを生成する
		D3D11_SAMPLER_DESC sampler_desc; //サンプラーステート(サンプリングのインターフェース？)描画処理を間引くようなもの
		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT; //最小化、拡大、ミップレベルサンプリングにはポイントサンプリングを使用
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;//テクスチャ座標を解決する手法		テクスチャをタイル状（平面に？平等に？）します
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;// //テクスチャの境界外	同じ
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP; //テクスチャの境界外	同じ
		sampler_desc.MipLODBias = 0; //float型 追加ミップマップ標準値デバック用とかで使えそう
		sampler_desc.MaxAnisotropy = 16; //clampに使用(最大値最小値のようなもの)
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampler_desc.BorderColor[0] = 0;
		sampler_desc.BorderColor[1] = 0;
		sampler_desc.BorderColor[2] = 0;
		sampler_desc.BorderColor[3] = 0;
		sampler_desc.MinLOD = 0;
		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = device->CreateSamplerState(&sampler_desc, sampler_states.emplace_back().ReleaseAndGetAddressOf()); //テクスチャのサンプラー状態を作成
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


		  sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;//テクスチャのサンプリング方法 (縮小、拡大、ミップレベル サンプリングに線形補間を使用)
		  hr = device->CreateSamplerState(&sampler_desc, sampler_states.emplace_back().ReleaseAndGetAddressOf());//テクスチャのサンプリング情報をカプセル化するオブジェクトを作成
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		sampler_desc.Filter = D3D11_FILTER_ANISOTROPIC;
		hr = device->CreateSamplerState(&sampler_desc, sampler_states.emplace_back().ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		// SHADOW
		sampler_desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sampler_desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL; //D3D11_COMPARISON_LESS_EQUAL
		sampler_desc.BorderColor[0] = 1;
		sampler_desc.BorderColor[1] = 1;
		sampler_desc.BorderColor[2] = 1;
		sampler_desc.BorderColor[3] = 1;
		hr = device->CreateSamplerState(&sampler_desc, sampler_states.emplace_back().ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		//UNIT10
//// サンプラステートの生成
//
//		sampler_desc = {};
//		sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
//		sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
//		sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
//		sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
//		sampler_desc.MipLODBias = 0;
//		sampler_desc.MaxAnisotropy = 16;
//		sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
//		sampler_desc.BorderColor[0] = FLT_MAX;
//		sampler_desc.BorderColor[1] = FLT_MAX;
//		sampler_desc.BorderColor[2] = FLT_MAX;
//		sampler_desc.BorderColor[3] = FLT_MAX;
//		sampler_desc.MinLOD = 0;
//		sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
//		hr = device->CreateSamplerState(&sampler_desc, shadowmap_sampler_state.GetAddressOf());
//

	}

	{
		//②framework クラスのコンストラクタでブレンドステートオブジェクトを作成する
		D3D11_BLEND_DESC blend_desc{};

		blend_desc.AlphaToCoverageEnable = FALSE;
		blend_desc.IndependentBlendEnable = FALSE;
		blend_desc.RenderTarget[0].BlendEnable = TRUE;
		blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		hr = device->CreateBlendState(&blend_desc, blend_states.emplace_back().GetAddressOf()); //blend_statesにあまり意味ない
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		blend_desc = {};

		//加算
		  //blend_desc.AlphaToCoverageEnable = FALSE;
		  //blend_desc.IndependentBlendEnable = FALSE;
		  //blend_desc.RenderTarget[0].BlendEnable = TRUE;
		  //blend_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		  //blend_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		  //blend_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		  //blend_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		  //blend_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		  //blend_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		  //blend_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		  //hr = device->CreateBlendState(&blend_desc, blend_states[1].GetAddressOf());
		  //_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}


	//ラスタライズの設定
	D3D11_RASTERIZER_DESC rasterizer_desc{};
	rasterizer_desc.FillMode = D3D11_FILL_SOLID;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.FrontCounterClockwise = FALSE;//三角形が表向きか背面かを決定します反時計回りでも描写してくれるカリング
	rasterizer_desc.DepthBias = 0;
	rasterizer_desc.DepthBiasClamp = 0;
	rasterizer_desc.SlopeScaledDepthBias = 0;
	rasterizer_desc.DepthClipEnable = TRUE;
	rasterizer_desc.ScissorEnable = FALSE;
	rasterizer_desc.MultisampleEnable = FALSE;
	rasterizer_desc.AntialiasedLineEnable = FALSE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states.emplace_back().GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizer_desc.FrontCounterClockwise = TRUE;//三角形が表向きか背面かを決定します反時計回りでも描写してくれるカリング

	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states.emplace_back().GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_desc.CullMode = D3D11_CULL_BACK;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states.emplace_back().GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizer_desc.CullMode = D3D11_CULL_NONE;
	rasterizer_desc.AntialiasedLineEnable = TRUE;
	hr = device->CreateRasterizerState(&rasterizer_desc, rasterizer_states.emplace_back().GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


}
