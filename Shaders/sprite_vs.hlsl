//�X�v���C�g�̃V�F�[�_�[

#include "sprite.hlsli"

VS_OUT main(float4 position : POSITION, float4 color : COLOR , float2 texcoord : TEXCOORD) //�����ŕ`�揀��
{
	VS_OUT vout;
	vout.position = position;
	vout.color = color;

	vout.texcoord = texcoord;

	return vout;
}