#pragma once

#include <d3d11.h>
#include <directxmath.h>

class CircleSprite
{
public:


	ID3D11VertexShader* vertex_shader; //頂点シェーダーのインターフェース（他のシステムと連結するため？）管理システム
	ID3D11PixelShader* pixel_shader; //ピクセルシェーダー以下略（ステージを制御する管理システム）
	ID3D11InputLayout* input_layout; //入力レイアウトいかｒｙ（アッセンブラーの送り込みの定義）
	ID3D11Buffer* vertex_buffer; //バッファーいｋ（頂点シェーダーのデータを格納）
	ID3D11Buffer* index_buffer; //インデックスバッファ（インデックスシェーダーのデータを格納）

	void render(ID3D11DeviceContext* immediate_context,
		float dx, float dy, float dw, float dh,
		float r, float g, float b, float a // 矩形のサイズ（スクリーン座標系）
		, float angle);

	/*デストラクタではすべての COM オブジェクトを解放する*/

	CircleSprite(ID3D11Device* device);
	~CircleSprite();


	static constexpr UINT NUM_CIRCLE_POINTS = 600; //円の頂点整数定数
};



//struct vertex //頂点の設定
//{
//	DirectX::XMFLOAT3 position;
//	DirectX::XMFLOAT4 color;
//};
