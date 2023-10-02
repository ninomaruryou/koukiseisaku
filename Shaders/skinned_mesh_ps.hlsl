#include "Skinned_Mesh.hlsli"

#define POINT 0			//��������
#define LINEAR 1		//�O���f�[�V����
#define ANISOTROPIC 2	//���ꂢ�����Ǐd��
 SamplerState sampler_states[3] : register(s0);
 Texture2D texture_maps[4] : register(t0);

 Texture2D shadow_map : register(t4);
 SamplerState shadow_sampler_state : register(s4);

 float4 main(VS_OUT pin) : SV_TARGET
 {
#if 1

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
	
	float4 normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord);
	normal = (normal * 2.0) - 1.0;
	N = normalize((normal.x * T) + (normal.y * B) + (normal.z * N));


	float depth = shadow_map.Sample(shadow_sampler_state, pin.shadow_texcoord.xy).r;
	// �[�x�l���r���ĉe���ǂ����𔻒肷��
	if (pin.shadow_texcoord.z - depth > shadow_bias)
	{
		color.rgb *= shadow_color.rgb;
	}


	float3 L = normalize(-light_direction.xyz);
	float3 diffuse = color.rgb * max(0, dot(N, L));
	float3 V = normalize(camera_position.xyz - pin.world_position.xyz);
	float3 specular = pow(max(0, dot(N, normalize(V + L))), 128); //���ʔ���




	return float4(diffuse + specular, alpha) * pin.color;
#else
	 //��������؂�

		float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
	float alpha = color.a;

	float depth = shadow_map.Sample(shadow_sampler_state, pin.shadow_texcoord.xy).r;
	// �[�x�l���r���ĉe���ǂ����𔻒肷��
	if (pin.shadow_texcoord.z - depth > shadow_bias)
	{
		color.rgb *= shadow_color.rgb;
	}

	return color;
#if 1
	// Inverse gamma process(�t�K���}�ߒ�)
	const float GAMMA = 2.2;
	color.rgb = pow(color.rgb, GAMMA);
#endif

	float3 N = normalize(pin.world_normal.xyz); //normal���K�����Ă���
#if 1
	float3 T = normalize(pin.world_tangent.xyz);//tangent�𐳋K�����Ă���
	float sigma = pin.world_tangent.w;//�V�O�}
	T = normalize(T - N * dot(N, T)); //���l���Ԃ�邩�琳�K�����Ă���
	float3 B = normalize(cross(N, T) * sigma);//�H

	float4 normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord);//�@���}�b�v�H
	normal = (normal * 2.0) - 1.0; //�H
	N = normalize((normal.x * T) + (normal.y * B) + (normal.z * N));//�H
#endif

	float3 L = normalize(-light_direction.xyz);//���C�g�̐��K��
	float3 V = normalize(camera_position.xyz - pin.world_position.xyz);//�J�����ƃ��f���̃x�N�g���̐��K��
	float3 H = normalize(L + V);//�����x�N�g���Ȃ����̌v�Z�Ȃ̂��͕s��

	float NoL = max(0, dot(N, L));//�[���ȉ��Ȃ�[���Ɂ@�@���ƃ��C�g��Dot
#if 0
	float NoH = max(0, dot(normalize(pin.world_normal.xyz), H));//���E�̖@����
#else
	float NoH = max(0, dot(N, H));
#endif
	float NoV = max(0, dot(N, V));


	float irradiance = smoothstep(0.0, 0.01, NoL);//���ˏƓx

	float3 ambient_color = color.rgb; //�����̐F
	float3 ambient = lerp(0.4 * ambient_color, 0, irradiance);//����


	float3 diffuse_color = color.rgb; //�f�B�t���[�Y�̐F
	float3 diffuse = irradiance * diffuse_color;//�f�B�t���[�Y�i�g�U���ˁj

	float specular_intensity = pow(NoH, 512);
	float specular_intensity_smooth = smoothstep(0.005, 0.01, specular_intensity);
	float3 specular_color = color.rgb;//���ʔ��˂̐F
	float3 specular = specular_intensity_smooth * specular_color; //���ʔ���

	const float rim_threshold = 5.0; //rim�̂������l
	const float rim_amount = 0.716; //�z�H
	float rim_dot = 1 - NoV; //
	float rim_intensity = rim_dot * pow(NoL, rim_threshold);//���x
	rim_intensity = smoothstep(rim_amount - 0.4, rim_amount + 0.4, rim_intensity); //
 	//smoothstep�Ƃ͈͓͂̔��ɂ���ꍇ�́A0 ���� 1 �̊Ԃ̊��炩�ȃn�[�}�C�g��Ԃ�Ԃ��܂��B�n�[�}�C�g�⊮(�ŏ��l�ƍő�l�̋��ڂ��Ȑ���ɂȂ��Ă�����)
	float3 rom_color = 1;
	float3 rim = rim_intensity * rom_color; //���f���̊z���̕���

#if 0
	float lightIntensity = dot(normal, light_direction);
	float threshold = 0.1; // �Z���V�F�[�f�B���O�̂������l

	// �Z���V�F�[�f�B���O�̐F���v�Z
	float3 cellShadingColor;
	if (lightIntensity > threshold) {
		cellShadingColor = float3(1.0, 1.0, 1.0); // ���F
	}
	else {
		cellShadingColor = float3(0.0, 0.0, 0.0); // ���F
	}
	return float4(cellShadingColor, 1.0);

#endif


#if 0
	//���f���̃e�N�X�`������F���t�F�b�`����
	//float4 color = albedoTexture.Sample(Sampler, In.TexCoord);

	//�n�[�t�����o�[�g�g�U�Ɩ��ɂ�郉�C�e�B���O�v�Z
	float p = dot(pin.world_tangent, light_direction.xyz);
	p = p * 0.5f + 0.5f;
	p = p * p;

	//�v�Z���ʂ��g�D�[���V�F�[�_�[�p�̃e�N�X�`������F���t�F�b�`����
	float4 Col = texture_maps[0].Sample(sampler_states[POINT], float2(p, 0.0f));

	//���܂����F����Z����
	color.xyz *= Col.xyz;

	float threshold = smoothstep(NoL - 0.1, NoL + 0.1, NoL); // �Z���V�F�[�f�B���O�̂������l

	

	float4 lastcolor = float4( ambient + diffuse + specular + rim, alpha) * pin.color;

	if (NoL > threshold) {
		//color.xyz = float3(1.0, 1.0, 1.0); // ���F
	}
	else {
		lastcolor.xyz *= 0.5f; // ���F
	}






	return lastcolor;
#endif


	{
		// �V���h�E�}�b�v����[�x�l�擾
		float depth = shadow_map.Sample(shadow_sampler_state, pin.shadow_texcoord.xy).r;
		// �[�x�l���r���ĉe���ǂ����𔻒肷��
		if (pin.shadow_texcoord.z - depth > shadow_bias)
		{
			color.rgb *= shadow_color.rgb;
		}
	}

	//return float4(ambient + diffuse + rim, alpha) * pin.color;
	return float4(color.rgb, alpha);

#endif

 }

//float4 main(VS_OUT pin) : SV_TARGET
//{
//	float3 N = normalize(pin.world_normal.xyz);
//	float3 L = normalize(-light_direction.xyz);
//	float3 diffuse = max(0, dot(N, L));
//	return float4(diffuse, 1) * pin.color;
//}

//float4 main(VS_OUT pin) : SV_TARGET
// {
//	 float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
//	 float3 N = normalize(pin.world_normal.xyz);
//	 float3 L = normalize(-light_direction.xyz);
//	 float3 diffuse = color.rgb * max(0, dot(N, L));
//	 return float4(diffuse, color.a) * pin.color;
// }
