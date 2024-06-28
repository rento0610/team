#include "EnemyManager.h"
#include "Collision.h"

void EnemyManager::Update(float elapsedTime)
{
    for (Enemy* enemy : enemies)
    {
        enemy->Update(elapsedTime);
    }
    CollisionEnemyVsEnemies();
    //破棄処理
    for (Enemy* enemy : removes)
    {
        //std::vectorから要素を削除する場合はイテレーターで削除しなければならない
        std::vector<Enemy*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);

        if (it != enemies.end())
        {
            enemies.erase(it);
        }
        //弾丸の破棄処理
        delete enemy;
    }
    //破棄リストをクリア
    removes.clear();
    //敵同士の衝突処理
    CollisionEnemyVsEnemies();
}

void EnemyManager::Render(ID3D11DeviceContext* context, Shader* shader)
{
    for (Enemy* enemy : enemies)
    {
        enemy->Render(context, shader);
    }
}
void EnemyManager::Register(Enemy* enemy)
{
    enemies.emplace_back(enemy);
}

void EnemyManager::Clear()
{
    for (Enemy* enemy : enemies)
    {
        delete enemy;
    }
    enemies.clear();
}
//エネミー同士の衝突処理
void EnemyManager::CollisionEnemyVsEnemies()
{
    size_t enemyCount = enemies.size();
    //i番目の敵以降の敵を1体ずつ衝突しているか確認している
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemyA = enemies.at(i);
        for (int j = i + 1; j < enemyCount; ++j)
        {
            Enemy* enemyB = enemies.at(j);
            DirectX::XMFLOAT3 outPosition;
            if (Collision::IntersectCylinderVsCylinder(
                enemyA->GetPosition(),
                enemyA->GetRadius(),
                enemyA->GetHeight(),
                enemyB->GetPosition(),
                enemyB->GetRadius(),
                enemyB->GetHeight(),
                outPosition))
            {
                enemyB->SetPosition(outPosition);
            }
        }
    }
}
void EnemyManager::DrawDebugPrimitive()
{
    for (Enemy* enemy : enemies)
    {
        enemy->DrawDebugPrimitive();
    }
}

void EnemyManager::Remove(Enemy* enemy)
{
    removes.insert(enemy);
}