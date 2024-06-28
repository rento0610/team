#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Character.h"
#include "ProjectileManager.h"
#include "Effect.h"

class Player :public Character
{
public:
    Player();
    ~Player() override;

    void Update(float elapsedTime);

    void Render(ID3D11DeviceContext* dc, Shader* shader);

    //�f�o�b�O�pGUI�`��
    void DrawDwbugGUI();
    //�f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

    void InputJump();
    //�e�ۂƓG�̏Փˏ���
    void CollisionProjectilesVsEnemies();


protected:
    void OnLanding() override;

private:
    Model* model = nullptr;
    float moveSpeed = 10.0f;

    void InputMove(float elapsedTime);
    void InputProjectile();
    void ColllisionPlayerVsEnemies();
    DirectX::XMFLOAT3 GetMoveVec() const;
    float jumpSpeed = 20.0f;
    float turnSpeed = DirectX::XMConvertToRadians(720);
    int jumpCount = 0;
    int jumpLimit = 2;
    ProjectileManager projectileManager;
    Effect* hitEffect = nullptr;
};