#pragma once


#include <d3d11.h>
#include <directxmath.h>
#include <vector>
#include <wrl/client.h>
//#include "texture.h"

using namespace Microsoft::WRL;

struct vertex;

class Sprite_Batch
{
	//バッファーとはデータの保存などを手助けするものもしくはGPU専用のメモリ？（緩衝体？）

	//変数はすべてインターフェースのようなもの？
	ComPtr<ID3D11VertexShader> vertex_shader; //頂点シェーダーのインターフェース（他のシステムと連結するため？）管理システム
	ComPtr<ID3D11PixelShader> pixel_shader; //ピクセルシェーダー（ステージを制御する管理システム）
	ComPtr<ID3D11InputLayout> input_layout; //入力レイアウト（アッセンブラーの送り込みの定義）
	ComPtr<ID3D11Buffer> vertex_buffer; //バッファー（頂点シェーダーのデータを格納）

	ComPtr<ID3D11ShaderResourceView> shader_resource_view;//GPUを使用してシェーダーバッファなどの情報をくれる
	D3D11_TEXTURE2D_DESC texture2d_desc; //2D テクスチャのデータ保存


	const size_t max_vertices;
	std::vector<vertex> vertices;

public:

	void render(ID3D11DeviceContext* immediate_context,
		float dx, float dy, float dw, float dh,
		float r, float g, float b, float a // 矩形のサイズ（スクリーン座標系）
		, float angle
	);
	void render(ID3D11DeviceContext* immediate_context,
		float dx, float dy, float dw, float dh,
		float r, float g, float b, float a // 矩形のサイズ（スクリーン座標系）
		, float angle
		, float sx, float sy, float sw, float sh //描写位置 サイズ
	);

	void render(ID3D11DeviceContext* immediate_context, float dx, float dy, float dw, float dh);

	void Begin(ID3D11DeviceContext* immediate_context);

	void End(ID3D11DeviceContext* immediate_context);
	/*デストラクタではすべての COM オブジェクトを解放する*/

	Sprite_Batch(ID3D11Device* device, const wchar_t* filename, size_t max_sprites); //wchar_t(専用文字パス)
	~Sprite_Batch();

	//画像の変更

};

