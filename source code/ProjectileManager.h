#pragma once

#include <set>
#include <vector>
#include "Projectile.h"

//�e�ۃ}�l�[�W���[
class ProjectileManager
{
public:
	ProjectileManager();
	~ProjectileManager();

	//�X�V����
	virtual void Updata(float elapsedTime);

	//�`�ʏ���
	virtual void Render(ID3D11DeviceContext* dc, float elapsedTime);

	//�f�o�b�N�v���~�e�B�u�`��
	virtual void DrawDebugPrimitive();

	//�e�ۓo�^
	void Register(Projectile* projectile);

	//�e�ۏ���
	void Clear();

	//�e�ې��擾
	int GetProjectileCount() const { return static_cast<int>(projectiles.size()); }

	//�e�ێ擾
	Projectile* GetProfectile(int index) { return projectiles.at(index); }

	//�e�ۍ폜
	void Remove(Projectile* projectile);

private:
	std::vector<Projectile*> projectiles;

	std::set<Projectile*> rmoves;
};
