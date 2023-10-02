#include "Static_mesh_instncing.hlsli"
#define POINT 0			//��������
#define LINEAR 1		//�O���f�[�V����
#define ANISOTROPIC 2	//���ꂢ�����Ǐd��
SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps[4] : register(t0);

Texture2D shadow_map : register(t4);
SamplerState shadow_sampler_state : register(s4);

float4 main(VS_OUT pin) : SV_TARGET
{


	float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);

	float alpha = color.a;

#if 1
	// Inverse gamma process(�t�K���}�ߒ�)
	const float GAMMA = 2.2;
	color.rgb = pow(color.rgb, GAMMA);
#endif


	float3 N = normalize(pin.world_normal.xyz);
	float3 T = normalize(pin.world_tangent.xyz);
	float sigma = pin.world_tangent.w;
	T = normalize(T - N * dot(N, T));
	float3 B = normalize(cross(N, T) * sigma);

	//return float4(N,1);

	float4 normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord);
	normal = (normal ) * 2.0 - 1.0;
	N = normalize((normal.x * T) + (normal.y * B) + (normal.z * N));

	float3 L = normalize(-light_direction.xyz);
	float3 diffuse = color.rgb * max(0, dot(N, L));
	float3 V = normalize(camera_position.xyz - pin.world_position.xyz);
	float3 specular = pow(max(0, dot(N, normalize(V + L))), 128); //���ʔ���

	//{
	//	// �V���h�E�}�b�v����[�x�l�擾
	//	float depth = shadow_map.Sample(shadow_sampler_state, pin.shadow_texcoord.xy).r;
	//	// �[�x�l���r���ĉe���ǂ����𔻒肷��
	//	if (pin.shadow_texcoord.z - depth > shadow_bias)
	//	{
	//		color.rgb *= shadow_color.rgb;
	//	}
	//}

	//return float4(color.rgb, 1);

	return float4(color.rgb + diffuse + specular, alpha) * pin.color;

}
