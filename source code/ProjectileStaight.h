#pragma once

#include "Projectile.h"
#include "Models.h"

//’¼i’eŠÛ
class ProjectileStaight : public Projectile
{
public:
	ProjectileStaight(ProjectileManager* manager);
	//ProjectileStaight();
	~ProjectileStaight();

	//XVˆ—
	virtual void Updata(float elapsedTime) override;

	//•`Êˆ—
	virtual void Render(ID3D11DeviceContext* dc,float elapsed_time) override;

	//”­Ë
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& positon);

private:
	Charater_model* model = nullptr;
	float speed = 10.0f;
	float lifeTime = 6.0;
};
