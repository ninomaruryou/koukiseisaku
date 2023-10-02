#pragma once

#include <DirectXMath.h>

//ƒRƒŠƒWƒ‡ƒ“
class Collision
{
public:
	//’e‚Æ‹…‚ÌŒğÛ”»’è
	static bool InterseetSpheVsSphere(
		const DirectX::XMFLOAT3& positionA,
		float raduusA,
		const DirectX::XMFLOAT3& radiusB,
		float radiusb,
		DirectX::XMFLOAT3& outPositionB
	);
	////‰~’Œ‚Æ‰~’Œ‚ÌŒğÛ”»’è
	//static bool InterseetCylinderVsCylinder(
	//	const DirectX::XMFLOAT3& positionA,
	//	float raduusA,
	//	float heightA,
	//	const DirectX::XMFLOAT3& positionB,
	//	float radiusB,
	//	float heightB,
	//	DirectX::XMFLOAT3& outPositionB
	//);

	////‹…‚Æ‰~’Œ‚ÌŒğ·”»’è
	//static bool IntersectSphrteVsCylinder(const DirectX::XMFLOAT3& spherePosition,
	//	float sphereRadius,
	//	const DirectX::XMFLOAT3& cylinderPosition,
	//	float cylinderRadius,
	//	float cylinderHeight,
	//	DirectX::XMFLOAT3& outCylinderPosition);

	////ƒŒƒC‚Æƒ‚ƒfƒ‹‚ÌŒğ·”»’è
	//static  bool IntersectRayVsModel(const DirectX::XMFLOAT3& start,
	//	const DirectX::XMFLOAT3& end, const Model* model, HitResult& result);
};
