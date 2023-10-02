#pragma once


#include <DirectXMath.h>

#include "Models.h"

using namespace DirectX;

enum camera_picture
{
    Square,
    the_way,
    Start_line
};
//�J����
class Camera
{
public:
	Camera()
     {
        eye = {};
        focus = {};
        up = {};
        front = {};
        right = {};
        view_projection = {}; //�r���[�E�v���W�F�N�V�����ϊ��s��

        V = {};//�r���[

        P = {};//�v���W�F�N�V����

     }

     ~Camera() = default;

public:
    ////�B��̃C���X�^���X�擾
    //static Camera& Instance()
    //{
    //    static Camera camera;
    //    return camera;
    //}
    //�w�����������
    void Activate(ID3D11DeviceContext* immediate_context, Character* character, float elapsedTime);


    void Updata(ID3D11DeviceContext* immediate_context, float elapsedTime, Character* character);
    //void Camera_move(float elapsedTime);

    //void Finish_Move(float elapsedTime);

    //�f�o�b�OIMGUI
    void DrawDebugGUI();

    void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);

    DirectX::XMFLOAT4X4 view_projection = {}; //�r���[�E�v���W�F�N�V�����ϊ��s��

    DirectX::XMMATRIX V = {};//�r���[

    DirectX::XMMATRIX P = {};//�v���W�F�N�V����

    DirectX::XMFLOAT3 eye = {};
    DirectX::XMFLOAT3 focus = {};
    DirectX::XMFLOAT3 up = {};
    DirectX::XMFLOAT3 front = {};
    DirectX::XMFLOAT3 right = {};

    float cameradistance = 15.0f;
    float light_view_size = 10;
    XMFLOAT3 camera_subpositiom = {0,2,0};
    float light_view_near_z{ 2.0f };
    float light_view_far_z{ 18.0f };

    DirectX::XMFLOAT4 camera_focus = { -133.858,0.0f,149.606f,1.0f };

    //DirectX::XMFLOAT4 camera_position{ -100.0f, 100.0f, 300.0f, 1.0f };
    DirectX::XMFLOAT4 camera_position{ 0.0f,0.0f,0.0f, 1.0f };
    DirectX::XMFLOAT4 light_direction{ -0.113f, -0.556f, 1.0f, 0.0f };

    bool Start_Flg = false;
    int camera_state = {};
    //kart* k;

    struct camera_size
    {
        float Width = 0;
        float Height = 0;
        float TopLeftX = 0; //������X���W
        float TopLeftY = 0; //������Y���W

        int camera_number = 0;

    };

    camera_size camerasize;

public:

    

    float t = 0;
    XMFLOAT3 angle = {};
    float maxAngleX = DirectX::XMConvertToRadians(45);
    float minAngleX = DirectX::XMConvertToRadians(-45);
    DirectX::XMFLOAT3 target = { 0,0,0 };

};