#pragma once

#include "Enemy.h"

class EnemySlime : public Enemy
{
public:
	EnemySlime();
	~EnemySlime();

	//	�X�V����
	void Update(float elapsedTime) override;

	//�`�揈��
	void Render(ID3D11DeviceContext* dc) override;

	//���S�����Ƃ��ɌĂ΂��
	void OnDead();
private:
	Charater_model* model = nullptr;
};
