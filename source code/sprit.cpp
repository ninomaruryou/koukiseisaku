#include <list>

#include "sprite.h"
#include "misc.h"
#include <sstream>


#include "vertex.h"
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


SpritrHLSI::SpritrHLSI(ID3D11Device* device, const wchar_t* filename)
{
	//②頂点バッファオブジェクトの生成
	HRESULT hr{ S_OK }; //エラー内容？ 現在は成功


	D3D11_BUFFER_DESC buffer_desc{};//バッファー
	buffer_desc.ByteWidth = sizeof(vertices_for_draw); //バッファーのサイズ
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC; //GPUによる読み取りと書き込み
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;//バッファーを頂点バッファーとして使う（割り当てる）
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //CPU使用(リソース)の許可あり
	buffer_desc.MiscFlags = 0; //その他のリソース？
	buffer_desc.StructureByteStride = 0; //バッファーの構造化なし　バッファーの構造化サイズ

	D3D11_SUBRESOURCE_DATA subresource_data{};//初期化データ
	subresource_data.pSysMem = vertices_for_draw; //初期化データのポインター
	subresource_data.SysMemPitch = 0; //テクスチャの 1 行の先頭から次の行までの距離
	subresource_data.SysMemSlicePitch = 0;//1 つの深度レベルの先頭から次の深さレベルまでの距離 (バイト単位)
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, &vertex_buffer); //バッファー (頂点バッファー、インデックス バッファー、またはシェーダー定数バッファー) 
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//頂点シェーダーオブジェクトの生成


	const char* cso_name{ "sprite_vs.cso" }; //シェーダーの呼び込みのための文字列

	FILE* fp{};
	fopen_s(&fp, cso_name, "rb"); //ファイルを開く
	_ASSERT_EXPR_A(fp, "CSO File not found"); 

	fseek(fp, 0, SEEK_END);
	long cso_sz{ ftell(fp) }; //ファイルの座標
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data{ std::make_unique<unsigned char[]>(cso_sz) }; //ユニークポインター内部データを読み込み
	fread(cso_data.get(), cso_sz, 1, fp); //データを読み込み
	fclose(fp); //閉じる

	//HRESULT hr{ S_OK }; //エラー内容？ 現在は成功

	hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, &vertex_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr)); //合ってるかどうか

	//入力レイアウトオブジェクトの生成


	/*HRESULT */ hr = device->CreateInputLayout(input_element_desc, _countof(input_element_desc),
		cso_data.get(), cso_sz, &input_layout); /*頂点シェーダーのデータを橋渡しするもの
	頂点シェーダーが変われば再度渡さないといけない*/
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//ピクセルシェーダーオブジェクトの生成

	/*画像を読み込みピクセルに分けて描写する*/

	/*const char**/ cso_name = { "sprite_ps.cso" }; //シェーダーの使用

	//FILE* fp{};
	fopen_s(&fp, cso_name, "rb"); //データ確認
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);//ファイルの移動
	cso_sz = { ftell(fp) }; //読み込み位置の移動
	fseek(fp, 0, SEEK_SET); //ファイルの移動

	/*std::unique_ptr<unsigned char[]>*/ cso_data = { std::make_unique<unsigned char[]>(cso_sz) };
	fread(cso_data.get(), cso_sz, 1, fp); //構造体ポインター
	fclose(fp);//閉じる

	/*HRESULT*/ hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, &pixel_shader); //ピクセル(点の部分) シェーダーを作成
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//③画像ファイルのロードとシェーダーリソースビューオブジェクト(ID3D11ShaderResourceView)の生成

		 ID3D11Resource * resource{}; //メモリ内のバッファーやテクスチャーなど、グラフィックスデータを格納するためのオブジェクト(GPU用？)
	 hr = DirectX::CreateWICTextureFromFile(device, filename, &resource, &shader_resource_view); //
	 _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	 resource->Release();

	//④テクスチャ情報(D3D11_TEXTURE2D_DESC)の取得

	 ID3D11Texture2D* texture2d{};
	  hr = resource->QueryInterface<ID3D11Texture2D>(&texture2d); //データのキャスト（変換）
	  _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));	//テクスチャ リソースのプロパティを取得します。
	  texture2d->GetDesc(&texture2d_desc); //情報の取得
	  texture2d->Release();

	
}

SpritrHLSI::~SpritrHLSI()
{
	vertex_shader->Release(); //頂点シェーダーのインターフェース（他のシステムと連結するため？）管理システム
	 pixel_shader->Release(); //ピクセルシェーダー（ステージを制御する管理システム）
	input_layout->Release(); //入力レイアウト（アッセンブラーの送り込みの定義）
	 vertex_buffer->Release(); //バッファー（頂点シェーダーのデータを格納）
	shader_resource_view->Release();//GPUを使用してシェーダーバッファなどの情報をくれる

}

namespace
{

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

		//x *= 0.1f;
		//y *= 0.01f;
	};
}

void SpritrHLSI::render(ID3D11DeviceContext* immediate_context,
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
	hr = immediate_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
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


	immediate_context->Unmap(vertex_buffer, 0);//マップの解除（GPUのメモリの分離）

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
	immediate_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);

	//②プリミティブタイプおよびデータの順序に関する情報のバインド
	//三角形のつながり方
	//immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST/*(頂点がそろってなければ繋がらない)*//*D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP頂点を無理やりそろわせるやつ*/);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//③入力レイアウトオブジェクトのバインド
	immediate_context->IASetInputLayout(input_layout);
	//④シェーダーのバインド
	immediate_context->VSSetShader(vertex_shader, nullptr, 0);
	immediate_context->PSSetShader(pixel_shader, nullptr, 0);

	//②シェーダー リソースのバインド
	immediate_context->PSSetShaderResources(0, 1, &shader_resource_view);

	//⑤プリミティブの描画
	immediate_context->Draw(4, 0);
}


void SpritrHLSI::render(ID3D11DeviceContext* immediate_context,
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
	immediate_context->RSGetViewports(&num_viewports, &viewport);

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


	//①テクスチャ座標を頂点バッファにセットする
	//※今回はテクスチャ全体を表示する
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	//GPUのメモリの書き込みのために一時的に拒否やつ
	hr = immediate_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	float u0{ sx / texture2d_desc.Width };
	float v0{ sy / texture2d_desc.Height };
	float u1{ (sx + sw) / texture2d_desc.Width };
	float v1{ (sy + sh) / texture2d_desc.Height };


	vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) }; //データから頂点数を割り出し
	if (vertices != nullptr)
	{
		//回転の中心を矩形の中心点にした場合
		float cx = dx + dw * 0.5f;
		float cy = dy + dh * 0.5f;
		rotate(x0, y0, cx, cy, angle);
		rotate(x1, y1, cx, cy, angle);
		rotate(x2, y2, cx, cy, angle);
		rotate(x3, y3, cx, cy, angle);

		vertices[0].position = { x0, y0 , 0 };
		vertices[1].position = { x1, y1 , 0 };
		vertices[2].position = { x2, y2 , 0 };
		vertices[3].position = { x3, y3 , 0 };
		vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { 1, 1, 1, a };
		//vertices[0].color = { 1,1,1,0 };
		//vertices[1].color = { 0,0,1,0 };
		//vertices[2].color = { 1,0,1,0 };
		//vertices[3].color = { 1,1,0,1 };

		vertices[0].texcoord = { u0, v0 };
		vertices[1].texcoord = { u1, v0 };
		vertices[2].texcoord = { u0, v1 };
		vertices[3].texcoord = { u1, v1 };
	}
	//④計算結果で頂点バッファオブジェクトを更新する

	immediate_context->Unmap(vertex_buffer, 0);//マップの解除（GPUのメモリの分離）










//2の内容
//①頂点バッファーのバインド
	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
	//②プリミティブタイプおよびデータの順序に関する情報のバインド
	//線のつながり方
	//immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST/*(頂点がそろってなければ繋がらない)*//*D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP頂点を無理やりそろわせるやつ*/);
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//③入力レイアウトオブジェクトのバインド
	immediate_context->IASetInputLayout(input_layout);
	//④シェーダーのバインド
	immediate_context->VSSetShader(vertex_shader, nullptr, 0);
	immediate_context->PSSetShader(pixel_shader, nullptr, 0);

	//②シェーダー リソースのバインド
	immediate_context->PSSetShaderResources(0, 1, &shader_resource_view);

	//ID3D11RasterizerState* rasterizer_state{};

	//immediate_context->RSSetState(rasterizer_state);


	//⑤プリミティブの描画
	immediate_context->Draw(4, 0);

	//rasterizer_state->Release();
}

//文字描写処理
 void SpritrHLSI::textout(ID3D11DeviceContext* immediate_context, std::string s,
	 float x, float y, float w, float h, float r, float g, float b, float a)
{
	float sw = static_cast<float>(texture2d_desc.Width / 16);
	float sh = static_cast<float>(texture2d_desc.Height / 16);
	float carriage = 0;
	for (const char c : s)
	{
			render(immediate_context, x + carriage, y, w, h, r, g, b, a, 0,
			sw * (c & 0x0F), sh * (c >> 4), sw, sh);
			carriage += w;
	}
}
