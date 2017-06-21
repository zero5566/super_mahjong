#include "cocos2d.h"
#include "fish_game_view.h"

using namespace std;
using namespace cocos2d;

#define JELLYFISH_1_RES_ID 1105
#define JELLYFISH_2_RES_ID 1204
#define DRAGON_TURTLE_RES_ID 1210
#define MAX_NORMAL_FISH_RES_ID 1120
#define BOSS_FISH_RES_ID 1301
#define LANTERN_FISH_RES_ID 1303
#define DRAGON_BOSS_RES_ID 1305
#define CARD_FISH1_RES_ID 1601
#define CARD_FISH2_RES_ID 1602
#define ELECTRIC_FISH_RES_ID 1402
#define BOMB_FISH_RES_ID 1403

namespace FishGameView
{
    extern Config* g_config;
    extern Utils* g_utils;
    extern Manager* g_manager;
    extern View* g_view;

    Fish* Fish::create(int fishResId, int fishId)
    {
        Fish* fish = new Fish();
        fish->xHead = fish->xTail = 0;
        fish->init(fishResId, fishId);
        fish->autorelease();
        return fish;
    }

    void Fish::init(int fishResId, int fishId)
    {
        this->fishResId = fishResId;
        this->fishId = fishId;
        this->isAlive = true;

        // 碰撞信息
        g_config->getCollisions(this->fishResId, colliders);

        // fish sprite
        Vec2 anchor = Vec2(colliders.center.x / colliders.sz.x, colliders.center.y / colliders.sz.y);
        setContentSize(Size(colliders.sz.x, colliders.sz.y));
        setAnchorPoint(anchor);
        setVisible(false);

        // 位置是否处于屏幕中
        isInScreen = false;
        Size vsize = Director::getInstance()->getVisibleSize();

        // 暂停信息
        isFrozen = false;

        // 移动信息
        beginMovingTick = 0;

        // 最后一次碰撞激光的时间
        lastCollideTick = 0;

#if (COCOS2D_DEBUG > 0)
        //drawNode = DrawNode::create();
        //addChild(drawNode);
        //labelFishId = Label::createWithSystemFont(StringUtils::format("%i", fishId), "Arial", 12);
        //Size csize = getContentSize();
        //labelFishId->setPosition(0, csize.height / 2);
        //addChild(labelFishId);
        //isHit = false;
#endif
    }

    bool Fish::setPath(double enterTime, double frozenTime, const vector<Vec2>& points, const vector<Vec4>& ctrls, const vector<float>& speeds)
    {
        path.clear();
        times.clear();
        assert(points.size() > 1);

        // 后面不会再插入第一个点
        path.push_back(points[0]);
        times.push_back(0);

        // 插值计算路径点path、移动时长times、旋转角度angles
        float totalTime = 0;
        vector<Vec2> vecPoints;
        for (size_t i = 0; i < points.size() - 1; ++i)
        {
            Vec2 p1c2 = Vec2(ctrls[i].z, ctrls[i].w);
            Vec2 p2c1 = Vec2(ctrls[i + 1].x, ctrls[i + 1].y);
            vecPoints.clear();
            g_utils->getCubicBezierVertices(points[i], p1c2, p2c1, points[i + 1], 10, vecPoints);
            path.insert(path.end(), vecPoints.begin() + 1, vecPoints.end());

            float speed = speeds[i];
            for (size_t j = 0; j < vecPoints.size() - 1; ++j)
            {
                Vec2 pt1(g_utils->vx2px(vecPoints[j].x), g_utils->vy2py(vecPoints[j].y));
                Vec2 pt2(g_utils->vx2px(vecPoints[j + 1].x), g_utils->vy2py(vecPoints[j + 1].y));
                float dist = pt1.distance(pt2);
                float time = dist / speed;
                totalTime += time;
                times.push_back(totalTime);

                Vec2 dir = pt2 - pt1;
                float angle = g_utils->getAngle(dir, Vec2(-1, 0));
                angles.push_back(angle);
            }
        }
        angles.push_back(*angles.rbegin());

        // 服务端经过时长
        float serverTimeElapse = g_utils->getServerTimeElapse(frozenTime);
        fishEnterTime = enterTime;
        beginMovingTick = g_utils->getTick() + (int64_t)((enterTime - serverTimeElapse) * 1000.0f);

        // 计算坐标、旋转
        if (!calcTransform())
        {
            CCLOG("fish set path end: %i", fishId);
            return false;
        }

        // 龙龟、电鳗、巨型灯笼鱼、冰龙：翻转
        if (fishResId == DRAGON_TURTLE_RES_ID || fishResId == ELECTRIC_FISH_RES_ID || fishResId == LANTERN_FISH_RES_ID || fishResId == DRAGON_BOSS_RES_ID)
        {
            xTail = path[0].x;
            xHead = path[0].x + (path[1].x - path[0].x) * 0.0001f;

            if (g_utils->getMySeat() <= 2 && path[1].x > path[0].x)
            {
                flipX();
            }
            else if (g_utils->getMySeat() >= 3)
            {
                flipY();

                if (path[1].x > path[0].x)
                {
                    flipX();
                }
            }
        }
        // 普通鱼（水母、话费鱼：无操作)
        else if (fishResId != JELLYFISH_1_RES_ID && fishResId != JELLYFISH_2_RES_ID && fishResId != CARD_FISH1_RES_ID && fishResId != CARD_FISH2_RES_ID && fishResId != BOMB_FISH_RES_ID)
        {
            if (g_utils->getMySeat() <= 2 && path[1].x > path[0].x || g_utils->getMySeat() >= 3 && path[1].x < path[0].x)
            {
                flipY();
            }
        }

        // 已经开始游动
        if (serverTimeElapse >= enterTime)
        {
            setVisible(true);
        }

        // 冰冻
        if (frozenTime > 0)
        {
            isFrozen = true;
        }
        else
        {
            scheduleUpdate();
        }
        return true;
    }

    void Fish::update(float delta)
    {
        if (isFrozen)
            return;

        // 还没开始游动
        if (g_utils->getTick() < beginMovingTick)
            return;

        setVisible(true);

        // 计算坐标、旋转
        if (!calcTransform())
            g_view->onFishExit(fishId);
    }

    bool Fish::calcTransform()
    {
        float timeElapse = (float)(g_utils->getTick() - beginMovingTick) / 1000.0f;
        size_t nextIndex = 1;
        while (nextIndex < times.size() && times[nextIndex] <= timeElapse)
            ++nextIndex;

        // 已经游到终点
        if (nextIndex >= times.size())
            return false;

        float alpha = (timeElapse - times[nextIndex - 1]) / (times[nextIndex] - times[nextIndex - 1]);

        // position
        Vec2 position = path[nextIndex - 1].lerp(path[nextIndex], alpha);
        setPosition(position);

        // 检查是否游出屏幕
        Vec2 head = convertToWorldSpace(colliders.head);
        Vec2 tail = convertToWorldSpace(colliders.tail);
        if (g_utils->checkInScreen(head, tail))
        {
            if (!isInScreen)
            {
                isInScreen = true;
            }
        }
        else
        {
            if (isInScreen)
            {
                isInScreen = false;
                g_view->onFishOutScreen(fishId);
            }
        }

        // 水母、话费鱼：不旋转
        if (fishResId == JELLYFISH_1_RES_ID || fishResId == JELLYFISH_2_RES_ID || fishResId == CARD_FISH1_RES_ID || fishResId == CARD_FISH2_RES_ID || fishResId == BOMB_FISH_RES_ID)
            return true;

        // 龙龟、电鳗、巨型灯笼鱼、冰龙 - 不旋转，调头
        if (fishResId == DRAGON_TURTLE_RES_ID || fishResId == ELECTRIC_FISH_RES_ID || fishResId == LANTERN_FISH_RES_ID || fishResId == DRAGON_BOSS_RES_ID)
        {
            if (xHead < xTail && xHead < position.x || xHead > xTail && xHead > position.x)
            {
                flipX();
                g_view->onFishTurn(fishId);
            }
            xTail = xHead;
            xHead = position.x;
            return true;
        }

        // rotation
        float diffAngle = angles[nextIndex] - angles[nextIndex - 1];
        if (diffAngle > 180) diffAngle -= 360;
        if (diffAngle < -180) diffAngle += 360;
        float rotation = angles[nextIndex - 1] + diffAngle * alpha;
        setRotation(rotation);

        return true;
    }

    void Fish::flipX()
    {
        Size fsize = getContentSize();
        colliders.center.x = fsize.width - colliders.center.x;
        for (auto& collider : colliders.rcs)
            collider.rc.origin.x = -(collider.rc.size.width + collider.rc.origin.x);
        Vec2 anchor = Vec2(colliders.center.x / fsize.width, colliders.center.y / fsize.height);
        setAnchorPoint(anchor);
    }

    void Fish::flipY()
    {
        Size fsize = getContentSize();
        colliders.center.y = fsize.height - colliders.center.y;
        for (auto& collider : colliders.rcs)
            collider.rc.origin.y = -(collider.rc.size.height + collider.rc.origin.y);
        Vec2 anchor = Vec2(colliders.center.x / fsize.width, colliders.center.y / fsize.height);
        setAnchorPoint(anchor);
    }

    void Fish::freeze(double frozenTime)
    {
        if (!isFrozen)
        {
            isFrozen = true;
            unscheduleUpdate();
        }
    }

    void Fish::unfreeze(double frozenTime, double unfrozenTime)
    {
        if (isFrozen)
        {
            isFrozen = false;
            float serverTimeElapse = g_utils->getServerTimeElapse(frozenTime);
            beginMovingTick = g_utils->getTick() + (int64_t)((fishEnterTime - serverTimeElapse) * 1000.0f);
            fishEnterTime += (unfrozenTime - frozenTime);
            scheduleUpdate();
        }
    }

    bool Fish::checkCollision(const Vec2& pt)
    {
        if (!isInScreen || !isAlive)
            return false;

        Vec2 fishPos = getPosition();
        float angle = CC_DEGREES_TO_RADIANS(getRotation());
        for (auto collider : colliders.rcs)
        {
            // 判断子弹距离
            float distSquared = fishPos.distanceSquared(pt);
            if (distSquared > collider.maxInnerDistSquared)
                continue;

            // 旋转pt到fishCollider的旋转坐标系，坐标为x，y
            float tempX = pt.x - fishPos.x;
            float tempY = pt.y - fishPos.y;
            float cos_angle = cosf(angle);
            float sin_angle = sinf(angle);
            float x = tempX * cos_angle - tempY * sin_angle;
            float y = tempX * sin_angle + tempY * cos_angle;

            // 测试Bounding Box
            if (x < collider.rc.origin.x || x > (collider.rc.origin.x + collider.rc.size.width) ||
                y < collider.rc.origin.y || y > (collider.rc.origin.y + collider.rc.size.height))
                continue;

            return true;
        }
        return false;
    }

    bool Fish::checkCollision(Bullet* bullet)
    {
        if (checkCollision(bullet->getPosition()))
        {
#if (COCOS2D_DEBUG > 0)
            isHit = true;
#endif
            return true;
        }
        return false;
    }

    bool Fish::checkInRange(const cocos2d::Vec2& center, float radiusSquared)
    {
        if ((fishResId >= BOSS_FISH_RES_ID) || !isInScreen || !isAlive)
            return false;

        // 碰撞盒
        for (auto collider : colliders.rcs)
        {
            Vec2 bl(colliders.center + collider.rc.origin);
            Vec2 br(bl.x + collider.rc.size.width, bl.y);
            Vec2 tr(br.x, br.y + collider.rc.size.height);
            Vec2 tl(bl.x, tr.y);

            if (convertToWorldSpace(bl).distanceSquared(center) < radiusSquared)
                return true;

            if (convertToWorldSpace(br).distanceSquared(center) < radiusSquared)
                return true;

            if (convertToWorldSpace(tr).distanceSquared(center) < radiusSquared)
                return true;

            if (convertToWorldSpace(tl).distanceSquared(center) < radiusSquared)
                return true;
        }

        return false;
    }

    bool Fish::checkLaserIntersect(Laser* laser)
    {
        if (fishResId >= ELECTRIC_FISH_RES_ID || !isInScreen || !isAlive)
            return false;

        // 判断时间间隔
        int64_t nowTick = g_utils->getTick();
        if (lastCollideTick + 1000 > nowTick)
            return false;

        const Vec2* route = laser->route;
        int routeCount = laser->routeCount;

        // 碰撞检测
        Vec2 head = convertToWorldSpace(colliders.head);
        Vec2 tail = convertToWorldSpace(colliders.tail);
        Vec2 bottom = convertToWorldSpace(colliders.bottom);
        Vec2 top = convertToWorldSpace(colliders.top);

        Vec2 ptCollide;
        for (int i = 0; i < routeCount - 1; ++i)
        {
            if (g_utils->getIntersectPoint(head, tail, route[i], route[i + 1], &ptCollide) ||
                g_utils->getIntersectPoint(bottom, top, route[i], route[i + 1], &ptCollide))
            {
                ptCollide = convertToNodeSpace(ptCollide);
                g_view->onLaserHitFish(laser->seatId, fishId, ptCollide);

                lastCollideTick = nowTick;
                return true;
            }
        }

        return false;
    }

    bool Fish::chechForChainCatch()
    {
        // 中小鱼，且在屏幕中、活着
        return (fishResId <= MAX_NORMAL_FISH_RES_ID) && isInScreen && isAlive;
    }

    Vec2 Fish::calcNextPosition(float time)
    {
        if (isFrozen)
            return getPosition();

        float timeElapse = (float)(g_utils->getTick() - beginMovingTick) / 1000.0f + time;
        size_t nextIndex = 1;
        while (nextIndex < times.size() && times[nextIndex] <= timeElapse)
            ++nextIndex;

        // 已经游到终点
        if (nextIndex >= times.size())
            return *path.rbegin();

        float alpha = (timeElapse - times[nextIndex - 1]) / (times[nextIndex] - times[nextIndex - 1]);

        // position
        return path[nextIndex - 1].lerp(path[nextIndex], alpha);
    }

    // reason: 1-炮弹击中，2-电击，3-核弹
    void Fish::die()
    {
        isAlive = false;
        unscheduleUpdate();
        runAction(Sequence::create(DelayTime::create(4), RemoveSelf::create(), nullptr));
    }

#if (COCOS2D_DEBUG > 0)
    void Fish::draw(Renderer *renderer, const Mat4& transform, uint32_t flags)
    {
        Node::draw(renderer, transform, flags);
        //drawNode->clear();
        //Color4F color = isHit ? Color4F::RED : Color4F::WHITE;
        //for (auto collider : colliders.rcs)
        //{
        //    Vec2 origin(_anchorPointInPoints + collider.rc.origin);
        //    drawNode->drawRect(origin, origin + collider.rc.size, color);
        //}
    }
#endif

} // namespace FishGameView
