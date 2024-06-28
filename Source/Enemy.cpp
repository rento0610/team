#include "Enemy.h"
#include "Graphics/Graphics.h"
#include "EnemyManager.h"

void Enemy::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();
    debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}
//�j��
void Enemy::Destroy()
{
    //�G�}�l�[�W���[�̍폜�Ɏ������g�̃A�h���X��n��
    EnemyManager::Instance().Remove(this);
}