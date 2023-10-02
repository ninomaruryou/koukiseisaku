#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <vector>
#include <string>
#include <fbxsdk.h>
#include <unordered_map>
#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/unordered_map.hpp>

#include "Mesh.h"


class Static_mesh_instncing
{
public:


	struct vertex //いつもの
	{
		DirectX::XMFLOAT3 position;//頂点の位置
		DirectX::XMFLOAT3 normal;//法線
		DirectX::XMFLOAT4 tangent;//法線マップ
		DirectX::XMFLOAT2 texcoord; //描画位置

		template < class T>
		void serialize(T& archive)
		{
			archive(position, normal, tangent, texcoord);
		}
	};

	struct constants
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 material_color;
	};

	struct instance
	{
		DirectX::XMFLOAT4X4 ndc_transform;//GPUに送るワードデータ
		//DirectX::XMFLOAT4 texcoord_transform;
		DirectX::XMFLOAT4 color;
	};

	instance* instances;
	UINT count_instance = 0;

	struct bone_influence
	{
		uint32_t bone_index;
		float bone_weight;
	};
	//using bone_influences_per_control_point = std::vector<bone_influence>;//なぜこれで行けるし

	struct mesh
	{
		uint64_t unique_id{ 0 };
		std::string name;
		// 'node_index' is an index that refers to the node array of the scene.
		int64_t node_index{ 0 };

		std::vector<vertex> vertices;//頂点情報
		std::vector<uint32_t> indices;//頂点順番


	private:


		DirectX::XMFLOAT3 bounding_box[2]//境界ボックス
		{
			{ +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX },
			{ -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX }
		};

		struct subset
		{
			uint64_t material_unique_id{ 0 };
			std::string material_name;

			uint32_t start_index_location{ 0 };
			uint32_t index_count{ 0 };

			// UNIT.30
			template < class T>
			void serialize(T& archive)
			{
				archive(material_unique_id, material_name, start_index_location, index_count);
			}
		};
		std::vector<subset> subsets;
		// RAYCAST
		const subset* find_subset(uint32_t index) const
		{
			for (const subset& subset : subsets)
			{
				if (subset.start_index_location <= index && subset.start_index_location + subset.index_count > index)
				{
					return &subset;
				}
			}
			return nullptr;
		}

		DirectX::XMFLOAT4X4 default_global_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

		//UNIT30
	public:
		template < class T>
		void serialize(T& archive)
		{
			//archive(unique_id, name, node_index, subsets, default_global_transform,
			//	bind_pose, bounding_box, vertices, indices);
			archive(unique_id, name, node_index, subsets, default_global_transform, bounding_box, vertices, indices);

		}
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
		friend class Static_mesh_instncing;

	};

	std::vector<mesh> meshes;

	struct material//マテリアルデータ
	{
		uint64_t unique_id{ 0 };//データ番号
		std::string name;

		DirectX::XMFLOAT4 Ka{ 0.2f, 0.2f, 0.2f, 1.0f };
		DirectX::XMFLOAT4 Kd{ 0.8f, 0.8f, 0.8f, 1.0f };
		DirectX::XMFLOAT4 Ks{ 1.0f, 1.0f, 1.0f, 1.0f };

		std::string texture_filenames[4];
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[4];

		// UNIT30
		template < class T>
		void serialize(T& archive)
		{
			archive(unique_id, name, Ka, Kd, Ks, texture_filenames);
		}

	};

	std::unordered_map<uint64_t, material> materials;

	struct HitResult
	{
		DirectX::XMFLOAT3 position = { 0,0,0 };
		DirectX::XMFLOAT3 normal = { 0,0,0 };
		float distance = 0.0f;
		int materialIndex = -1;
	};

	int MAX_INSTNCING = 1000;


private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> instance_buffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>        samplerStates[3];
public:
	Static_mesh_instncing(ID3D11Device* device, const char* fbx_filename, bool triangulate,int MAX_INSTNCING = 1000);
	virtual ~Static_mesh_instncing() = default;


	void create_com_objects(ID3D11Device* device, const char* fbx_filename);

	void render(ID3D11DeviceContext* immediate_context,
		const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color);


	void Begin(ID3D11DeviceContext* immediate_context);

	void End(ID3D11DeviceContext* immediate_context);

	bool raycast(const DirectX::XMFLOAT4& position, const DirectX::XMFLOAT4& direction,
		const DirectX::XMFLOAT4X4& world_transform,
		DirectX::XMFLOAT4& closest_point,
		DirectX::XMFLOAT3& intersected_normal,
		std::string& intersected_mesh, std::string& intersected_material);
protected:

	scene scene_view;
	// UNIT.18
	void fetch_meshes(FbxScene* fbx_scene, std::vector<mesh>& meshes);
	// UNIT.18
	void fetch_materials(FbxScene* fbx_scene, std::unordered_map<uint64_t, material>& materials);

};
