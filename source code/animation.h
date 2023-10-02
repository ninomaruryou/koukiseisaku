#pragma once

#include "skinned_mesh.h"
#include "Mesh.h"

class Animation
{
public:
    //アニメーション再生の時間確保
    float animation_tick{ 0 };

    //アニメーションの種類
    int clip_index = { 0 };

    //アニメーションの処理
    animation::keyframe& update_animation(Skinned_Mesh& skin, float elapsedTime);


};
