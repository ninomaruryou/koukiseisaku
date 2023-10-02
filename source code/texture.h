#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <vector>
#include "framework.h"
//#include <WICTextureLoader.h>

#include "misc.h"
#include "../DirectXTK-jul2022/Inc/WICTextureLoader.h"
using namespace DirectX;

#include <wrl.h>
using namespace Microsoft::WRL;

#include <string>
#include <map>

static std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>> resources;

class Texture
{
public:
	Texture();
	~Texture(){}

	static  HRESULT load_texture_from_file(ID3D11Device* device, const wchar_t* filename,
		ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc);

	static  HRESULT create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader);

	static  HRESULT create_vs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader,
		ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements);

	static  HRESULT make_dummy_texture(ID3D11Device* device, ID3D11ShaderResourceView** shader_resource_view, DWORD value/*0xAABBGGRR*/, UINT dimension);

	//UNIT36
	static HRESULT load_texture_from_memory(ID3D11Device* device, const void* data, size_t size,
		ID3D11ShaderResourceView** shader_resource_view);
};


