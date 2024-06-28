#pragma once

#include <DirectXMath.h>

class Character
{
public:
    Character() {}
    virtual ~Character(){}

    void UpdateTransform();

    const DirectX::XMFLOAT3& GetPosition() const { return position; };

    void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; };

    const DirectX::XMFLOAT3 GetAngle() const { return angle; };

    void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; };

    const DirectX::XMFLOAT3& GetScale() const { return scale; }

    void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
    //���a�擾
    float GetRadius() const { return radius; }
    //�Ռ���^����
    void AddImpulse(const DirectX::XMFLOAT3& impulse);

    bool IsGround() const { return isGround; };

    float GetHeight() const { return height; }

    bool ApplyDamage(int damage, float invincibleTime);

    int GetHealth() const { return health; }

    int GetMaxHealth() const { return maxHealth; }
protected:
    void Jump(float speed);

    void UpdateVelocity(float elapssedTime);
    //���G���ԍX�V
    void UpdateInvincibleTimer(float elapsedTime);
     void Move(float vx, float vz, float speed);
     void Turn(float elapsedTime, float vx, float vz, float speed);
     virtual void OnLanding() {}

     virtual void onDamaged() {}

     virtual void OnDead() {}
    //�p����ł��A�N�Z�X�ł���悤�ɂ���
protected:
    DirectX::XMFLOAT3 position = { 0,0,0 };
    DirectX::XMFLOAT3 angle = { 0,0,0 };
    DirectX::XMFLOAT3 scale = { 1,1,1 };
    DirectX::XMFLOAT4X4 transform = {
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1
    };

    float radius = 0.5f;
    float gravity = -1.0f;
    DirectX::XMFLOAT3 velocity = { 0,0,0 };
    bool isGround = false;
    float height = 2.0f;
    int health = 5;
    int maxHealth = 5;
    float invincibleTimer = 1.0f;
    float friction = 0.5f;
    float acceleration = 1.0f;
    float maxMoveSpeed = 5.0f;
    float moveVecX = 0.0f;
    float moveVecZ = 0.0f;
    float airControl = 0.04f;
    float stepOffset = 1.0f;
    float slopeRate = 1.0f;
private:
    //�������͍X�V����
    void UpdateVerticalVelocity(float elapsedFrame);
    //�����ړ��X�V����
    void UpdateVerticalMove(float elapsedTime);
    //�������͍X�V����
    void UpdateHorizontalVelocity(float elapsedFrame);
    //�����ړ��X�V����
    void UpdateHorizontalMove(float elapsedTime);
};