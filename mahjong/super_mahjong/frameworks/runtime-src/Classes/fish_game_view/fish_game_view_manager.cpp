#include "cocos2d.h"
#include "fish_game_view.h"

using namespace std;
using namespace cocos2d;

#define DRAGON_RES_ID 1305
#define MINION_RES_ID 1121

namespace FishGameView
{
    extern Utils* g_utils;
    extern View* g_view;

    void Manager::init()
    {
        Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
    }

    void Manager::release()
    {
        for (auto _pair : bullets)
            _pair.second->release();
        for (auto _pair : fishes)
            _pair.second->release();
        for (auto _pair : lasers)
            _pair.second->release();
        for (auto _pair : pitons)
            _pair.second->release();
        bullets.clear();
        fishes.clear();
        fishHierarchy.clear();
        lasers.clear();
        pitons.clear();
        locks.clear();
        Director::getInstance()->getScheduler()->unscheduleUpdate(this);
    }

    void Manager::update(float delta)
    {
        auto _bullets(bullets);
        auto _pitons(pitons);

        // 计算子弹碰撞
        for (auto iter = _bullets.begin(); iter != _bullets.end(); ++iter)
        {
            Bullet* bullet = iter->second;
            if (!bullet->enableCollide)
                continue;

            bool collided = false;

            // 子弹与鱼碰撞
            for (auto fish_pair : fishes)
            {
                Fish* fish = fish_pair.second;
                if (fish->checkCollision(bullet))
                {
                    collided = true;
                    g_view->onBulletHitFish(bullet->seatId, bullet->bulletId, fish->fishId);
                    break;
                }
            }

            if (collided)
                continue;

            // 子弹与冰锥碰撞
            for (auto piton_pair : _pitons)
            {
                Piton* piton = piton_pair.second;
                if (piton->checkCollision(bullet))
                {
                    g_view->onBulletHitPiton(bullet->seatId, bullet->bulletId, piton->pitonId);
                    break;
                }
            }
        }

        // 激光与鱼的碰撞
        for (auto laser_pair : lasers)
        {
            Laser* laser = laser_pair.second;
            if (!laser->enableCollide)
                continue;
            for (auto fish_pair : fishes)
            {
                Fish* fish = fish_pair.second;
                fish->checkLaserIntersect(laser);
            }
        }
    }

    void Manager::addBullet(Bullet* bullet)
    {
        bullet->retain();
        int bId = bullet->seatId * 100 + bullet->bulletId;
        bullets[bId] = bullet;
    }

    void Manager::delBullet(Bullet* bullet)
    {
        int bId = bullet->seatId * 100 + bullet->bulletId;
        auto iter = bullets.find(bId);
        if (iter != bullets.end())
        {
            checkReleaseLock(iter->second);
            iter->second->release();
            bullets.erase(iter);
        }
    }

    void Manager::bulletDie(int seatId, int bulletId)
    {
        if (bulletId == 0)
        {
            vector<Bullet*> vec;
            for (auto bullet_pair : bullets)
            {
                if (bullet_pair.second->seatId == seatId)
                    vec.push_back(bullet_pair.second);
            }
            for (Bullet* bullet : vec)
            {
                delBullet(bullet);
                bullet->die();
            }
        }
        else
        {
            int bId = seatId * 100 + bulletId;
            auto iter = bullets.find(bId);
            if (iter != bullets.end())
            {
                Bullet* bullet = iter->second;
                delBullet(bullet);
                bullet->die();
            }
        }
    }

    Bullet* Manager::getBullet(int seatId, int bulletId)
    {
        int bId = seatId * 100 + bulletId;
        auto iter = bullets.find(bId);
        if (iter != bullets.end())
            return iter->second;
        return nullptr;
    }

    Vec2 Manager::getBulletPos(int seatId, int bulletId)
    {
        int bId = seatId * 100 + bulletId;
        auto iter = bullets.find(bId);
        if (iter != bullets.end())
            return iter->second->getPosition();
        return Vec2::ZERO;
    }

    void Manager::addFish(Fish* fish)
    {
        fish->retain();
        fishes[fish->fishId] = fish;
        fishHierarchy[fish->fishResId].insert(fish);
    }

    void Manager::delFish(Fish* fish)
    {
        auto iter = fishes.find(fish->fishId);
        if (iter != fishes.end())
        {
            auto iterHierarchy = fishHierarchy.find(iter->second->fishResId);
            if (iterHierarchy != fishHierarchy.end())
                iterHierarchy->second.erase(fish);

            checkReleaseLock(iter->second);
            iter->second->release();
            fishes.erase(iter);
        }
    }

    void Manager::fishDie(int fishId)
    {
        auto iter = fishes.find(fishId);
        if (iter != fishes.end())
        {
            Fish* fish = iter->second;
            delFish(fish);
            fish->die();
        }
    }

    void Manager::fishDie(const std::vector<int>& vec)
    {
        for (int fishId : vec)
        {
            auto iter = fishes.find(fishId);
            if (iter != fishes.end())
            {
                Fish* fish = iter->second;
                delFish(fish);
                fish->die();
            }
        }
    }

    Fish* Manager::getFish(int fishId)
    {
        auto iter = fishes.find(fishId);
        if (iter != fishes.end())
            return iter->second;
        return nullptr;
    }

    Fish* Manager::getIceDragon()
    {
        auto iter = fishHierarchy.find(DRAGON_RES_ID);
        if (iter != fishHierarchy.end())
        {
            auto& _set = iter->second;
            if (!_set.empty())
                return *_set.begin();
        }
        return nullptr;
    }

    Vec2 Manager::getFishPos(int fishId)
    {
        auto iter = fishes.find(fishId);
        if (iter != fishes.end())
            return iter->second->getPosition();
        return Vec2(0, 0);
    }

    int Manager::getFishAtPosition(const Vec2& pt)
    {
        for (auto iter = fishHierarchy.rbegin(); iter != fishHierarchy.rend(); ++iter)
        {
            auto fishset = iter->second;
            for (auto fish : fishset)
            {
                if (fish->checkCollision(pt))
                    return fish->fishId;
            }
        }
        return 0;
    }

    void Manager::getFishesInRange(std::vector<int>& vec, const Vec2& center, float radius)
    {
        float radiusSquared = radius * radius;
        for (auto fish_pair : fishes)
        {
            Fish* fish = fish_pair.second;
            if (fish->checkInRange(center, radiusSquared))
                vec.push_back(fish->fishId);
        }
    }

    void Manager::getNormalFishesInRange(std::vector<int>& vec, const Vec2& center, float radius)
    {
        float radiusSquared = radius * radius;
        for (auto fish_pair : fishes)
        {
            Fish* fish = fish_pair.second;
            if (fish->chechForChainCatch() && fish->checkInRange(center, radiusSquared))
                vec.push_back(fish->fishId);
        }
    }

    void Manager::getNearbyFishes(std::vector<int>& vec, const Vec2& center, int count)
    {
        std::map<float, int> dist_map;
        for (auto fish_pair : fishes)
        {
            Fish* fish = fish_pair.second;
            if (fish->chechForChainCatch())
            {
                const Vec2& pt = fish->getPosition();
                float dist = center.distanceSquared(pt);
                dist_map[dist] = fish->fishId;
            }
        }
        for (auto dist_pair : dist_map)
        {
            if (count-- <= 0)
                break;
            vec.push_back(dist_pair.second);
        }
    }

    void Manager::getRandomFishes(std::vector<int>& vec, int fishId, int count)
    {
        std::vector<int> dist_vector;
        for (auto fish_pair : fishes)
        {
            Fish* fish = fish_pair.second;
            if (fish->fishId != fishId && fish->chechForChainCatch())
            {
                dist_vector.push_back(fish->fishId);
            }
        }
        while (count-- > 0 && dist_vector.size() > 0)
        {
            size_t index = rand() % dist_vector.size();
            vec.push_back(dist_vector[index]);
            dist_vector.erase(dist_vector.begin() + index);
        }
    }

    void Manager::freeze(double frozenTime)
    {
        for (auto fish_pair : fishes)
        {
            Fish* fish = fish_pair.second;
            fish->freeze(frozenTime);
        }
    }

    void Manager::unfreeze(double frozenTime, double unfrozenTime)
    {
        for (auto fish_pair : fishes)
        {
            Fish* fish = fish_pair.second;
            fish->unfreeze(frozenTime, unfrozenTime);
        }
    }

    void Manager::addLaser(int seatId, Laser* laser)
    {
        laser->retain();
        lasers[laser->seatId] = laser;
    }

    void Manager::delLaser(Laser* laser)
    {
        auto iter = lasers.find(laser->seatId);
        if (iter != lasers.end())
        {
            iter->second->release();
            lasers.erase(iter);
        }
    }

    void Manager::laserEnd(int seatId)
    {
        auto iter = lasers.find(seatId);
        if (iter != lasers.end())
        {
            Laser* laser = iter->second;
            delLaser(laser);
            laser->die();
        }
    }

    void Manager::getLaserPath(int seatId, vector<Vec2>& path)
    {
        auto iter = lasers.find(seatId);
        if (iter != lasers.end())
        {
            Laser* laser = iter->second;
            laser->getPath(path);
        }
    }

    void Manager::getLaserDists(int seatId, vector<float>& dists)
    {
        auto iter = lasers.find(seatId);
        if (iter != lasers.end())
        {
            Laser* laser = iter->second;
            laser->getDists(dists);
        }
    }

    void Manager::getLaserAngles(int seatId, vector<float>& angles)
    {
        auto iter = lasers.find(seatId);
        if (iter != lasers.end())
        {
            Laser* laser = iter->second;
            laser->getAngles(angles);
        }
    }

    void Manager::addPiton(Piton* piton)
    {
        piton->retain();
        pitons[piton->pitonId] = piton;
    }

    void Manager::delPiton(Piton* piton)
    {
        auto iter = pitons.find(piton->pitonId);
        if (iter != pitons.end())
        {
            iter->second->release();
            pitons.erase(iter);
        }
    }

    Vec2 Manager::getPitonPos(int pitonId)
    {
        auto iter = pitons.find(pitonId);
        if (iter != pitons.end())
            return iter->second->getPosition();
        return Vec2(0, 0);
    }

    void Manager::pitonDie(int pitonId)
    {
        auto iter = pitons.find(pitonId);
        if (iter != pitons.end())
        {
            Piton* piton = iter->second;
            delPiton(piton);
            piton->die();
        }
    }

    void Manager::bulletLockFish(Bullet* bullet, Fish* fish)
    {
        locks[fish].insert(bullet);
    }

    void Manager::checkReleaseLock(Bullet* bullet)
    {
        auto iter = locks.find(bullet->lockFish);
        if (iter != locks.end())
        {
            auto& _set = iter->second;
            _set.erase(bullet);
        }
    }

    void Manager::checkReleaseLock(Fish* fish)
    {
        auto iter = locks.find(fish);
        if (iter != locks.end())
        {
            auto& _set = iter->second;
            for (Bullet* bullet : _set)
                bullet->moveFreely();
            locks.erase(iter);
        }
    }

} // namespace FishGameView
