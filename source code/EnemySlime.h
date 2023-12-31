#pragma once

#include "Enemy.h"

class EnemySlime : public Enemy
{
public:
	EnemySlime();
	~EnemySlime();

	//	更新処理
	void Update(float elapsedTime) override;

	//描画処理
	void Render(ID3D11DeviceContext* dc) override;

	//死亡したときに呼ばれる
	void OnDead();
private:
	Charater_model* model = nullptr;
};
