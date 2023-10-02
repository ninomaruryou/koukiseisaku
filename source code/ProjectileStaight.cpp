#include "ProjectileStaight.h"

//コンストラクタ
ProjectileStaight::ProjectileStaight(ProjectileManager* manager)
	: Projectile(manager)
{
	//model = new Model("Data/Model/SpikeBall/SpikeBall.mdl");
	model = new Charater_model(".\\resources\\cube.000.fbx",false);

	//表示サイズを調整
	//scale.x = scale.y = scale.z = 0.5f;
	scale.x = scale.y = scale.z = 3.0f;

}

//デストラクタ
ProjectileStaight::~ProjectileStaight()
{
	delete model;
}

//更新処理
void ProjectileStaight::Updata(float elapsedTime)
{
	//寿命処理
	this->lifeTime -= 1.0f * elapsedTime;
	if (lifeTime < 0)
	{
		Destroy();
	}

	//移動
	float movespeed = this->speed * elapsedTime;

	model->position.x += direction.x * movespeed;
	model->position.z += direction.z * movespeed;



	//モデル行列
	model->Set_Transform(&model->world);
}

//描写処理
void ProjectileStaight::Render(ID3D11DeviceContext* dc,float elapsed_time)
{
	model->Render(dc,elapsed_time);
}

//発射
void ProjectileStaight::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& positon)
{
	this->direction = direction;
	model->position = positon;
}
