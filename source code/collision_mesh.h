#pragma once
#include <d3d11.h>
#include <directxmath.h>

#include <vector>
#include <string>

class collision_mesh
{
public:
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
	};
	std::vector<mesh> meshes;

	collision_mesh(ID3D11Device* device, const std::string& filename, bool triangulate = false);

	// The coordinate system of all function arguments is world space.
	bool raycast(_In_ DirectX::XMFLOAT3 ray_position, _In_ DirectX::XMFLOAT3 ray_direction, _In_ const DirectX::XMFLOAT4X4& transform, _Out_ DirectX::XMFLOAT3& intersection_position, _Out_ DirectX::XMFLOAT3& intersection_normal,
		_Out_ std::string& intersection_mesh, _Out_ std::string& intersection_material, _In_ float ray_length_limit = 1.0e+7f, _In_ bool skip_if = false/*Once the first intersection is found, the process is interrupted.*/) const;
};

