#pragma once

#include <DirectXMath.h>

//�R���W����
class Collision
{
public:
	//�e�Ƌ��̌��۔���
	static bool InterseetSpheVsSphere(
		const DirectX::XMFLOAT3& positionA,
		float raduusA,
		const DirectX::XMFLOAT3& radiusB,
		float radiusb,
		DirectX::XMFLOAT3& outPositionB
	);
	////�~���Ɖ~���̌��۔���
	//static bool InterseetCylinderVsCylinder(
	//	const DirectX::XMFLOAT3& positionA,
	//	float raduusA,
	//	float heightA,
	//	const DirectX::XMFLOAT3& positionB,
	//	float radiusB,
	//	float heightB,
	//	DirectX::XMFLOAT3& outPositionB
	//);

	////���Ɖ~���̌�������
	//static bool IntersectSphrteVsCylinder(const DirectX::XMFLOAT3& spherePosition,
	//	float sphereRadius,
	//	const DirectX::XMFLOAT3& cylinderPosition,
	//	float cylinderRadius,
	//	float cylinderHeight,
	//	DirectX::XMFLOAT3& outCylinderPosition);

	////���C�ƃ��f���̌�������
	//static  bool IntersectRayVsModel(const DirectX::XMFLOAT3& start,
	//	const DirectX::XMFLOAT3& end, const Model* model, HitResult& result);
};
