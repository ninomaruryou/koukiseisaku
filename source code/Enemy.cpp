#include "Enemy.h"

#include "EnemyManager.h"

void Enemy::Update(float elapsedTime)
{
	
}


void Enemy::DrawDebugPrimitive()
{

}

void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}
