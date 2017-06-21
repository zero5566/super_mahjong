#ifndef __FISH_GAME_VIEW_BULLET_H__
#define __FISH_GAME_VIEW_BULLET_H__

namespace FishGameView
{
    class Fish;

    /**
    * 炮弹
    */
    class Bullet : public cocos2d::Node
    {
    public:
        static Bullet* create(int seatId, int bulletId, int weaponId, cocos2d::Vec2 touchPoint, Fish* fish);
        void init(int seatId, int bulletId, int weaponId, cocos2d::Vec2 touchPoint, Fish* fish);

        void moveToFish(Fish* fish);
        void moveFreely();

        void moveToEdgePoint();
        void onReachEdgePoint();
        void onReachLockPoint();

        void update(float delta);
        void die();

    public:
        int seatId;
        int bulletId;
        int bulletSpeed;

        Fish* lockFish;
        bool enableCollide;

        // 自由运动参数
        cocos2d::Vec2 moveDir;
        int64_t beginMovingTick;
        int64_t prevCollideTick;
        int64_t nextCollideTick;
        cocos2d::Vec2 prevPoint;
        cocos2d::Vec2 nextPoint;
    };

} // namespace FishGameView

#endif // __FISH_GAME_VIEW_BULLET_H__
