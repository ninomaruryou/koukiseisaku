#pragma once
#include <directxmath.h>

#include <d3d11.h>
#include <vector>
#include <string>
#include <fbxsdk.h>
#include <cereal/archives/binary.hpp>



struct scene
{
	struct node
	{
		uint64_t unique_id{ 0 };
		std::string name;
		FbxNodeAttribute::EType attribute{ FbxNodeAttribute::EType::eUnknown };
		int64_t parent_index{ -1 };

		template < class T>
		void serialize(T& archive)
		{
			archive(unique_id, name, attribute, parent_index);
		}
	};
	std::vector<node> nodes;

	template < class T>
	void serialize(T& archive)
	{
		archive(nodes);
	}
	int64_t indexof(uint64_t unique_id) const
	{
		int64_t index{ 0 };
		for (const node& node : nodes)
		{
			if (node.unique_id == unique_id)
			{
				return index;
			}
			++index;
		}
		return -1;
	}

#if 0

	int64_t indexof(uint64_t unique_id) const
	{
		int64_t index{ 0 };
		for (const node& node : nodes)
		{
			if (node.unique_id == unique_id)
			{
				return index;
			}
			++index;
		}
		return -1;
	}
#endif

};

struct animation
{
	std::string name;
	float sampling_rate{ 0 };//フレーム

	struct keyframe
	{
		struct node
		{
			// 'global_transform' is used to convert from local space of node to global space of scene.
			DirectX::XMFLOAT4X4 global_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
			// The transformation data of a node includes its translation, rotation and scaling vectors
			 // with respect to its parent. 
			DirectX::XMFLOAT3 scaling{ 1, 1, 1 };
			DirectX::XMFLOAT4 rotation{ 0, 0, 0, 1 }; // Rotation quaternion
			DirectX::XMFLOAT3 translation{ 0, 0, 0 };

			template < class T>
			void serialize(T& archive)
			{
				archive(global_transform, scaling, rotation, translation);
			}
		};
		std::vector<node> nodes;
		template < class T>
		void serialize(T& archive)
		{
			archive(nodes);
		}
	};
	std::vector<keyframe> sequence;

	template < class T>
	void serialize(T& archive)
	{
		archive(name, sampling_rate, sequence);
	}
};
//シリアライズしたいデータを持つ構造体に serialize テンプレート関数を定義する

struct skeleton
{
	struct bone
	{

		uint64_t unique_id{ 0 };
		std::string name;
		// 'parent_index' is index that refers to the parent bone's position in the array that contains itself.
		int64_t parent_index{ -1 }; // -1 : the bone is orphan
	   // 'node_index' is an index that refers to the node array of the scene.
		int64_t node_index{ 0 };

		// 'offset_transform' is used to convert from model(mesh) space to bone(node) scene.
		DirectX::XMFLOAT4X4 offset_transform{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

		bool is_orphan() const { return parent_index < 0; };

		template < class T>
		void serialize(T& archive)
		{
			archive(unique_id, name, parent_index, node_index, offset_transform);
		}
	};
	template < class T>
	void serialize(T& archive)
	{
		archive(bones);//関数にデータを入れる？
	}

	std::vector<bone> bones;
	int64_t indexof(uint64_t unique_id) const
	{
		int64_t index{ 0 };
		for (const bone& bone : bones)
		{
			if (bone.unique_id == unique_id)
			{
				return index;
			}
			++index;
		}
		return -1;
	}
};
