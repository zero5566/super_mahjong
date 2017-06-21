#ifndef __FISH_GAME_VIEW_FISH_H__
#define __FISH_GAME_VIEW_FISH_H__

namespace FishGameView
{
    class Bullet;
    class Laser;

    /**
    * 鱼
    */
    class Fish : public cocos2d::Node
    {
    public:
        static Fish* create(int fishResId, int fishId);
        void init(int fishResId, int fishId);

        bool setPath(double enterTime, double frozenTime, const std::vector<cocos2d::Vec2>& points, const std::vector<cocos2d::Vec4>& ctrls, const std::vector<float>& speeds);
        void update(float delta);
        bool calcTransform();

        void flipX();
        void flipY();

        void freeze(double frozenTime);
        void unfreeze(double frozenTime, double unfrozenTime);

        bool checkCollision(const cocos2d::Vec2& pt);
        bool checkCollision(Bullet* bullet);
        bool checkInRange(const cocos2d::Vec2& center, float radiusSquared);
        bool checkLaserIntersect(Laser* laser);
        bool chechForChainCatch();

        cocos2d::Vec2 calcNextPosition(float time);

        void die();

    public:
        int fishResId;
        int fishId;
        bool isAlive;

        // 碰撞信息
        Box colliders;

        // 鱼是否在屏幕中可见
        bool isInScreen;

        // 是否冰冻
        bool isFrozen;

        // 移动信息
        double fishEnterTime;
        int64_t beginMovingTick;

        // 最后一次碰撞激光的时间
        int64_t lastCollideTick;

        // 路径信息：path.size() == times.size()
        std::vector<cocos2d::Vec2> path;
        std::vector<float> times;
        std::vector<float> angles;

        // 调头参数：龙龟、电鳗、巨型灯笼鱼、冰龙
        float xHead;
        float xTail;

#if (COCOS2D_DEBUG > 0)
        virtual void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4& transform, uint32_t flags) override;
        cocos2d::DrawNode* drawNode;
        cocos2d::Label* labelFishId;
        bool isHit;
#endif
    };

} // namespace FishGameView

#endif // __FISH_GAME_VIEW_FISH_H__
