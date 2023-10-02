#pragma once

#include"Scene.h"
#include "Camera.h"
//�V�[���}�l�[�W���[
class SceneManager
{
private:
    SceneManager() {}
    ~SceneManager() {}

public:
    //�B��̃C���X�^���X�擾
    static SceneManager& Instance()
    {
        static SceneManager instance;
        return instance;
    }

    gamepad pad;

    void Initialize();

    //�X�V����
    void Update(ID3D11Device* device, float elapsedtime);

    //�`�揈��
    void Render(ID3D11DeviceContext* immediate_context, float elapsedTime);
    ////�`�揈��
    //void Render(ID3D11DeviceContext* immediate_context, float elapsedTime, States* states, Shadowmap* shadowmap);

    //�V�[���N���A
    void Clear();

    //�V�[���؂�ւ�
    void ChangeScene(Scene* scene);

private:
    bool isInitialize = false;
    Scene* currentScene = nullptr;
    Scene* nextScene = nullptr;



};