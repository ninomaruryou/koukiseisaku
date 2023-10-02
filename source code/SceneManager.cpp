#include"SceneManager.h"

void SceneManager::Initialize()
{
}


//�X�V����
void SceneManager::Update(ID3D11Device* device, float elapsedTime)
{
    if (elapsedTime > 1 / 20.0f) elapsedTime = 1 / 20.0f;

    if (nextScene != nullptr)
    {
        //�Â��V�[�����I������
        Clear();

        //�V�����V�[����ݒ�
        currentScene = nextScene;
        nextScene = nullptr;

        //�V�[������������

        if (isInitialize == false) {
            currentScene->Initialize(device);
        }
    }

    if (currentScene != nullptr)
    {
        currentScene->Update(device,elapsedTime);
    }
}

//�`�揈��
void SceneManager::Render(ID3D11DeviceContext* immediate_context, float elapsedTime)
{
    if (currentScene != nullptr)
    {
    	currentScene->Render(immediate_context, elapsedTime);
    }
}

//void SceneManager::Render(ID3D11DeviceContext* immediate_context, float elapsedTime, States* states, Shadowmap* shadowmap)
//{
//    if (currentScene != nullptr)
//    {
//        currentScene->Render(immediate_context, elapsedTime, states, shadowmap);
//    }
//
//}


//�V�[���N���A
void SceneManager::Clear()
{
    if (currentScene != nullptr)
    {
        currentScene->Finalize();
        delete currentScene;
        currentScene = nullptr;
    }
}

//�V�[���؂�ւ�
void SceneManager::ChangeScene(Scene* scene)
{
    //�V�����V�[����ݒ�
    nextScene = scene;
}