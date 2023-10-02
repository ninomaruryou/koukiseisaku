#include"SceneManager.h"

void SceneManager::Initialize()
{
}


//更新処理
void SceneManager::Update(ID3D11Device* device, float elapsedTime)
{
    if (elapsedTime > 1 / 20.0f) elapsedTime = 1 / 20.0f;

    if (nextScene != nullptr)
    {
        //古いシーンを終了処理
        Clear();

        //新しいシーンを設定
        currentScene = nextScene;
        nextScene = nullptr;

        //シーン初期化処理

        if (isInitialize == false) {
            currentScene->Initialize(device);
        }
    }

    if (currentScene != nullptr)
    {
        currentScene->Update(device,elapsedTime);
    }
}

//描画処理
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


//シーンクリア
void SceneManager::Clear()
{
    if (currentScene != nullptr)
    {
        currentScene->Finalize();
        delete currentScene;
        currentScene = nullptr;
    }
}

//シーン切り替え
void SceneManager::ChangeScene(Scene* scene)
{
    //新しいシーンを設定
    nextScene = scene;
}