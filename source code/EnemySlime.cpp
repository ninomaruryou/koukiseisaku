#include "EnemySlime.h"
#include "EnemyManager.h"
#include "framework.h"
#include "COM_obj_.h"

EnemySlime::EnemySlime()
{
	model = new Charater_model(".\\resources\\Mr.Incredible",false);

	//���f�����傫���̂ŃX�P�[�����O
	scale.x = scale.y = scale.z = 1;

}

//�f�X�g���N�^
EnemySlime::~EnemySlime()
{
	delete model;
}

void EnemySlime::Update(float elapsedTime)
{
	//���f���s��X�V
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

