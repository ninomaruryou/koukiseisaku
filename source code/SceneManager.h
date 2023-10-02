#pragma once

#include"Scene.h"
#include "Camera.h"
//シーンマネージャー
class SceneManager
{
private:
    SceneManager() {}
    ~SceneManager() {}

public:
    //唯一のインスタンス取得
    static SceneManager& Instance()
    {
        static SceneManager instance;
        return instance;
    }

    gamepad pad;

    void Initialize();

    //更新処理
    void Update(ID3D11Device* device, float elapsedtime);

    //描画処理
    void Render(ID3D11DeviceContext* immediate_context, float elapsedTime);
    ////描画処理
    //void Render(ID3D11DeviceContext* immediate_context, float elapsedTime, States* states, Shadowmap* shadowmap);

    //シーンクリア
    void Clear();

    //シーン切り替え
    void ChangeScene(Scene* scene);

private:
    bool isInitialize = false;
    Scene* currentScene = nullptr;
    Scene* nextScene = nullptr;



};