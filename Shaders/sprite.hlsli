//スプライトのシェーダー

struct VS_OUT 
{
	float4 position : SV_POSITION;//位置
	float4 color : COLOR;//色
	float2 texcoord : TEXCOORD; //高度な（画像などの複雑なピクセル渡すためのデータ）
};

