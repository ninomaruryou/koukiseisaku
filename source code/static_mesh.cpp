
#include "static_mesh.h"

#include <fstream>

//#include <iosfwd>
#include "texture.h"

//#include <locale.h> 

//#include <iomanip>
using namespace DirectX;
//③ コンストラクタの OBJ ファイルパーサー部でテクスチャ座標とマテリアルファイル名を取得する

Static_Mesh::Static_Mesh(ID3D11Device* device, const wchar_t* obj_filename)
{
	std::vector<vertex> vertices;
	std::vector<uint32_t> indices;
	uint32_t current_index{ 0 };

	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT2> texcoords;
	std::vector<std::wstring> mtl_filenames;
	DirectX::XMFLOAT3 bounding_box_max = {};
	DirectX::XMFLOAT3 bounding_box_min = {};

	

	const wchar_t* objFileName = obj_filename;
	std::wifstream fin(objFileName); //なぜこれなのか<iosfwd>だとエラーが発生する#include <fstream>
	//wofstream fin(obj_filename); 

	_ASSERT_EXPR(fin, L"'OBJ file not found.");

	wchar_t command[256]; //文字列データ格納場所
	while (fin)
	{
		fin >> command;
		//std::wstring line;
		//std::getline(fin, line);

		if (0 == wcscmp(command, L"v"))//wcscmp文字列の比較
		{
			float x, y, z;
			fin >> x >> y >> z;
			positions.push_back({ x, y, z });//頂点位置
			fin.ignore(1024, L'\n');//\nは日本のみ海外だとバックスラッシュ

			/*
			 '\n'だと一括りで読み込まれる
			 どちらも違う
			*/
			//メモリ
			//char a = '\n';こっちは10
			//char i = '/n';これは12142

			//位置の最大値を更新する
			if (x > bounding_box_max.x)
				bounding_box_max.x = x;
			if (y > bounding_box_max.y)
				bounding_box_max.y = y;
			if (z > bounding_box_max.z)
				bounding_box_max.z = z;

			if (x < bounding_box_min.x)
				bounding_box_min.x = x;
			if (y < bounding_box_min.y)
				bounding_box_min.y = y;
			if (z < bounding_box_min.z)
				bounding_box_min.z = z;

		}
		else if (0 == wcscmp(command, L"vn")) //ほうせん？
		{
			float i, j, k;
			fin >> i >> j >> k;
			normals.push_back({ i, j, k });//法線
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"vt"))//描画場所
		{
			float u, v;
			fin >> u >> v;
			texcoords.push_back({ u, 1.0f - v });
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"f"))//描画順番
		{
			for (size_t i = 0; i < 3; i++)
			{
				vertex vertex;
				size_t v, vt, vn;

				fin >> v;
				vertex.position = positions.at(v - 1);
				if (L'/' == fin.peek())
				{
					fin.ignore(1);
					if (L'/' != fin.peek())
					{
						fin >> vt;
						vertex.texcoord = texcoords.at(vt - 1);
					}
					if (L'/' == fin.peek())
					{
						fin.ignore(1);
						fin >> vn;
						vertex.normal = normals.at(vn - 1);
					}
				}
				vertices.push_back(vertex);
				indices.push_back(current_index++);
			}
			fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"mtllib"))
		{
			 wchar_t mtllib[256];
			 fin >> mtllib;
			 mtl_filenames.push_back(mtllib);
			  fin.ignore(1024, L'\n');
		}
		else if (0 == wcscmp(command, L"usemtl"))
		{
			 wchar_t usemtl[MAX_PATH]{ 0 };
			 fin >> usemtl;
			 subsets.push_back({ usemtl, static_cast<uint32_t>(indices.size()), 0 });
		}
		else
		{
			fin.ignore(1024, L'\n');
		}

	}
	fin.close();

	bounding_box = std::make_unique<Bounding_Box>(device, bounding_box_max, bounding_box_min);
	
#if TEXTURE

	//④ MTL ファイルパーサー部の実装（MTL ファイルをロードし、テクスチャファイル名を取得）
	std::filesystem::path mtl_filename(obj_filename);
	mtl_filename.replace_filename(std::filesystem::path(mtl_filenames[0]).filename());
	
	 fin.open(mtl_filename);
	_ASSERT_EXPR(fin, L"'MTL file not found.");
	 //if (materials.size() == 0)
	 //{
		// for (const subset& subset : subsets)
		// {
		// 	materials.push_back({ subset.usemtl });
		// }
	 //}
	 while (fin)
	 {
		 fin >> command;
		 if (0 == wcscmp(command, L"map_Kd")) //ディフューズカラー
		 {
		 	fin.ignore();
		 	wchar_t map_Kd[256];
			fin >> map_Kd;
			
			std::filesystem::path path(obj_filename);
			path.replace_filename(std::filesystem::path(map_Kd).filename());
			//materials.texture_filename = path;
			//materials.rbegin()->texture_filename = path;
			materials.rbegin()->texture_filenames[0] = path;
			fin.ignore(1024, L'\n');
		 }
		 else if (0 == wcscmp(command, L"newmtl"))
		 {
			  fin.ignore();
			  wchar_t newmtl[256];
			  material material;
			  fin >> newmtl;
			  material.name = newmtl;
			  materials.push_back(material);
		 }
		 else if (0 == wcscmp(command, L"map_bump") || 0 == wcscmp(command, L"bump"))
		 {
			 fin.ignore();
			 wchar_t map_bump[256];
			 fin >> map_bump;
			 
			  std::filesystem::path path(obj_filename);
		  path.replace_filename(std::filesystem::path(map_bump).filename());
		  materials.rbegin()->texture_filenames[1] = path;
		  fin.ignore(1024, L'\n');
		 }
		 else if (0 == wcscmp(command, L"Kd"))
		 {
			  float r, g, b;
			  fin >> r >> g >> b;
			  materials.rbegin()->Kd = { r, g, b, 1 };
			  fin.ignore(1024, L'\n');
		 }
		 else
		 {
		 	fin.ignore(1024, L'\n');
		 }
	 }
	fin.close();

	if (materials.size() == 0)
	{
		 for (const subset& subset : subsets)
		 {
			 materials.push_back({ subset.usemtl });
		 }
	}

	std::vector<subset>::reverse_iterator iterator = subsets.rbegin();
	 iterator->index_count = static_cast<uint32_t>(indices.size()) - iterator->index_start;
	 for (iterator = subsets.rbegin() + 1; iterator != subsets.rend(); ++iterator)
	 {
	 	iterator->index_count = (iterator - 1)->index_start - iterator->index_start;
	 }

#endif


	create_com_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size());



	HRESULT hr{ S_OK };

	Texture texture;

	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
   { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
   { "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	texture.create_vs_from_cso(device, "static_mesh_vs.cso", vertex_shader.GetAddressOf(),
		input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	texture.create_ps_from_cso(device, "static_mesh_ps.cso", pixel_shader.GetAddressOf());

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = make_dummy_texture(device, materials[0].shader_resource_views[0].GetAddressOf(), 0xFFFFFFFF, 16);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	hr = make_dummy_texture(device, materials[0].shader_resource_views[1].GetAddressOf(), 0xFFFF7F7F, 16);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	D3D11_TEXTURE2D_DESC texture2d_desc{};
	 //load_texture_from_file(device, texture_filename.c_str(), 
	 // shader_resource_view.GetAddressOf(), &texture2d_desc);
	for (material& material : materials)
	{
		
		 texture.load_texture_from_file(device, material.texture_filenames[0].c_str(),
		 material.shader_resource_views[0].GetAddressOf(), &texture2d_desc);
		 texture.load_texture_from_file(device, material.texture_filenames[1].c_str(),
		 material.shader_resource_views[1].GetAddressOf(), &texture2d_desc);
	}


}


void Static_Mesh::create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count, uint32_t* indices, size_t index_count)
{
	HRESULT hr{ S_OK };
	//説明はspritなどを参照  bufferの違い
	D3D11_BUFFER_DESC buffer_desc{};//バッファー
	D3D11_SUBRESOURCE_DATA subresource_data{};
	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * vertex_count);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffer_desc.CPUAccessFlags = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	subresource_data.pSysMem = vertices;
	subresource_data.SysMemPitch = 0;
	subresource_data.SysMemSlicePitch = 0;
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, vertex_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * index_count);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subresource_data.pSysMem = indices;
	hr = device->CreateBuffer(&buffer_desc, &subresource_data, index_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}

void Static_Mesh::render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color, ID3D11RasterizerState* rasterizer_states)
{
	//スプライトバッチに書いてある
	uint32_t stride{ sizeof(vertex) };
	uint32_t offset{ 0 };
	//IAは入力アッセンブラーの略？

	//アセンブラーの意味はパイプラインの最初の部分　
	//描画するための頂点データやプリミティブ（三角形、線など）の情報を受け取り、
	//グラフィックスパイプライン内の後続のステージで処理されるためのデータを準備するもの
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);//頂点バッファーの配列を 入力アセンブラー ステージにバインドします。
	immediate_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);//インデックス バッファーを入力アセンブラー ステージにバインドします。
	immediate_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediate_context->IASetInputLayout(input_layout.Get());

	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

	//immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());

	//constants data{ world, material_color };
	//immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
	//immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

	for (const material& material : materials)
	{
		 //immediate_context->PSSetShaderResources(0, 1, material.shader_resource_view.GetAddressOf());
		//シェーダーのマテリアル情報を取得
		immediate_context->PSSetShaderResources(0, 1, material.shader_resource_views[0].GetAddressOf());
		immediate_context->PSSetShaderResources(1, 1, material.shader_resource_views[1].GetAddressOf());

		 constants data{ world, material_color };
		 XMStoreFloat4(&data.material_color, XMLoadFloat4(&material_color) * XMLoadFloat4(&material.Kd));
		 immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
		 immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());



		 for (const subset& subset : subsets)
		 {
		 	if (material.name == subset.usemtl)
		 	{
		 		immediate_context->DrawIndexed(subset.index_count, subset.index_start, 0);
		 	}
		 }
	}

	//immediate_context->PSSetShaderResources(0, 1, shader_resource_view.GetAddressOf());

	//D3D11_BUFFER_DESC buffer_desc{};
	//index_buffer->GetDesc(&buffer_desc);//バッファー の空きを作る
	//immediate_context->DrawIndexed((buffer_desc.ByteWidth / sizeof(uint32_t)), 0, 0);//インデックスで描写
	//immediate_context->DrawIndexed(6, 0, 0);//インデックスで描写


	immediate_context->RSSetState(rasterizer_states);

	bounding_box->render(immediate_context, world, { 1, 1, 1, 1 });
}

HRESULT Static_Mesh::make_dummy_texture(ID3D11Device* device, ID3D11ShaderResourceView** shader_resource_view,
	 DWORD value/*0xAABBGGRR*/, UINT dimension)
{
	HRESULT hr{ S_OK };
	
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
	
	 size_t texels = dimension * dimension;
	std::unique_ptr<DWORD[]> sysmem{std::make_unique< DWORD[]>(texels) };
	 for (size_t i = 0; i < texels; ++i) sysmem[i] = value;
	
	 D3D11_SUBRESOURCE_DATA subresource_data{};
	 subresource_data.pSysMem = sysmem.get();
	 subresource_data.SysMemPitch = sizeof(DWORD) * dimension;
	
	 ComPtr<ID3D11Texture2D> texture2d;
	 hr = device->CreateTexture2D(&texture2d_desc, &subresource_data, texture2d.ReleaseAndGetAddressOf());
	 _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	
	 D3D11_SHADER_RESOURCE_VIEW_DESC shader_resource_view_desc{};
	 shader_resource_view_desc.Format = texture2d_desc.Format;
	 shader_resource_view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	 shader_resource_view_desc.Texture2D.MipLevels = 1;
	 hr = device->CreateShaderResourceView(texture2d.Get(), &shader_resource_view_desc,
	 shader_resource_view);
	 _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	
	 return hr;
}