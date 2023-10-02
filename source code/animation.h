#pragma once

#include "skinned_mesh.h"
#include "Mesh.h"

class Animation
{
public:
    //�A�j���[�V�����Đ��̎��Ԋm��
    float animation_tick{ 0 };

    //�A�j���[�V�����̎��
    int clip_index = { 0 };

    //�A�j���[�V�����̏���
    animation::keyframe& update_animation(Skinned_Mesh& skin, float elapsedTime);


};
