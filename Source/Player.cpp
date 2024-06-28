#include <imgui.h>
#include "Player.h"
#include "Input/Input.h"
#include "Graphics/Graphics.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "ProjectileStraight.h"
#include "ProjectileHoming.h"

Player::Player()
{
    model = new Model("Data/Model/Mr.Incredible/Mr.Incredible.mdl");

    scale.x = scale.y = scale.z = 0.01f;

    hitEffect = new Effect("Data/Effect/Hit.efk");
}

Player::~Player()
{
    delete hitEffect;
    delete model;
}

void Player::Update(float elapsedTime)
{
    InputMove(elapsedTime);

    InputJump();

    InputProjectile();
    
    ColllisionPlayerVsEnemies();
    
    CollisionProjectilesVsEnemies();

    //�I�u�W�F�N�g�s����X�V
    UpdateTransform();

    UpdateVelocity(elapsedTime);

    projectileManager.Update(elapsedTime);
    //���f���s��X�V
    model->UpdateTransform(transform);
}

void Player::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
    projectileManager.Render(dc, shader);
}

void Player::DrawDwbugGUI()
{
    ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

    if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
    {
        if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::InputFloat3("Position", &position.x);
            DirectX::XMFLOAT3 a;
            a.x = DirectX::XMConvertToDegrees(angle.x);
            a.y = DirectX::XMConvertToDegrees(angle.y);
            a.z = DirectX::XMConvertToDegrees(angle.z);
            ImGui::InputFloat3("Angle", &a.x);
            angle.x = DirectX::XMConvertToRadians(a.x);
            angle.y = DirectX::XMConvertToRadians(a.y);
            angle.z = DirectX::XMConvertToRadians(a.z);

            ImGui::InputFloat3("Scale", &scale.x);
        }
    }
    ImGui::End();
}

DirectX::XMFLOAT3 Player::GetMoveVec() const
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    float ax = gamePad.GetAxisLX();
    float ay = gamePad.GetAxisLY();

    Camera& camera = Camera::Instance();
    const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
    const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

    float cameraRightX = cameraRight.x;
    float cameraRightZ = cameraRight.z;
    float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
    if (cameraRightLength > 0.0f)
    {
        cameraRightX /= cameraRightLength;
        cameraRightZ /= cameraRightLength;
    }

    float cameraFrontX = cameraFront.x;
    float cameraFrontZ = cameraFront.z;

    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
    if (cameraFrontLength > 0.0f)
    {
        cameraFrontX /= cameraFrontLength;
        cameraFrontZ /= cameraFrontLength;
    }

    DirectX::XMFLOAT3 vec;
    vec.x = (cameraRightX * ax) + (cameraFrontX * ay);
    vec.z = (cameraRightZ * ax) + (cameraFrontZ * ay);
    vec.y = 0.0f;
    return vec;
}

void Player::DrawDebugPrimitive()
{
    DebugRenderer* debugRenderer = Graphics::Instance().GetDebugRenderer();

    //�Փ˔���p�p�̃f�o�b�O����`��
    debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
    projectileManager.DrawDebugPrimitive();
}

void Player::InputMove(float elapsedTime)
{
    DirectX::XMFLOAT3 moveVec = GetMoveVec();

    Move(moveVec.x,moveVec.z,moveSpeed);

    Turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
}


void Player::InputJump()
{
    GamePad& gamePad = Input::Instance().GetGamePad();
    if (position.y <= 0) jumpCount = 0;
    if (gamePad.GetButtonDown() & GamePad::BTN_A)
    {
        if (jumpCount < jumpLimit)
        {
            jumpCount++;
            Jump(jumpSpeed);
        }
    }
}

void Player::InputProjectile()
{
#if true
    GamePad& gamePad = Input::Instance().GetGamePad();

    //���i�e�۔���
    if (gamePad.GetButtonDown() & GamePad::BTN_X)
    {
        //�O����
        DirectX::XMFLOAT3 dir;
        dir.x = sinf(angle.y);
        dir.y = 0.0f;
        dir.z = cosf(angle.y);
            //���ˈʒu(�v���C���[�̍�������)
        DirectX::XMFLOAT3 pos;
        pos.x = position.x;
        pos.y = position.y + height / 2;
        pos.z = position.z;
        //����
        ProjectileStraight* projectile = new ProjectileStraight(&projectileManager);
        projectile->Launch(dir, pos);
    }
    if (gamePad.GetButtonDown() & GamePad::BTN_Y)
    {
        //�O����
        DirectX::XMFLOAT3 dir;
        dir.x = sinf(angle.y);
        dir.y = 0.0f;
        dir.z = cosf(angle.y);
        //���ˈʒu(�v���C���[�̍�������)
        DirectX::XMFLOAT3 pos;
        pos.x = position.x;
        pos.y = position.y + height / 2;
        pos.z = position.z;
        //�^�[�Q�b�g
        DirectX::XMFLOAT3 target;
        target.x = pos.x + dir.x * 1000.0f;
        target.y = pos.y + dir.y * 1000.0f;
        target.z = pos.z + dir.z * 1000.0f;
        //��ԋ߂��̓G���^�[�Q�b�g�ɂ���
        //��ԉ����ꏊ��ݒ肵�Ă���
        float dist = FLT_MAX;
        //�G�}�l�[�W���[���擾
        EnemyManager& enemyManager = EnemyManager::Instance();
        int enemyCount = enemyManager.GetEnemyCount();
        // �G�̐����擾
        // ��ԋ߂��G�G�̐������`�F�b�N����
        for (int i=0;i<enemyCount;++i)
        {
            //�G�Ƃ̋�������
            //1�Ԗڂ̓G���擾
            Enemy* enemy = EnemyManager::Instance().GetEnemy(i);
            //�v���C���[(����)�̈ʒu��XMVECTOR�ɕϊ�
            DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
            DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
            DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E,P);
            //�G�̈ʒu��XMVECTOR�ɕϊ�
            //�G�����ւ̃x�N�g�����v�Z
            //���̃x�N�g���̒������v�Z��
            DirectX::XMVECTOR D = DirectX::XMVector3Length(V);
            
            //�P��̒l�ɂ���
            float d;
            DirectX::XMStoreFloat(&d, D);
            //���܂Ōv�Z���������̒��ň�ԏ����������Ȃ�
            if (d < dist)
            {
                dist = d;
                //target�̈ʒu���X�V(�G�̈ʒu��ݒ�)
                target = enemy->GetPosition();
                //target��y�������̍����ɂ��Ă���
                target.y += enemy->GetHeight() * 0.5f;
            }
        }
        //����
        ProjectileHoming* projectile = new ProjectileHoming(&projectileManager);
        projectile->Launch(dir, pos, target);
    }
#endif
}

void Player::OnLanding()
{
    jumpCount = 0;
}

void Player::ColllisionPlayerVsEnemies()
{
    EnemyManager& enemyManager = EnemyManager::Instance();

    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy = enemyManager.GetEnemy(i);

        DirectX::XMFLOAT3 outPosition;
        if (Collision::IntersectCylinderVsCylinder(position,radius,height,enemy->GetPosition(),enemy->GetRadius(),enemy->GetHeight(),outPosition))
        {
            //enemy->SetPosition(outPosition);
            
            DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
            DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
            DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P,E);
            DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
            DirectX::XMFLOAT3 normal;
            DirectX::XMStoreFloat3(&normal, N);
            //enemy->ApplyDamage(1,0.5f);
            if (normal.y > 0.8f)
            {
                Jump(jumpSpeed * 0.5f);
            }
            else
            {
                enemy->SetPosition(outPosition);
            }
        }
    }
    //�G�̐^��t�߂ɓ������������`�F�b�N
}

void Player::CollisionProjectilesVsEnemies()
{
    EnemyManager& enemyManager = EnemyManager::Instance();
//�S�Ă̒e�ۂƑS�Ă̓G�𑍓�����ŏՓˏ���
    int projectileCount = projectileManager.GetProjectileCount();
    int enemyCount = enemyManager.GetEnemyCount();
    for (int i = 0; i < projectileCount; ++i)
    {
        Projectile* projectile = projectileManager.GetProlectile(i);

        for (int j = 0; j < enemyCount; ++j)
        {
            Enemy* enemy = enemyManager.GetEnemy(j);

            //�Փˏ���
            DirectX::XMFLOAT3 outPosition;
            if (Collision::IntersectSphereVsCylinder(
                projectile->GetPosition(),
                projectile->GetRadius(),
                enemy->GetPosition(),
                enemy->GetRadius(),
                enemy->GetHeight(),
                outPosition))
            {
                //�_���[�W��^����
                if (enemy->ApplyDamage(1, 0.5f))
                {
                    //������΂�
                    {
                        DirectX::XMFLOAT3 impulse;
                        const float power = 10.0f; //������΂���
                        //�G�̈ʒu���擾
                        const DirectX::XMFLOAT3& e = enemy->GetPosition();
                        //�G�̈ʒu���擾
                        const DirectX::XMFLOAT3& p = projectile->GetPosition();
                        //�e����G�̈ʒu�ւ̃x�N�g�����擾
                        float vx = e.x - p.x;
                        float vz = e.z - p.z;
                        //���̃x�N�g���𐳋K��(�������v�Z���A�����Ŋ���)
                        float lengthXZ = sqrtf(vx*vx+vz*vz);
                        vx /= lengthXZ;
                        vz /= lengthXZ;
                        //�Ռ��̒l��ݒ�(xz�͐��K�������x�N�g����power���X�P�[�����O)
                        impulse.x = vx*power;
                        impulse.y = power * 0.5f; //y�͂�����ƕ�������
                        impulse.z = vz*power;
                        enemy->AddImpulse(impulse);
                    }
                    //�q�b�g�G�t�F�N�g�Đ�
                    {
                        DirectX::XMFLOAT3 e = enemy->GetPosition();
                        e.y += enemy->GetHeight() * 0.5f;
                        hitEffect->Play(e);
                    }
                    //�e�۔j��
                    projectile->Destroy();
                }
            }
        }
    }
}
