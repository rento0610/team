#include "ProjectileHoming.h"

ProjectileHoming::ProjectileHoming(ProjectileManager* manager):Projectile(manager)
{
    model = new Model("Data/Model/Sword/Sword.mdl");

    scale.x = scale.y = scale.z = 3.0f;
}
ProjectileHoming::~ProjectileHoming()
{
    delete model;
}

void ProjectileHoming::Update(float elapsedTime)
{
#if true
    //��������
    lifeTimer -= elapsedTime;
    if (lifeTimer <= 0)
    {
        Destroy();
    }
    //�ړ�
    {
        float moveSpeed = this->moveSpeed * elapsedTime;
        //�ʒu+=�O����*����
        position.x += direction.x * moveSpeed;
        position.y += direction.y * moveSpeed;
        position.z += direction.z * moveSpeed;
    }
    //����
    {
        float turnSpeed = this->turnSpeed * elapsedTime;

        //�^�[�Q�b�g�܂ł̃x�N�g�����Z�o
        DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
        DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);
        DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Target,Position);

        DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
        float lengthSq;
        DirectX::XMStoreFloat(&lengthSq, LengthSq);
        if (lengthSq > 0.00001f)
        {
            //�^�[�Q�b�g�܂ł̃x�N�g����P�ʃx�N�g����(���K��) �x�N�g����=���K��
            Vec = DirectX::XMVector3Normalize(Vec);
            //�����Ă������(�O������)�x�N�g����XMVECTOR�ɕϊ�
            DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);
            //�O�������x�N�g���ƃ^�[�Q�b�g�܂ł̃x�N�g���̓���(�p�x)���Z�o
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Direction,Vec);

            float dot;
            DirectX::XMStoreFloat(&dot, Dot);

            //2�̒P�ʃx�N�g���̊p�x���������ق�
            //1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
            float rot = 1.0f - dot;
            if (rot > turnSpeed) rot = turnSpeed;
            //��]�p�x������Ȃ��]����������
            //��]�p�x�͐�Βl��0���ォ�ǂ������`�F�b�N����
            if (fabsf(rot)>0.0001f)
            {
                //��]�����v�Z(�O�������x�N�g���ƃ^�[�Q�b�g�܂ł̃x�N�g���̊O��)
                DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Direction, Vec);
                Axis = DirectX::XMVector3Normalize(Axis);
                //��]���Ɖ�]�ʂ���DirectX::XMMatriRotationAxis���g���ĉ�]�s����Z�o
                DirectX::XMMATRIX Rotation =DirectX::XMMatrixRotationAxis(Axis,rot) ;
                //���݂̍s�����]������
                    DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
                    //��]��̎p��=���݂̎p��*��]�s��
                    Transform = DirectX::XMMatrixMultiply(Transform,Rotation);
                    //��]��̑O�����������o��,�P�ʃx�N�g��������
                    Direction = DirectX::XMVector3Normalize(Transform.r[2]);
                    DirectX::XMStoreFloat3(&direction, Direction);
            }
        }
    }
    UpdateTransform();

    model->UpdateTransform(transform);
#endif
}

void ProjectileHoming::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}

void ProjectileHoming::Launch(const DirectX::XMFLOAT3& direction,
    const DirectX::XMFLOAT3& position,
    const DirectX::XMFLOAT3& target)
{
    this->direction = direction;
    this->position = position;
    this->target = target;

    UpdateTransform();
}