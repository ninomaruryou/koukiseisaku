#pragma once
#include "Models.h"


class Enemy : public Charater_model
{
public:
	Enemy() {}

	//更新処理
	void Update(float elapsedTime) override;

	//描画処理
	virtual void Render(ID3D11DeviceContext* dc) {};

	void Destroy();

	//デバックプリミティブ描画
	void DrawDebugPrimitive();

};
