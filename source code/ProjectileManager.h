#pragma once

#include <set>
#include <vector>
#include "Projectile.h"

//弾丸マネージャー
class ProjectileManager
{
public:
	ProjectileManager();
	~ProjectileManager();

	//更新処理
	virtual void Updata(float elapsedTime);

	//描写処理
	virtual void Render(ID3D11DeviceContext* dc, float elapsedTime);

	//デバックプリミティブ描画
	virtual void DrawDebugPrimitive();

	//弾丸登録
	void Register(Projectile* projectile);

	//弾丸処理
	void Clear();

	//弾丸数取得
	int GetProjectileCount() const { return static_cast<int>(projectiles.size()); }

	//弾丸取得
	Projectile* GetProfectile(int index) { return projectiles.at(index); }

	//弾丸削除
	void Remove(Projectile* projectile);

private:
	std::vector<Projectile*> projectiles;

	std::set<Projectile*> rmoves;
};
