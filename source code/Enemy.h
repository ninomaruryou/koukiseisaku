#pragma once
#include "Models.h"


class Enemy : public Charater_model
{
public:
	Enemy() {}

	//�X�V����
	void Update(float elapsedTime) override;

	//�`�揈��
	virtual void Render(ID3D11DeviceContext* dc) {};

	void Destroy();

	//�f�o�b�N�v���~�e�B�u�`��
	void DrawDebugPrimitive();

};
