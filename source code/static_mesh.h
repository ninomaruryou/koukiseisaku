#pragma once
#include <cstdint>
#include <d3d11.h>
#include <directxmath.h>
#include <wrl.h>
#include <string>
#include <filesystem>
#include <vector>
//#include <WICTextureLoader.h>

# define  TEXTURE (1)

class Bounding_Box;

class  Static_Mesh
{
public:
    struct vertex
    {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT3 normal;
        DirectX::XMFLOAT2 texcoord;
    };

    //①サブセットは対応するマテリアル名、そのマテリアルを使用するメッシュのインデックス開始番号とインデックス数
	//※OBJ ファイルのキーワード usemtl に続く文字列がマテリアル名で、次の行以降に連続するキーワード f がその
	//マテリアルを使用するポリゴンであり、次のキーワード usemtl が出現するまで適用される

    struct subset
    {
        std::wstring usemtl;
        uint32_t index_start{ 0 }; // start position of index buffer
        uint32_t index_count{ 0 }; // number of vertices (indices)
    };
    std::vector<subset> subsets;

    struct constants
    {
        DirectX::XMFLOAT4X4 world;
        DirectX::XMFLOAT4 material_color;

    };

    std::unique_ptr<Bounding_Box> bounding_box;

private:
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

    //std::wstring texture_filename;
    //Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
    struct material
    {
        std::wstring name;
        DirectX::XMFLOAT4 Ka{ 0.2f, 0.2f, 0.2f, 1.0f };
        DirectX::XMFLOAT4 Kd{ 0.8f, 0.8f, 0.8f, 1.0f };
        DirectX::XMFLOAT4 Ks{ 1.0f, 1.0f, 1.0f, 1.0f };
        std::wstring texture_filenames[2];
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[2];

    };
     std::vector<material> materials;
public:
    Static_Mesh(ID3D11Device* device, const wchar_t* obj_filename);
    virtual ~Static_Mesh() = default;

    void render(ID3D11DeviceContext* immediate_context,
        const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& material_color, ID3D11RasterizerState* rasterizer_state);

protected:

    void create_com_buffers(ID3D11Device* device, vertex* vertices, size_t vertex_count,
        uint32_t* indices, size_t index_count);

    HRESULT make_dummy_texture(ID3D11Device* device, ID3D11ShaderResourceView** shader_resource_view,
        DWORD value/*0xAABBGGRR*/, UINT dimension);

};

class Bounding_Box

{
    struct vertex
    {
        DirectX::XMFLOAT3 position;
    };

    struct constants
    {
        DirectX::XMFLOAT4X4 world;
        DirectX::XMFLOAT4 material_color;

    };

    std::vector<vertex> vertices; //頂点位置と法線
    std::vector<uint32_t> indices; //描画順番

private://スプライトと大差ない
    Microsoft::WRL::ComPtr<ID3D11Buffer> vertex_buffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> index_buffer;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
    Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;

public:
    Bounding_Box(ID3D11Device* device, DirectX::XMFLOAT3 max_position, DirectX::XMFLOAT3 min_position);

    virtual ~Bounding_Box() = default;

    void render(ID3D11DeviceContext * immediate_context,
        const DirectX::XMFLOAT4X4 & world, const DirectX::XMFLOAT4 & material_color);

    void create_com_buffers(ID3D11Device * device, vertex * vertices, size_t vertex_count,
        uint32_t * indices, size_t index_count);

};
