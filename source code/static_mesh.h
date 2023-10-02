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

    //�@�T�u�Z�b�g�͑Ή�����}�e���A�����A���̃}�e���A�����g�p���郁�b�V���̃C���f�b�N�X�J�n�ԍ��ƃC���f�b�N�X��
	//��OBJ �t�@�C���̃L�[���[�h usemtl �ɑ��������񂪃}�e���A�����ŁA���̍s�ȍ~�ɘA������L�[���[�h f ������
	//�}�e���A�����g�p����|���S���ł���A���̃L�[���[�h usemtl ���o������܂œK�p�����

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

    std::vector<vertex> vertices; //���_�ʒu�Ɩ@��
    std::vector<uint32_t> indices; //�`�揇��

private://�X�v���C�g�Ƒ卷�Ȃ�
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
