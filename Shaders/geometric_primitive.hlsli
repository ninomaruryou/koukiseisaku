struct VS_OUT
{
    float4 position : SV_POSITION;//位置
    float4 color : COLOR;//色
    float2 texcoord : TEXCOORD; //高度な（画像などの複雑なピクセル渡すためのデータ）
};

cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 world;
    float4 material_color;
};
cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 view_projection;
    float4 light_direction;
};