#ifndef __FISH_GAME_VIEW_LASER1_H__
#define __FISH_GAME_VIEW_LASER1_H__

namespace FishGameView
{
#define MAX_LASER_LINE 8  // 最大激光条数

    /**
    * 炮弹
    */
    class Laser : public cocos2d::Node
    {
    public:
        static Laser* create(int seatId, int weaponId, int maxLine, const cocos2d::Vec2& touchPoint);
        void init(int seatId, int weaponId, int maxLine, const cocos2d::Vec2& touchPoint);

        void caclPath(const cocos2d::Vec2& touchPoint);
        void caclTimes();
        void caclAngles();
        void getPath(std::vector<cocos2d::Vec2>& path);
        void getDists(std::vector<float>& dists);
        void getAngles(std::vector<float>& angles);

        void update(float delta);
        void die();

    public:
        int seatId;
        int maxLine;
        int laserSpeed;
        bool enableCollide;

        // 路径信息
        cocos2d::Vec2 path[MAX_LASER_LINE + 1];
        float dists[MAX_LASER_LINE];
        float times[MAX_LASER_LINE + 1];
        float angles[MAX_LASER_LINE];

        // 运动信息
        cocos2d::Vec2 route[MAX_LASER_LINE + 1];
        int routeCount;
        int line;
        int64_t beginMovingTick;
    };

} // namespace FishGameView

#endif // __FISH_GAME_VIEW_LASER1_H__
