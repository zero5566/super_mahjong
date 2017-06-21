#include "cocos2d.h"
#include "fish_game_view.h"

using namespace std;
using namespace cocos2d;

namespace FishGameView
{
    extern Config* g_config;
    extern Utils* g_utils;
    extern View* g_view;

    Laser* Laser::create(int seatId, int weaponId, int maxLine, const Vec2& touchPoint)
    {
        Laser* laser = new Laser();
        laser->init(seatId, weaponId, maxLine, touchPoint);
        laser->autorelease();
        return laser;
    }

    void Laser::init(int seatId, int weaponId, int maxLine, const Vec2& touchPoint)
    {
        this->seatId = seatId;
        this->maxLine = maxLine;
        const WeaponConfig* cfg = g_config->getWeaponConfig(weaponId);
        laserSpeed = cfg->bulletSpeed;
        this->enableCollide = true;

        // 计算数值
        caclPath(touchPoint);
        caclTimes();
        caclAngles();

        // 运动信息
        routeCount = 0;
        line = -1;
        beginMovingTick = g_utils->getTick();
        scheduleUpdate();
    }

    void Laser::caclPath(const Vec2& touchPoint)
    {
        Vec2 seatPoint = g_utils->getSeatPosition(seatId);
        Vec2 firePoint = g_utils->getFirePosition(seatId);
        Vec2 dirFire = touchPoint - firePoint;
        Vec2 dirMove = firePoint - seatPoint;
        float angle = g_utils->getAngle(dirFire, dirMove);
        angle = clampf(angle, -98, 98);
        dirMove.rotate(Vec2::ZERO, CC_DEGREES_TO_RADIANS(-angle));
        if (seatId >= 3)
            angle += 180;

        Vec2 dir[MAX_LASER_LINE];
        dir[0] = dirMove.getNormalized();
        path[0] = firePoint;
        for (int i = 1; i <= maxLine; ++i)
        {
            path[i] = g_utils->getCollisionEdgePos(path[i - 1], dir[i - 1]);
            if (i < maxLine)
            {
                dir[i] = dir[i - 1];
                Size vsize = Director::getInstance()->getVisibleSize();
                if (fabs(path[i].x) < 0.01f || fabs(path[i].x - vsize.width) < 0.01f)
                    dir[i].x = -dir[i - 1].x;
                if (fabs(path[i].y) < 0.01f || fabs(path[i].y - vsize.height) < 0.01f)
                    dir[i].y = -dir[i - 1].y;
            }
        }

        for (int i = 1; i <= maxLine; ++i)
            path[i] = path[i] + dir[i - 1] * 10;
        path[maxLine] = path[maxLine - 1] + dir[maxLine - 1] * 1280;
    }

    void Laser::caclTimes()
    {
        times[0] = 0;
        for (int i = 1; i <= maxLine; ++i)
        {
            dists[i - 1] = path[i - 1].distance(path[i]);
            times[i] = dists[i - 1] / laserSpeed + times[i - 1];
        }
    }

    void Laser::caclAngles()
    {
        Vec2 seatPoint = g_utils->getSeatPosition(seatId);
        Vec2 firePoint = g_utils->getFirePosition(seatId);
        Vec2 dirMove = firePoint - seatPoint;
        for (int i = 0; i < maxLine; ++i)
        {
            angles[i] = g_utils->getAngle(path[i + 1] - path[i], dirMove);
            angles[i] += (seatId >= 3 ? 180 : 0);
        }
    }

    void Laser::getPath(vector<Vec2>& path)
    {
        for (int i = 0; i <= maxLine; ++i)
            path.push_back(this->path[i]);
    }

    void Laser::getDists(vector<float>& dists)
    {
        for (int i = 0; i < maxLine; ++i)
            dists.push_back(this->dists[i]);
    }

    void Laser::getAngles(vector<float>& angles)
    {
        for (int i = 0; i < maxLine; ++i)
            angles.push_back(this->angles[i]);
    }

    void Laser::update(float delta)
    {
        if (line >= maxLine)
            return;

        float timeElapse = (float)(g_utils->getTick() - beginMovingTick) / 1000.0f;
        int _line = 0;
        while (_line < maxLine && times[_line + 1] <= timeElapse)
            ++_line;

        if (_line >= maxLine)
        {
            line = maxLine;
            route[maxLine] = convertToWorldSpace(path[maxLine]);
            routeCount = maxLine + 1;
        }
        else
        {
            if (line != _line)
            {
                if (enableCollide)
                {
                    line = _line;
                    route[line] = convertToWorldSpace(path[line]);
                    routeCount = line + 2;
                }
                else
                {
                    unscheduleUpdate();
                    return;
                }
            }

            float alpha = (timeElapse - times[line]) / (times[line + 1] - times[line]);
            float dist = dists[line] * alpha;
            Vec2 headPos = path[line].lerp(path[line + 1], alpha);
            route[line + 1] = convertToWorldSpace(headPos);

            g_view->onLaserUpdate(seatId, line, dist, headPos.x, headPos.y);
        }
    }

    void Laser::die()
    {
        enableCollide = false;
        runAction(Sequence::create(DelayTime::create(1.167f), RemoveSelf::create(), nullptr));
    }

} // namespace FishGameView
