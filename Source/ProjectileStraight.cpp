#include "ProjectileStraight.h"
#include "StageManager.h"

ProjectileStraight::ProjectileStraight(ProjectileManager* manager):Projectile(manager)
{
    //model = new Model("Data/Model/SpikeBall/SpikeBall.mdl");
    model = new Model("Data/Model/Sword/Sword.mdl");
    

    //scale.x = scale.y = scale.z = 0.5f;
    scale.x = scale.y = scale.z = 3.0f;
}

ProjectileStraight::~ProjectileStraight()
{
    delete model;
}

void ProjectileStraight::Update(float elapsedTime)
{
    //��������
    lifeTimer -= elapsedTime;
    if (lifeTimer <= 0)
    {
        Destroy();
    }
    float speed = this->speed * elapsedTime;
#if false
    position.x += direction.x * speed;
    position.y += direction.y * speed;
    position.z += direction.z * speed;
#endif

    float mx = direction.x * speed;
    float my = direction.y * speed;
    float mz = direction.z * speed;

    //���˂̏���
    {
        //���C�̊J�n�ʒu�ƏI�_�ʒu
        DirectX::XMFLOAT3 start = { position.x,position.y,position.z };
        DirectX::XMFLOAT3 end = { position.x + mx,position.y + my,position.z + mz };

        //���C�L���X�g�ɂ��ǂ̔���
        HitResult hit;
        if (StageManager::Instance().RayCast(start, end, hit))
        {
            //�ǂ܂ł̃x�N�g��
            DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);//�ǂ̏Փˏ���
            DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);//���C�̏I�_�ʒu
            DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Start,End);//�ǂ̏Փˈʒu�܂ł̃x�N�g��

            //�ǂ̖@���x�N�g��
            DirectX::XMVECTOR Normal=DirectX::XMLoadFloat3(&hit.normal);

            //�ǂ̏Փˈʒu�܂ł̃x�N�g����@���Ɏˉe
            DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Vec,Normal);
            float dot = DirectX::XMVectorGetX(Dot);//�ˉe�̒P��̒l

            //�ǂ̖@���x�N�g����dot��2�{���X�P�[�����O��(XMVectorScale)
            DirectX::XMVECTOR CollectPosition = DirectX::XMVectorScale(Normal,2);
            //End�̈ʒu����CollectPosition���̃x�N�g���𑫂�
            DirectX::XMVECTOR ReflectPosition = DirectX::XMVectorAdd(End,CollectPosition);

            //ReflectPosition����hit.position�Ŕ��˃x�N�g�����v�Z
            DirectX::XMVECTOR ReflectVec = DirectX::XMVectorSubtract(ReflectPosition,DirectX::XMLoadFloat3(&hit.position));
            //���˃x�N�g���𐳋K��
            ReflectVec = DirectX::XMVector3Normalize(ReflectVec);
            //float�ɕϊ�
            DirectX::XMFLOAT3 reflectVec;
            DirectX::XMStoreFloat3(&reflectVec, ReflectVec);

            //�ʒu��x��z�̐�����hit.position�ɏ㏑��
            position.x = hit.position.x;
            position.z = hit.position.z;
            //direction��x��y�̐������㏑��
            direction.x = reflectVec.x;
            direction.y = reflectVec.y;
        }
        else
        {
            position.x += mx;
            position.z += mz;
        }
    }

    UpdateTransform();

    model->UpdateTransform(transform);
}

void ProjectileStraight::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    shader->Draw(dc, model);
}

void ProjectileStraight::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{
    this->direction = direction;
    this->position = position;
}