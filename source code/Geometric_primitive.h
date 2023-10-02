#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <wrl/client.h>

#define BOX (1);
#define CIRCLE (0);
#define SPHERE (0)
class Geometric_Primitive
 {
  public:
 	 struct vertex
 		 {
 		 DirectX::XMFLOAT3 position;
 		 DirectX::XMFLOAT3 normal;
 		};
 	 struct constants
 		{
 		 DirectX::XMFLOAT4X4 world;
 		  DirectX::XMFLOAT4 material_color;
 		 
 	 };

#if BOX
     vertex vertices[36]; //頂点位置と法線
     uint32_t indices[36]{}; //描画順番
#endif
#if CIRCLE
     static  constexpr UINT NUM_CIRCLE_POINTS = 300;//円の頂点整数定数
     vertex vertices[((NUM_CIRCLE_POINTS  * 2) + 2) * 2]; //頂点位置と法線
     uint32_t indices[((NUM_CIRCLE_POINTS * 3) * 2) * 2]{}; //描画順番

#endif
#if SPHERE

    static  constexpr UINT m_iUMax = 24;
    static  constexpr UINT m_iVMax = 4;

     static  constexpr UINT NUM_CIRCLE_POINTS = 4;//円の頂点整数定数

     vertex vertices[m_iUMax * (m_iVMax + 1)]; //頂点位置と法線
     uint32_t indices[2 * m_iVMax * (m_iUMax + 1)]{}; //描画順番

#endif


private://スプライトと大差ない
	 Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	 Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
	
	 Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	 Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	 Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	 Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
	
public:
	Geometric_Primitive(ID3D11Device * device);
	virtual ~Geometric_Primitive() = default;

	void render(ID3D11DeviceContext * immediate_context,
 	  const DirectX::XMFLOAT4X4 & world, const DirectX::XMFLOAT4 & material_color);
 
	void create_com_buffers(ID3D11Device * device, vertex * vertices, size_t vertex_count,
 			  uint32_t * indices, size_t index_count);
 				 
 };
