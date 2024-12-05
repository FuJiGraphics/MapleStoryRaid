#pragma once
#include <VegaEngine2.h>
#include "Utils/Timer.h"
#include "CallbackComponent.h"
#include <random>
#include <vector>
#include <string>

namespace fz {

    class SpawnerSystem : public VegaScript {
    public:
        void Start() override
        {
            spawnInterval = 10.0f;  // �⺻ ���� �ֱ�
            maxMonsters = 10;       // �⺻ �ִ� ���� ��

            // ���� Ÿ�Ժ��� ������ ����ŭ ����
            SpawnMonster("RibbonPig", ribbonPigCount);
            SpawnMonster("Spoa", spoaCount);
            SpawnMonster("Snail", snailCount);

            timer["SpawnTimer"].Start(spawnInterval); // ���� �ֱ� ����
        }

        void OnUpdate(float dt) override
        {
            timer.Update(dt);

            // Ÿ�̸� �Ϸ� �� ���� ����
            if (timer["SpawnTimer"].Done())
            {
                if (spoaList.size() < spoaCount)
                    this->SpawnMonster("Spoa", 1);

                timer["SpawnTimer"].Start(spawnInterval); // Ÿ�̸� ����
            }
        }

        void NotifyMonsterDeath(GameObject obj)
        {
            // Spoa
            {
                auto it = std::find(spoaList.begin(), spoaList.end(), obj);
                if (it != spoaList.end())
                {
                    spoaList.erase(it);
                    return;
                }
            }
            // Ribbonpig
            {
                auto it = std::find(ribbonpigList.begin(), ribbonpigList.end(), obj);
                if (it != ribbonpigList.end())
                {
                    ribbonpigList.erase(it);
                    return;
                }
            }
            // Snail
            {
                auto it = std::find(snailList.begin(), snailList.end(), obj);
                if (it != snailList.end())
                {
                    snailList.erase(it);
                    return;
                }
            }
        }
  
    private:
        void SpawnMonster(const std::string& monsterTag, int count)
        {
            for (int i = 0; i < count; ++i)
            {
                float x, y;

                // ���� �� ����
                int floor = std::uniform_int_distribution<int>(1, 2)(rd);

                if (floor == 1) // 1��
                {
                    x = std::uniform_real_distribution<float>(-600.0f, 700.0f)(rd);
                    y = 310.0f;
                }
                else // 2��
                {
                    x = std::uniform_real_distribution<float>(0.0f, 600.0f)(rd);
                    y = 40.0f;
                }

                GameObject newObj = GetCurrentScene()->Instantiate(monsterTag, { x, y });
                newObj.AddComponent<CallbackComponent>();
                if (newObj.HasComponent<CallbackComponent>())
                {
                    auto& callbackComp = newObj.GetComponent<CallbackComponent>();
                    callbackComp.Callbacks["Die"].push_back(BIND_EVENT_FUNC(SpawnerSystem::NotifyMonsterDeath));
                }

                // ������ ���� �� ī��Ʈ ����
                if (monsterTag == "RibbonPig")
                {
                    if (ribbonpigList.size() < ribbonPigCount)
                        ribbonpigList.push_back(newObj);
                }
                else if (monsterTag == "Spoa")
                {
                    if (spoaList.size() < spoaCount)
                        spoaList.push_back(newObj);
                }
                else if (monsterTag == "Snail")
                {
                    if (snailList.size() < snailCount)
                        snailList.push_back(newObj);
                }

                FZLOG_DEBUG("Spawned monster: {0} at position ({1}, {2}) on floor {3}. Current monsters: RibbonPigs({4}), Spoas({5}), Snails({6})",
                    monsterTag, x, y, floor, ribbonPigCount, spoaCount, snailCount);
            }
        }

        // Stage1
        std::vector<GameObject> spoaList;
        std::vector<GameObject> ribbonpigList;
        std::vector<GameObject> snailList;

        float spawnInterval = 10.0f;
        int maxMonsters = 10;

        // �� ���� Ÿ�Ժ� ī��Ʈ
        int ribbonPigCount = 0;
        int spoaCount = 3;
        int snailCount = 0;

        Timer timer;
        std::random_device rd;
    };

} // namespace fz
