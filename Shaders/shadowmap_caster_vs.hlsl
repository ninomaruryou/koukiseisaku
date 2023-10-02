#include "Static_mesh_instncing.hlsli"

Texture2D shadow_map : register(t4);
SamplerState shadow_sampler_state : register(s4);

VS_OUT main(VS_IN vin, VS_INSTNCING world)
{
	vin.normal.w = 0;
	float sigma = vin.tangent.w;
	vin.tangent.w = 0;

	VS_OUT vout;
	vout.position = mul(vin.position, mul(world.transform, view_projection));

	vout.world_position = mul(vin.position, world.transform);
	vout.world_normal = normalize(mul(vin.normal, world.transform));
	vout.world_tangent = normalize(mul(vin.tangent, world.transform));
	vout.world_tangent.w = sigma;

	vout.texcoord = vin.texcoord;

	vout.color = world.color;


	return vout;

}
