#include "EnemySlime.h"
#include "EnemyManager.h"
#include "framework.h"
#include "COM_obj_.h"

EnemySlime::EnemySlime()
{
	model = new Charater_model(".\\resources\\Mr.Incredible",false);

	//モデルが大きいのでスケーリング
	scale.x = scale.y = scale.z = 1;

}

//デストラクタ
EnemySlime::~EnemySlime()
{
	delete model;
}

void EnemySlime::Update(float elapsedTime)
{
	//モデル行列更新
	model->Set_Transform(&model->world);


}


void EnemySlime::Render(ID3D11DeviceContext* dc)
{
	model->Render(dc);
}

void EnemySlime::OnDead()
{
	Destroy();
}

