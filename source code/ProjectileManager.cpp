#include "ProjectileManager.h"

#include "RenderContext.h"

//�R���X�g���N�^
ProjectileManager::ProjectileManager()
{

}

//�f�X�g���N�^
ProjectileManager::~ProjectileManager()
{
	Clear();
}

//�X�V����
void ProjectileManager::Updata(float elapsedTime)
{
	//�X�V����
	for (auto& obj : projectiles)
	{
		obj->Updata(elapsedTime);
	}

	//�j������
	//projectiles�͈̔�for������erase()����ƕs����������Ă��܂����߁A
	//�X�V�������I�������ɔj�����X�g�ɐς܂ꂽ�I�u�W�F�N�g���폜����B
	for (Projectile* projectile : rmoves)
	{
		//std::vector����폜����ɂ̓C���e���\�^�[�ō폜���Ȃ��Ă͂Ȃ�Ȃ�
		std::vector<Projectile*>::iterator it = std::find(projectiles.begin(),
			projectiles.end(), projectile
		);

		if (it != projectiles.end())
		{
			projectiles.erase(it);
		}

		//�e�ۂ̔j������
		delete projectile;
	}

	rmoves.clear();
}

//�`�揈��
void ProjectileManager::Render(ID3D11DeviceContext* dc,float elapsedTime)
{
	for (auto& obj : projectiles)
	{
		obj->Render(dc,elapsedTime);
	}
}

//�f�o�b�N�v���~�e�B�u�`��
void ProjectileManager::DrawDebugPrimitive()
{
	for (auto& obj : projectiles)
	{
		obj->DrawDebugPrimitive();
	}
}

//�e�ۓo�^
void ProjectileManager::Register(Projectile* projectile)
{
	projectiles.emplace_back(projectile);
}


//�e�ۍ폜
void ProjectileManager::Clear()
{
	for (auto& obj : projectiles)
	{
		delete obj;
	}
}

void ProjectileManager::Remove(Projectile* projectile)
{
	//�j�����X�g�ɒǉ�
	rmoves.insert(projectile);
}
