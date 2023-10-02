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

	//�X�V����
	void Updata(float elapsedTime);

	//�`�ʏ���
	void Rebder(ID3D11DeviceContext* context);

	void Registe(Enemy* enemy);


	void DrawDebugPrimitive();
	//�G�l�~�[�S�폜
	void Clear();

	//�G�l�~�[�폜
	void Remove(Enemy* enemy);

	//�G�l�~�[�ƃG�l�~�[�Ƃ̏Փˏ���
	void CollisionEnemyVsEnemes();

	int GetEnemyConut() const { return static_cast<int>(enemies.size()); }

	Enemy* GetEnemy(int index) { return enemies.at(index); }

private:
	std::vector<Enemy*> enemies;

	std::set<Enemy*> removees;
};
