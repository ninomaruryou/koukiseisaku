#pragma once


#include <d3d11.h>
#include <directxmath.h>
#include <string>

#include <WICTextureLoader.h>

class SpritrHLSI
{

	//変数はすべてインターフェースのようなもの？
	ID3D11VertexShader* vertex_shader; //頂点シェーダーのインターフェース（他のシステムと連結するため？）管理システム
	ID3D11PixelShader* pixel_shader; //ピクセルシェーダー（ステージを制御する管理システム）
	ID3D11InputLayout* input_layout; //入力レイアウト（アッセンブラーの送り込みの定義）
	ID3D11Buffer* vertex_buffer; //バッファー（頂点シェーダーのデータを格納）

	ID3D11ShaderResourceView* shader_resource_view;//GPUを使用してシェーダーバッファなどの情報をくれる
	D3D11_TEXTURE2D_DESC texture2d_desc; //2D テクスチャのデータ保存

	//バッファーとはデータの保存などを手助けするものもしくはGPU専用のメモリ？（緩衝体？）


public:
	//②render メンバ関数の変更
	void render(ID3D11DeviceContext* immediate_context,
		float dx, float dy, float dw, float dh,
		float r, float g, float b, float a // 矩形のサイズ（スクリーン座標系）
		,float angle
	);
	void render(ID3D11DeviceContext* immediate_context,
		float dx, float dy, float dw, float dh,
		float r, float g, float b, float a // 矩形のサイズ（スクリーン座標系）
		,float angle
		, float sx, float sy, float sw, float sh //描写位置 サイズ
	);

	void textout(ID3D11DeviceContext* immediate_context, std::string s,
		float x, float y, float w, float h, float r, float g, float b, float a);

	/*デストラクタではすべての COM オブジェクトを解放する*/

	SpritrHLSI(ID3D11Device* device,const wchar_t* filename); //wchar_t(専用文字パス)
	~SpritrHLSI();

};

