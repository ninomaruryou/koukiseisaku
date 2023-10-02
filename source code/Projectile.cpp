#include "ProjectileManager.h"

Projectile::Projectile(ProjectileManager* manager) : manager_(manager)
{
	manager->Register(this);
}

//デバックプリミティブ描画
void Projectile::DrawDebugPrimitive()
{
	//DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	////衝突判定用のデバック球を描画
	//debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));

}

void Projectile::UpdateTransform()
{
#if 0
	// とりあえず、仮で回転は無視した行列を制作する。
	transform._11 = right.x;
	transform._12 = right.y;
	transform._13 = right.z;
	transform._14 = 0;
	transform._21 = up.x;
	transform._22 = up.y;
	transform._23 = up.z;
	transform._24 = 0;
	transform._31 = front.x;
	transform._32 = front.y;
	transform._33 = front.z;
	transform._34 = 0;
	transform._41 = position.x;
	transform._42 = position.y;
	transform._43 = position.z;
	transform._44 = 1.0f;

#else
	DirectX::XMVECTOR Front, Up, Right;

	//前ベクトルを算出
	//DirectX::XMMATRIX  Transform = DirectX::XMMatrixRotationRollPitchYaw(direction.y, direction.x, direction.z);
	//Front = Transform.r[2];

	Front = DirectX::XMLoadFloat3(&direction);

	//仮の上ベクトルを算出
	DirectX::XMFLOAT3 i = { 0,1,0 };
	Up = DirectX::XMLoadFloat3(&i);
	//右ベクトルを算出
	Right = DirectX::XMVector3Cross(Front, Up);

	//上ベクトルを算出
	Up = DirectX::XMVector3Cross(Front, Right);

	//計算結果を取り出し
	DirectX::XMFLOAT3 right, up, front;
	Right = DirectX::XMVector3Normalize(Right);
	Up = DirectX::XMVector3Normalize(Up);
	Front = DirectX::XMVector3Normalize(Front);
	DirectX::XMStoreFloat3(&right, Right);
	DirectX::XMStoreFloat3(&up, Up);
	DirectX::XMStoreFloat3(&front, Front);

	//算出した軸ベクトルから行列を作成
	transform._11 = right.x * scale.x;
	transform._12 = right.y * scale.x;
	transform._13 = right.z * scale.x;
	transform._14 = 0;
	transform._21 = up.x * scale.y;
	transform._22 = up.y * scale.y;
	transform._23 = up.z * scale.y;
	transform._24 = 0;
	transform._31 = front.x * scale.z;
	transform._32 = front.y * scale.z;
	transform._33 = front.z * scale.z;
	transform._34 = 0;
	transform._41 = position.x;
	transform._42 = position.y;
	transform._43 = position.z;
	transform._44 = 1.0f;

#endif
	//発射方向
	this->direction = front;
}


void Projectile::Updata(float elapsedTime)
{

}

void Projectile::Destroy()
{
	manager_->Remove(this);
}
