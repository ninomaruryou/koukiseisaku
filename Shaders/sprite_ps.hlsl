//スプライトのシェーダー
#include "sprite.hlsli"

Texture2D color_map : register(t0);
SamplerState point_sampler_state : register(s0); //かくかく
SamplerState linear_sampler_state : register(s1);//グラデーション
SamplerState anisotropic_sampler_state : register(s2); //きれいだけど重い

//float4 main(VS_OUT pin) : SV_TARGET //色を付ける
//{
//	//if(length(pin.position.xy - float2(1280,720)*0.5) < 400)
//	//{
//	//	return  pin.color;
//	//}else
//	//{
//	//	//hlslのピクセルシェーダーだけで使える。
//	//	//描画しない
//	//	discard;
//	//}
//
//	//return pin.color;
//	//return color_map.Sample(linear_sampler_state,pin.texcoord) * pin.color; //色を合成（上乗せ）
//	return color_map.Sample(point_sampler_state, pin.texcoord) * pin.color; //ピクセルじゃないとブレンダーが機能しない colorがプラスじゃないのは1値以上は白になるから
// }
float4 main(VS_OUT pin) : SV_TARGET
{
	float4 color = color_map.Sample(anisotropic_sampler_state, pin.texcoord);
	float alpha = color.a;
#if 1
  // Inverse gamma process
	const float GAMMA = 2.2;
	color.rgb = pow(color.rgb, GAMMA);
#endif
	return float4(color.rgb, alpha) * pin.color;
}
