#include "ProjectileStaight.h"

//�R���X�g���N�^
ProjectileStaight::ProjectileStaight(ProjectileManager* manager)
	: Projectile(manager)
{
	//model = new Model("Data/Model/SpikeBall/SpikeBall.mdl");
	model = new Charater_model(".\\resources\\cube.000.fbx",false);

	//�\���T�C�Y�𒲐�
	//scale.x = scale.y = scale.z = 0.5f;
	scale.x = scale.y = scale.z = 3.0f;

}

//�f�X�g���N�^
ProjectileStaight::~ProjectileStaight()
{
	delete model;
}

//�X�V����
void ProjectileStaight::Updata(float elapsedTime)
{
	//��������
	this->lifeTime -= 1.0f * elapsedTime;
	if (lifeTime < 0)
	{
		Destroy();
	}

	//�ړ�
	float movespeed = this->speed * elapsedTime;

	model->position.x += direction.x * movespeed;
	model->position.z += direction.z * movespeed;



	//���f���s��
	model->Set_Transform(&model->world);
}

//�`�ʏ���
void ProjectileStaight::Render(ID3D11DeviceContext* dc,float elapsed_time)
{
	model->Render(dc,elapsed_time);
}

//����
void ProjectileStaight::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& positon)
{
	this->direction = direction;
	model->position = positon;
}
