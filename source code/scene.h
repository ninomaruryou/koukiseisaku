#pragma once

#include<d3d11.h>
#include "GamePad.h"
#include "audio.h"
#include "COM_obj_.h"
#include "Camera.h"
//シーン
class Scene
{
public:
    Scene() {}
    virtual ~Scene() {}

    //初期化
    virtual void Initialize(ID3D11Device* device) = 0;

    //終了化
    virtual void Finalize() = 0;

    //更新処理
    virtual void Update(ID3D11Device* device,float elapsedTime) = 0;

    //描画処理
    virtual void Render(ID3D11DeviceContext* dc, float elapsedTime) = 0;

    //準備完了しているか
    bool IsReady() const { return ready; }

    //準備完了設定
    void SetReady() { ready = true; }

    //一回だけ通る初期化
    bool isInitialize = false;
    //チュートリアルと本ステージを区別するやつ
    static int crrent_stage;
    static float Lamp_timer;
    static float gool_timer;

    //static std::unique_ptr<audio> bgm[8];
    //static std::unique_ptr<audio> se[8];



private:
    bool ready = false;
};

