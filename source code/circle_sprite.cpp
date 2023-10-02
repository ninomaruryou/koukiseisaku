#include <list>

#include "circle_sprite.h"
#include "misc.h"
#include <sstream>

#include "vertex.h"

namespace
{

	//vertex vertices[] //頂点情報のセット初期設定
	//{
	// { { -0.5, +0.5, 0 }, { 1, 1, 1, 1 } },
	// { { +0.5, +0.5, 0 }, { 1, 0, 0, 1 } },
	// { { -0.5, -0.5, 0 }, { 0, 1, 0, 1 } },
	// { { +0.5, -0.5, 0 }, { 0, 0, 1, 1 } },
	//};
	//vertex vertices_for_draw[] //頂点情報のセット初期設定
	//{
	//	vertices[0],
	//	vertices[1],
	//	vertices[2],
	//	vertices[2],
	//	vertices[1],
	//	vertices[3],
	//};

	//std::list<vertex*> vertexList_;


	//D3D11_BUFFER_DESC buffer_desc = {};


}
//バッファーとはデータの保存などを手助けするもの（緩衝体？）
CircleSprite::CircleSprite(ID3D11Device* device)
{
	//②頂点バッファオブジェクトの生成
	HRESULT hr{ S_OK }; //エラー内容？ 現在は成功


	UINT vertex_count = NUM_CIRCLE_POINTS + 1;

	D3D11_BUFFER_DESC buffer_desc{};//バッファー
	buffer_desc.ByteWidth = sizeof(vertex) * vertex_count; //バッファーのサイズ
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC; //GPUによる読み取りと書き込み
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;//バッファーを頂点バッファーとして使う（割り当てる）
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //CPU使用(リソース)の許可あり
	buffer_desc.MiscFlags = 0; //その他のリソース？
	buffer_desc.StructureByteStride = 0; //バッファーの構造化なし　バッファーの構造化サイズ
	D3D11_SUBRESOURCE_DATA subresource_data{};//サブリソース
	subresource_data.pSysMem = nullptr; //初期化データのポインター
	subresource_data.SysMemPitch = 0; //テクスチャの 1 行の先頭から次の行までの距離
	subresource_data.SysMemSlicePitch = 0;//1 つの深度レベルの先頭から次の深さレベルまでの距離 (バイト単位)
	hr = device->CreateBuffer(&buffer_desc, nullptr, &vertex_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	{
		//インデックスバッファ生成
		D3D11_BUFFER_DESC buffer_desc{};//バッファー
		buffer_desc.ByteWidth = sizeof(UINT) * NUM_CIRCLE_POINTS * 3; //バッファーのサイズ
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC; //GPUによる読み取りと書き込み
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;//バッファーを頂点バッファーとして使う（割り当てる）
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; //CPU使用(リソース)の許可あり
		buffer_desc.MiscFlags = 0; //その他のリソース？
		buffer_desc.StructureByteStride = 0; //バッファーの構造化なし　バッファーの構造化サイズ
		D3D11_SUBRESOURCE_DATA subresource_data{};//サブリソース
		subresource_data.pSysMem = nullptr; //初期化データのポインター
		subresource_data.SysMemPitch = 0; //テクスチャの 1 行の先頭から次の行までの距離
		subresource_data.SysMemSlicePitch = 0;//1 つの深度レベルの先頭から次の深さレベルまでの距離 (バイト単位)
		hr = device->CreateBuffer(&buffer_desc, nullptr, &index_buffer);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	//頂点シェーダーオブジェクトの生成


	const char* cso_name{ "sprite_vs.cso" };

	FILE* fp{};
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data{ std::make_unique<unsigned char[]>(cso_sz) };
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	//HRESULT hr{ S_OK }; //エラー内容？ 現在は成功

	hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, &vertex_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr)); //合ってるかどうか

	//入力レイアウトオブジェクトの生成

	D3D11_INPUT_ELEMENT_DESC input_element_desc[] //画像データ
	{
	   { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};


	/*HRESULT */ hr = device->CreateInputLayout(input_element_desc, _countof(input_element_desc),
		cso_data.get(), cso_sz, &input_layout); 
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//ピクセルシェーダーオブジェクトの生成

	/*const char**/ cso_name = { "sprite_ps.cso" };

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

	//int centerX = 500
	//	, centerY = 500
	//	,
	//	radius = 500;

	// 円を描く
	//for (int i = 0; i <= 360; i++) {
	//	int angle = i * DirectX::XM_PI / 180.0;
	//	float x = centerX + radius * cos(angle);
	//	float y = centerY + radius * sin(angle);

	//	vertex v = { { x, y, 0 }, { 1, 1, 1, 1 } };

	//	vertexList_.emplace_back(&v);

	//}

}

CircleSprite::~CircleSprite()
{

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


	};

}


void CircleSprite::render(ID3D11DeviceContext* immediate_context,
	float dx, float dy, float dw, float dh,
	float r, float g, float b, float a // 矩形のサイズ（スクリーン座標系）
	, float angle
)
{

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

	//④計算結果で頂点バッファオブジェクトを更新する
	 
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
	//GPUのメモリの書き込みのために一時的に拒否やつ
	hr = immediate_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource); 
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	vertex vertices[NUM_CIRCLE_POINTS + 1]; //頂点の変数配列

	vertices[NUM_CIRCLE_POINTS].position.x = 0;
	vertices[NUM_CIRCLE_POINTS].position.y = 0;
	vertices[NUM_CIRCLE_POINTS].position.z = 0;
	vertices[NUM_CIRCLE_POINTS].color = { 1,1,1,1 };

	for(UINT i = 0;i<NUM_CIRCLE_POINTS;++i) //点の場所指定
	{
		//円の点（angleのようなもの）
		float theta = (float)i / (NUM_CIRCLE_POINTS);
		theta *= DirectX::XM_PI;
		;
		float x = cosf(theta);
		float y = sinf(theta);


		vertices[i].position = { x,y,0 };
		vertices[i].color = { 1,1,1,1 };
	}


	std::memcpy(mapped_subresource.pData, vertices, sizeof(vertices)); //バイト数コピー


	//vertex* vertices{ reinterpret_cast<vertex*>(mapped_subresource.pData) };
	//if (vertices != nullptr)
	//{
	//	vertices[0].position = { x0, y0 , 0 };
	//	vertices[1].position = { x1, y1 , 0 };
	//	vertices[2].position = { x2, y2 , 0 };
	//	vertices[3].position = { x2, y2 , 0 };
	//	vertices[4].position = { x1, y1 , 0 };
	//	vertices[5].position = { x3, y3 , 0 };




	//	vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color =
	//		vertices[4].color = vertices[5].color
	//		= { 1.0f,1.0f,1.0f,1.0f };//オブジェの色


	//}



	////回転の中心を矩形の中心点にした場合
	//float cx = dx + dw * 0.5f;
	//float cy = dy + dh * 0.5f;
	//rotate(x0, y0, cx, cy, angle);
	//rotate(x1, y1, cx, cy, angle);
	//rotate(x2, y2, cx, cy, angle);
	//rotate(x3, y3, cx, cy, angle);

	immediate_context->Unmap(vertex_buffer, 0);//マップの解除（GPUのメモリの分離）


	{//インデックスバッファに書き込み


		HRESULT hr{ S_OK };
		D3D11_MAPPED_SUBRESOURCE mapped_subresource{};
		//GPUのメモリの書き込みのために一時的に拒否やつ
		hr = immediate_context->Map(index_buffer,0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


		UINT indices[NUM_CIRCLE_POINTS * 3]; //円の三角形のデータ変数

		for (UINT i = 0; i < NUM_CIRCLE_POINTS; ++i)//円の三角形の順番作成
		{
			UINT offset = i * 3; 
			indices[offset] = i + 1;
			if(i == NUM_CIRCLE_POINTS - 1)
			{
				indices[offset] = 0; //中心点
			}
			indices[offset + 1] = i;//三角形の周り
			indices[offset + 2] = NUM_CIRCLE_POINTS;//三角形の中心点
		}

		std::memcpy(mapped_subresource.pData, indices, sizeof(indices));

		immediate_context->Unmap(index_buffer, 0);//マップの解除（GPUのメモリの分離）
	}

	//2の内容  //バインドとは関連づけるするもの
	//①頂点バッファーのバインド　
	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset); //頂点バッファー入力アセンブラー
	//インデックスバッファセット
	immediate_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);

	//②プリミティブタイプおよびデータの順序に関する情報のバインド
	//三角形のつながり方
	immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST/*(頂点がそろってなければ繋がらない)*//*D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP頂点を無理やりそろわせるやつ*/);
	//③入力レイアウトオブジェクトのバインド
	immediate_context->IASetInputLayout(input_layout);
	//④シェーダーのバインド
	immediate_context->VSSetShader(vertex_shader, nullptr, 0);
	immediate_context->PSSetShader(pixel_shader, nullptr, 0);
	//⑤プリミティブの描画
	//immediate_context->Draw(NUM_CIRCLE_POINTS + 1, 0);
	immediate_context->DrawIndexed(NUM_CIRCLE_POINTS * 3, 0, 0);//インデックスの描画


}
