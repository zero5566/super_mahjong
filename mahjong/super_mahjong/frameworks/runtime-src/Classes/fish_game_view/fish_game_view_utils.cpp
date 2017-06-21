#include "cocos2d.h"
#include "fish_game_view.h"

using namespace std;
using namespace cocos2d;

#define DESIGN_WIDTH  1136      // 设计宽度
#define DESIGN_HEIGHT 640       // 设计高度
#define FIRE_FENCE_RADIUS 90    // 火墙半径

namespace FishGameView
{
    Utils::Utils()
    {
        timeDifferences = 0;
        serverStartTime = 0;
        mySeat = 0;

        Size vsize = Director::getInstance()->getVisibleSize();
        visibleWidth = vsize.width;
        visibleHeight = vsize.height;
        width2design = visibleWidth / DESIGN_WIDTH;
        design2width = DESIGN_WIDTH / visibleWidth;
        height2design = visibleHeight / DESIGN_HEIGHT;
        design2height = DESIGN_HEIGHT / visibleHeight;

        minPos = Vec2(0.001f, 0.001f);
        maxPos = Vec2(visibleWidth - 0.001f, visibleHeight - 0.001f);
        cornerPos[0] = Vec2(0, 0);
        cornerPos[1] = Vec2(visibleWidth, 0);
        cornerPos[2] = Vec2(visibleWidth, visibleHeight);
        cornerPos[3] = Vec2(0, visibleHeight);
        seatPos[0] = Vec2(visibleWidth * 3 / 10, 0);
        seatPos[1] = Vec2(visibleWidth * 7 / 10, 0);
        seatPos[2] = Vec2(visibleWidth * 7 / 10, visibleHeight);
        seatPos[3] = Vec2(visibleWidth * 3 / 10, visibleHeight);
        batteryPos[0] = Vec2(visibleWidth * 3 / 10, 26);
        batteryPos[1] = Vec2(visibleWidth * 7 / 10, 26);
        batteryPos[2] = Vec2(visibleWidth * 7 / 10, visibleHeight - 26);
        batteryPos[3] = Vec2(visibleWidth * 3 / 10, visibleHeight - 26);
        firePos[0] = Vec2(visibleWidth * 3 / 10, 40);
        firePos[1] = Vec2(visibleWidth * 7 / 10, 40);
        firePos[2] = Vec2(visibleWidth * 7 / 10, visibleHeight - 40);
        firePos[3] = Vec2(visibleWidth * 3 / 10, visibleHeight - 40);
    }

    void Utils::setServerTime(double _serverNowTime, double _serverStartTime)
    {
        timeDifferences = (int64_t)_serverNowTime - getTick();
        if (_serverStartTime > 0)
            serverStartTime = (int64_t)_serverStartTime;
    }

    int64_t Utils::getServerNowTime()
    {
        return getTick() + timeDifferences;
    }

    int64_t Utils::getServerStartTime()
    {
        return serverStartTime;
    }

    double Utils::getServerTimeElapse(double serverNowTime)
    {
        if (serverNowTime > 0)
            return serverNowTime - (double)serverStartTime / 1000.0;
        return (double)(getTick() + timeDifferences - serverStartTime) / 1000.0;
    }

    void Utils::setMySeat(int seatId)
    {
        mySeat = seatId;
    }

    int Utils::getMySeat()
    {
        return mySeat;
    }

    int64_t Utils::getTick()
    {
        timeval tv;
        gettimeofday(&tv, nullptr);
        return ((int64_t)tv.tv_sec * 1000) + ((int64_t)tv.tv_usec / 1000);
    }

    float Utils::vx2px(float vx)
    {
        return vx * design2width;
    }

    float Utils::vy2py(float vy)
    {
        return vy * design2height;
    }

    float Utils::px2vx(float px)
    {
        return px * width2design;
    }

    float Utils::py2vy(float py)
    {
        return py * height2design;
    }

    Vec2 Utils::getSeatPosition(int seatId)
    {
        assert(seatId >= 1 && seatId <= 4);
        return seatPos[seatId - 1];
    }

    Vec2 Utils::getBatteryPosition(int seatId)
    {
        assert(seatId >= 1 && seatId <= 4);
        return batteryPos[seatId - 1];
    }

    Vec2 Utils::getFirePosition(int seatId)
    {
        assert(seatId >= 1 && seatId <= 4);
        return firePos[seatId - 1];
    }

    bool Utils::checkInScreen(const Vec2& head, const Vec2& tail)
    {
        return (head.x > 0 && head.x < visibleWidth && head.y > 0 && head.y < visibleHeight) || (tail.x > 0 && tail.x < visibleWidth && tail.y > 0 && tail.y < visibleHeight);
    }

    float Utils::getAngle(const Vec2& dir1, const Vec2& dir2)
    {
        return CC_RADIANS_TO_DEGREES(atan2f(dir1.cross(dir2), dir1.dot(dir2)));
    }

    bool Utils::getIntersectPoint(const Vec2& A, const Vec2& B, const Vec2& C, const Vec2& D, Vec2* pt)
    {
        if ((A.x == B.x && A.y == B.y) || (C.x == D.x && C.y == D.y))
            return false;
        float denom = (D.y - C.y) * (B.x - A.x) - (D.x - C.x) * (B.y - A.y);
        if (denom == 0)
            return false;
        float S = ((A.y - C.y) * (D.x - C.x) - (A.x - C.x) * (D.y - C.y)) / denom;
        float T = ((A.y - C.y) * (B.x - A.x) - (A.x - C.x) * (B.y - A.y)) / denom;
        if (S < 0.0f || S > 1.0f || T < 0.0f || T > 1.0f)
            return false;
        if (pt != nullptr)
        {
            pt->x = A.x + S * (B.x - A.x);
            pt->y = A.y + S * (B.y - A.y);
        }
        return true;
    }

    bool Utils::getIntersectPoint(const Vec2& A, const Vec2& B, const Vec2& center, float radius, Vec2* pt)
    {
        Vec2 dir1 = (B - A).getNormalized();
        Vec2 dir2 = Vec2(dir1.y, -dir1.x) * radius;
        Vec2 C = center + dir2;
        Vec2 D = center - dir2;
        Vec2 cross;
        if (!getIntersectPoint(A, B, C, D, &cross))
            return false;

        float length = sqrtf(radius * radius - center.distanceSquared(cross));
        if (pt != nullptr)
            *pt = cross - dir1 * length;

        return true;
    }

    Vec2 Utils::getCollisionEdgePos(const Vec2& pt, const Vec2& dir)
    {
        Vec2 pt1 = pt; pt1.clamp(minPos, maxPos);
        Vec2 pt2 = pt + dir * 4000000;
        Vec2 crosspoint = Vec2::ZERO;

        if (getIntersectPoint(pt1, pt2, cornerPos[0], cornerPos[1], &crosspoint) ||
            getIntersectPoint(pt1, pt2, cornerPos[2], cornerPos[3], &crosspoint) ||
            getIntersectPoint(pt1, pt2, cornerPos[0], cornerPos[3], &crosspoint) ||
            getIntersectPoint(pt1, pt2, cornerPos[1], cornerPos[2], &crosspoint)) {
        }

        crosspoint.clamp(minPos, maxPos);
        return crosspoint;
    }

    int Utils::getCollisionEdgePosOrBattery(const Vec2& A, const Vec2& dir, Vec2* pt)
    {
        Vec2 ptFar = A + dir * 4000000;
        for (int seatId = 1; seatId <= 4; ++seatId)
        {
            Vec2 batteryPosition = batteryPos[seatId - 1];
            if (getIntersectPoint(A, ptFar, batteryPosition, FIRE_FENCE_RADIUS, pt))
                return seatId;
        }
        *pt = getCollisionEdgePos(A, dir);
        return 0;
    }

    void Utils::getCubicBezierVertices(const Vec2& origin, const Vec2& control1, const Vec2& control2, const Vec2& destination, unsigned int segments, vector<Vec2>& path)
    {
        float t = 0.0f;
        for (unsigned int i = 0; i < segments; i++)
        {
            float x = powf(1 - t, 3) * origin.x + 3.0f * powf(1 - t, 2) * t * control1.x + 3.0f * (1 - t) * t * t * control2.x + t * t * t * destination.x;
            float y = powf(1 - t, 3) * origin.y + 3.0f * powf(1 - t, 2) * t * control1.y + 3.0f * (1 - t) * t * t * control2.y + t * t * t * destination.y;
            path.push_back(Vec2(x, y));
            t += 1.0f / segments;
        }
        path.push_back(Vec2(destination.x, destination.y));
    }

} // namespace FishGameView
