#include "cocos2d.h"
#include "fish_game_view.h"

#include "cocostudio/CocoStudio.h"

using namespace std;
using namespace cocos2d;

#define MAX_SEAT 4

namespace FishGameView
{
    Config* g_config = nullptr;
    Utils* g_utils = nullptr;
    Manager* g_manager = nullptr;
    View* g_view = nullptr;

    View* View::create()
    {
        View* view = new View();
        view->init();
        return view;
    }

    bool View::init()
    {
        Node::init();

        g_utils = new Utils();
        g_config = new Config();
        g_manager = new Manager();
        g_view = this;

        return true;
    }

    void View::onEnter()
    {
        Node::onEnter();
        g_manager->init();

        fishLayer = Node::create();
        addChild(fishLayer);
        effectLayer = Node::create();
        addChild(effectLayer);
        bulletLayer = Node::create();
        addChild(bulletLayer);
    }

    void View::onExit()
    {
        Node::onExit();
        CC_SAFE_DELETE(g_utils);
        CC_SAFE_DELETE(g_config);
        g_manager->release();
        CC_SAFE_DELETE(g_manager);
    }

    void View::setServerTime(double serverNowTime, double serverStartTime)
    {
        g_utils->setServerTime(serverNowTime, serverStartTime);
    }

    void View::setMySeat(int seatId)
    {
        g_utils->setMySeat(seatId);
    }

    Node* View::getEffectLayer()
    {
        return effectLayer;
    }

    Bullet* View::playerFire(int seatId, int bulletId, int weaponId, const Vec2& pt, int fishId)
    {
        assert(seatId >= 1 && seatId <= MAX_SEAT);
        Fish* fish = nullptr;
        if (fishId > 0)
        {
            fish = g_manager->getFish(fishId);
            if (fish == nullptr)
                return nullptr;
        }
        Bullet* bullet = Bullet::create(seatId, bulletId, weaponId, pt, fish);
        bulletLayer->addChild(bullet);
        g_manager->addBullet(bullet);
        if (fish != nullptr)
            g_manager->bulletLockFish(bullet, fish);
        return bullet;
    }

    Vec2 View::getBulletPos(int seatId, int bulletId)
    {
        return g_manager->getBulletPos(seatId, bulletId);
    }

    void View::bulletDie(int seatId, int bulletId)
    {
        g_manager->bulletDie(seatId, bulletId);
    }

    Fish* View::fishEnter(int fishResId, int fishId, int pathId, double enterTime, double frozenTime)
    {
        Path* path = g_config->getPath(pathId);
        Fish* fish = Fish::create(fishResId, fishId);
        if (fish->setPath(enterTime, frozenTime, path->points, path->ctrls, path->speeds))
        {
            fishLayer->addChild(fish, fishResId);
            g_manager->addFish(fish);
            return fish;
        }
        else
        {
            return nullptr;
        }
    }

    Vec2 View::getFishPos(int fishId)
    {
        return g_manager->getFishPos(fishId);
    }

    void View::fishDie(int fishId)
    {
        g_manager->fishDie(fishId);
    }

    void View::fishDie(const vector<int>& vec)
    {
        g_manager->fishDie(vec);
    }

    int View::getFishAtPosition(const Vec2& pt)
    {
        return g_manager->getFishAtPosition(pt);
    }

    void View::getFishesInRange(std::vector<int>& vec, const Vec2& center, float radius)
    {
        return g_manager->getFishesInRange(vec, center, radius);
    }

    void View::getNormalFishesInRange(std::vector<int>& vec, const Vec2& center, float radius)
    {
        return g_manager->getNormalFishesInRange(vec, center, radius);
    }

    void View::getNearbyFishes(std::vector<int>& vec, const Vec2& center, int count)
    {
        return g_manager->getNearbyFishes(vec, center, count);
    }

    void View::getRandomFishes(std::vector<int>& vec, int fishId, int count)
    {
        return g_manager->getRandomFishes(vec, fishId,count);
    }

    void View::freeze(double frozenTime)
    {
        if (frozenTime > 0)
            g_manager->freeze(frozenTime);
    }

    void View::unfreeze(double frozenTime, double unfrozenTime)
    {
        g_manager->unfreeze(frozenTime, unfrozenTime);
    }

    Laser* View::laserBegin(int seatId, int weaponId, int maxLine, const Vec2& pt)
    {
        Laser* laser = Laser::create(seatId, weaponId, maxLine, pt);
        bulletLayer->addChild(laser);
        g_manager->addLaser(seatId, laser);
        return laser;
    }

    void View::laserEnd(int seatId)
    {
        g_manager->laserEnd(seatId);
    }

    void View::getLaserPath(int seatId, vector<Vec2>& path)
    {
        return g_manager->getLaserPath(seatId, path);
    }

    void View::getLaserDists(int seatId, vector<float>& dists)
    {
        return g_manager->getLaserDists(seatId, dists);
    }

    void View::getLaserAngles(int seatId, vector<float>& angles)
    {
        return g_manager->getLaserAngles(seatId, angles);
    }

    Piton* View::castIcePiton(int pitonId, double angle)
    {
        Fish* boss = g_manager->getIceDragon();
        if (boss == nullptr)
            return nullptr;
        Vec2 bossPosition = boss->getPosition();
        Piton* piton = Piton::create(pitonId, bossPosition, angle);
        bulletLayer->addChild(piton);
        g_manager->addPiton(piton);
        return piton;
    }

    Vec2 View::getPitonPos(int pitonId)
    {
        return g_manager->getPitonPos(pitonId);
    }

    void View::pitonDie(int pitonId)
    {
        g_manager->pitonDie(pitonId);
    }

} // namespace FishGameView
