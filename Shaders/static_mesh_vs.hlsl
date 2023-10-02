#include "Skinned_Mesh.hlsli"

VS_OUT main(float4 position : POSITION, float4 normal : NORMAL, float2 texcoord : TEXCOORD)
 {
	VS_OUT vout;
	vout.position = mul(position, mul(world, view_projection));//mulä|ÇØéZ

	vout.world_position = mul(position, world);
	normal.w = 0;
	vout.world_normal = normalize(mul(normal, world));//ê≥ãKâª
	//float4 N = normalize(mul(normal, world));
	//float4 L = normalize(-light_direction);

	//vout.color = material_color;

	//vout.color.rgb = material_color.rgb * max(0, dot(L, N));
	//vout.color.a = material_color.a;
	//
	vout.texcoord = texcoord;



	return vout;
 }

//VS_OUT main(VS_IN vin)
//{
//	VS_OUT vout;
//	vout.position = mul(vin.position, mul(world, view_projection));
//
//	vout.world_position = mul(vin.position, world);
//	vin.normal.w = 0;
//	vout.world_normal = normalize(mul(vin.normal, world));
//	
//	 vout.texcoord = vin.texcoord;
//	
//	  #if 0
//		 vout.color = material_color;
//	 #else
//		 vout.color = 0;
//	 const float4 bone_colors[4] = {
//	 {1, 0, 0, 1},
//	 {0, 1, 0, 1},
//	 {0, 0, 1, 1},
//	 {1, 1, 1, 1},
//	 };
//	 for (int bone_index = 0; bone_index < 4; ++bone_index)
//		 {
//		 vout.color += bone_colors[vin.bone_indices[bone_index] % 4]
//			 * vin.bone_weights[bone_index];
//		 }
//	 #endif
//		
//	 return vout;
//}
