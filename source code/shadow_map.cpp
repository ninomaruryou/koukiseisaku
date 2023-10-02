#include "shadow map.h"

#include <functional>
#include <fstream>

using namespace DirectX;

// UNIT.18
//#include "shader.h"

// UNIT,19
#include <filesystem>
#include "texture.h"

// UNIT.21
inline XMFLOAT4X4 to_xmfloat4x4(const FbxAMatrix& fbxamatrix)
{
	XMFLOAT4X4 xmfloat4x4;
	for (int row = 0; row < 4; row++)
	{
		for (int column = 0; column < 4; column++)
		{
			xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
		}
	}
	return xmfloat4x4;
}
inline XMFLOAT3 to_xmfloat3(const FbxDouble3& fbxdouble3)
{
	XMFLOAT3 xmfloat3;
	xmfloat3.x = static_cast<float>(fbxdouble3[0]);
	xmfloat3.y = static_cast<float>(fbxdouble3[1]);
	xmfloat3.z = static_cast<float>(fbxdouble3[2]);
	return xmfloat3;
}
inline XMFLOAT4 to_xmfloat4(const FbxDouble4& fbxdouble4)
{
	XMFLOAT4 xmfloat4;
	xmfloat4.x = static_cast<float>(fbxdouble4[0]);
	xmfloat4.y = static_cast<float>(fbxdouble4[1]);
	xmfloat4.z = static_cast<float>(fbxdouble4[2]);
	xmfloat4.w = static_cast<float>(fbxdouble4[3]);
	return xmfloat4;
}
// UNIT.22
struct bone_influence
{
	uint32_t bone_index;
	float bone_weight;
};
using bone_influences_per_control_point = std::vector<bone_influence>;//名前を分かりやすく短くするため(名前長いけど)

Shadow_map::Shadow_map(ID3D11Device* device, const char* fbx_filename, bool triangulate,float MAX)
{
	std::filesystem::path cereal_filename(fbx_filename);//ファイル読み込み
	cereal_filename.replace_extension("cereal");
	if (std::filesystem::exists(cereal_filename.c_str()))
	{
		std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
		cereal::BinaryInputArchive deserialization(ifs);
		deserialization(scene_view, meshes, materials, animation_clips); //シリアライズは不完全でも読み込むため注意が必要	

	}
	else
	{
		//FBX読み込み
		FbxManager* fbx_manager{ FbxManager::Create() };
		FbxScene* fbx_scene{ FbxScene::Create(fbx_manager, "") };
		FbxImporter* fbx_importer{ FbxImporter::Create(fbx_manager, "") };//読み込み用のクラス
		bool import_status{ false };
		import_status = fbx_importer->Initialize(fbx_filename);
		_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());
		import_status = fbx_importer->Import(fbx_scene);
		_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());

		FbxGeometryConverter fbx_converter(fbx_manager);
		if (triangulate)//三角形化
		{
			fbx_converter.Triangulate(fbx_scene, true/*replace*/, false/*legacy*/);
			fbx_converter.RemoveBadPolygonsFromMeshes(fbx_scene);
		}

		// Serialize an entire scene graph into sequence container
		std::function<void(FbxNode*)> traverse{ [&](FbxNode* fbx_node) {
	#if 0
			if (fbx_node->GetNodeAttribute())
			{
				switch (fbx_node->GetNodeAttribute()->GetAttributeType())
				{
				case FbxNodeAttribute::EType::eNull:
				case FbxNodeAttribute::EType::eMesh:
				case FbxNodeAttribute::EType::eSkeleton:
				case FbxNodeAttribute::EType::eUnknown:
				case FbxNodeAttribute::EType::eMarker:
				case FbxNodeAttribute::EType::eCamera:
				case FbxNodeAttribute::EType::eLight:
					scene::node& node{ scene_view.nodes.emplace_back() };
					node.attribute = fbx_node->GetNodeAttribute()->GetAttributeType();
					node.name = fbx_node->GetName();
					node.unique_id = fbx_node->GetUniqueID();
					node.parent_index = scene_view.indexof(fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0);
					break;
				}
			}
	#else
			scene::node& node{ scene_view.nodes.emplace_back() };
			node.attribute = fbx_node->GetNodeAttribute() ? fbx_node->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eUnknown;
			node.name = fbx_node->GetName();
			node.unique_id = fbx_node->GetUniqueID();
			node.parent_index = scene_view.indexof(fbx_node->GetParent() ? fbx_node->GetParent()->GetUniqueID() : 0);
	#endif
			for (int child_index = 0; child_index < fbx_node->GetChildCount(); ++child_index)
			{
				traverse(fbx_node->GetChild(child_index));
			}
		} };
		traverse(fbx_scene->GetRootNode());

		// UNIT.18
		fetch_meshes(fbx_scene, meshes);

		// UNIT.19
		fetch_materials(fbx_scene, materials);


		// UNIT.17
		fbx_manager->Destroy();

		//std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
		//cereal::BinaryOutputArchive serialization(ofs);
		//serialization(scene_view, meshes, materials);

	}
// UNIT.18
create_com_objects(device, fbx_filename,MAX);

}

// UNIT.18
void Shadow_map::create_com_objects(ID3D11Device* device, const char* fbx_filename, float MAX)
{
	// UNIT.18
	for (mesh& mesh : meshes)
	{
		HRESULT hr{ S_OK };
		D3D11_BUFFER_DESC buffer_desc{};
		D3D11_SUBRESOURCE_DATA subresource_data{};
		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(vertex) * mesh.vertices.size());
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		subresource_data.pSysMem = mesh.vertices.data();
		subresource_data.SysMemPitch = 0;
		subresource_data.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&buffer_desc, &subresource_data, mesh.vertex_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		buffer_desc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		subresource_data.pSysMem = mesh.indices.data();
		hr = device->CreateBuffer(&buffer_desc, &subresource_data, mesh.index_buffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
#if 1
		mesh.vertices.clear();//バッファーにデータを移すためクリアしても大丈夫
		mesh.indices.clear();
#endif
	}

#if 0

	// UNIT.19
	for (std::unordered_map<uint64_t, material>::iterator iterator = materials.begin(); iterator != materials.end(); ++iterator)
	{
		if (iterator->second.texture_filenames[0].size() > 0)
		{
			std::filesystem::path path(fbx_filename);
			path.replace_filename(iterator->second.texture_filenames[0]);
			D3D11_TEXTURE2D_DESC texture2d_desc;
			Texture::load_texture_from_file(device, path.c_str(), iterator->second.shader_resource_views[0].GetAddressOf(), &texture2d_desc);
		}
		else
		{
			Texture::make_dummy_texture(device, iterator->second.shader_resource_views[0].GetAddressOf(), 0xFFFFFFFF, 16);
		}
	}
#else
	//UNIT29
	for (std::unordered_map<uint64_t, material>::iterator iterator = materials.begin();
		iterator != materials.end(); ++iterator)
	{
		for (size_t texture_index = 0; texture_index < 2; ++texture_index)//なぜ四つ目も必要なのか
		{
			if (iterator->second.texture_filenames[texture_index].size() > 0)//マテリアル情報をshader_resource_viewsに変換
			{
				std::filesystem::path path(fbx_filename);
				path.replace_filename(iterator->second.texture_filenames[texture_index]);
				D3D11_TEXTURE2D_DESC texture2d_desc;
				Texture::load_texture_from_file(device, path.c_str(),
					iterator->second.shader_resource_views[texture_index].GetAddressOf(), &texture2d_desc);
			}
			else
			{
				Texture::make_dummy_texture(device,
					iterator->second.shader_resource_views[texture_index].GetAddressOf(),
					texture_index == 1 ? 0xFFFF7F7F : 0xFFFFFFFF, 16);//ダミー作成
			}
		}

	}


#endif

	// UNIT.18
	HRESULT hr = S_OK;
	// 入力レイアウト
	D3D11_INPUT_ELEMENT_DESC input_element_desc[] //SamanticIndexの値を間違えても例外が発生しないので注意が必要
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		//UNIT29
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "WORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1 },

	};
	Texture::create_vs_from_cso(device, "shadowmap_caster_vs.cso", vertex_shader.ReleaseAndGetAddressOf(), input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	Texture::create_ps_from_cso(device, "Static_mesh_instncing_ps.cso", pixel_shader.ReleaseAndGetAddressOf());

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	buffer_desc = {};
	buffer_desc.ByteWidth = sizeof(instance) * MAX;
	buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	instance* instances = new instance[MAX];

	D3D11_SUBRESOURCE_DATA subresource_data{};
	subresource_data.pSysMem = instances;
	subresource_data.SysMemPitch = 0; //Not use for vertex buffers.mm 
	subresource_data.SysMemSlicePitch = 0; //Not use for vertex buffers.

	hr = device->CreateBuffer(&buffer_desc, &subresource_data, instance_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	delete[] instances;


}


// UNIT.18
void Shadow_map::fetch_meshes(FbxScene* fbx_scene, std::vector<mesh>& meshes)
{
	// Fetch all meshes from the scene.
	for (const scene::node& node : scene_view.nodes)
	{
		// Skip if node attribute is not eMesh.
		if (node.attribute != FbxNodeAttribute::EType::eMesh)
		{
			continue;
		}

		FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };
		FbxMesh* fbx_mesh{ fbx_node->GetMesh() };

		mesh& mesh{ meshes.emplace_back() };
		mesh.unique_id = fbx_mesh->GetNode()->GetUniqueID();
		mesh.name = fbx_mesh->GetNode()->GetName();
		mesh.node_index = scene_view.indexof(mesh.unique_id);
		// UNIT.21
		mesh.default_global_transform = to_xmfloat4x4(fbx_mesh->GetNode()->EvaluateGlobalTransform());
		// UNIT.22
		std::vector<bone_influences_per_control_point> bone_influences;

		// UNIT.20
		// Build subsets for each material (各素材のサブセットを構築する)
		std::vector<mesh::subset>& subsets{ mesh.subsets };
		const int material_count{ fbx_mesh->GetNode()->GetMaterialCount() };
		subsets.resize(material_count > 0 ? material_count : 1);
		for (int material_index = 0; material_index < material_count; ++material_index)
		{
			const FbxSurfaceMaterial* fbx_material{ fbx_mesh->GetNode()->GetMaterial(material_index) };
			subsets.at(material_index).material_name = fbx_material->GetName();
			subsets.at(material_index).material_unique_id = fbx_material->GetUniqueID();
		}


		if (material_count > 0)
		{
			// Count the faces of each material（各素材の面を数える）
			const int polygon_count{ fbx_mesh->GetPolygonCount() };
			for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
			{
				const int material_index{ fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) };
				subsets.at(material_index).index_count += 3;
			}

			// Record the offset (How many vertex)　（オフセットを記録する（何番目の頂点か））
			uint32_t offset{ 0 };
			for (mesh::subset& subset : subsets)
			{
				subset.start_index_location = offset;
				offset += subset.index_count;
				// This will be used as counter in the following procedures, reset to zero　
				// （これは、以下の手順でカウンターとして使用され、ゼロにリセットされます。）
				subset.index_count = 0;
			}
		}
		const int polygon_count{ fbx_mesh->GetPolygonCount() };
		mesh.vertices.resize(polygon_count * 3LL);
		mesh.indices.resize(polygon_count * 3LL);

#if 1//UNIT29
		for (const vertex& v : mesh.vertices)
		{
			mesh.bounding_box[0].x = std::min<float>(mesh.bounding_box[0].x, v.position.x);
			mesh.bounding_box[0].y = std::min<float>(mesh.bounding_box[0].y, v.position.y);
			mesh.bounding_box[0].z = std::min<float>(mesh.bounding_box[0].z, v.position.z);
			mesh.bounding_box[1].x = std::max<float>(mesh.bounding_box[1].x, v.position.x);
			mesh.bounding_box[1].y = std::max<float>(mesh.bounding_box[1].y, v.position.y);
			mesh.bounding_box[1].z = std::max<float>(mesh.bounding_box[1].z, v.position.z);
		}

#endif

		FbxStringList uv_names;
		fbx_mesh->GetUVSetNames(uv_names);
		const FbxVector4* control_points{ fbx_mesh->GetControlPoints() };
		for (int polygon_index = 0; polygon_index < polygon_count; ++polygon_index)
		{
			// UNIT.20
			const int material_index{ material_count > 0 ? fbx_mesh->GetElementMaterial()->GetIndexArray().GetAt(polygon_index) : 0 };
			mesh::subset& subset{ subsets.at(material_index) };
			const uint32_t offset{ subset.start_index_location + subset.index_count };

			for (int position_in_polygon = 0; position_in_polygon < 3; ++position_in_polygon)
			{
				const int vertex_index{ polygon_index * 3 + position_in_polygon };

				vertex vertex;
				const int polygon_vertex{ fbx_mesh->GetPolygonVertex(polygon_index, position_in_polygon) };
				vertex.position.x = static_cast<float>(control_points[polygon_vertex][0]);
				vertex.position.y = static_cast<float>(control_points[polygon_vertex][1]);
				vertex.position.z = static_cast<float>(control_points[polygon_vertex][2]);

				//UNIT29 ファイルに法線ベクトル情報を持たない場合は GenerateTangentsData 関数で生成する
				if (fbx_mesh->GenerateTangentsData(0, false))
				{
					const FbxGeometryElementTangent* tangent = fbx_mesh->GetElementTangent(0);
					vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[0]);
					vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[1]);
					vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[2]);
					vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertex_index)[3]);
				}

				// UNIT.22 //bone_influence の vector
				//const bone_influences_per_control_point& influences_per_control_point{ bone_influences.at(polygon_vertex) };
				//for (size_t influence_index = 0; influence_index < influences_per_control_point.size(); ++influence_index)
				{
#if 0
				else
				{
					size_t minimum_value_index = 0;
					float minimum_value = FLT_MAX;
					for (size_t i = 0; i < MAX_BONE_INFLUENCES; ++i)
					{
						if (minimum_value > vertex.bone_weights[i])
						{
							minimum_value = vertex.bone_weights[i];
							minimum_value_index = i;
						}
					}
					vertex.bone_weights[minimum_value_index] += influences_per_control_point.at(influence_index).bone_weight;
					vertex.bone_indices[minimum_value_index] = influences_per_control_point.at(influence_index).bone_index;
				}
#endif
				}

				if (fbx_mesh->GetElementNormalCount() > 0)
				{
					FbxVector4 normal;
					fbx_mesh->GetPolygonVertexNormal(polygon_index, position_in_polygon, normal);
					vertex.normal.x = static_cast<float>(normal[0]);//法線情報を入れてる
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
				}

				if (fbx_mesh->GetElementUVCount() > 0)
				{
					FbxVector2 uv;
					bool unmapped_uv;
					fbx_mesh->GetPolygonVertexUV(polygon_index, position_in_polygon, uv_names[0], uv, unmapped_uv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}

				mesh.vertices.at(vertex_index) = std::move(vertex);
				// UNIT.20
#if 0
				mesh.indices.at(vertex_index) = vertex_index;
#else
				mesh.indices.at(static_cast<size_t>(offset) + position_in_polygon) = vertex_index;
				subset.index_count++;
#endif
			}
		}
	}
}

// UNIT.19
void Shadow_map::fetch_materials(FbxScene* fbx_scene, std::unordered_map<uint64_t, material>& materials)
{
	const size_t node_count{ scene_view.nodes.size() };
	for (size_t node_index = 0; node_index < node_count; ++node_index)
	{
		const scene::node& node{ scene_view.nodes.at(node_index) };
		const FbxNode* fbx_node{ fbx_scene->FindNodeByName(node.name.c_str()) };

		const int material_count{ fbx_node->GetMaterialCount() };
		for (int material_index = 0; material_index < material_count; ++material_index)
		{
			const FbxSurfaceMaterial* fbx_material{ fbx_node->GetMaterial(material_index) };

			material material;
			material.name = fbx_material->GetName();
			material.unique_id = fbx_material->GetUniqueID();
			FbxProperty fbx_property;
			//データにマテリアル指定をした後に値を入れる
			fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sDiffuse);//色

			if (fbx_property.IsValid())
			{
				const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
				material.Kd.x = static_cast<float>(color[0]);
				material.Kd.y = static_cast<float>(color[1]);
				material.Kd.z = static_cast<float>(color[2]);
				material.Kd.w = 1.0f;

				const FbxFileTexture* fbx_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
				material.texture_filenames[0] = fbx_texture ? fbx_texture->GetRelativeFileName() : "";
			}

#if 1
			//UNIT29
			fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sNormalMap);//法線マップデータ
			if (fbx_property.IsValid())
			{
				const FbxFileTexture* file_texture{ fbx_property.GetSrcObject<FbxFileTexture>() };
				material.texture_filenames[1] = file_texture ? file_texture->GetRelativeFileName() : "";
			}
#endif


			fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sAmbient);
			if (fbx_property.IsValid())
			{
				const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
				material.Ka.x = static_cast<float>(color[0]);
				material.Ka.y = static_cast<float>(color[1]);
				material.Ka.z = static_cast<float>(color[2]);
				material.Ka.w = 1.0f;
			}
			fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sSpecular);//反射
			if (fbx_property.IsValid())
			{
				const FbxDouble3 color{ fbx_property.Get<FbxDouble3>() };
				material.Ks.x = static_cast<float>(color[0]);
				material.Ks.y = static_cast<float>(color[1]);
				material.Ks.z = static_cast<float>(color[2]);
				material.Ks.w = 1.0f;
			}
			materials.emplace(material.unique_id, std::move(material));
		}
	}
#if 1

	// Append default(dummy) material
	materials.emplace();
#endif
}

void Shadow_map::Begin(ID3D11DeviceContext* immediate_context)
{
	HRESULT hr = S_OK;

	D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	D3D11_MAPPED_SUBRESOURCE mapped_buffer;
	hr = immediate_context->Map(instance_buffer.Get(), 0, map, 0, &mapped_buffer);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	instances = static_cast<instance*>(mapped_buffer.pData);
	count_instance = 0;
}

// UNIT.25
void Shadow_map::render(ID3D11DeviceContext* immediate_context, const XMFLOAT4X4& world, const XMFLOAT4& material_color)
{
	for (mesh& mesh : meshes)
	{

		{
			XMStoreFloat4x4(&instances[count_instance].ndc_transform,
				XMLoadFloat4x4(&mesh.default_global_transform) * XMLoadFloat4x4(&world));
		}

		count_instance++;

	}

}



void Shadow_map::End(ID3D11DeviceContext* immediate_context)
{
	immediate_context->Unmap(instance_buffer.Get(), 0);

	UINT strides[2] = { sizeof(vertex), sizeof(instance) };//順番がスロット番号になる？
	UINT offsets[2] = { 0, 0 };


	for (mesh& mesh : meshes)
	{
		ID3D11Buffer* vbs[2] = { mesh.vertex_buffer.Get(), instance_buffer.Get() };

		constants data;

		for (const mesh::subset& subset : mesh.subsets)
		{
			const material& material{ materials.at(subset.material_unique_id) };

			immediate_context->IASetVertexBuffers(0, 2, vbs, strides, offsets);
			immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			immediate_context->IASetInputLayout(input_layout.Get());
			immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);

			immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
			immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);
			immediate_context->PSSetShaderResources(0, 1, material.shader_resource_views[0].GetAddressOf());
			immediate_context->PSSetShaderResources(1, 1, material.shader_resource_views[1].GetAddressOf());

			XMStoreFloat4(&data.material_color, XMLoadFloat4(&XMFLOAT4(1, 1, 1, 1)) * XMLoadFloat4(&material.Kd));

			immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
			immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

			immediate_context->DrawIndexedInstanced(subset.index_count, count_instance, subset.start_index_location, 0, 0);

		}
	}

}
