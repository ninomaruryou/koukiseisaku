#include "texture.h"

#include "DDSTextureLoader.h"
//テクスチャの読み込み
HRESULT Texture::load_texture_from_file(ID3D11Device* device, const wchar_t* filename,
	ID3D11ShaderResourceView** shader_resource_view, D3D11_TEXTURE2D_DESC* texture2d_desc)
{
	HRESULT hr{ S_OK };
	ComPtr<ID3D11Resource> resource;

#if 0
	auto it = resources.find(filename); 
	if (it != resources.end())
	{
		*shader_resource_view = it->second.Get();
		(*shader_resource_view)->AddRef();//参照カウントを追加
		(*shader_resource_view)->GetResource(resource.GetAddressOf());//テクスチャのリソースの取得
	}
	else//ない場合
	{
		hr = CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shader_resource_view); //テクスチャの作成
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		resources.insert(std::make_pair(filename, *shader_resource_view));
	}

	ComPtr<ID3D11Texture2D> texture2d;
	hr = resource.Get()->QueryInterface<ID3D11Texture2D>(texture2d.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	texture2d->GetDesc(texture2d_desc);
#endif

	//UNIT31DDS ファイルをロードしシェーダーリソースビューを生成する
	std::filesystem::path dds_filename(filename);
	 dds_filename.replace_extension("dds");
	 if (std::filesystem::exists(dds_filename.c_str()))
	 {
		 Microsoft::WRL::ComPtr<ID3D11DeviceContext> immediate_context;
		 device->GetImmediateContext(immediate_context.GetAddressOf());
		 hr = DirectX::CreateDDSTextureFromFile(device, immediate_context.Get(), dds_filename.c_str(),
			 resource.GetAddressOf(), shader_resource_view);
		 _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	 }
	 else
	 {
		 hr = CreateWICTextureFromFile(device, filename, resource.GetAddressOf(), shader_resource_view);
		 _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		 resources.insert(std::make_pair(filename, *shader_resource_view));
	 }

	return hr;
}

void release_all_textures()
 {
	 resources.clear();
 }


Texture::Texture()
{

}

HRESULT Texture::create_vs_from_cso(ID3D11Device* device, const char* cso_name, ID3D11VertexShader** vertex_shader,
	ID3D11InputLayout** input_layout, D3D11_INPUT_ELEMENT_DESC* input_element_desc, UINT num_elements)
{
	FILE* fp{ nullptr };
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR_A(fp, "CSO File not found");

	fseek(fp, 0, SEEK_END);
	long cso_sz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data{std::make_unique<unsigned char[]>(cso_sz) };

	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);
	
	 HRESULT hr{ S_OK };
	hr = device->CreateVertexShader(cso_data.get(), cso_sz, nullptr, vertex_shader);//コンパイルされたシェーダーから頂点シェーダー オブジェクト
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	
	 if (input_layout)
	{
		hr = device->CreateInputLayout(input_element_desc, num_elements,
		 cso_data.get(), cso_sz, input_layout);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	
	 return hr;
}

HRESULT Texture::create_ps_from_cso(ID3D11Device* device, const char* cso_name, ID3D11PixelShader** pixel_shader)
{
	 FILE * fp{ nullptr };
	 fopen_s(&fp, cso_name, "rb");//データ確認
	 _ASSERT_EXPR_A(fp, "CSO File not found");
	
	 fseek(fp, 0, SEEK_END);//ファイルの移動
	 long cso_sz{ ftell(fp) }; //読み込み位置の移動
	 fseek(fp, 0, SEEK_SET);//ファイルの移動

	 std::unique_ptr<unsigned char[]> cso_data{std::make_unique<unsigned char[]>(cso_sz) };
	 fread(cso_data.get(), cso_sz, 1, fp);//構造体ポインター
	 fclose(fp);
	
	 HRESULT hr{ S_OK };
	 hr = device->CreatePixelShader(cso_data.get(), cso_sz, nullptr, pixel_shader); //ピクセル(点の部分) シェーダーを作成
	 _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	
	 return hr;
}

HRESULT Texture::make_dummy_texture(ID3D11Device* device, ID3D11ShaderResourceView** shader_resource_view, DWORD value/*0xAABBGGRR*/, UINT dimension)
{
	HRESULT hr{ S_OK };

	std::wstringstream keyname;
	keyname << std::setw(8) << std::setfill(L'0') << std::hex << std::uppercase << value << L"." << std::dec << dimension;
	std::map<std::wstring, ComPtr<ID3D11ShaderResourceView>>::iterator it = resources.find(keyname.str().c_str());
	if (it != resources.end())
	{
		*shader_resource_view = it->second.Get();
		(*shader_resource_view)->AddRef();
	}
	else
	{
		D3D11_TEXTURE2D_DESC texture2d_desc{};
		texture2d_desc.Width = dimension;
		texture2d_desc.Height = dimension;
		texture2d_desc.MipLevels = 1;
		texture2d_desc.ArraySize = 1;
		texture2d_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texture2d_desc.SampleDesc.Count = 1;
		texture2d_desc.SampleDesc.Quality = 0;
		texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
		texture2d_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texture2d_desc.CPUAccessFlags = 0;
		texture2d_desc.MiscFlags = 0;

		size_t texels = static_cast<size_t>(dimension) * dimension;
		std::unique_ptr<DWORD[]> sysmem{std::make_unique< DWORD[]>(texels) };
		for (size_t i = 0; i < texels; ++i)
		{
			sysmem[i] = value;
		}

		D3D11_SUBRESOURCE_DATA subresource_data{};
		subresource_data.pSysMem = sysmem.get();
		subresource_data.SysMemPitch = sizeof(DWORD) * dimension;
		subresource_data.SysMemSlicePitch = 0;

		ComPtr<ID3D11Texture2D> texture2d;
		hr = device->CreateTexture2D(&texture2d_desc, &subresource_data, &texture2d);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
		shader_resource_view_desc.Format = texture2d_desc.Format;
		shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shader_resource_view_desc.Texture2D.MipLevels = 1;

		hr = device->CreateShaderResourceView(texture2d.Get(), &shader_resource_view_desc, shader_resource_view);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		resources.insert(std::make_pair(keyname.str().c_str(), *shader_resource_view));
	}
	return hr;
}

HRESULT Texture::load_texture_from_memory(ID3D11Device* device, const void* data, size_t size,
	ID3D11ShaderResourceView** shader_resource_view)
{
	HRESULT hr{ S_OK };
	ComPtr<ID3D11Resource> resource;

	hr = CreateDDSTextureFromMemory(device, reinterpret_cast<const uint8_t*>(data),
		size, resource.GetAddressOf(), shader_resource_view);
	if (hr != S_OK)
	{
		hr = CreateWICTextureFromMemory(device, reinterpret_cast<const uint8_t*>(data),
			size, resource.GetAddressOf(), shader_resource_view);
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	return hr;
}
