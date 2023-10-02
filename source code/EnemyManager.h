#pragma once

#include <set>
#include <vector>
#include "Enemy.h"
#include "RenderContext.h"

class EnemyManager
{
private:
	EnemyManager() {}
	~EnemyManager() {}

public:
	static EnemyManager& Instance()
	{
		static EnemyManager instance;
		return instance;
	}

	//更新処理
	void Updata(float elapsedTime);

	//描写処理
	void Rebder(ID3D11DeviceContext* context);

	void Registe(Enemy* enemy);


	void DrawDebugPrimitive();
	//エネミー全削除
	void Clear();

	//エネミー削除
	void Remove(Enemy* enemy);

	//エネミーとエネミーとの衝突処理
	void CollisionEnemyVsEnemes();

	int GetEnemyConut() const { return static_cast<int>(enemies.size()); }

	Enemy* GetEnemy(int index) { return enemies.at(index); }

private:
	std::vector<Enemy*> enemies;

	std::set<Enemy*> removees;
};
