#include "static_mesh.h"
#include "texture.h"
#include "vertex.h"


Bounding_Box::Bounding_Box(ID3D11Device* device, DirectX::XMFLOAT3 max_position, DirectX::XMFLOAT3 min_position)
{

	vertices.resize(8);

	vertices[0].position = { min_position.x ,max_position.y,min_position.z };
	vertices[1].position = { min_position.x ,max_position.y,max_position.z };
	vertices[2].position = { max_position.x ,max_position.y,min_position.z };
	vertices[3].position = { max_position.x ,max_position.y,max_position.z };

	vertices[4].position = { min_position.x ,min_position.y,min_position.z };
	vertices[5].position = { min_position.x ,min_position.y,max_position.z };
	vertices[6].position = { max_position.x ,min_position.y,min_position.z };
	vertices[7].position = { max_position.x ,min_position.y,max_position.z };

	indices.resize(36);

	indices[0] = 0;
	indices[1] = 2;
	indices[2] = 1;

	indices[3] = 1;
	indices[4] = 2;
	indices[5] = 3;

	indices[6] = 4;
	indices[7] = 6;
	indices[8] = 5;

	indices[9] = 5;
	indices[10] = 6;
	indices[11] = 7;

	indices[12] = 3;
	indices[13] = 2;
	indices[14] = 7;

	indices[15] = 4;
	indices[16] = 5;
	indices[17] = 6;

	indices[18] = 4;
	indices[19] = 5;
	indices[20] = 0;

	indices[21] = 1;
	indices[22] = 5;
	indices[23] = 0;

	indices[24] = 2;
	indices[25] = 6;
	indices[26] = 7;

	indices[24] = 2;
	indices[25] = 6;
	indices[26] = 7;

	indices[24] = 2;
	indices[25] = 6;
	indices[26] = 4;

	indices[27] = 1;
	indices[28] = 5;
	indices[29] = 3;

	indices[30] = 1;
	indices[31] = 5;
	indices[32] = 3;

	indices[33] = 3;
	indices[34] = 7;
	indices[35] = 5;

	create_com_buffers(device, vertices.data(), vertices.size(), indices.data(), indices.size()); //バッファの生成

	HRESULT hr{ S_OK };

	//Texture texture;

	D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
   { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	Texture::create_vs_from_cso(device, "geometric_primitive_vs.cso", vertex_shader.GetAddressOf(),
		input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	Texture::create_ps_from_cso(device, "geometric_primitive_ps.cso", pixel_shader.GetAddressOf());

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	
}


void Bounding_Box::create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count, uint32_t* indices, size_t index_count)
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

void Bounding_Box::render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color)
{
	//スプライトバッチに書いてある
	uint32_t stride{ sizeof(vertex) };
	uint32_t offset{ 0 };
	immediate_context->IASetVertexBuffers(0, 1, vertex_buffer.GetAddressOf(), &stride, &offset);
	immediate_context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	immediate_context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	immediate_context->IASetInputLayout(input_layout.Get());
	immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
	immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

	constants data{ world, material_color };
	immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
	immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

	D3D11_BUFFER_DESC buffer_desc{};
	index_buffer->GetDesc(&buffer_desc);//バッファー の空きを作る
	immediate_context->DrawIndexed((buffer_desc.ByteWidth / sizeof(uint32_t)), 0, 0);//インデックスで描写

}
