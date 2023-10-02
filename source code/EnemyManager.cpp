#include "EnemyManager.h"

#include "Collision.h"

//更新処理

void EnemyManager::Updata(float elapsedTime)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Update(elapsedTime);

		//破棄処理参照
		//projectiles
		for (Enemy* enemy : removees)
		{
			//std::vectorから削除するにはインテレ―ターで削除しなくてはならない
			std::vector<Enemy*>::iterator it = std::find(enemies.begin(),
				enemies.end(), enemy
			);

			if (it != enemies.end())
			{
				enemies.erase(it);
			}

			//弾丸の破棄処理
			delete enemy;
		}
	}

	CollisionEnemyVsEnemes();
}

void EnemyManager::Rebder(ID3D11DeviceContext* imm_context)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Render(imm_context);
	}
}

void EnemyManager::Registe(Enemy* enemy)
{
	enemies.emplace_back(enemy);
}

void EnemyManager::DrawDebugPrimitive()
{
	for (Enemy* enemy : enemies)
	{
		enemy->DrawDebugPrimitive();
	}

}


void EnemyManager::Clear()
{
	for (Enemy* enemy : enemies)
	{
		delete enemy;
	}

}

void EnemyManager::CollisionEnemyVsEnemes()
{

	//全ての敵と総当たりで衝突処理
	int enemyCount = enemies.size();
	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemies.at(i);

		for (int i = 0; i < enemyCount; ++i)
		{
			Enemy* enemy2 = enemies.at(i);

			if (enemy == nullptr)continue;
			if (enemy2 == nullptr)continue;

			if (enemy == enemy2) continue;

			//衝突処理
			DirectX::XMFLOAT3 outPosition;

			if (Collision::InterseetSpheVsSphere(enemy->position, 1,
				enemy2->position, 1,
				outPosition))
			{
				enemy2->position = outPosition;
			}
		}
	}
}

void EnemyManager::Remove(Enemy* enemy)
{
	//破棄リストに追加
	removees.insert(enemy);

}
