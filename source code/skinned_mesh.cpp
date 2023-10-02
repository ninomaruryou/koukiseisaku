

// UNIT.17
#include "misc.h"
#include "skinned_mesh.h"

#include <functional>
#include <iostream>
#include <fstream>

#include "raycast.h"
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
void fetch_bone_influences(const FbxMesh* fbx_mesh, std::vector<bone_influences_per_control_point>& bone_influences)
{
	const int control_points_count{ fbx_mesh->GetControlPointsCount() };
	bone_influences.resize(control_points_count);

	const int skin_count{ fbx_mesh->GetDeformerCount(FbxDeformer::eSkin) };
	for (int skin_index = 0; skin_index < skin_count; ++skin_index)
	{
		const FbxSkin* fbx_skin{ static_cast<FbxSkin*>(fbx_mesh->GetDeformer(skin_index, FbxDeformer::eSkin)) };

		const int cluster_count{ fbx_skin->GetClusterCount() };
		for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
		{
			const FbxCluster* fbx_cluster{ fbx_skin->GetCluster(cluster_index) };

			const int control_point_indices_count{ fbx_cluster->GetControlPointIndicesCount() };
			for (int control_point_indices_index = 0; control_point_indices_index < control_point_indices_count; ++control_point_indices_index)
			{
#if 1
				int control_point_index{ fbx_cluster->GetControlPointIndices()[control_point_indices_index] };
				double control_point_weight{ fbx_cluster->GetControlPointWeights()[control_point_indices_index] };
				bone_influence& bone_influence{ bone_influences.at(control_point_index).emplace_back() };
				bone_influence.bone_index = static_cast<uint32_t>(cluster_index);
				bone_influence.bone_weight = static_cast<float>(control_point_weight);
#else
				bone_influences.at((fbx_cluster->GetControlPointIndices())[control_point_indices_index]).emplace_back()
					= { static_cast<uint32_t>(cluster_index),  static_cast<float>((fbx_cluster->GetControlPointWeights())[control_point_indices_index]) };
#endif
			}
		}
	}
}

// UNIT.17
Skinned_Mesh::Skinned_Mesh(ID3D11Device* device, const char* fbx_filename, bool triangulate, float sampling_rate/*UNIT.25*/)
{
	//UNIT30
	//※このコードでは与えられた変数 fbx_filename の拡張子を“cereal”に変え、そのファイルが存在する場合はシリアライズされたデータか
	//らロードする。また、存在しない場合は従来通り FBX ファイルからロードする。
	//※skinnd_mesh クラスのデータ構造に変更があった場合はシリアライズされたファイルは無効になるので削除する必要がある。
	std::filesystem::path cereal_filename(fbx_filename);//ファイル読み込み
	 cereal_filename.replace_extension("cereal");
	 if (std::filesystem::exists(cereal_filename.c_str()))
	 {
		 std::ifstream ifs(cereal_filename.c_str(), std::ios::binary);
	 	 cereal::BinaryInputArchive deserialization(ifs);
		 deserialization(scene_view,meshes ,materials, animation_clips); //シリアライズは不完全でも読み込むため注意が必要	

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

		 // UNIT.25
#if 0
		 float sampling_rate{ 0 };
#endif
		 fetch_animations(fbx_scene, animation_clips, sampling_rate);

		 // UNIT.17
		 fbx_manager->Destroy();

		 std::ofstream ofs(cereal_filename.c_str(), std::ios::binary);
		 cereal::BinaryOutputArchive serialization(ofs);
		 serialization(scene_view, meshes, materials, animation_clips);
		 
	 }
	 // UNIT.18
	create_com_objects(device, fbx_filename);
}
// UNIT.18
void Skinned_Mesh::fetch_meshes(FbxScene* fbx_scene, std::vector<mesh>& meshes)
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

		/*mesh& mesh{ meshes.emplace_back() };
		mesh.unique_id = fbx_mesh->GetNode()->GetUniqueID();
		mesh.name = fbx_mesh->GetNode()->GetName();
		mesh.node_index = scene_view.indexof(mesh.unique_id);*/
		mesh& mesh{ meshes.emplace_back() };
#if 0
		mesh.unique_id = fbx_mesh->GetNode()->GetUniqueID();
		mesh.name = fbx_mesh->GetNode()->GetName();
		mesh.node_index = scene_view.indexof(mesh.unique_id);
		// UNIT.21
		mesh.default_global_transform = to_xmfloat4x4(fbx_mesh->GetNode()->EvaluateGlobalTransform());
#else
		mesh.unique_id = fbx_node->GetUniqueID();
		mesh.name = fbx_node->GetName();
		mesh.node_index = scene_view.indexof(mesh.unique_id);
		// UNIT.21
		mesh.default_global_transform = to_xmfloat4x4(fbx_node->EvaluateGlobalTransform());
#endif

		// UNIT.21
		mesh.default_global_transform = to_xmfloat4x4(fbx_mesh->GetNode()->EvaluateGlobalTransform());
		// UNIT.22
		std::vector<bone_influences_per_control_point> bone_influences;
		fetch_bone_influences(fbx_mesh, bone_influences);
		// UNIT.24
		fetch_skeleton(fbx_mesh, mesh.bind_pose);

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
				const bone_influences_per_control_point& influences_per_control_point{ bone_influences.at(polygon_vertex) };
				for (size_t influence_index = 0; influence_index < influences_per_control_point.size(); ++influence_index)
				{
					if (influence_index < MAX_BONE_INFLUENCES)//ボーン情報
					{
						vertex.bone_weights[influence_index] = influences_per_control_point.at(influence_index).bone_weight;
						vertex.bone_indices[influence_index] = influences_per_control_point.at(influence_index).bone_index;
					}
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
	}
}
// UNIT.18
void Skinned_Mesh::create_com_objects(ID3D11Device* device, const char* fbx_filename)
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
#if 0
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
		// UNIT.22
		{ "WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	Texture::create_vs_from_cso(device, "skinned_mesh_vs.cso", vertex_shader.ReleaseAndGetAddressOf(), input_layout.ReleaseAndGetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	Texture::create_ps_from_cso(device, "skinned_mesh_ps.cso", pixel_shader.ReleaseAndGetAddressOf());

	D3D11_BUFFER_DESC buffer_desc{};
	buffer_desc.ByteWidth = sizeof(constants);
	buffer_desc.Usage = D3D11_USAGE_DEFAULT;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&buffer_desc, nullptr, constant_buffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	
}
// UNIT.25
void Skinned_Mesh::render(ID3D11DeviceContext* immediate_context, const XMFLOAT4X4& world, const XMFLOAT4& material_color, const animation::keyframe* keyframe/*UNIT.25*/)
{

	for (mesh& mesh : meshes)
	{
		uint32_t stride{ sizeof(vertex) };
		uint32_t offset{ 0 };
		//IAは入力アセンブラーの略

		//アセンブラーの意味はパイプラインの最初の部分　
		//描画するための頂点データやプリミティブ（三角形、線など）の情報を受け取り、
		//グラフィックスパイプライン内の後続のステージで処理されるためのデータを準備するもの
		immediate_context->IASetVertexBuffers(0, 1, mesh.vertex_buffer.GetAddressOf(), &stride, &offset);//頂点バッファーの配列を 入力アセンブラー ステージにバインドします。
		immediate_context->IASetIndexBuffer(mesh.index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		immediate_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);//入力アセンブラー ステージの入力データを記述するデータ順序をバインドします。
		immediate_context->IASetInputLayout(input_layout.Get());//入力レイアウト オブジェクトをバインド

		immediate_context->VSSetShader(vertex_shader.Get(), nullptr, 0);
		immediate_context->PSSetShader(pixel_shader.Get(), nullptr, 0);

		constants data;
		if (keyframe && keyframe->nodes.size() > 0)
		{
			const animation::keyframe::node& mesh_node{ keyframe->nodes.at(mesh.node_index) };
			XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh_node.global_transform) * XMLoadFloat4x4(&world));

			const size_t bone_count{ mesh.bind_pose.bones.size() };
			_ASSERT_EXPR(bone_count < MAX_BONES, L"The value of the 'bone_count' has exceeded MAX_BONES.");

			for (size_t bone_index = 0; bone_index < bone_count; ++bone_index)
			{
				const skeleton::bone& bone{ mesh.bind_pose.bones.at(bone_index) };
				const animation::keyframe::node& bone_node{ keyframe->nodes.at(bone.node_index) };
				XMStoreFloat4x4(&data.bone_transforms[bone_index],
					XMLoadFloat4x4(&bone.offset_transform) *
					XMLoadFloat4x4(&bone_node.global_transform) *
					XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.default_global_transform))
				);
			}
		}
		else
		{
			XMStoreFloat4x4(&data.world,
				XMLoadFloat4x4(&mesh.default_global_transform) * XMLoadFloat4x4(&world));
			for (size_t bone_index = 0; bone_index < MAX_BONES; ++bone_index)
			{
				data.bone_transforms[bone_index] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			}
		}
		for (const mesh::subset& subset : mesh.subsets)
		{
			const material& material{ materials.at(subset.material_unique_id) };
			XMStoreFloat4(&data.material_color, XMLoadFloat4(&material_color) * XMLoadFloat4(&material.Kd));
			immediate_context->UpdateSubresource(constant_buffer.Get(), 0, 0, &data, 0, 0);
			immediate_context->VSSetConstantBuffers(0, 1, constant_buffer.GetAddressOf());

			immediate_context->PSSetShaderResources(0, 1, material.shader_resource_views[0].GetAddressOf());
			immediate_context->PSSetShaderResources(1, 1, material.shader_resource_views[1].GetAddressOf());

			immediate_context->DrawIndexed(subset.index_count, subset.start_index_location, 0);
		}
	}


}
// UNIT.19
void Skinned_Mesh::fetch_materials(FbxScene* fbx_scene, std::unordered_map<uint64_t, material>& materials)
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
// UNIT.24 FBX メッシュからバインドポーズの情報を抽出する
void Skinned_Mesh::fetch_skeleton(FbxMesh* fbx_mesh, skeleton& bind_pose)
{
	const int deformer_count = fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int deformer_index = 0; deformer_index < deformer_count; ++deformer_index)
	{
		FbxSkin* skin = static_cast<FbxSkin*>(fbx_mesh->GetDeformer(deformer_index, FbxDeformer::eSkin));
		const int cluster_count = skin->GetClusterCount();
		bind_pose.bones.resize(cluster_count);
		for (int cluster_index = 0; cluster_index < cluster_count; ++cluster_index)
		{
			FbxCluster* cluster = skin->GetCluster(cluster_index);

			skeleton::bone& bone{ bind_pose.bones.at(cluster_index) };
			bone.name = cluster->GetLink()->GetName();
			bone.unique_id = cluster->GetLink()->GetUniqueID();
			bone.parent_index = bind_pose.indexof(cluster->GetLink()->GetParent()->GetUniqueID());
			bone.node_index = scene_view.indexof(bone.unique_id);

			//'reference_global_init_position' is used to convert from local space of model(mesh) to global space of scene.
			//(reference_global_init_position' は、モデル（メッシュ）のローカル空間からシーンのグローバル空間への変換に使用します。)
			FbxAMatrix reference_global_init_position;
			cluster->GetTransformMatrix(reference_global_init_position);

			// 'cluster_global_init_position' is used to convert from local space of bone to global space of scene.
			// (cluster_global_init_position」は、ボーンのローカル空間からシーンのグローバル空間への変換に使用されます。)
			FbxAMatrix cluster_global_init_position;
			cluster->GetTransformLinkMatrix(cluster_global_init_position);

			// Matrices are defined using the Column Major scheme. When a FbxAMatrix represents a transformation (translation, rotation and scale), the last row of the matrix represents the translation part of the transformation.
			// Compose 'bone.offset_transform' matrix that trnasforms position from mesh space to bone space. This matrix is called the offset matrix.
			//行列はカラムメジャー方式で定義されます。FbxAMatrixが変換（平行移動、回転、スケール）を表す場合、行列の最後の行は、変換の平行移動部分を表します。
			// メッシュ空間からボーン空間へ位置をトランスフォームする 'bone.offset_transform' 行列を構成する。この行列をオフセット行列と呼びます。
			bone.offset_transform = to_xmfloat4x4(cluster_global_init_position.Inverse() * reference_global_init_position);


		}
	}
}
// UNIT.25
void Skinned_Mesh::fetch_animations(FbxScene* fbx_scene, std::vector<animation>& animation_clips, float sampling_rate /*If this value is 0, the animation data will be sampled at the default frame rate.*/)
{
	FbxArray<FbxString*> animation_stack_names;
	fbx_scene->FillAnimStackNameArray(animation_stack_names);
	const int animation_stack_count{ animation_stack_names.GetCount() };
	for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
	{
		animation& animation_clip{ animation_clips.emplace_back() };
		animation_clip.name = animation_stack_names[animation_stack_index]->Buffer();

		FbxAnimStack* animation_stack{ fbx_scene->FindMember<FbxAnimStack>(animation_clip.name.c_str()) };
		fbx_scene->SetCurrentAnimationStack(animation_stack);

		const FbxTime::EMode time_mode{ fbx_scene->GetGlobalSettings().GetTimeMode() };
		FbxTime one_second;
		one_second.SetTime(0, 0, 1, 0, 0, time_mode);
		animation_clip.sampling_rate = sampling_rate > 0 ? sampling_rate : static_cast<float>(one_second.GetFrameRate(time_mode));
		const FbxTime sampling_interval{ static_cast<FbxLongLong>(one_second.Get() / animation_clip.sampling_rate) };

		const FbxTakeInfo* take_info{ fbx_scene->GetTakeInfo(animation_clip.name.c_str()) };
		const FbxTime start_time{ take_info->mLocalTimeSpan.GetStart() };//アニメーションタイマー
		const FbxTime stop_time{ take_info->mLocalTimeSpan.GetStop() };
		for (FbxTime time = start_time; time < stop_time; time += sampling_interval)
		{
			animation::keyframe& keyframe{ animation_clip.sequence.emplace_back() };

			const size_t node_count{ scene_view.nodes.size() };
			keyframe.nodes.resize(node_count);
			for (size_t node_index = 0; node_index < node_count; ++node_index)
			{
				FbxNode* fbx_node{ fbx_scene->FindNodeByName(scene_view.nodes.at(node_index).name.c_str()) };
				if (fbx_node)
				{
					animation::keyframe::node& node{ keyframe.nodes.at(node_index) };
					// 'global_transform' is a transformation matrix of a node with respect to the scene's global coordinate system.
					//'global_transform' は、シーンのグローバル座標系に対するノードの変換行列である。
					node.global_transform = to_xmfloat4x4(fbx_node->EvaluateGlobalTransform(time));

					//UNIT27
					// 'local_transform' is a transformation matrix of a node with respect to
					 // its parent's local coordinate system.// 親のローカル座標系に関するノードの変換行列です。
					 const FbxAMatrix & local_transform{ fbx_node->EvaluateLocalTransform(time) };
					node.scaling = to_xmfloat3(local_transform.GetS());
					node.rotation = to_xmfloat4(local_transform.GetQ());
					node.translation = to_xmfloat3(local_transform.GetT());

				}
#if 0
				else
				{
					animation::keyframe::node& keyframe_node{ keyframe.nodes.at(node_index) };
					keyframe_node.global_transform = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
					keyframe_node.translation = { 0, 0, 0 };
					keyframe_node.rotation = { 0, 0, 0 };
					keyframe_node.scaling = { 1, 1, 1 };
				}
#endif
			}
		}
	}
	for (int animation_stack_index = 0; animation_stack_index < animation_stack_count; ++animation_stack_index)
	{
		delete animation_stack_names[animation_stack_index];
	}
				}

void Skinned_Mesh::update_animation(animation::keyframe& keyframe)
{
	size_t node_count{ keyframe.nodes.size() };
	for (size_t node_index = 0; node_index < node_count; ++node_index)
	{
		animation::keyframe::node& node{ keyframe.nodes.at(node_index) };
		XMMATRIX S{ XMMatrixScaling(node.scaling.x, node.scaling.y, node.scaling.z) };
		XMMATRIX R{ XMMatrixRotationQuaternion(XMLoadFloat4(&node.rotation)) };
		XMMATRIX T{ XMMatrixTranslation(node.translation.x, node.translation.y, node.translation.z) };

		int64_t parent_index{ scene_view.nodes.at(node_index).parent_index };
		XMMATRIX P{ parent_index < 0 ? XMMatrixIdentity() :
		XMLoadFloat4x4(&keyframe.nodes.at(parent_index).global_transform) };

		XMStoreFloat4x4(&node.global_transform, S * R * T * P);
	}
}

bool Skinned_Mesh::append_animations(const char* animation_filename, float sampling_rate)
{
	FbxManager * fbx_manager{ FbxManager::Create() };
	FbxScene * fbx_scene{ FbxScene::Create(fbx_manager, "") };
	
	 FbxImporter * fbx_importer{ FbxImporter::Create(fbx_manager, "") };
	bool import_status{ false };
	import_status = fbx_importer->Initialize(animation_filename);
	_ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());
	 import_status = fbx_importer->Import(fbx_scene);
	 _ASSERT_EXPR_A(import_status, fbx_importer->GetStatus().GetErrorString());
	
	 fetch_animations(fbx_scene, animation_clips, sampling_rate);//アニメーション生成	
	
	 fbx_manager->Destroy();
	
	 return true;
}

void Skinned_Mesh::blend_animations(const animation::keyframe* keyframes[2], float factor,
	animation::keyframe& keyframe)
{
	size_t node_count{ keyframes[0]->nodes.size() };
	keyframe.nodes.resize(node_count);
	for (size_t node_index = 0; node_index < node_count; ++node_index)
	{
		XMVECTOR S[2]{
		XMLoadFloat3(&keyframes[0]->nodes.at(node_index).scaling),
		 XMLoadFloat3(&keyframes[1]->nodes.at(node_index).scaling) };
		XMStoreFloat3(&keyframe.nodes.at(node_index).scaling, XMVectorLerp(S[0], S[1], factor));

		XMVECTOR R[2]{
		XMLoadFloat4(&keyframes[0]->nodes.at(node_index).rotation),
		XMLoadFloat4(&keyframes[1]->nodes.at(node_index).rotation) };
		XMStoreFloat4(&keyframe.nodes.at(node_index).rotation, XMQuaternionSlerp(R[0], R[1], factor));

		XMVECTOR T[2]{ XMLoadFloat3(&keyframes[0]->nodes.at(node_index).translation),
		XMLoadFloat3(&keyframes[1]->nodes.at(node_index).translation) };
		XMStoreFloat3(&keyframe.nodes.at(node_index).translation, XMVectorLerp(T[0], T[1], factor));
	}
}

bool Skinned_Mesh::raycast(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT4& direction,
	const DirectX::XMFLOAT4X4& world_transform, DirectX::XMFLOAT4& closest_point,
	DirectX::XMFLOAT3& intersected_normal, std::string& intersected_mesh, std::string& intersected_material)
{
	float closest_distance{ FLT_MAX };

	for (const mesh& mesh : meshes)
	{
		XMFLOAT4 ray_position = position;
		XMFLOAT4 ray_direction = direction;
		ray_direction.w = 0;

		// Convert to model space.モデル空間に変換します
		XMMATRIX concatenated_matrix{
			XMLoadFloat4x4(&mesh.default_global_transform) *
			XMLoadFloat4x4(&world_transform) };
		XMMATRIX inverse_concatenated_matrix{ XMMatrixInverse(nullptr, concatenated_matrix) };//反転した行列
		XMStoreFloat4(&ray_position, XMVector3TransformCoord(XMLoadFloat4(&ray_position), inverse_concatenated_matrix));//変換した始点
		XMStoreFloat4(&ray_direction, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat4(&ray_direction), inverse_concatenated_matrix)));//変換した終点

#if 1
		const float* min{ reinterpret_cast<const float*>(&mesh.bounding_box[0]) };
		const float* max{ reinterpret_cast<const float*>(&mesh.bounding_box[1]) };
		if (!intersect_ray_aabb(reinterpret_cast<const float*>(&ray_position), reinterpret_cast<const float*>(&ray_direction), min, max))//バウンディングボックスで確認
		{
			continue;
		}
#endif

		float distance{ 1.0e+7f };//レイの距離
		XMFLOAT4 intersection{};//レイの当たった場所（交点)
		const float* vertex_positions{ reinterpret_cast<const float*>(mesh.vertices.data()) };
		const uint32_t* indices{ mesh.indices.data() };
		const size_t index_count{ mesh.indices.size() };//最大数

		const int intersected_triangle_index{ intersect_ray_triangles(vertex_positions, 0L, sizeof(vertex), indices, index_count, ray_position, ray_direction, intersection, distance) };
		if (intersected_triangle_index >= 0)
		{
			if (closest_distance > distance)
			{
				closest_distance = distance;
				// Convert model space to original space.モデル空間を元の空間に変換します。
				XMStoreFloat4(&closest_point, XMVector3TransformCoord(XMLoadFloat4(&intersection), concatenated_matrix));//当たった場所
				XMStoreFloat3(&intersected_normal, XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat3(&mesh.vertices.at(indices[intersected_triangle_index * 3]).normal), concatenated_matrix)));
				intersected_mesh = mesh.name;
				intersected_material = mesh.find_subset(intersected_triangle_index * 3)->material_name;
			}
		}
	}
	return closest_distance < FLT_MAX;
}