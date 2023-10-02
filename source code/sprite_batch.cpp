#include <list>

#include "sprite_batch.h"
#include "misc.h"
#include <sstream>
#include "vertex.h"

#include "texture.h"

#include "../DirectXTK-jul2022/Inc/WICTextureLoader.h"

namespace
{


	vertex vertices[] //頂点情報のセット初期設定
	{
	 { { -0.5, +0.5, 0 }, { 1, 1, 1, 1 },{0,0}  },
	 { { +0.5, +0.5, 0 }, { 1, 0, 0, 1 },{1,0}  },
	 { { -0.5, -0.5, 0 }, { 0, 1, 0, 1 },{0,1}  },
	 { { +0.5, -0.5, 0 }, { 0, 0, 1, 1 },{1,1}  },
	};
	vertex vertices_for_draw[] //頂点情報のセット初期設定
	{
		vertices[0],
		vertices[1],
		vertices[2],
		vertices[2],
		vertices[1],
		vertices[3],
	};

	//②入力レイアウト要素の追加

	D3D11_INPUT_ELEMENT_DESC input_element_desc[]//リソースデータ形式
	{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, //色ではなくチャンネル
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};


}

Sprite_Batch::Sprite_Batch(ID3D11Device* device, const wchar_t* filename, size_t max_sprites) : max_vertices(max_sprites * 6)
{
	//②頂点バッファオブジェクトの生成
	HRESULT hr{ S_OK }; //エラー内容？ 現在は成功

	std::unique_ptr<vertex[]> vertices{ std::make_unique<vertex[]>(max_vertices) }; //スマートポインターでverticesを作成make_uniqueはスマートポインターの補助関数


	D3D11_BUFFER_DESC buffer_desc{};//バッファー
	buffer_desc.ByteWidth = sizeof(vertex) * max_vertices; //バッファーのサイズ
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC; //GPUによる読み取りと書き込み
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;//バッファーを頂点バッファーとして使う（割り当てる）
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //CPU使用(リソース)の許可あり
	buffer_desc.MiscFlags = 0; //その他のリソース？
	buffer_desc.StructureByteStride = 0; //バッファーの構造化なし　バッファーの構造化サイズ

	D3D11_SUBRESOURCE_DATA subresource_data{};//初期化データ
	subresource_data.pSysMem = vertices.get(); //初期化データのポインター
	subresource_data.SysMemPitch = 0; //テクスチャの 1 行の先頭から次の行までの距離
	subresource_data.SysMemSlicePitch = 0;//1 つの深度レベルの先頭から次の深さレベルまでの距離 (バイト単位)
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.GetAddressOf()); //バッファーを生成 (頂点バッファー、インデックス バッファー、またはシェーダー定数バッファー) 
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//頂点シェーダーオブジェクトの生成

	Texture* texture{};


	const char* cso_name{ "sprite_vs.cso" }; //シェーダーの呼び込みのための文字列


	hr = texture->create_vs_from_cso(device, cso_name, vertex_shader.ReleaseAndGetAddressOf(),
		input_layout.ReleaseAndGetAddressOf(), input_element_desc, _countof(input_element_desc));

	//FILE* fp{};
	//fopen_s(&fp, cso_name, "rb"); //ファイルを開く
	//_ASSERT_EXPR_A(fp, "CSO File not found");

	//fseek(fp, 0, SEEK_END);
	//long cso_sz{ ftell(fp) }; //ファイルの座標
	//fseek(fp, 0, SEEK_SET);

	//std::unique_ptr<unsigned char[]> cso_data{ std::make_unique<unsigned char[]>(cso_sz) }; //ユニークポインター内部データを読み込み
	//fread(cso_data.get(), cso_sz, 1, fp); //データを読み込み
	//fclose(fp); //閉じる

	//HRESULT hr{ S_OK }; //エラー内容？ 現在は成功

	//hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, &vertex_shader);
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr)); //合ってるかどうか

	//入力レイアウトオブジェクトの生成


	///*HRESULT */ hr = device->CreateInputLayout(input_element_desc, _countof(input_element_desc),
	//	cso_data.get(), cso_sz, &input_layout); /*頂点シェーダーのデータを橋渡しするもの
	//頂点シェーダーが変われば再度渡さないといけない*/
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//ピクセルシェーダーオブジェクトの生成

	/*画像を読み込みピクセルに分けて描写する*/

	/*const char**/ cso_name = { "sprite_ps.cso" }; //シェーダーの使用

	hr = texture->create_ps_from_cso(device, cso_name, pixel_shader.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//FILE* fp{};
	//fopen_s(&fp, cso_name, "rb"); //データ確認
	//_ASSERT_EXPR_A(fp, "CSO File not found");

	//fseek(fp, 0, SEEK_END);//ファイルの移動
	//cso_sz = { ftell(fp) }; //読み込み位置の移動
	//fseek(fp, 0, SEEK_SET); //ファイルの移動

	///*std::unique_ptr<unsigned char[]>*/ cso_data = { std::make_unique<unsigned char[]>(cso_sz) };
	//fread(cso_data.get(), cso_sz, 1, fp); //構造体ポインター
	//fclose(fp);//閉じる

	///*HRESULT*/ hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, &pixel_shader); //ピクセル(点の部分) シェーダーを作成
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//create_ps_from_cso(device.Get(), cso_name, pixel_shader.ReleaseAndGetAddressOf());

	//③画像ファイルのロードとシェーダーリソースビューオブジェクト(ID3D11ShaderResourceView)の生成

	//ID3D11Resource* resource{}; //メモリ内のバッファーやテクスチャーなど、グラフィックスデータを格納するためのオブジェクト(GPU用？)
	//hr = DirectX::CreateWICTextureFromFile(device, filename, &resource, &shader_resource_view); //
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//resource->Release();

	//④テクスチャ情報(D3D11_TEXTURE2D_DESC)の取得

	//ID3D11Texture2D* texture2d{};
	//hr = resource->QueryInterface<ID3D11Texture2D>(&texture2d); //データのキャスト（変換）
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));	//テクスチャ リソースのプロパティを取得します。
	//texture2d->GetDesc(&texture2d_desc); //情報の取得
	//texture2d->Release();

	//③と④をまとめてやる
	hr = texture->load_texture_from_file(device, filename,
	shader_resource_view.ReleaseAndGetAddressOf(), &texture2d_desc);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));	//テクスチャ リソースのプロパティを取得します。

}

Sprite_Batch::~Sprite_Batch()
{
	//vertex_shader->Release(); //頂点シェーダーのインターフェース（他のシステムと連結するため？）管理システム
	//pixel_shader->Release(); //ピクセルシェーダー（ステージを制御する管理システム）
	//input_layout->Release(); //入力レイアウト（アッセンブラーの送り込みの定義）
	//vertex_buffer->Release(); //バッファー（頂点シェーダーのデータを格納）
	//shader_resource_view->Release();//GPUを使用してシェーダーバッファなどの情報をくれる

}

namespace
{

	inline  void rotate (float& x, float& y, float cx, float cy, float angle)
	{
		x -= cx;
		y -= cy;

		float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
		float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
		float tx{ x }, ty{ y };
		x = cos * tx + -sin * ty;
		y = sin * tx + cos * ty;

		x += cx;
		y += cy;

	}
}

void Sprite_Batch::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy, float dw, float dh,
	float r, float g, float b, float a // 矩形のサイズ（スクリーン座標系）
	, float angle

)
{


	//①テクスチャ座標を頂点バッファにセットする
		//※今回はテクスチャ全体を表示する
	//①スクリーン（ビューポート）のサイズを取得する
	D3D11_VIEWPORT viewport{};
	UINT num_viewports{ 1 };
	immediate_context->RSGetViewports(&num_viewports, &viewport);


	//3の内容
	//②render メンバ関数の引数（dx, dy, dw, dh）から矩形の各頂点の位置（スクリーン座標系）を計算する
	// (x0, y0) *----* (x1, y1) 
	// |   /|
	// |  / |
	// | /  |
	// |/   |
	// (x2, y2) *----* (x3, y3) 

	// left-top
	float x0{ dx };
	float y0{ dy };
	// right-top
	float x1{ dx + dw };
	float y1{ dy };
	// left-bottom
	float x2{ dx };
	float y2{ dy + dh };
	// right-bottom
	float x3{ dx + dw };
	float y3{ dy + dh };


	auto rotate = [](float& x, float& y, float cx, float cy, float angle)
	{
		x -= cx;
		y -= cy;

		float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
		float sin{ sinf(DirectX::XMConvertToRadians(angle)) };
		float tx{ x }, ty{ y };
		x = cos * tx + -sin * ty;
		y = sin * tx + cos * ty;

		x += cx;
		y += cy;
	};
	//回転の中心を矩形の中心点にした場合
	float cx = dx + dw * 0.5f;
	float cy = dy + dh * 0.5f;
	rotate(x0, y0, cx, cy, angle);
	rotate(x1, y1, cx, cy, angle);
	rotate(x2, y2, cx, cy, angle);
	rotate(x3, y3, cx, cy, angle);

	//③スクリーン座標系から NDC への座標変換をおこなう
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;


	//①テクスチャ座標を頂点バッファにセットする
	//※今回はテクスチャ全体を表示する
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	//GPUのメモリの書き込みのために一時的に拒否やつ
	hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) }; //データから頂点数を割り出し
	if (vertices != nullptr)
	{
		vertices[0].position = { x0, y0 , 0 };
		vertices[1].position = { x1, y1 , 0 };
		vertices[2].position = { x2, y2 , 0 };
		vertices[3].position = { x3, y3 , 0 };
		//vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { 1, 1, 1, 1 };
		vertices[0].color = { 1,1,1,1 };
		vertices[1].color = { 0,0,1,1 };
		vertices[2].color = { 1,0,1,1 };
		vertices[3].color = { 1,1,0,1 };

		//vertices[0].texcoord = { 0, 0 };
		//vertices[1].texcoord = { 1, 0 };
		//vertices[2].texcoord = { 0, 1 };
		//vertices[3].texcoord = { 1, 1 };
		vertices[0].texcoord = { 0, 0 };
		vertices[1].texcoord = { 1, 0 };
		vertices[2].texcoord = { 0, 1 };
		vertices[3].texcoord = { 1, 1 };


	}


	immediate_context->Unmap(vertex_buffer.Get(), 0);//マップの解除（GPUのメモリの分離）

	//④計算結果で頂点バッファオブジェクトを更新する

	//HRESULT hr{ S_OK };
	//D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	////GPUのメモリの書き込みのために一時的に拒否やつ
	//hr = immediate_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
	//if (vertices != nullptr)
	//{

	//	//回転の中心を矩形の中心点にした場合
	//	float cx = dx + dw * 0.5f;
	//	float cy = dy + dh * 0.5f;
	//	rotate(x0, y0, cx, cy, angle);
	//	rotate(x1, y1, cx, cy, angle);
	//	rotate(x2, y2, cx, cy, angle);
	//	rotate(x3, y3, cx, cy, angle);

	//	vertices[0].position = { x0, y0 , 0 };
	//	vertices[1].position = { x1, y1 , 0 };
	//	vertices[2].position = { x2, y2 , 0 };
	//	vertices[3].position = { x3, y3 , 0 };
	//	 vertices[4].position = { x1, y1 , 0 };
	//	 vertices[5].position = { x3, y3 , 0 };






	//}


	//immediate_context->Unmap(vertex_buffer, 0);//マップの解除（GPUのメモリの分離）

	//2の内容
	//①頂点バッファーのバインド
	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);

	//②プリミティブタイプおよびデータの順序に関する情報のバインド
	//三角形のつながり方
	//immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST/*(頂点がそろってなければ繋がらない)*//*D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP頂点を無理やりそろわせるやつ*/);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//③入力レイアウトオブジェクトのバインド
	immediate_context->IASetInputLayout(input_layout.Get());
	//④シェーダーのバインド
	//immediate_context->VSSetShader(vertex_shader, nullptr, 0);
	//immediate_context->PSSetShader(pixel_shader, nullptr, 0);

	//②シェーダー リソースのバインド
	//immediate_context->PSSetShaderResources(0, 1, &shader_resource_view);

	//⑤プリミティブの描画
	immediate_context->Draw(4, 0);
}


void Sprite_Batch::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy, float dw, float dh,
	float r, float g, float b, float a // 矩形のサイズ（スクリーン座標系）
	, float angle
	, float sx, float sy, float sw, float sh
)
{

	//①テクスチャ座標を頂点バッファにセットする
		//※今回はテクスチャ全体を表示する
	//①スクリーン（ビューポート）のサイズを取得する
	D3D11_VIEWPORT viewport{};
	UINT num_viewports{ 1 };
	immediate_context->RSGetViewports(&num_viewports, &viewport); //ラスタライザーの設定

	// left-top
	float x0{ dx };
	float y0{ dy };
	// right-top
	float x1{ dx + dw };
	float y1{ dy };
	// left-bottom
	float x2{ dx };
	float y2{ dy + dh };
	// right-bottom
	float x3{ dx + dw };
	float y3{ dy + dh };



	//③スクリーン座標系から NDC への座標変換をおこなう
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;

	float u0{ sx / texture2d_desc.Width };
	float v0{ sy / texture2d_desc.Height };
	float u1{ (sx + sw) / texture2d_desc.Width };
	float v1{ (sy + sh) / texture2d_desc.Height };

	vertices.push_back({ { x0, y0 , 0 }, { r, g, b, a }, { u0, v0 } });
	vertices.push_back({ { x1, y1 , 0 }, { r, g, b, a }, { u1, v0 } });
	vertices.push_back({ { x2, y2 , 0 }, { r, g, b, a }, { u0, v1 } });
	vertices.push_back({ { x2, y2 , 0 }, { r, g, b, a }, { u0, v1 } });
	vertices.push_back({ { x1, y1 , 0 }, { r, g, b, a }, { u1, v0 } });
	vertices.push_back({ { x3, y3 , 0 }, { r, g, b, a }, { u1, v1 } });


	//rasterizer_state->Release();
}

void Sprite_Batch::render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh
)
{

	//①テクスチャ座標を頂点バッファにセットする
		//※今回はテクスチャ全体を表示する
	//①スクリーン（ビューポート）のサイズを取得する
	D3D11_VIEWPORT viewport{};
	UINT num_viewports{ 1 };
	immediate_context->RSGetViewports(&num_viewports, &viewport); //ラスタライザーの設定

	// left-top
	float x0{ dx };
	float y0{ dy };
	// right-top
	float x1{ dx + dw };
	float y1{ dy };
	// left-bottom
	float x2{ dx };
	float y2{ dy + dh };
	// right-bottom
	float x3{ dx + dw };
	float y3{ dy + dh };



	//③スクリーン座標系から NDC への座標変換をおこなう
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;

	vertices.push_back({ { x0, y0 , 0 }, { 1, 1, 1, 1 }, { 0, 0 } });
	vertices.push_back({ { x1, y1 , 0 }, { 1, 1, 1, 1 }, { 1, 0 } });
	vertices.push_back({ { x2, y2 , 0 }, { 1, 1, 1, 1 }, { 0, 1 } });
	vertices.push_back({ { x2, y2 , 0 }, { 1, 1, 1, 1 }, { 0, 1 } });
	vertices.push_back({ { x1, y1 , 0 }, { 1, 1, 1, 1 }, { 1, 0 } });
	vertices.push_back({ { x3, y3 , 0 }, { 1, 1, 1, 1 }, { 1, 1} });
	//vertices.push_back({ { x3, y3 , 0 }, { 1, 1, 1, 1 }, { 1, 1} });
	//vertices.push_back({ { x3, y3 , 0 }, { 1, 1, 1, 1 }, { 1, 1} });


}


void Sprite_Batch::Begin(ID3D11DeviceContext* immediate_context)
{
	vertices.clear();
	//④シェーダーのバインド
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0); //VSのバインド
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0); //PSのバインド
	immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());	//シェーダー リソースのバインド

}

void Sprite_Batch::End(ID3D11DeviceContext* immediate_context)
{
	//①テクスチャ座標を頂点バッファにセットする
//※今回はテクスチャ全体を表示する
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	//GPUのメモリの書き込みのために一時的に拒否やつ
	hr = immediate_context->Map(vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	size_t vertex_count = vertices.size();
	_ASSERT_EXPR(max_vertices >= vertex_count, "Buffer overflow");
	 vertex * data{ reinterpret_cast<vertex*>(mapped_subresource.pData) };//データから頂点数を割り出し
	if (data != nullptr)
	{
		const vertex * p = vertices.data();
		memcpy_s(data, max_vertices * sizeof(vertex), p, vertex_count * sizeof(vertex));
	}
	//④計算結果で頂点バッファオブジェクトを更新する

	immediate_context->Unmap(vertex_buffer.Get(), 0);//マップの解除（GPUのメモリの分離）



//2の内容
//①頂点バッファーのバインド
	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	//②プリミティブタイプおよびデータの順序に関する情報のバインド
	//線のつながり方
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST/*(頂点がそろってなければ繋がらない)*//*D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP頂点を無理やりそろわせるやつ*/);
	//immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//③入力レイアウトオブジェクトのバインド
	immediate_context->IASetInputLayout(input_layout.Get());
	//④シェーダーのバインド
	//immediate_context->VSSetShader(vertex_shader, nullptr, 0);
	//immediate_context->PSSetShader(pixel_shader, nullptr, 0);

	//②シェーダー リソースのバインド
	//immediate_context->PSSetShaderResources(0, 1, &shader_resource_view);

	//ID3D11RasterizerState* rasterizer_state{};

	//immediate_context->RSSetState(rasterizer_state);


	//⑤プリミティブの描画
	immediate_context->Draw(static_cast<UINT>(vertex_count), 0);

}


