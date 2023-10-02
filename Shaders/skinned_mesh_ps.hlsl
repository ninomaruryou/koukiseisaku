#include "Skinned_Mesh.hlsli"

#define POINT 0			//かくかく
#define LINEAR 1		//グラデーション
#define ANISOTROPIC 2	//きれいだけど重い
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
	 // Inverse gamma process(逆ガンマ過程)
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
	// 深度値を比較して影かどうかを判定する
	if (pin.shadow_texcoord.z - depth > shadow_bias)
	{
		color.rgb *= shadow_color.rgb;
	}


	float3 L = normalize(-light_direction.xyz);
	float3 diffuse = color.rgb * max(0, dot(N, L));
	float3 V = normalize(camera_position.xyz - pin.world_position.xyz);
	float3 specular = pow(max(0, dot(N, normalize(V + L))), 128); //鏡面反射




	return float4(diffuse + specular, alpha) * pin.color;
#else
	 //ここが区切り

		float4 color = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
	float alpha = color.a;

	float depth = shadow_map.Sample(shadow_sampler_state, pin.shadow_texcoord.xy).r;
	// 深度値を比較して影かどうかを判定する
	if (pin.shadow_texcoord.z - depth > shadow_bias)
	{
		color.rgb *= shadow_color.rgb;
	}

	return color;
#if 1
	// Inverse gamma process(逆ガンマ過程)
	const float GAMMA = 2.2;
	color.rgb = pow(color.rgb, GAMMA);
#endif

	float3 N = normalize(pin.world_normal.xyz); //normal正規化している
#if 1
	float3 T = normalize(pin.world_tangent.xyz);//tangentを正規化している
	float sigma = pin.world_tangent.w;//シグマ
	T = normalize(T - N * dot(N, T)); //数値がぶれるから正規化している
	float3 B = normalize(cross(N, T) * sigma);//？

	float4 normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord);//法線マップ？
	normal = (normal * 2.0) - 1.0; //？
	N = normalize((normal.x * T) + (normal.y * B) + (normal.z * N));//？
#endif

	float3 L = normalize(-light_direction.xyz);//ライトの正規化
	float3 V = normalize(camera_position.xyz - pin.world_position.xyz);//カメラとモデルのベクトルの正規化
	float3 H = normalize(L + V);//合成ベクトルなぜこの計算なのかは不明

	float NoL = max(0, dot(N, L));//ゼロ以下ならゼロに　法線とライトのDot
#if 0
	float NoH = max(0, dot(normalize(pin.world_normal.xyz), H));//世界の法線の
#else
	float NoH = max(0, dot(N, H));
#endif
	float NoV = max(0, dot(N, V));


	float irradiance = smoothstep(0.0, 0.01, NoL);//放射照度

	float3 ambient_color = color.rgb; //環境光の色
	float3 ambient = lerp(0.4 * ambient_color, 0, irradiance);//環境光


	float3 diffuse_color = color.rgb; //ディフューズの色
	float3 diffuse = irradiance * diffuse_color;//ディフューズ（拡散反射）

	float specular_intensity = pow(NoH, 512);
	float specular_intensity_smooth = smoothstep(0.005, 0.01, specular_intensity);
	float3 specular_color = color.rgb;//鏡面反射の色
	float3 specular = specular_intensity_smooth * specular_color; //鏡面反射

	const float rim_threshold = 5.0; //rimのしきい値
	const float rim_amount = 0.716; //額？
	float rim_dot = 1 - NoV; //
	float rim_intensity = rim_dot * pow(NoL, rim_threshold);//強度
	rim_intensity = smoothstep(rim_amount - 0.4, rim_amount + 0.4, rim_intensity); //
 	//smoothstepとはの範囲内にある場合は、0 から 1 の間の滑らかなハーマイト補間を返します。ハーマイト補完(最小値と最大値の境目が曲線状になっているやつ)
	float3 rom_color = 1;
	float3 rim = rim_intensity * rom_color; //モデルの額縁の部分

#if 0
	float lightIntensity = dot(normal, light_direction);
	float threshold = 0.1; // セルシェーディングのしきい値

	// セルシェーディングの色を計算
	float3 cellShadingColor;
	if (lightIntensity > threshold) {
		cellShadingColor = float3(1.0, 1.0, 1.0); // 白色
	}
	else {
		cellShadingColor = float3(0.0, 0.0, 0.0); // 黒色
	}
	return float4(cellShadingColor, 1.0);

#endif


#if 0
	//モデルのテクスチャから色をフェッチする
	//float4 color = albedoTexture.Sample(Sampler, In.TexCoord);

	//ハーフランバート拡散照明によるライティング計算
	float p = dot(pin.world_tangent, light_direction.xyz);
	p = p * 0.5f + 0.5f;
	p = p * p;

	//計算結果よりトゥーンシェーダー用のテクスチャから色をフェッチする
	float4 Col = texture_maps[0].Sample(sampler_states[POINT], float2(p, 0.0f));

	//求まった色を乗算する
	color.xyz *= Col.xyz;

	float threshold = smoothstep(NoL - 0.1, NoL + 0.1, NoL); // セルシェーディングのしきい値

	

	float4 lastcolor = float4( ambient + diffuse + specular + rim, alpha) * pin.color;

	if (NoL > threshold) {
		//color.xyz = float3(1.0, 1.0, 1.0); // 白色
	}
	else {
		lastcolor.xyz *= 0.5f; // 黒色
	}






	return lastcolor;
#endif


	{
		// シャドウマップから深度値取得
		float depth = shadow_map.Sample(shadow_sampler_state, pin.shadow_texcoord.xy).r;
		// 深度値を比較して影かどうかを判定する
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
