#include"Camera.h"
#include "scene.h"
#include "SceneManager.h"
#include "../imgui/imgui.h"
void Camera::Activate(ID3D11DeviceContext* immediate_context, Character* character, float elapsedTime)
{
    D3D11_VIEWPORT viewport;
    UINT num_viewports{ 1 };
    immediate_context->RSGetViewports(&num_viewports, &viewport);

    float aspect_ratio{ viewport.Width / viewport.Height };
    P = { DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30), aspect_ratio, 0.1f, 1000.0f) };

    XMVECTOR F = XMVector3Normalize(XMLoadFloat3(&character->forward));//���K��
    XMVECTOR U = XMVectorSet(0, 1, 0, 0);
    XMVECTOR R = XMVector3Normalize(XMVector3Cross(U, F));//�E�x�N�g���Z�o�i�O�ρj


    XMVECTOR P = XMLoadFloat3(&character->position);
    XMVECTOR C = XMLoadFloat4(&camera_position);
    XMVECTOR V = XMVector3Normalize(C - P);

    // �J����
    using namespace DirectX;

    const float focal_length = cameradistance; //�œ_�����i�J�����ƃv���C���[�̋����j
    XMStoreFloat4(&camera_position, P + V * focal_length);
    const float liftup = 0.0f;//�J�����̍�����ݒ�
    camera_position.y = character->position.y + liftup;
    camera_position.w = 1.0f;

    DirectX::XMVECTOR eye{ DirectX::XMVectorSet(camera_position.x, camera_position.y + 2,camera_position.z , 1.0f) };
    //DirectX::XMVECTOR eye{ DirectX::XMVectorSet(0.0f,5.0f,-20.0f,1.0f) };
    DirectX::XMVECTOR up{ DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f) };

    DirectX::XMVECTOR focus{ DirectX::XMVectorSet(camera_focus.x,camera_focus.y,camera_focus.z, 1.0f) };//�Q�[�v���R���Q��
    this->V = { DirectX::XMMatrixLookAtLH(eye, focus, up) };

    DirectX::XMStoreFloat4x4(&view_projection, this->V * this->P);

}

//void Camera::Camera_move(float elapsedTime)
//{
//    //�X�^�[�g���C���J�����f��
//    if (0.0f < elapsedTime < 3.0f)
//    {
//        XMVECTOR V1 = XMLoadFloat4(&camera_position2) - XMLoadFloat4(&camera_position1);//�P�ƂQ�̋���
//        XMVECTOR Pt = XMLoadFloat4(&camera_position1) + elapsedTime * V1;
//        XMStoreFloat4(&camera_position, Pt);
//    }
//}
//
//void Camera::Finish_Move(float elapsedTime)
//{
//    //XMStoreFloat4(&camera_position, XMVectorAdd(XMLoadFloat3(&k->position), XMVectorScale(XMVectorNegate(XMVector3Normalize(XMLoadFloat3(&k->forward))), -cameradistance)));
//
//    //goal_angle += elapsedTime;
//    XMVECTOR V = XMLoadFloat4(&camera_position) - XMLoadFloat4(&camera_focus);
//    XMMATRIX R = XMMatrixRotationRollPitchYaw(0, 0, angle);
//    XMVECTOR V1 = XMVector3TransformNormal(V, R);
//    XMStoreFloat4(&camera_position, XMLoadFloat4(&camera_focus) + V1);
//    camera_position.w = 1;
//}

void Camera::DrawDebugGUI()
{
#ifdef USE_IMGUI

    ImGui::Begin("camera");



    //ImGui::SliderFloat("camera_PosX ", &camera_position.x, -500.0f, 500.0f);
    //ImGui::SliderFloat("camera_PosY ", &camera_position.y, -500.0f, 500.0f);
    //ImGui::SliderFloat("camera_PosZ ", &camera_position.z, -500.0f, 500.0f);
    ImGui::SliderFloat("subpositiomX ", &camera_subpositiom.x, -100.0f, 100.0f);
    ImGui::SliderFloat("subpositiomY ", &camera_subpositiom.y, -100.0f, 100.0f);
    ImGui::SliderFloat("subpositiomZ ", &camera_subpositiom.z, -100.0f, 100.0f);

    ImGui::SliderFloat("cameradistance ", &cameradistance, -100.0f, 100.0f);

    //ImGui::SliderFloat("camera_focus.x ", &camera_focus.x, -500.0f, 500.0f);
    //ImGui::SliderFloat("camera_focus.y ", &camera_focus.y, -500.0f, 500.0f);
    //ImGui::SliderFloat("camera_focus.z ", &camera_focus.z, -500.0f, 500.0f);
    ImGui::End();
#endif
}

void Camera::SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
    //���_�A���~�X�A���������r���[�s����쐬
    DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
    DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
    DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
    DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);

    DirectX::XMStoreFloat4x4(&view_projection, View * P);

    //�r���[���t�s�񉻂��A���[���h�s��ɖ߂�
    DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
    DirectX::XMFLOAT4X4 world;
    DirectX::XMStoreFloat4x4(&world, World);
    //�J�����̕��������o��


    this->right.x = world._11;
    this->right.y = world._12;
    this->right.z = world._13;


    this->up.x = world._21;
    this->up.y = world._22;
    this->up.z = world._23;

    this->front.x = world._31;
    this->front.y = world._32;
    this->front.z = world._33;
    //���_�A�����X��ۑ�
    this->eye = eye;
    this->focus = focus;

}

void Camera::Updata(ID3D11DeviceContext* immediate_context, float elapsedTime, Character* character)
{
    D3D11_VIEWPORT viewport;
    UINT num_viewports{ 1 };
    immediate_context->RSGetViewports(&num_viewports, &viewport);

    float aspect_ratio{ viewport.Width / viewport.Height };
    P = { DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(30), aspect_ratio, 0.1f, 100.0f) };


    ////�J������]�l����]�s��ɕϊ�
    DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
    ////��]�s�񂩂�S�����x�N�g��(z)�����o��
    DirectX::XMVECTOR Front = Transform.r[2];
    DirectX::XMFLOAT3 front;
    DirectX::XMStoreFloat3(&front, Front);

    ////�����_������x�N�g�������Ɉ�苗�����ꂽ�J�������_�����߂�

    //DirectX::XMVECTOR eye = DirectX::XMVectorAdd(DirectX::XMVectorScale(Front, -range), Target);
    DirectX::XMFLOAT3 eye;
    eye.x = front.x * -cameradistance + character->position.x + camera_subpositiom.x;
    eye.y = front.y * -cameradistance + character->position.y + camera_subpositiom.y;
    eye.z = front.z * -cameradistance + character->position.z + camera_subpositiom.z;

    gamepad& pad = SceneManager::Instance().pad;

    if(pad.button_state(gamepad::button::down, trigger_mode::falling_edge))
    {
	    
    }


    // X���̃J������]�𐧌�
    if (angle.x > maxAngleX) {
        angle.x = maxAngleX;
    }
    if (angle.x < minAngleX) {
        angle.x = minAngleX;
    }
    //X���̉�]�l��-3.14~3.14�Ɏ��܂�悤�ɂ���
    if (angle.y < -DirectX::XM_PI) {
        angle.y += DirectX::XM_2PI;
    }
    if (angle.y > DirectX::XM_PI) {
        angle.y -= DirectX::XM_2PI;
    }
    //Y���̉�]�l��-3.14~3.14�Ɏ��܂�悤�ɂ���
    if (angle.y < -DirectX::XM_PI) {
        angle.y = DirectX::XM_PI;
    }
    if (angle.y > DirectX::XM_PI) {
        angle.y = -DirectX::XM_PI;
    }

    const float liftup = 0.5f;//�J�����̍�����ݒ�

    SetLookAt(eye, { character->position.x,character->position.y + liftup,character->position.z }, DirectX::XMFLOAT3(0, 1, 0));

}
