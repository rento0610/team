#include "StageManager.h"

void StageManager::Update(float elapsedTime)
{
    for (Stage* stage : stages) stage->Update(elapsedTime);
}

void StageManager::Render(ID3D11DeviceContext* dc, Shader* shader)
{
    for (Stage* stage : stages) stage->Render(dc,shader);
}
//ƒXƒe[ƒW“o˜^
void StageManager::Register(Stage* stage)
{
    stages.emplace_back(stage);
}

void StageManager::Clear()
{
    for (Stage* stage : stages) delete stage;
}
bool StageManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    
        bool result = false;

        // Initial distance is set to the maximum possible value
        hit.distance = FLT_MAX;

        // Iterate through all stages
        for (Stage* stage : stages)
        {
            // Temporary hit result for the current stage
            HitResult tmp;

            // Perform raycast on the current stage
            if (stage->RayCast(start, end, tmp))
            {
                // Check if the current hit is closer than the previous hits
                if (tmp.distance < hit.distance)
                {
                    // Update hit with the closer hit information
                    hit = tmp;
                    result = true;
                }
            }
        }

        return result;
    

}