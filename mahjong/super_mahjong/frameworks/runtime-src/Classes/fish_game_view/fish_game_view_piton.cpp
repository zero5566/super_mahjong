#include "cocos2d.h"
#include "fish_game_view.h"

using namespace std;
using namespace cocos2d;

#define PITON_SPEED 100
#define PITON_MAX_INNERDIST_SQ 17000
#define PITON_RC_LEFT -20
#define PITON_RC_BOTTOM -150
#define PITON_RC_RIGHT 20
#define PITON_RC_TOP 0

namespace FishGameView
{
    extern Utils* g_utils;
    extern Manager* g_manager;
    extern View* g_view;

    Piton* Piton::create(int pitonId, const Vec2& ptStart, float angle)
    {
        Piton* piton = new Piton();
        piton->init(pitonId, ptStart, angle);
        piton->autorelease();
        return piton;
    }

    void Piton::init(int pitonId, const Vec2& ptStart, float angle)
    {
        this->pitonId = pitonId;
        this->enableCollide = true;

        // 方向
        Vec2 dirMove(sinf(CC_DEGREES_TO_RADIANS(angle)), cosf(CC_DEGREES_TO_RADIANS(angle)));
        Vec2 currPoint = ptStart + dirMove * 100;
        setPosition(currPoint);
        setRotation(angle);

        // 碰撞点、距离、时间
        Vec2 collisionPoint;
        hitBattery = g_utils->getCollisionEdgePosOrBattery(currPoint, dirMove, &collisionPoint);
        float dist = currPoint.distance(collisionPoint);
        float duration = dist / PITON_SPEED;

        // 移动
        MoveTo* move = MoveTo::create(duration, collisionPoint);
        CallFunc* func = CallFunc::create(CC_CALLBACK_0(Piton::onReachCollisionPoint, this));
        Sequence* seq = Sequence::create(move, func, nullptr);
        runAction(seq);
    }

    bool Piton::checkCollision(Bullet* bullet)
    {
        if (!enableCollide)
            return false;

        const Vec2& pitonPosition = getPosition();
        const Vec2& bulletPosition = bullet->getPosition();

        // 判断子弹距离
        float distSquared = pitonPosition.distanceSquared(bulletPosition);
        if (distSquared > PITON_MAX_INNERDIST_SQ)
            return false;

        // 旋转bulletPosition到冰锥的旋转坐标系，坐标为x，y
        float tempX = bulletPosition.x - pitonPosition.x;
        float tempY = bulletPosition.y - pitonPosition.y;
        float angle = CC_DEGREES_TO_RADIANS(getRotation());
        float cos_angle = cosf(angle);
        float sin_angle = sinf(angle);
        float x = tempX * cos_angle - tempY * sin_angle;
        float y = tempX * sin_angle + tempY * cos_angle;

        // 测试Bounding Box
        if (x < PITON_RC_LEFT || x > PITON_RC_RIGHT || y < PITON_RC_BOTTOM || y > PITON_RC_TOP)
            return false;

        return true;
    }

    void Piton::onReachCollisionPoint()
    {
        g_view->onPitonHit(hitBattery, pitonId);
    }

    void Piton::die()
    {
        enableCollide = false;
        this->stopAllActions();
        this->runAction(Sequence::create(DelayTime::create(0.43f), RemoveSelf::create(), nullptr));
    }

} // namespace FishGameView
