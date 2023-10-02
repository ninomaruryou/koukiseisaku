#include "bidirectional_reflectance_distribution_function.hlsli"
#include "gltf_model.hlsli"
//UNIT35
struct texture_info
{
	int index;
	int texcoord;
};
struct normal_texture_info
{
	int index;
	int texcoord;
	float scale;
};
struct occlusion_texture_info
{
	int index;
	int texcoord;
	float strength;
};
struct pbr_metallic_roughness
{
	float4 basecolor_factor;
	texture_info basecolor_texture;
	float metallic_factor;
	float roughness_factor;
	texture_info metallic_roughness_texture;
};
struct material_constants
{
	float3 emissive_factor;
	int alpha_mode; // "OPAQUE" : 0, "MASK" : 1, "BLEND" : 2
	float alpha_cutoff;
	bool double_sided;

	pbr_metallic_roughness pbr_metallic_roughness;

	normal_texture_info normal_texture;
	occlusion_texture_info occlusion_texture;
	texture_info emissive_texture;
};

StructuredBuffer<material_constants> materials : register(t0);

//UNIT36
//① ピクセルシェーダー(gltf_model_ps.hlsl)にテクスチャとサンプラを定義する
#define BASECOLOR_TEXTURE 0
#define METALLIC_ROUGHNESS_TEXTURE 1
#define NORMAL_TEXTURE 2
#define EMISSIVE_TEXTURE 3
#define OCCLUSION_TEXTURE 4
Texture2D<float4> material_textures[5] : register(t1);

//#define POINT 0
//#define LINEAR 1
//#define ANISOTROPIC 2
//SamplerState sampler_states[3] : register(s0);

// SHADOW
SamplerComparisonState comparison_sampler_state : register(s5);
Texture2D shadow_map : register(t8);

float4 main(VS_OUT pin) : SV_TARGET
{
	//material_constants m = materials[material];

	//float3 N = normalize(pin.w_normal.xyz);
	//float3 L = normalize(-light_direction.xyz);

	//float3 color = max(0, dot(N, L)) * m.pbr_metallic_roughness.basecolor_factor.rgb;
	////float3 color = max(0, dot(N, L));

	//return float4(color, 1);

	//material_constants m = materials[material];

	//return material_textures[3].Sample(sampler_states[2], pin.texcoord).rgba;

	//float4 basecolor = m.pbr_metallic_roughness.basecolor_texture.index > -1 ?
	//	material_textures[BASECOLOR_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord) :
	//m.pbr_metallic_roughness.basecolor_factor;
	//float3 emmisive = m.emissive_texture.index > -1 ?
	//	material_textures[EMISSIVE_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord).rgb:
	//m.emissive_factor;

	//float3 N = normalize(pin.w_normal.xyz);
	//float3 L = normalize(-light_direction.xyz);

	//float3 color = max(0, dot(N, L)) * basecolor.rgb + emmisive;
	//return float4(color, basecolor.a);

//}
////UNIT38
//float4 main(VS_OUT pin) : SV_TARGET
//{
   const float GAMMA = 2.2;

	const material_constants m = materials[material];

	float4 basecolor_factor = m.pbr_metallic_roughness.basecolor_factor;
   const int basecolor_texture = m.pbr_metallic_roughness.basecolor_texture.index;
   if (basecolor_texture > -1)
		{
	   float4 sampled = material_textures[BASECOLOR_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
	   sampled.rgb = pow(sampled.rgb, GAMMA);
		basecolor_factor *= sampled;
   }

	float3 emmisive_factor = m.emissive_factor;
   const int emissive_texture = m.emissive_texture.index;
   if (emissive_texture > -1)
   {
	   float4 sampled = material_textures[EMISSIVE_TEXTURE].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
	   sampled.rgb = pow(sampled.rgb, GAMMA);
	   emmisive_factor *= sampled.rgb;
   }

		float roughness_factor = m.pbr_metallic_roughness.roughness_factor;
	float metallic_factor = m.pbr_metallic_roughness.metallic_factor;
	const int metallic_roughness_texture = m.pbr_metallic_roughness.metallic_roughness_texture.index;
	if (metallic_roughness_texture > -1)
	{
		float4 sampled = material_textures[METALLIC_ROUGHNESS_TEXTURE].Sample(sampler_states[LINEAR],
			pin.texcoord);
		roughness_factor *= sampled.g;
		metallic_factor *= sampled.b;
	}

	float occlusion_factor = 1.0;
   const int occlusion_texture = m.occlusion_texture.index;
   if (occlusion_texture > -1)
   {
	   float4 sampled = material_textures[OCCLUSION_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
	   occlusion_factor *= sampled.r;
   }

	const float occlusion_strength = m.occlusion_texture.strength;
	const float3 f0 = lerp(0.04, basecolor_factor.rgb, metallic_factor);
	const float3 f90 = 1.0;
	const float alpha_roughness = roughness_factor * roughness_factor;
	const float3 c_diff = lerp(basecolor_factor.rgb, 0.0, metallic_factor);

	const float3 P = pin.w_position.xyz;
	const float3 V = normalize(camera_position.xyz - pin.w_position.xyz);

	float3 N = normalize(pin.w_normal.xyz);
   float3 T = has_tangent ? normalize(pin.w_tangent.xyz) : float3(1, 0, 0);
   float sigma = has_tangent ? pin.w_tangent.w : 1.0;
   T = normalize(T - N * dot(N, T));
   float3 B = normalize(cross(N, T) * sigma);

	const int normal_texture = m.normal_texture.index;
	if (normal_texture > -1)
	{
		float4 sampled = material_textures[NORMAL_TEXTURE].Sample(sampler_states[LINEAR], pin.texcoord);
		float3 normal_factor = sampled.xyz;
		normal_factor = (normal_factor * 2.0) - 1.0;
		normal_factor = normalize(normal_factor * float3(m.normal_texture.scale, m.normal_texture.scale, 1.0));
		N = normalize((normal_factor.x * T) + (normal_factor.y * B) + (normal_factor.z * N));
	}

	float3 diffuse = 0;
	float3 specular = 0;

	// Loop for shading process for each light
	float3 L = normalize(-light_direction.xyz);
	float3 Li = float3(1.0, 1.0, 1.0); // Radiance of the light
	const float NoL = max(0.0, dot(N, L));
	const float NoV = max(0.0, dot(N, V));
	if (NoL > 0.0 || NoV > 0.0)
	{
		const float3 R = reflect(-L, N);
		const float3 H = normalize(V + L);

		const float NoH = max(0.0, dot(N, H));
		const float HoV = max(0.0, dot(H, V));

		//UNIT39
		diffuse += ibl_radiance_lambertian(N, V, roughness_factor, c_diff, f0);
		specular += ibl_radiance_ggx(N, V, roughness_factor, f0);

		diffuse += Li * NoL * brdf_lambertian(f0, f90, c_diff, HoV);
		specular += Li * NoL * brdf_specular_ggx(f0, f90, alpha_roughness, HoV, NoL, NoV, NoH);

	}

	float3 emmisive = emmisive_factor;
   diffuse = lerp(diffuse, diffuse * occlusion_factor, occlusion_strength);
   specular = lerp(specular, specular * occlusion_factor, occlusion_strength);

   // SHADOW
#if 1
   const float shadow_depth_bias = 0.001;
#else
	// Here we have a maximum bias of 0.01 and a minimum of 0.001 based on the surface's normal and light direction. 
   const float shadow_depth_bias = max(0.01 * (1.0 - dot(N, L)), 0.001);
#endif

   float4 light_view_position = mul(pin.w_position, light_view_projection); // World to Clip space
   light_view_position = light_view_position / light_view_position.w;  // Clip to NDC
   float2 light_view_texcoord = 0;
   // NDC to Texture coordinate
   light_view_texcoord.x = light_view_position.x * +0.5 + 0.5;
   light_view_texcoord.y = light_view_position.y * -0.5 + 0.5;
   float depth = saturate(light_view_position.z - shadow_depth_bias);

   float3 shadow_factor = 1.0f;
   shadow_factor = shadow_map.SampleCmpLevelZero(comparison_sampler_state, light_view_texcoord, depth).xxx;


	float3 Lo = diffuse + specular + emmisive;
   return float4((diffuse + specular) * shadow_factor, 1);
   //return float4(light_view_position.xyz, 1);
}