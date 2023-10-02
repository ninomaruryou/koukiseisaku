#include "ProjectileManager.h"

#include "RenderContext.h"

//コンストラクタ
ProjectileManager::ProjectileManager()
{

}

//デストラクタ
ProjectileManager::~ProjectileManager()
{
	Clear();
}

//更新処理
void ProjectileManager::Updata(float elapsedTime)
{
	//更新処理
	for (auto& obj : projectiles)
	{
		obj->Updata(elapsedTime);
	}

	//破棄処理
	//projectilesの範囲for文中でerase()すると不具合が発生してしまうため、
	//更新処理が終わった後に破棄リストに積まれたオブジェクトを削除する。
	for (Projectile* projectile : rmoves)
	{
		//std::vectorから削除するにはインテレ―ターで削除しなくてはならない
		std::vector<Projectile*>::iterator it = std::find(projectiles.begin(),
			projectiles.end(), projectile
		);

		if (it != projectiles.end())
		{
			projectiles.erase(it);
		}

		//弾丸の破棄処理
		delete projectile;
	}

	rmoves.clear();
}

//描画処理
void ProjectileManager::Render(ID3D11DeviceContext* dc,float elapsedTime)
{
	for (auto& obj : projectiles)
	{
		obj->Render(dc,elapsedTime);
	}
}

//デバックプリミティブ描画
void ProjectileManager::DrawDebugPrimitive()
{
	for (auto& obj : projectiles)
	{
		obj->DrawDebugPrimitive();
	}
}

//弾丸登録
void ProjectileManager::Register(Projectile* projectile)
{
	projectiles.emplace_back(projectile);
}


//弾丸削除
void ProjectileManager::Clear()
{
	for (auto& obj : projectiles)
	{
		delete obj;
	}
}

void ProjectileManager::Remove(Projectile* projectile)
{
	//破棄リストに追加
	rmoves.insert(projectile);
}
