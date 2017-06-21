#include "cocos2d.h"
#include "fish_game_view.h"

using namespace std;
using namespace cocos2d;

#define BATTERY_RADIUS 100      // 炮台半径

namespace FishGameView
{
    extern Config* g_config;
    extern Utils* g_utils;
    extern Manager* g_manager;
    extern View* g_view;

    Bullet* Bullet::create(int seatId, int bulletId, int weaponId, Vec2 touchPos, Fish* fish)
    {
        Bullet* bullet = new Bullet();
        bullet->init(seatId, bulletId, weaponId, touchPos, fish);
        bullet->autorelease();
        return bullet;
    }

    void Bullet::init(int seatId, int bulletId, int weaponId, Vec2 touchPoint, Fish* fish)
    {
        // 初始化参数
        this->seatId = seatId;
        this->bulletId = bulletId;
        const WeaponConfig* cfg = g_config->getWeaponConfig(weaponId);
        this->bulletSpeed = cfg->bulletSpeed;

        // 运动方向
        Vec2 seatPoint = g_utils->getSeatPosition(this->seatId);
        Vec2 firePoint = g_utils->getFirePosition(this->seatId);
        Vec2 targetPoint = (fish == nullptr) ? touchPoint : fish->getPosition();
        Vec2 dirFire = targetPoint - firePoint;
        Vec2 dirTemp = firePoint - seatPoint;
        float angle = g_utils->getAngle(dirFire, dirTemp);
        angle = clampf(angle, -98, 98);
        dirTemp.rotate(Vec2::ZERO, CC_DEGREES_TO_RADIANS(-angle));
        if (this->seatId >= 3)
            angle += 180;
        moveDir = dirTemp.getNormalized();

        // 位置、旋转
        Vec2 currPoint = firePoint + moveDir * BATTERY_RADIUS;
        setPosition(currPoint);
        setRotation(angle);

        // 记录时钟
        beginMovingTick = prevCollideTick = nextCollideTick = g_utils->getTick();

        // 是否锁定
        if (fish != nullptr)
        {
            moveToFish(fish);
        }
        else
        {
            moveFreely();
        }
    }

    void Bullet::moveToFish(Fish* fish)
    {
        this->lockFish = fish;
        this->enableCollide = false;

        Vec2 fishPos = fish->getPosition();
        Vec2 currPoint = getPosition();
        Vec2 pt1(g_utils->vx2px(currPoint.x), g_utils->vy2py(currPoint.y));
        Vec2 pt2(g_utils->vx2px(fishPos.x), g_utils->vy2py(fishPos.y));
        float dist = pt1.distance(pt2);
        float time = dist / bulletSpeed;

        Vec2 fishNextPosition = fish->calcNextPosition(time);

        MoveTo* move = MoveTo::create(time, fishNextPosition);
        CallFunc* func = CallFunc::create(CC_CALLBACK_0(Bullet::onReachLockPoint, this));
        Sequence* seq = Sequence::create(move, func, nullptr);
        runAction(seq);
    }

    void Bullet::moveFreely()
    {
        lockFish = nullptr;
        enableCollide = true;

        stopAllActions();
        scheduleUpdate();
        moveToEdgePoint();
    }

    void Bullet::moveToEdgePoint()
    {
        Vec2 currPoint = getPosition();

        prevPoint = currPoint;
        nextPoint = g_utils->getCollisionEdgePos(currPoint, moveDir);

        Vec2 pt1(g_utils->vx2px(currPoint.x), g_utils->vy2py(currPoint.y));
        Vec2 pt2(g_utils->vx2px(nextPoint.x), g_utils->vy2py(nextPoint.y));
        float dist = pt1.distance(pt2);
        float time = 1000 * dist / bulletSpeed;

        prevCollideTick = nextCollideTick;
        nextCollideTick += (int64_t)time;
    }

    void Bullet::onReachEdgePoint()
    {
        setPosition(nextPoint);

        // rotate
        Size vsize = Director::getInstance()->getVisibleSize();
        if (fabs(nextPoint.x) < 0.01f || fabs(nextPoint.x - vsize.width) < 0.01f)
        {
            moveDir.x = -moveDir.x;
            setRotation(-getRotation());
        }
        if (fabs(nextPoint.y) < 0.01f || fabs(nextPoint.y - vsize.height) < 0.01f)
        {
            moveDir.y = -moveDir.y;
            setRotation(180 - getRotation());
        }
    }

    void Bullet::onReachLockPoint()
    {
        if (lockFish != nullptr)
            g_view->onBulletHitFish(seatId, bulletId, lockFish->fishId);
    }

    void Bullet::update(float delta)
    {
        assert(lockFish == nullptr);

        int64_t tick = g_utils->getTick();
        if (nextCollideTick > tick)
        {
            float alpha = (float)(tick - prevCollideTick) / (float)(nextCollideTick - prevCollideTick);
            Vec2 currPoint = prevPoint.lerp(nextPoint, alpha);
            setPosition(currPoint);
        }
        else
        {
            onReachEdgePoint();
            moveToEdgePoint();
        }
    }

    void Bullet::die()
    {
        enableCollide = false;
        unscheduleUpdate();
        stopAllActions();
        runAction(Sequence::create(DelayTime::create(1), RemoveSelf::create(), nullptr));
    }

} // namespace FishGameView
