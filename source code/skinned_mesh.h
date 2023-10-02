
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

using namespace DirectX;

//DirectXMath 構造体用の serialize テンプレート関数を定義
namespace DirectX
{
	template < class T>
	void serialize(T& archive, DirectX::XMFLOAT2& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y)
		);
	}

	template < class T>
	void serialize(T& archive, DirectX::XMFLOAT3& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z)
		);
	}

	template < class T>
	void serialize(T& archive, DirectX::XMFLOAT4& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w)
		);
	}

	template < class T>
	void serialize(T& archive, DirectX::XMFLOAT4X4& m)
	{
		archive(
			cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12),
			cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
			cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22),
			cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
			cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32),
			cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
			cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42),
			cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
		);
	}
}

class Skinned_Mesh
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

	struct bone_influence
	{
		uint32_t bone_index;
		float bone_weight;
	};

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
		friend class Skinned_Mesh;

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

	std::vector<animation> animation_clips;

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>        samplerStates[3];
public:
	Skinned_Mesh(ID3D11Device* device, const char* fbx_filename, bool triangulate = false, float sampling_rate = 0);
	virtual ~Skinned_Mesh() = default;


	void create_com_objects(ID3D11Device* device, const char* fbx_filename);


	void render(ID3D11DeviceContext* immediate_context,
		const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color = {1,1,1,1}, const animation::keyframe* keyframe = nullptr);


	void update_animation(animation::keyframe& keyframe);

	bool append_animations(const char* animation_filename, float sampling_rate);

	void blend_animations(const animation::keyframe* keyframes[2], float factor,
		animation::keyframe& keyframe);

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
	// UNIT.24
	void fetch_skeleton(FbxMesh* fbx_mesh, skeleton& bind_pose);
	// UNIT.25
	void fetch_animations(FbxScene* fbx_scene, std::vector<animation>& animation_clips, float sampling_rate /*If this value is 0, the animation data will be sampled at the default frame rate.*/);


	//HRESULT make_dummy_texture(ID3D11Device* device, ID3D11ShaderResourceView** shader_resource_view,
	//	DWORD value/*0xAABBGGRR*/, UINT dimension);


};
