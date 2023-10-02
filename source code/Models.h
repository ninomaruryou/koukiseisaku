#pragma once

#include <DirectXMath.h>

#include "ProjectileManager.h"
#include "skinned_mesh.h"
#include "Static_mesh_instncing.h"
#include "framework.h"
#include "shadow map.h"
class Character
{
public:
	 Character(){}
    ~Character() = default;

    virtual  void Update(float elapsedTime);

    virtual void Render(ID3D11DeviceContext*, float);
    void Render(gltf_model* model,ID3D11DeviceContext*, float elapsedTime);

    void Gravity(float dt);

	virtual XMFLOAT4X4 Set_Transform(XMFLOAT4X4* world);


	//�AIntegrate�֐�����������B
// �^���̑�P�@���w�Î~���Ă��鎿�_�́A�͂��������Ȃ�����Î~�𑱂���B�^�����Ă��鎿�_�́A�͂��������Ȃ����蓙�������^���𑱂���x
// ���_�̉^���͓��������^���Ƃ��A���x��duration���|���A�ʒu�ɉ��Z����B
// �����܂Ŋ�����������s���ē�����m�F���邱��


//�CIntegrate�֐���ύX����B
// �^���̑�Q�@���w���̂��͂��󂯂�ƁA���̗͂̓��������ɉ����x��������B�����x�͗͂̑傫���ɔ�Ⴕ�A���ʂɔ���Ⴗ��B�x
// �����x�͍��͂ɂ��ω����邪���ʂɔ���Ⴗ��
// ���x�͉����x��duration���|�����l�����Z����
// ���͖͂��t���[���[���N���A���邱��
	void Integrate(float duration)
	{

		assert(mass > 0);
		XMStoreFloat3(&acceleration, XMLoadFloat3(&resultant) * (1 / mass));

		XMStoreFloat3(&velocity, XMLoadFloat3(&velocity) + XMLoadFloat3(&acceleration) * duration);

		float speed = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z) * 3.6/*km*/;
		if (speed > max_velocity)
		{
			float a = max_velocity / speed;
			velocity.x *= a;
			velocity.y *= a;
			velocity.z *= a;
		}

		XMStoreFloat3(&position, XMLoadFloat3(&position) + XMLoadFloat3(&velocity) * duration);

		resultant = {};
	}

	//�BAddForce�֐�����������
	// AddForce���\�b�h���Ăяo�����s�x�A�����ŗ^����ꂽ�O�͂������o�ϐ����́iresultant�j�ɉ��Z�����

	void AddForce(XMVECTOR force)
	{
		XMStoreFloat3(&resultant, XMLoadFloat3(&resultant) + force);
	}

	//�D�����x�͓ǂݎ���p�Ƃ��Aprivate�ϐ��{�Q�b�^�[�Ƃ��AGetAcceleration�֐�����������B
	//��SceneGame::Render�֐����̓��Y�ӏ��̃R�����g���O������
	XMFLOAT3 GetAcceleration() { return acceleration; }
	XMFLOAT3 acceleration;
	XMFLOAT3 resultant;

	float mass = 0.5f;//����
	XMFLOAT3 position = {  };//�ʒu
	XMFLOAT3 scale = { 1,1,1 };
	XMFLOAT3 forward = { -1.0f,0,-0.05f };//�O��
	XMFLOAT3 velocity = { 0,0,0 };//���x

	float max_velocity = 25;

	XMFLOAT4 Angle = {};

	XMFLOAT4 color = { 1,1,1,1 };
	DirectX::XMFLOAT4X4 world;

	const DirectX::XMFLOAT4X4 coordinate_system_transforms[4] = {
	{ -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }, // 0:RHS Y-UP
	{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 }, // 1:LHS Y-UP
	{ -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 }, // 2:RHS Z-UP
	{ 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 }, // 3:LHS Z-UP
	};

	float scale_factor = 1.0f; // To change the units from centimeters to meters, set 'scale_factor' to 0.01.
	XMFLOAT3 angle = {};
    float value = 0;
	
};




class Charater_model : public Character
{
public:
	Charater_model(){}
	Charater_model(const char* fbx_filename);
	Charater_model(const char* fbx_filename,bool in_shadow = false, int MAX = 1);

	virtual void Update(float elapsedTime);

	bool AppkyDamage(int damage, float invincibleTime);

	void Render(ID3D11DeviceContext*, float elapsedTime = 0)override;

	void ShadowRender(ID3D11DeviceContext*, float elapsedTime = 0);

	virtual void OnDead() {}

	void IMGUI();

	std::unique_ptr<Skinned_Mesh> model;

	std::unique_ptr<Shadow_map> shadow_map;

	int  health = 5; //���N���
	int maxhealth = 5;
	float invincibleTimer = 1.0f;

};




class GlTF_Character : public Character
{
public:
	GlTF_Character() {}
	GlTF_Character(const char* fbx_filename);

	virtual void Update(float elapsedTime);

	bool AppkyDamage(int damage, float invincibleTime);

	void Render(ID3D11DeviceContext*, float elapsedTime = 0)override;

	void ShadowRender(ID3D11DeviceContext*, float elapsedTime = 0);

	XMFLOAT4X4 Set_Transform(XMFLOAT4X4* world) override;

	virtual void OnDead() {}

	void IMGUI();

	std::unique_ptr<gltf_model> model;

	std::unique_ptr<Shadow_map> shadow_map;

	int  health = 5; //���N���
	int maxhealth = 5;
	float invincibleTimer = 1.0f;

	float timer = 0;
};

class Player :public GlTF_Character
{
public:
	Player(ID3D11Device* device, const char* fbx_filename, bool in_shadow = false, int MAX = 1);

	void Update(float elapsedTime) override;

	void Render(ID3D11DeviceContext*, float) override;

	void InputProjectile();
	ProjectileManager projectileManager;

	void Turn(float elapsedTime, float vx, float vz);

	void CollisionProjectilesVsEnemies();



};



class Instncing_Character
{
public:
	Instncing_Character(){}
	Instncing_Character(ID3D11Device* device, const char* fbx_filename, int MAX);

	void Render( float);

	std::unique_ptr<Static_mesh_instncing> model;

	std::vector<Character> characters;

	
};

