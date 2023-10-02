#include "Static_mesh_instncing.hlsli"

VS_OUT main(VS_IN vin, VS_INSTNCING instncing)
{
	vin.normal.w = 0;
	float sigma = vin.tangent.w;
	vin.tangent.w = 0;


	//float4 blended_position = { 0, 0, 0, 1 };
	//float4 blended_normal = { 0, 0, 0, 0 };
	//float4 blended_tangent = { 0, 0, 0, 0 };

	//for (int bone_index = 0; bone_index < 4; ++bone_index)//xyz情報を入れる(アニメーション)
	//{
	//	blended_position += vin.bone_weights[bone_index]
	//		* mul(vin.position, bone_transforms[vin.bone_indices[bone_index]]);
	//	blended_normal += vin.bone_weights[bone_index]
	//		* mul(vin.normal, bone_transforms[vin.bone_indices[bone_index]]);
	//	blended_tangent += vin.bone_weights[bone_index]
	//		* mul(vin.tangent, bone_transforms[vin.bone_indices[bone_index]]);
	//}



	//vin.position = float4(blended_position.xyz, 1.0f);
	//vin.normal = float4(blended_normal.xyz, 0.0f);
	//vin.tangent = float4(blended_tangent.xyz, 0.0f);

	VS_OUT vout;
	vout.position = mul(vin.position, mul(instncing.transform, view_projection));

	vout.world_position = mul(vin.position, instncing.transform);
	vout.world_normal = normalize(mul(vin.normal, instncing.transform));
	vout.world_tangent = normalize(mul(vin.tangent, instncing.transform));
	
	//vout.world_position = mul(vin.position, instancing_world[1]);
	//vout.world_normal = normalize(mul(vin.normal, instancing_world[1]));
	//vout.world_tangent = normalize(mul(vin.tangent, instancing_world[1]));
	vout.world_tangent.w = sigma;

	vout.texcoord = vin.texcoord;
	//// シャドウマップ用のパラメーター計算
	//{
	//	// ライトから見たNDC座標を算出
	//	float4 wvpPos = mul(vin.position, mul(world.transform, light_view_projection));
	//	// NDC座標からUV座標を算出する
	//	wvpPos /= wvpPos.w;
	//	wvpPos.y = -wvpPos.y;
	//	wvpPos.xy = 0.5f * wvpPos.xy + 0.5f;
	//	vout.shadow_texcoord = wvpPos.xyz;
	//}

#if 1
	vout.color = instncing.color;
#else

#endif

	return vout;

}
