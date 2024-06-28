#include "Graphics/Graphics.h"
#include "Effect.h"
#include "EffectManager.h"

Effect::Effect(const char* filename)
{
    //�G�t�F�N�g��ǂݍ��݂���O�Ƀ��b�N����
    //���}���`�X���b�h��Effect���쐬�����DeviceContext�𓯎��A�N�Z�X����
    //�t���[�Y����\��������̂Ŕr�����䂷��
    std::lock_guard<std::mutex> lock(Graphics::Instance().GetMutex());

    char16_t utf16Filename[256];
    Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filename);

    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    effekseerEffect = Effekseer::Effect::Create(effekseerManager, (EFK_CHAR*)utf16Filename);
}

Effekseer::Handle Effect::Play(const DirectX::XMFLOAT3& position, float scale)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    Effekseer::Handle handle = effekseerManager->Play(effekseerEffect, position.x, position.y,
        position.z);
    effekseerManager->SetScale(handle, scale, scale, scale);
    return handle;
}

void Effect::Stop(Effekseer::Handle handle)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    effekseerManager->StopEffect(handle);
}
void Effect::SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    effekseerManager->SetLocation(handle, position.x, position.y, position.z);
}
void Effect::SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale)
{
    Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

    effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
}