#ifndef __FISH_GAME_VIEW_PITON_H__
#define __FISH_GAME_VIEW_PITON_H__

namespace FishGameView
{
    class Bullet;

    /**
    * 冰锥
    */
    class Piton : public cocos2d::Node
    {
    public:
        static Piton* create(int pitonId, const cocos2d::Vec2& ptStart, float angle);
        void init(int pitonId, const cocos2d::Vec2& ptStart, float angle);

        bool checkCollision(Bullet* bullet);
        void onReachCollisionPoint();
        void die();

    public:
        int pitonId;
        bool enableCollide;
        int hitBattery;
    };

} // namespace FishGameView

#endif // __FISH_GAME_VIEW_PITON_H__
