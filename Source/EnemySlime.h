#pragma once

#include "Graphics/Model.h"
#include "Enemy.h"

class EnemySlime :public Enemy
{
public:
    EnemySlime();
    ~EnemySlime()override;

    void Update(float elapsedTime) override;

    void Render(ID3D11DeviceContext* dc, Shader* shader) override;
protected:
    //���S�������ɌĂ΂��
    void OnDead() override;
private:
    Model* model = nullptr;
};