#include "ProjectileManager.h"

Projectile::Projectile(ProjectileManager* manager) : manager_(manager)
{
	manager->Register(this);
}

//�f�o�b�N�v���~�e�B�u�`��
void Projectile::DrawDebugPrimitive()
{
	//DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

	////�Փ˔���p�̃f�o�b�N����`��
	//debugRenderer->DrawSphere(position, radius, DirectX::XMFLOAT4(0, 0, 0, 1));

}

void Projectile::UpdateTransform()
{
#if 0
	// �Ƃ肠�����A���ŉ�]�͖��������s��𐧍삷��B
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

	//�O�x�N�g�����Z�o
	//DirectX::XMMATRIX  Transform = DirectX::XMMatrixRotationRollPitchYaw(direction.y, direction.x, direction.z);
	//Front = Transform.r[2];

	Front = DirectX::XMLoadFloat3(&direction);

	//���̏�x�N�g�����Z�o
	DirectX::XMFLOAT3 i = { 0,1,0 };
	Up = DirectX::XMLoadFloat3(&i);
	//�E�x�N�g�����Z�o
	Right = DirectX::XMVector3Cross(Front, Up);

	//��x�N�g�����Z�o
	Up = DirectX::XMVector3Cross(Front, Right);

	//�v�Z���ʂ����o��
	DirectX::XMFLOAT3 right, up, front;
	Right = DirectX::XMVector3Normalize(Right);
	Up = DirectX::XMVector3Normalize(Up);
	Front = DirectX::XMVector3Normalize(Front);
	DirectX::XMStoreFloat3(&right, Right);
	DirectX::XMStoreFloat3(&up, Up);
	DirectX::XMStoreFloat3(&front, Front);

	//�Z�o�������x�N�g������s����쐬
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
	//���˕���
	this->direction = front;
}


void Projectile::Updata(float elapsedTime)
{

}

void Projectile::Destroy()
{
	manager_->Remove(this);
}
