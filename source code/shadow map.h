#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <directxmath.h>
#include <vector>
#include <string>
#include <fbxsdk.h>
#include <unordered_map>
#include <cereal/types/memory.hpp>
#include "Mesh.h"

using namespace DirectX;

class Shadow_map
{
public:
	static const int MAX_BONE_INFLUENCES{ 4 }; //影響を受けるボーン最大（色）
	struct vertex //いつもの
	{
		DirectX::XMFLOAT3 position;//頂点の位置
		DirectX::XMFLOAT3 normal;//法線
		DirectX::XMFLOAT4 tangent;//法線マップ
		DirectX::XMFLOAT2 texcoord; //描画位置
		float bone_weights[MAX_BONE_INFLUENCES]{ 1, 0, 0, 0 };//骨（頂点の変わりみたいなもの）
		uint32_t bone_indices[MAX_BONE_INFLUENCES]{};//骨の順番

		template < class T>
		void serialize(T& archive)
		{
			archive(position, normal, tangent, texcoord, bone_weights, bone_indices);
		}
	};

	static const int MAX_BONES{ 256 };
	struct constants
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 material_color;
		DirectX::XMFLOAT4X4 bone_transforms[MAX_BONES]{ { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };//ボーンの位置

	};

	struct instance
	{
		DirectX::XMFLOAT4X4 ndc_transform;//GPUに送るワードデータ
		//DirectX::XMFLOAT4 texcoord_transform;
		//DirectX::XMFLOAT4 color;
	};

	instance* instances;
	UINT count_instance = 0;


	struct bone_influence
	{
		uint32_t bone_index;
		float bone_weight;
	};
	//struct constants
	//{
	//	DirectX::XMFLOAT4X4 world;
	//	//DirectX::XMFLOAT4 material_color;
	//	DirectX::XMFLOAT4 Ka;
	//	DirectX::XMFLOAT4 Kd;
	//	DirectX::XMFLOAT4 Ks;

	//};

	struct subset
	{
		std::wstring usemtl;
		uint32_t index_start{ 0 }; 	// start position of index buffer
		uint32_t index_count{ 0 }; 	// number of vertices (indices)
	};
	std::vector<subset> subsets;

	struct mesh
	{
		uint64_t unique_id{ 0 };
		std::string name;
		// 'node_index' is an index that refers to the node array of the scene.
		int64_t node_index{ 0 };

		std::vector<vertex> vertices;//頂点情報
		std::vector<uint32_t> indices;//頂点順番


	private:

		skeleton bind_pose;

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
			archive(unique_id, name, node_index, subsets, default_global_transform, bind_pose, bounding_box, vertices, indices);

		}
	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> matrx_buffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;
		friend class Shadow_map;

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

	DirectX::XMFLOAT3 bounding_box[2]{ { D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX, D3D11_FLOAT32_MAX }, { -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX } };

private:

	std::vector<animation> animation_clips;


	Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> instance_buffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>        samplerStates[3];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> shadowmap_depth_stencil_view;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shadowmap_shader_resource_view;
	Microsoft::WRL::ComPtr<ID3D11Buffer> scene_constant_buffer;

public:
	Shadow_map(ID3D11Device* device, const char* fbx_filename, bool triangulate,float MAX = 1000);
	virtual ~Shadow_map() = default;

	void render(ID3D11DeviceContext* immediate_context,
		const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color);


	void Begin(ID3D11DeviceContext* immediate_context);

	void End(ID3D11DeviceContext* immediate_context);

	// UNIT.18
	void fetch_meshes(FbxScene* fbx_scene, std::vector<mesh>& meshes);
	// UNIT.18
	void fetch_materials(FbxScene* fbx_scene, std::unordered_map<uint64_t, material>& materials);

protected:
	scene scene_view;

	void create_com_objects(ID3D11Device* device, const char* fbx_filename,float MAX);

	//void create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count, uint32_t* indices, size_t index_count);
};
