#pragma once

#include<d3d11.h>
#include "GamePad.h"
#include "audio.h"
#include "COM_obj_.h"
#include "Camera.h"
//�V�[��
class Scene
{
public:
    Scene() {}
    virtual ~Scene() {}

    //������
    virtual void Initialize(ID3D11Device* device) = 0;

    //�I����
    virtual void Finalize() = 0;

    //�X�V����
    virtual void Update(ID3D11Device* device,float elapsedTime) = 0;

    //�`�揈��
    virtual void Render(ID3D11DeviceContext* dc, float elapsedTime) = 0;

    //�����������Ă��邩
    bool IsReady() const { return ready; }

    //���������ݒ�
    void SetReady() { ready = true; }

    //��񂾂��ʂ鏉����
    bool isInitialize = false;
    //�`���[�g���A���Ɩ{�X�e�[�W����ʂ�����
    static int crrent_stage;
    static float Lamp_timer;
    static float gool_timer;

    //static std::unique_ptr<audio> bgm[8];
    //static std::unique_ptr<audio> se[8];



private:
    bool ready = false;
};

