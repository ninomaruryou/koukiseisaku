#pragma once

//#include"kart.h"
//#include"CameraController.h"
#include "Scene.h"
#include "sprite_batch.h"
//#include "Stage.h"
//#include "Camera.h"
#include <xaudio2.h>
#include "audio.h"
//#include "UI.h"
#include "../imgui/imgui.h"
#include "fullscreen_quad.h"
#include <mutex>
//#include"EffectManager.h"
//#include "Effect.h"
//#include "anime_Biking.h"
#include "Models.h"
#include "shadow map.h"
#include "COM_obj_.h"
#include "framebuffer.h"
#include "skinned_mesh.h"
#include "../source code/animation.h"
#include "ProjectileManager.h"
#include "ProjectileStaight.h"
#include "../source code/Camera.h"
#include "collision_mesh.h"

using namespace DirectX;
// �Q�[���V�[��
class SceneGame : public Scene
{
public:
    SceneGame(){}
    ~SceneGame() override {}

    Microsoft::WRL::ComPtr<IDXGIAdapter3> adapter;

    ComPtr<ID3D11SamplerState> sampler_states[3]; //�T���v�����O���@�̎w��

    size_t video_memory_usage()
    {
        DXGI_QUERY_VIDEO_MEMORY_INFO video_memory_info;
        adapter->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &video_memory_info);
        return video_memory_info.CurrentUsage / 1024 / 1024;
    }

    // ������
    void Initialize(ID3D11Device* device) override;

    // �I����
    void Finalize() override;

    // �X�V����
    void Update(ID3D11Device* device,float elapsedTime) override;

    // �`�揈��
    void Render(ID3D11DeviceContext* immediate_context, float elapsedTime) override;

    void IMGUIRender();

    int camrea_count = 4;


    std::vector<std::unique_ptr<Camera>> cameras;

private:
    struct Light
    {
        DirectX::XMFLOAT4 light = { 0,-1,1,1 };

    }light_;

    DirectX::XMFLOAT3 Translation = {};

    std::unique_ptr <Sprite_Batch> sprite_batches[8];

    std::unique_ptr<framebuffer> framebuffers[8];

    //std::unique_ptr<Shadow_model> Shadow[8];

    //UNIT32
    std::unique_ptr<fullscreen_quad> bit_block_transfer;

    ComPtr<ID3D11PixelShader> pixel_shaders[8]; //�s�N�Z���V�F�[�_�[

    //�f�v�X�X�e���V���`��̕��@
    ComPtr<ID3D11DepthStencilState> depth_stencil_states[4];

    ComPtr<ID3D11RasterizerState> rasterizer_states[4]; //���X�^���C�U�[

    // SKYMAP
    ComPtr<ID3D11ShaderResourceView> skymap;

    std::unique_ptr<Skinned_Mesh> Skinned_Meshes[8];

    std::string material_name;

    DirectX::XMFLOAT4X4 stage_world;
    std::unique_ptr<Static_mesh_instncing> static_mesh_instncings[8];

    XMFLOAT4 color;

    DirectX::XMFLOAT3 camera_position = {0,2,10};

    const uint32_t shadowmap_width = 2048;
    const uint32_t shadowmap_height = 2048;

    //Shadowmap shadowmap;
    std::unique_ptr<Shadow> double_speed_z;

    struct Parametric
    {
        float extraction_threshold{ 0.8f };
        float gaussian_sigma{ 1.0f };
        float bloom_intensity{ 0.0f };
        float exposure{ 1.0f };
    }parametric;

    DirectX::XMFLOAT4X4 light_view_projection;


    struct scene_constants//�萔�o�b�t�@�\����()��ɃJ����
    {
        DirectX::XMFLOAT4X4 view_projection; //�r���[�E�v���W�F�N�V�����ϊ��s��
        DirectX::XMFLOAT4 light_direction = { 1, -1, 1, 0 }; //���C�g�̌���
        DirectX::XMFLOAT4 camera_position;
        // SHADOW
        DirectX::XMFLOAT4X4 light_view_projection;

    };

    scene_constants data{};

    struct shadowmap_constants
    {
        DirectX::XMFLOAT4X4 light_view_projection;	// ���C�g�̈ʒu���猩���ˉe�s��
        DirectX::XMFLOAT3	shadow_color;			// �e�F
        float				shadow_bias;			// �[�x�o�C�A�X
    };

    Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffers[8];

    float				shadow_bias{ 0.008f };
    DirectX::XMFLOAT3	shadow_color{ 0.3f, 0.3f, 0.3f };

    //DirectX::XMFLOAT4X4 world_transform{ -0.01f, 0, 0, 0, 0, 0.01f, 0, 0, 0, 0, 0.01f, 0, 0, 0, 0, 1 };
    DirectX::XMFLOAT3 Itemposition = { -25,0,0 };
    //CameraController* cameraController = nullptr;
    bool crashed = false;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_views[8];
    ComPtr<ID3D11RenderTargetView> render_target_view;//�����_�[ �^�[�Q�b�g �C���^�[�t�F�[�X

    struct Ghost_Record
    {
	    DirectX::XMFLOAT3 position;
	    DirectX::XMFLOAT3 forword;
    };

    Instncing_Character Boxs;

    std::unique_ptr<Player> player;
    std::vector<std::unique_ptr<Character>> players;
    std::unique_ptr<collision_mesh> collision_mesh;

    std::unique_ptr<GlTF_Character> Stage;

    Animation anime;

    std::vector<gltf_model::node> animated_nodes = {};
};
