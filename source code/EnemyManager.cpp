#include "EnemyManager.h"

#include "Collision.h"

//�X�V����

void EnemyManager::Updata(float elapsedTime)
{
	for (Enemy* enemy : enemies)
	{
		enemy->Update(elapsedTime);

		//�j�������Q��
		//projectiles
		for (Enemy* enemy : removees)
		{
			//std::vector����폜����ɂ̓C���e���\�^�[�ō폜���Ȃ��Ă͂Ȃ�Ȃ�
			std::vector<Enemy*>::iterator it = std::find(enemies.begin(),
				enemies.end(), enemy
			);

			if (it != enemies.end())
			{
				enemies.erase(it);
			}

			//�e�ۂ̔j������
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

	//�S�Ă̓G�Ƒ�������ŏՓˏ���
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

			//�Փˏ���
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
	//�j�����X�g�ɒǉ�
	removees.insert(enemy);

}
