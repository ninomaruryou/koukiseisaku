#pragma once
#include <d3d11.h>
 #include <wrl.h>
#include <directxmath.h>
#include <string>
#include <unordered_map>

#include "../tinygltf-release/tiny_gltf.h"

using namespace DirectX;


class gltf_model
{
	std::string filename;
public:
	gltf_model(ID3D11Device* device, const std::string& filename,bool cereal = false);
	virtual ~gltf_model() = default;
	struct scene
	{
		std::string name;
		std::vector<int> nodes; // Array of 'root' nodes「ルート」ノードの配列

		template < class T>
		void serialize(T& archive)
		{
			archive(name, nodes);
		}

	};
	std::vector<scene> scenes;

	//① gltf_model クラスに node 構造体とコンテナ変数を定義する
	//ノードデータの取得
	struct node
	{
		std::string name;
		int skin{ -1 }; // index of skin referenced by this node このノードによって参照されるスキンのインデックス
		int mesh{ -1 }; // index of mesh referenced by this node このノードによって参照されるメッシュのインデックス

		std::vector<int> children; // An array of indices of child nodes of this node (このノードの子ノードのインデックスの配列)

		// Local transforms ローカル変換
		DirectX::XMFLOAT4 rotation{ 0, 0, 0, 1 };
		DirectX::XMFLOAT3 scale{ 1, 1, 1 };
		DirectX::XMFLOAT3 translation{ 0, 0, 0 };

		DirectX::XMFLOAT4X4 global_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };




		template < class T>
		void serialize(T& archive)
		{
			archive(name,skin,mesh,children,rotation,scale,translation,global_transform);
		}

	};
	std::vector<node> nodes;


	struct buffer_view
	{
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN;
		Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
		size_t stride_in_bytes{ 0 };
		size_t size_in_bytes{ 0 };
		
		template < class T>
		void serialize(T& archive)
		{
			archive(format, buffer, stride_in_bytes, size_in_bytes);
		}

		size_t count() const
		{
			return size_in_bytes / stride_in_bytes;
		}


	};

	struct mesh
	{
		std::string name;
		struct subset
		{
			std::string material_name;
			std::vector<DirectX::XMFLOAT3> positions;

		};
		std::vector<subset> subsets;

		DirectX::XMFLOAT3 bounding_box[2]
		{
			{ +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX },
			{ -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX }
		};

		struct primitive
		{
			int material;
			std::map<std::string, buffer_view> vertex_buffer_views;
			buffer_view index_buffer_view;


			size_t index_count;
			size_t vertex_count;

			std::vector<DirectX::XMFLOAT3> positions;
			std::vector<DirectX::XMFLOAT3> normals;

			std::vector<int> indices;




			// The coordinate values of the bounding box (AABB) of the entire model in the global coordinate system.
			DirectX::XMFLOAT3 min_value = { +std::numeric_limits<float>::max(), +std::numeric_limits<float>::max(), +std::numeric_limits<float>::max() };
			DirectX::XMFLOAT3 max_value = { -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max() };


			template < class T>
			void serialize(T& archive)
			{
				archive(material,vertex_buffer_views,index_buffer_view);
			}

		};
		std::vector<primitive> primitives;


		template < class T>
		void serialize(T& archive)
		{
			archive(name, primitives);
		}

	};
	std::vector<mesh> meshes;

	// RAYCAST
	DirectX::XMFLOAT4X4 default_global_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	DirectX::XMFLOAT4X4 global_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

	//シェーダーオブジェクトの定義
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
	struct primitive_constants
	{
		DirectX::XMFLOAT4X4 world;
		int material{ -1 };
		int has_tangent{ 0 };
		int skin{ -1 };
		int pad;

	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> primitive_cbuffer;

	struct texture_info
	{
		int index = -1;
		int texcoord = 0;

		template < class T>
		void serialize(T& archive)
		{
			archive(index, texcoord);
		}

	};
	struct normal_texture_info
	{
		int index = -1;
		int texcoord = 0;
		float scale = 1;
		template < class T>
		void serialize(T& archive)
		{
			archive(index, texcoord, scale);
		}

	};
	struct occlusion_texture_info
	{
		int index = -1;
		int texcoord = 0;
		float strength = 1;
		template < class T>
		void serialize(T& archive)
		{
			archive(index, texcoord, strength);
		}


	};
	struct pbr_metallic_roughness
	{
		float basecolor_factor[4] = { 1, 1, 1, 1 };
		texture_info basecolor_texture;
		float metallic_factor = 1;
		float roughness_factor = 1;
		texture_info metallic_roughness_texture;
		template < class T>
		void serialize(T& archive)
		{
			archive(basecolor_factor, basecolor_texture, metallic_factor, roughness_factor, metallic_roughness_texture);
		}


	};
	struct material {
		std::string name;
		struct cbuffer
		{
			float emissive_factor[3] = { 0, 0, 0 };
			int alpha_mode = 0; // "OPAQUE" : 0, "MASK" : 1, "BLEND" : 2
			float alpha_cutoff = 0.5f;
			bool double_sided = false;

			pbr_metallic_roughness pbr_metallic_roughness;

			normal_texture_info normal_texture;
			occlusion_texture_info occlusion_texture;
			texture_info emissive_texture;
			template < class T>
			void serialize(T& archive)
			{
				archive(emissive_factor, alpha_mode, alpha_cutoff, double_sided,pbr_metallic_roughness,normal_texture,occlusion_texture,emissive_texture);
			}

		};
		cbuffer data;

		template < class T>
		void serialize(T& archive)
		{
			archive(name,data);
		}

	};
	std::vector<material> materials;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> material_resource_view;

	struct texture
	{
		std::string name;
		int source{ -1 };
		template < class T>
		void serialize(T& archive)
		{
			archive(name,source);
		}

	};
	std::vector<texture> textures;
	struct image
	{
		std::string name;
		int width{ -1 };
		int height{ -1 };
		int component{ -1 };
		int bits{ -1 };
		int pixel_type{ -1 };
		int buffer_view;
		std::string mime_type;
		std::string uri;
		bool as_is{ false };

		template < class T>
		void serialize(T& archive)
		{
			archive(name, width, height, component, bits, pixel_type, buffer_view, mime_type, uri, as_is);
		}

	};

	//struct batched_buffer
	//{
	//	size_t index_count;
	//	size_t vertex_count;

	//	D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	//	std::vector<unsigned int> indices;
	//	struct structure_of_arrays
	//	{
	//			DirectX::XMFLOAT3 positions;
	//			DirectX::XMFLOAT3 normals;

	//		//std::vector<DirectX::XMFLOAT4> tangents;
	//		//std::vector<DirectX::XMFLOAT2> texcoords_0;
	//		//std::vector<DirectX::XMFLOAT2> texcoords_1;
	//	};
	//	std::vector<structure_of_arrays> vertices;
	//	DirectX::XMFLOAT3 min_value = { +std::numeric_limits<float>::max(), +std::numeric_limits<float>::max(), +std::numeric_limits<float>::max() };
	//	DirectX::XMFLOAT3 max_value = { -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max() };
	//};
	//std::unordered_map<int/*material*/, batched_buffer> batched_buffers;


	std::vector<image> images;

	struct skin
	{
		std::vector<DirectX::XMFLOAT4X4> inverse_bind_matrices;
		std::vector<int> joints;
		template < class T>
		void serialize(T& archive)
		{
			archive(inverse_bind_matrices,joints);
		}

	};
	std::vector<skin> skins;

	//④gltf_model クラスにボーン行列の構造体と定数バッファを定義する
	static const size_t PRIMITIVE_MAX_JOINTS = 512;
	struct primitive_joint_constants
	{
		DirectX::XMFLOAT4X4 matrices[PRIMITIVE_MAX_JOINTS];
		template < class T>
		void serialize(T& archive)
		{
			archive(matrices);
		}

	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> primitive_joint_cbuffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer> world_buffer;


	struct animation
	{
		std::string name;

		struct channel
		{
			int sampler{ -1 };
			int target_node{ -1 };
			std::string target_path;
			template < class T>
			void serialize(T& archive)
			{
				archive(sampler,target_node,target_path);
			}

		};
		std::vector<channel> channels;

		struct sampler
		{
			int input{ -1 };
			int output{ -1 };
			std::string interpolation;
			template < class T>
			void serialize(T& archive)
			{
				archive(input,output,interpolation);
			}

		};
		std::vector<sampler> samplers;

		std::unordered_map<int, std::vector<float>> timelines;
		std::unordered_map<int, std::vector<DirectX::XMFLOAT3>> scales;
		std::unordered_map<int, std::vector<DirectX::XMFLOAT4>> rotations;
		std::unordered_map<int, std::vector<DirectX::XMFLOAT3>> translations;

		template < class T>
		void serialize(T& archive)
		{
			archive(name, channels, samplers,timelines,scales,rotations,translations);
		}

	};
	std::vector<animation> animations;


	std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> texture_resource_views;

	void gltf_model::fetch_nodes(const tinygltf::Model& gltf_model);

	void gltf_model::cumulate_transforms(std::vector<node>& nodes);

	gltf_model::buffer_view make_buffer_view(const tinygltf::Accessor& accessor);

	//meshes 読み込み
	void fetch_meshes(ID3D11Device* device,tinygltf::Model& gltf_model);

	void fetch_materials(ID3D11Device* device, const tinygltf::Model& gltf_model);
	void render(ID3D11DeviceContext* immediate_context, const DirectX::XMFLOAT4X4& world, const std::vector<node>& animated_nodes, ID3D11PixelShader** replacement_pixel_shader/*SHADOW*/);

	void fetch_textures(ID3D11Device* device, const tinygltf::Model& gltf_model);

	void fetch_animations(const tinygltf::Model& gltf_model);

	void animate(size_t animation_index, float time, std::vector<node>& animated_nodes, bool loopback);

	bool raycast(_In_ DirectX::XMFLOAT3 ray_position, _In_ DirectX::XMFLOAT3 ray_direction, _In_ const DirectX::XMFLOAT4X4& transform, _Out_ DirectX::XMFLOAT3& intersection_position, _Out_ DirectX::XMFLOAT3& intersection_normal,
		_Out_ std::string& intersection_mesh, _Out_ std::string& intersection_material, _In_ float ray_length_limit = 1.0e+7f, _In_ bool skip_if = false/*Once the first intersection is found, the process is interrupted.*/) const;

	UINT uint;

};

template <typename T>
static void compute_normals(const DirectX::XMFLOAT3* positions, size_t vertex_count, const T* indices, size_t index_count, DirectX::XMFLOAT3* normals)
{
	for (size_t vertex_index = 0; vertex_index < vertex_count; ++vertex_index)
	{
		normals[vertex_index].x = 0;
		normals[vertex_index].y = 0;
		normals[vertex_index].z = 0;
	}

	for (size_t index_index = 0; index_index < index_count; index_index += 3)
	{
		const T i1 = indices[index_index + 0];
		const T i2 = indices[index_index + 1];
		const T i3 = indices[index_index + 2];

		const DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&positions[i1]);
		const DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&positions[i2]);
		const DirectX::XMVECTOR v3 = DirectX::XMLoadFloat3(&positions[i3]);

		DirectX::XMFLOAT3 face_normal;
		DirectX::XMStoreFloat3(&face_normal, DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v2, v1), DirectX::XMVectorSubtract(v3, v1))));

		DirectX::XMStoreFloat3(&normals[i1], DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&face_normal), DirectX::XMLoadFloat3(&normals[i1])));
		DirectX::XMStoreFloat3(&normals[i2], DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&face_normal), DirectX::XMLoadFloat3(&normals[i2])));
		DirectX::XMStoreFloat3(&normals[i3], DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&face_normal), DirectX::XMLoadFloat3(&normals[i3])));
	}

	for (size_t vertex_index = 0; vertex_index < vertex_count; ++vertex_index)
	{
		DirectX::XMStoreFloat3(&normals[vertex_index], DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&normals[vertex_index])));
	}
}

template <typename T>
static void compute_tangents(const DirectX::XMFLOAT3* positions, const DirectX::XMFLOAT3* normals, const DirectX::XMFLOAT2* texcoords, size_t vertex_count, const T* indices, size_t index_count, DirectX::XMFLOAT4* tangents)
{
	std::unique_ptr<DirectX::XMFLOAT3[]> tan1 = std::make_unique<DirectX::XMFLOAT3[]>(vertex_count);
	std::unique_ptr<DirectX::XMFLOAT3[]> tan2 = std::make_unique<DirectX::XMFLOAT3[]>(vertex_count);

	for (size_t index_index = 0; index_index < index_count; index_index += 3)
	{
		const T i1 = indices[index_index + 0];
		const T i2 = indices[index_index + 1];
		const T i3 = indices[index_index + 2];

		const DirectX::XMFLOAT3 v1 = positions[i1];
		const DirectX::XMFLOAT3 v2 = positions[i2];
		const DirectX::XMFLOAT3 v3 = positions[i3];

		const DirectX::XMFLOAT2 w1 = texcoords[i1];
		const DirectX::XMFLOAT2 w2 = texcoords[i2];
		const DirectX::XMFLOAT2 w3 = texcoords[i3];

		const float x1 = v2.x - v1.x;
		const float x2 = v3.x - v1.x;
		const float y1 = v2.y - v1.y;
		const float y2 = v3.y - v1.y;
		const float z1 = v2.z - v1.z;
		const float z2 = v3.z - v1.z;

		const float s1 = w2.x - w1.x;
		const float s2 = w3.x - w1.x;
		const float t1 = w2.y - w1.y;
		const float t2 = w3.y - w1.y;

		float denominator = s1 * t2 - s2 * t1;
		if (denominator == 0.0f)
		{
			denominator = 1.0f;
		}
		const float r = 1.0f / denominator;

		const DirectX::XMFLOAT3 sdir = { (t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r };
		const DirectX::XMFLOAT3 tdir = { (s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r };

		tan1[i1] = { tan1[i1].x + sdir.x, tan1[i1].y + sdir.y, tan1[i1].z + sdir.z };
		tan1[i2] = { tan1[i2].x + sdir.x, tan1[i2].y + sdir.y, tan1[i2].z + sdir.z };
		tan1[i3] = { tan1[i3].x + sdir.x, tan1[i3].y + sdir.y, tan1[i3].z + sdir.z };

		tan2[i1] = { tan2[i1].x + tdir.x, tan2[i1].y + tdir.y, tan2[i1].z + tdir.z };
		tan2[i2] = { tan2[i2].x + tdir.x, tan2[i2].y + tdir.y, tan2[i2].z + tdir.z };
		tan2[i3] = { tan2[i3].x + tdir.x, tan2[i3].y + tdir.y, tan2[i3].z + tdir.z };
	}

	for (size_t vertex_index = 0; vertex_index < vertex_count; ++vertex_index)
	{
		DirectX::XMVECTOR n = DirectX::XMLoadFloat3(&normals[vertex_index]);
		DirectX::XMVECTOR t = DirectX::XMLoadFloat3(&tan1[vertex_index]);

		// Gram-Schmidt orthogonalize        
		DirectX::XMStoreFloat4(&tangents[vertex_index], DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(t, DirectX::XMVectorMultiply(n, DirectX::XMVector3Dot(n, t)))));
		// Calculate handedness        
		tangents[vertex_index].w = (DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMVector3Cross(n, t), DirectX::XMLoadFloat3(&tan2[vertex_index]))) < 0.0f) ? -1.0f : +1.0f;
	}
}
