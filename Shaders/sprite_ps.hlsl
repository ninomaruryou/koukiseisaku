//�X�v���C�g�̃V�F�[�_�[
#include "sprite.hlsli"

Texture2D color_map : register(t0);
SamplerState point_sampler_state : register(s0); //��������
SamplerState linear_sampler_state : register(s1);//�O���f�[�V����
SamplerState anisotropic_sampler_state : register(s2); //���ꂢ�����Ǐd��

//float4 main(VS_OUT pin) : SV_TARGET //�F��t����
//{
//	//if(length(pin.position.xy - float2(1280,720)*0.5) < 400)
//	//{
//	//	return  pin.color;
//	//}else
//	//{
//	//	//hlsl�̃s�N�Z���V�F�[�_�[�����Ŏg����B
//	//	//�`�悵�Ȃ�
//	//	discard;
//	//}
//
//	//return pin.color;
//	//return color_map.Sample(linear_sampler_state,pin.texcoord) * pin.color; //�F�������i��悹�j
//	return color_map.Sample(point_sampler_state, pin.texcoord) * pin.color; //�s�N�Z������Ȃ��ƃu�����_�[���@�\���Ȃ� color���v���X����Ȃ��̂�1�l�ȏ�͔��ɂȂ邩��
// }
float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = color_map.Sample(anisotropic_sampler_state, pin.texcoord);
	float alpha = color.a;
#if 1
  // Inverse gamma process
	const float GAMMA = 2.2;
	color.rgb = pow(color.rgb, GAMMA);
#endif
	return float4(color.rgb, alpha) * pin.color;
}
