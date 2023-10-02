#include "Models.h"
#include "../imgui/imgui.h"
#include "SceneManager.h"
#include "ProjectileStaight.h"
#include "Collision.h"
#include "EnemyManager.h"
#include "../Mathf.h"

Charater_model::Charater_model( const char* fbx_filename)
{
	model = std::make_unique<Skinned_Mesh>(Framework::immediate_obj.device.Get(), fbx_filename);

	scale = { 0.2f,0.2f,0.2f };
}

Charater_model::Charater_model(const char* fbx_filename, bool in_shadow, int MAX)
{
	model = std::make_unique<Skinned_Mesh>(Framework::immediate_obj.device.Get(), fbx_filename);

	if(in_shadow)
	shadow_map = std::make_unique<Shadow_map>(Framework::immediate_obj.device.Get(), fbx_filename,MAX);
}


void Character::Update(float elapsedTime)
{
	Gravity(-2);

	Integrate(elapsedTime);

	Set_Transform(&world);
}

void Character::Render(ID3D11DeviceContext* immediate_context, float elapsed_time)
{

}

void Character::Render(gltf_model* model, ID3D11DeviceContext* immediate_context, float elapsedTime)
{
	static std::vector<gltf_model::node> animated_nodes{ model->nodes };

	model->render(immediate_context, world,animated_nodes,nullptr);
}


XMFLOAT4X4 Character::Set_Transform(XMFLOAT4X4* world)
{
	DirectX::XMMATRIX C{ DirectX::XMLoadFloat4x4(&coordinate_system_transforms[0]) * DirectX::XMMatrixScaling(scale_factor, scale_factor, scale_factor) };

	// UNIT.18
	DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) };
	DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(0, 0, 0) };
	DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(position.x, position.y, position.z) };
	// UNIT.21
	//DirectX::XMStoreFloat4x4(&world, S * R * T);
	DirectX::XMStoreFloat4x4(world, C * S * R * T);

	return *world;
}


void Character::Gravity(float dt)
{
	resultant.y += dt;
}

void Charater_model::Update(float elapsedTime)
{
	//Gravity(-1);

	gamepad& pad = SceneManager::Instance().pad;


	position.x += velocity.x * elapsedTime;
	position.y += velocity.y * elapsedTime;
	position.z += velocity.z * elapsedTime;

	Set_Transform(&world);

	

}


void Charater_model::Render(ID3D11DeviceContext* immediate_context, float elapsed_time)
{
	model->render(immediate_context,world, { 1,1,1,1 });

}

void Charater_model::ShadowRender(ID3D11DeviceContext* immediate_context, float elapsedTime)
{
	shadow_map->Begin(immediate_context);

	shadow_map->render(immediate_context, world, { 1,1,1,1 });

	shadow_map->End(immediate_context);
}


void Charater_model::IMGUI()
{
	ImGui::Begin("ImGUI");

	ImGui::SliderFloat("positionx", &position.x, 0.0f, 100.0f);
	ImGui::SliderFloat("positiony", &position.y, 0.0f, 100.0f);
	ImGui::SliderFloat("positionz", &position.z, 0.0f, 100.0f);
	ImGui::End();
}

bool Charater_model::AppkyDamage(int damage, float invincibleTime)
{
	//ダメージが0の場合は健康状態を変更する必要がない
	if (damage < 1) return false;

	//死亡している場合は健康状態を変更しない
	if (health < 1) return false;

	if (invincibleTimer > 0) return false;
	//ダメージ処理
	health -= damage;
	invincibleTimer = invincibleTime;

	//死亡通知
	if (health <= 0)
	{
		OnDead();
	}
	//ダメージ通知
	else
	{
		return true;
	}

	//健康状態が変更した場合はtrueを返す
	return true;

}

void Player::Update(float elapsedTime)
{
	Gravity(-2);

	gamepad& pad = SceneManager::Instance().pad;

	


	if (pad.button_state(gamepad::button::up, trigger_mode::none))
	{
		AddForce(XMVectorSet(0, 0, 5, 0));
	}
	if (pad.button_state(gamepad::button::down, trigger_mode::none))
	{
		AddForce(XMVectorSet(0, 0, -5, 0));
	}
	if (pad.button_state(gamepad::button::left, trigger_mode::none))
	{
		AddForce(XMVectorSet(-5,0,0,0));
	}
	if (pad.button_state(gamepad::button::right, trigger_mode::none))
	{
		AddForce(XMVectorSet(5, 0, 0, 0));

	}

	Turn(elapsedTime, acceleration.x, acceleration.z);

	InputProjectile();

	projectileManager.Updata(elapsedTime);

	Integrate(elapsedTime);

	//position.x += velocity.x * elapsedTime;
	//position.y += velocity.y * elapsedTime;
	//position.z += velocity.z * elapsedTime;

	Set_Transform(&world);

}



Player::Player(ID3D11Device* device, const char* fbx_filename, bool in_shadow, int MAX)
{
	model = std::make_unique<gltf_model>(device, fbx_filename);

	if(in_shadow)
		shadow_map = std::make_unique<Shadow_map>(Framework::immediate_obj.device.Get(), fbx_filename, MAX);

	scale_factor = 0.01f;
}

void Player::InputProjectile()
{
	gamepad& pad = SceneManager::Instance().pad;

	//直進発射
	if (pad.button_state(gamepad::button::x, trigger_mode::falling_edge))
	{

		//前方向
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(angle.y);
		dir.y = 0;
		dir.z = cosf(angle.y);
		//発射位置（プレイヤーの腰当たり）
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + (1 * 0.5f);
		pos.z = position.z;
		//発射
		ProjectileStaight* projectile = new ProjectileStaight(&projectileManager);
		projectile->Launch(dir, pos);
		//projectileManager.Register(projectile);
	}

	

}

void Player::Turn(float elapsedTime, float vx, float vz)
{
	//進行ベクトルがゼロベクトルの場合は処理する必要なし
	float Length = sqrtf(acceleration.x * acceleration.x + acceleration.z * acceleration.z);
	if (Length <= 0.0f)
		return;
	//進行ベクトルを単位ベクトル化
	vx /= Length;
	vz /= Length;

	//自身の回転値から前方を求める
	float frontX = sinf(angle.y);
	float frontZ = cosf(angle.y);

	float ay = angle.y;

	//　回転角を求めるため、2つの単体ベクトルの内積を計算する	
	float dot = (vx * frontX) + (vz * frontZ);

	//内積値は-1.0～1.0で表現されており、２つの単体ベクトルの角度が
	//小さいほど1.0に近ずくという性質を利用して回転速度を調整する
	float rot = 1.0f - dot; //内積は反対のため１引いて反対にする

	//左右判定を行うために２つの単体ベクトルのベクトルの外積を計算する
	float cross = (frontX * vz) - (vx * frontZ);

	//２Dの外積値が正の場合か負の場合によって左右判定が行える
	//左右判定を行うことによって左右判定を選択する
	if (cross < 0.0f)
	{
		acceleration.x += frontX;
		acceleration.z += frontZ;
		angle.y += rot;
	}
	else
	{
		acceleration.x -= frontX;
		acceleration.z -= frontZ;
		angle.y -= rot;
	}

	angle.y = Mathf::Lerp(ay, angle.y, 0.08f);

}


void Player::CollisionProjectilesVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	//全ての弾丸と全ての敵を総当たりで衝突処理
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyConut();
	for (int i = 0; i < projectileCount; i++)
	{
		Projectile* projectile = projectileManager.GetProfectile(i);
		for (int i = 0; i < enemyCount; ++i)
		{
			Enemy* enemy = enemyManager.GetEnemy(i);


			//衝突処理
			DirectX::XMFLOAT3 outPositon;
			if (Collision::InterseetSpheVsSphere(projectile->GetPosition(), projectile->GetRadiys(),
				enemy->position, 1,
				outPositon))
			{
				//ダメージを与える
				if (enemy->AppkyDamage(1, 0.5f))
				{
					////吹き飛ばす
					//{
					//	using namespace DirectX;
					//	DirectX::XMVECTOR p = XMLoadFloat3(&projectile->GetPosition());

					//	XMFLOAT3 e = { enemy->position.x,enemy->position.y,enemy->position.z };

					//	DirectX::XMVECTOR E = XMLoadFloat3(&e);

					//	XMFLOAT3 impulse;

					//	DirectX::XMVECTOR Impulse = E - p;

					//	Impulse = DirectX::XMVector3Normalize(Impulse);

					//	Impulse *= 5;
					//	DirectX::XMStoreFloat3(&impulse, Impulse);

					//	impulse.y = 10;

					//	//enemy->AddImpulse(impulse);
					//}


					//弾丸破棄
					projectile->Destroy();
				}
			}
		}
	}
}


void Player::Render(ID3D11DeviceContext* immediate_context, float elapsedTime)
{
	
	projectileManager.Render(immediate_context, elapsedTime);
}

GlTF_Character::GlTF_Character(const char* fbx_filename)
{
	model = std::make_unique<gltf_model>(Framework::immediate_obj.device.Get(), fbx_filename);

	scale = { 10.0f,10.0f,10.0f };

}

void GlTF_Character::Render(ID3D11DeviceContext* immediate_context, float elapsed_time)
{
	static std::vector<gltf_model::node> animated_nodes{ model->nodes};

	model->render(immediate_context, world,animated_nodes,nullptr);

}

void GlTF_Character::Update(float elapsedTime)
{
	Set_Transform(&world);
}

XMFLOAT4X4 GlTF_Character::Set_Transform(XMFLOAT4X4* world)
{

	// UNIT.18
	DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) };

	DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
	DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
	DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);

	DirectX::XMMATRIX R{ Y * X * Z};
	DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(position.x, position.y, position.z) };
	// UNIT.21
	//DirectX::XMStoreFloat4x4(&world, S * R * T);
	DirectX::XMStoreFloat4x4(world,  S * R * T);

	return *world;
}

void GlTF_Character::ShadowRender(ID3D11DeviceContext* immediate_context, float elapsedTime)
{
	shadow_map->Begin(immediate_context);

	shadow_map->render(immediate_context, world, { 1,1,1,1 });

	shadow_map->End(immediate_context);

}

void GlTF_Character::IMGUI()
{
	
}
