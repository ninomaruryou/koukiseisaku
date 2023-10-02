#pragma once

#include "Enemy.h"

class EnemySlime : public Enemy
{
public:
	EnemySlime();
	~EnemySlime();

	//	XVˆ—
	void Update(float elapsedTime) override;

	//•`‰æˆ—
	void Render(ID3D11DeviceContext* dc) override;

	//€–S‚µ‚½‚Æ‚«‚ÉŒÄ‚Î‚ê‚é
	void OnDead();
private:
	Charater_model* model = nullptr;
};
