#include "Geometric_primitive.h"
#include "texture.h"
#include "misc.h"

Geometric_Primitive::Geometric_Primitive(ID3D11Device* device)
{
	// サイズが 1.0 の正立方体データを作成する（重心を原点にする）。正立方体のコントロールポイント数は 8 個、
	// 1 つのコントロールポイントの位置には法線の向きが違う頂点が 3 個あるので頂点情報の総数は 8x3=24 個、
	// 頂点情報配列（vertices）にすべて頂点の位置・法線情報を格納する。


#if BOX

	vertices[0].position = { -1.0f,0.5f, -0.0f };
	vertices[1].position = { -0.5f, 0.5f, -0.5f };
	vertices[2].position = { 0.0f, 0.5f, -0.0f };
	vertices[3].position = { 0.0f, 0.5f, -0.5f };

	vertices[4].position = { -0.5f,0.0f, -0.0f };
	vertices[5].position = { -0.0f, 0.0f, -0.0f };
	vertices[6].position = { -0.5f, 0.0f, -0.5f };
	vertices[7].position = { 0.0f, 0.0f, -0.5f };

	vertices[8].position = { -0.5f,0.5f, -0.0f };
	vertices[9].position = { -0.5f, 0.5f, -0.5f };
	vertices[10].position = { 0.0f, 0.5f, -0.0f };
	vertices[11].position = { 0.0f, 0.5f, -0.5f };

	vertices[12].position = { -0.5f,0.0f, -0.0f };
	vertices[13].position = { -0.0f, 0.0f, -0.0f };
	vertices[14].position = { -0.5f, 0.0f, -0.5f };
	vertices[15].position = { 0.0f, 0.0f, -0.5f };

	vertices[16].position = { -0.5f,0.5f, -0.0f };
	vertices[17].position = { -0.5f, 0.5f, -0.5f };
	vertices[18].position = { 0.0f, 0.5f, -0.0f };
	vertices[19].position = { 0.0f, 0.5f, -0.5f };

	vertices[20].position = { -0.5f,0.0f, -0.0f };
	vertices[21].position = { -0.0f, 0.0f, -0.0f };
	vertices[22].position = { -0.5f, 0.0f, -0.5f };
	vertices[23].position = { 0.0f, 0.0f, -0.5f };

	vertices[0].normal = { 0, -1 , 0 };
	vertices[1].normal = { 0, -1 , 0 };
	vertices[2].normal = { 0, -1 , 0 };
	vertices[3].normal = { 0, -1 , 0 };

	vertices[4].normal = { 0, 1, 0 };
	vertices[5].normal = { 0, 1, 0 };
	vertices[6].normal = { 0, 1, 0 };
	vertices[7].normal = { 0, 1, 0 };

	vertices[8].normal = { 1, 0 , 0 };
	vertices[9].normal = { 1, 0 , 0 };
	vertices[10].normal= { -1, 0 , 0};
	vertices[11].normal= { -1, 0 , 0 };

	vertices[12].normal = { 1, 0 , 0 };
	vertices[13].normal = { -1, 0 , 0 };
	vertices[14].normal = { 1, 0 , 0 };
	vertices[15].normal = { -1, 0 , 0 };

	vertices[16].normal = { 0, 0 , -1 };
	vertices[17].normal = { 0, 0 , 1 };
	vertices[18].normal = { 0, 0, -1 };
	vertices[19].normal = { 0, 0 ,1 };

	vertices[20].normal = { 0, 0 , -1 };
	vertices[21].normal = { 0, 0 ,-1 };
	vertices[22].normal = { 0, 0 , 1 };
	vertices[23].normal = { 0, 0 ,1 };


	// 正立方体は 6 面持ち、1 つの面は 2 つの 3 角形ポリゴンで構成されるので 3 角形ポリゴンの総数は 6x2=12 個、
 // 正立方体を描画するために 12 回の 3 角形ポリゴン描画が必要、よって参照される頂点情報は 12x3=36 回、
 // 3 角形ポリゴンが参照する頂点情報のインデックス（頂点番号）を描画順に配列（indices）に格納する。
 // 時計回りが表面になるように格納すること。

	indices[0] = 0;
	indices[1] = 2;
	indices[2] = 1;

	indices[3] = 1;
	indices[4] = 2;
	indices[5] = 3;

	indices[6] = 21;
	indices[7] = 16;
	indices[8] = 20;

	indices[9] = 21;
	indices[10] = 18;
	indices[11] = 16;

	indices[12] = 13;
	indices[13] = 11;
	indices[14] = 10;

	indices[15] = 15;
	indices[16] = 11;
	indices[17] = 13;

	indices[18] = 8;
	indices[19] = 9;
	indices[20] = 14;

	indices[21] = 12;
	indices[22] = 8;
	indices[23] = 14;

	indices[24] = 22;
	indices[25] = 19;
	indices[26] = 23;

	indices[27] = 17;
	indices[28] = 19;
	indices[29] = 22;

	indices[30] = 4;
	indices[31] = 6;
	indices[32] = 5;

	indices[33] = 5;
	indices[34] = 6;
	indices[35] = 7;

#endif

#if CIRCLE


	for (UINT i = 0; i < NUM_CIRCLE_POINTS; ++i) //点の場所指定
	{
		//円の点（angleのようなもの）
		float theta = (float)i / (NUM_CIRCLE_POINTS);
		theta *= DirectX::XM_2PI;
		
		float x = cosf(theta);
		float y = sinf(theta);


		vertices[i].position = { x,y,0 };
		vertices[i + NUM_CIRCLE_POINTS + 1].position = { x,y,1 };
		vertices[i].normal = { 0,1,0 };
	}

	vertices[NUM_CIRCLE_POINTS].position = {0,0,0};
	vertices[NUM_CIRCLE_POINTS * 2 + 1 ].position = {0,0,1};


	for (UINT i = 0; i < NUM_CIRCLE_POINTS; ++i)//円の三角形の順番作成
	{
		UINT offset = i * 3;
		indices[offset] = i + 1;
		indices[offset + (NUM_CIRCLE_POINTS * 3)] = i + 2 + (NUM_CIRCLE_POINTS);

		if (i == NUM_CIRCLE_POINTS - 1)
		{
			indices[offset] = 0; //中心点
			indices[offset + (NUM_CIRCLE_POINTS * 3)] = (NUM_CIRCLE_POINTS + 1); //中心点

		}
		indices[offset + 1] = i;//三角形の周り
		indices[offset + 1 + (NUM_CIRCLE_POINTS * 3)] = i + (NUM_CIRCLE_POINTS + 1);//三角形の周り

		indices[offset + 2] = (NUM_CIRCLE_POINTS);//三角形の中心点
		indices[offset + 2 + (NUM_CIRCLE_POINTS * 3)] = NUM_CIRCLE_POINTS + (NUM_CIRCLE_POINTS + 1);//三角形の中心点
	}

	for (UINT i = 0; i < NUM_CIRCLE_POINTS; ++i)//	円柱の周り
	{
		UINT offset = i * 3;

		indices[offset + ((NUM_CIRCLE_POINTS * 3) * 2)] = indices[offset];
		indices[offset + ((NUM_CIRCLE_POINTS * 3) * 2) + 1] = indices[offset + 1 + (NUM_CIRCLE_POINTS * 3)];
		indices[offset + ((NUM_CIRCLE_POINTS * 3) * 2) + 2] = indices[offset + (NUM_CIRCLE_POINTS * 3)];
	}
	for (UINT i = 0; i < NUM_CIRCLE_POINTS; ++i)//	円柱の周り
	{
		UINT offset = i * 3;
		indices[offset + ((NUM_CIRCLE_POINTS * 3) * 3)] = indices[offset + 1 + (NUM_CIRCLE_POINTS * 3)];
		indices[offset + ((NUM_CIRCLE_POINTS * 3) * 3) + 1] = indices[offset];
		indices[offset + ((NUM_CIRCLE_POINTS * 3) * 3) + 2] = indices[offset + 1];
	}

#endif

#if SPHERE

	//for (UINT i = 0; i < NUM_CIRCLE_POINTS; ++i) //点の場所指定
	//{
	//	float theta = (float)i / (NUM_CIRCLE_POINTS);
	//	theta *= DirectX::XM_2PI;

	//	float x = cosf(theta);
	//	float y = sinf(theta);

	//	for (UINT a = 0; a < NUM_CIRCLE_POINTS; ++a) //点の場所指定
	//	{

	//		float theta_z = (float)i / (NUM_CIRCLE_POINTS);
	//		theta_z *= DirectX::XM_2PI;

	//		//円の点（angleのようなもの）
	//		float z = cosf(theta_z);

	//		vertices[a + (NUM_CIRCLE_POINTS * i)].position = { x,y,z };
	//		vertices[a].normal = { 0,1,0 };
	//	}

	//}

	for (int v = 0; v <= m_iVMax; v++)
	{
		for (int u = 0; u < m_iUMax; u++)
		{
			//float theta = (float)i / (NUM_CIRCLE_POINTS);
			//theta *= DirectX::XM_2PI;

			//float x = cosf(theta);
			//float y = sinf(theta);

			double dTheta = DirectX::XMConvertToRadians(180.0f * v / m_iVMax);
			double dPhi = DirectX::XMConvertToRadians(360.0f * u / m_iUMax);
			FLOAT fX = static_cast<FLOAT>(sin(dTheta) * cos(dPhi));
			FLOAT fY = static_cast<FLOAT>(cos(dTheta));
			FLOAT fZ = static_cast<FLOAT>(sin(dTheta) * sin(dPhi));
			vertices[m_iUMax * v + u].position = DirectX::XMFLOAT3(fX, fY, fZ);
		}
	}

	int i = 0;
	for (int v = 0; v < m_iVMax; v++)
	{
		for (int u = 0; u <= m_iUMax; u++)
		{
			if (u == m_iUMax)
			{
				indices[i] = v * m_iUMax;
				i++;
				indices[i] = (v + 1) * m_iUMax;
				i++;
			}
			else
			{
				indices[i] = (v * m_iUMax) + u;
				i++;
				indices[i] = indices[i - 1] + m_iUMax;
				i++;
			}
		}
	}


#endif


	 create_com_buffers(device, vertices, (sizeof(vertices) / sizeof(vertex)), indices, (sizeof(indices) / sizeof(uint32_t))); //バッファの生成
	
	 HRESULT hr{ S_OK };

	 Texture texture;

	 D3D11_INPUT_ELEMENT_DESC input_element_desc[]
	 {
	 { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		 D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	 };
	 texture.create_vs_from_cso(device, "geometric_primitive_vs.cso", vertex_shader.GetAddressOf(),
	 input_layout.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	 texture.create_ps_from_cso(device, "geometric_primitive_ps.cso", pixel_shader.GetAddressOf());
	
	 D3D11_BUFFER_DESC buffer_desc{};
	 buffer_desc.ByteWidth = sizeof(constants);
	 buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	 buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	 hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.GetAddressOf());
	 _ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
}

void Geometric_Primitive::create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count,
	uint32_t * indices, size_t index_count)
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

void Geometric_Primitive::render(ID3D11DeviceContext* immediate_context,
	const DirectX::XMFLOAT4X4 & world, const DirectX::XMFLOAT4& material_color)
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
	//immediate_context->DrawIndexed(6, 0, 0);//インデックスで描写
//インデックスは頂点などのデータ管理メモリ？
}