#pragma once

#include <directxmath.h>

#include "RenderContext.h"


class ProjectileManager;

class Projectile
{
public:
	//Projectile(){}
	Projectile(ProjectileManager* manager);
	virtual ~Projectile() {}

	//�X�V����
	virtual void Updata(float elapsedTime) = 0;

	//�`�ʏ���
	virtual void Render(ID3D11DeviceContext* dc, float elapsed_time) = 0;

	//�f�o�b�N�v���~�e�B�u�`��
	virtual void DrawDebugPrimitive();

	//�ʒu���
	const DirectX::XMFLOAT3& GetPosition() const { return position; }

	//�����擾
	const DirectX::XMFLOAT3& GetDirection() const { return direction; }

	//�X�P�[���擾
	const DirectX::XMFLOAT3& GetScale() const { return scale; }

	//�j��
	void Destroy();

	//���a�擾
	float GetRadiys() const { return radius; }

protected:
	//�s��X�V����
	void UpdateTransform();


protected:
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 direction = { 0,0,1 }; //����
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };

	ProjectileManager* manager_ = nullptr;

	float radius = 0.5f;
};
