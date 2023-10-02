static const int MAX_WORLD = 100;

struct VS_IN
{
	float4 position : POSITION;//float4メモリがちょうどいい
	float4 normal : NORMAL;//法線
	float4 tangent : TANGENT;//法線マップ
	float2 texcoord : TEXCOORD;
};

struct VS_INSTNCING
{
	row_major float4x4 transform : WORLD;
	float4 color : COLOR;
};

struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 world_position : POSITION;
	float4 world_normal : NORMAL;
	float4 world_tangent : TANGENT; //ワールド座標の位置
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
	float3 shadow_texcoord : TEXCOORD1;// シャドウマップ用のパラメーター変数

};


cbuffer OBJECT_CONSTANT_BUFFER : register(b0)//rendar関数から持ってきている
{
	//row_major float4x4 world;
	float4 material_color;

};
cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
	row_major float4x4 view_projection;
	float4 light_direction;
	float4 camera_position;
};

//cbuffer OBJECT_CONSTANT_BUFFER : register(b2)
//{
//	//uint4 instancing_id;
//		//GPUインスタンス座標
//	row_major float4x4 instancing_world[MAX_instancing];
//
//};

cbuffer SHADOWMAP_CONSTANT_BUFFER : register(b6)
{
	row_major float4x4 light_view_projection;
	float3 shadow_color;
	float shadow_bias;
};
