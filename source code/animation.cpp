#include"animation.h"

#include "Mesh.h"

animation::keyframe& Animation::update_animation(Skinned_Mesh& skin, float elapsedTime)
{
    int frame_index = { 0 };

    animation& animation{ skin.animation_clips.at(clip_index) };
    frame_index = static_cast<int>(animation_tick * animation.sampling_rate);
    if (frame_index > animation.sequence.size() - 1)
    {
        frame_index = 0;
        animation_tick = 0;
    }
    else
    {
        animation_tick += elapsedTime;
    }


    //animation::keyframe& keyframe{ animation.sequence.at(frame_index) };

    return animation.sequence.at(frame_index);
}
