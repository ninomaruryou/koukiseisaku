#include "Collision.h"


//ãÖÇ∆ãÖÇÃåç∑îªíË
bool Collision::InterseetSpheVsSphere(const DirectX::XMFLOAT3& positionA, float radiusA,
	const DirectX::XMFLOAT3& positionB, float radiusB, DirectX::XMFLOAT3& outPositionB)
{
	DirectX::XMVECTOR PositionA = DirectX::XMLoadFloat3(&positionA);
	DirectX::XMVECTOR PositionB = DirectX::XMLoadFloat3(&positionB);

	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(PositionB, PositionA);
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
	float  lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);

	//ãóó£îªíË
	float range = radiusA + radiusB;
	if (lengthSq > range * range)
	{
		return false;
	}



	//AÇ∆BÇâüÇµèoÇ∑
	DirectX::XMVECTOR outPosition;

	Vec = DirectX::XMVector3Normalize(Vec);
	Vec = DirectX::XMVectorScale(Vec, range);
	Vec = DirectX::XMVectorAdd(PositionA, Vec);


	DirectX::XMStoreFloat3(&outPositionB, Vec);


	return true;
}
