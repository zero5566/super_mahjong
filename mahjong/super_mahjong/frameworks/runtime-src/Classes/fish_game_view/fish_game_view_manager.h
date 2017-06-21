#ifndef __FISH_GAME_VIEW_MANAGER_H__
#define __FISH_GAME_VIEW_MANAGER_H__

namespace FishGameView
{
    class Fish;
    class Bullet;
    class Laser;
    class Piton;

    /**
    * 管理器
    */
    class Manager
    {
    public:
        void init();
        void release();
        void update(float delta);

        // Bullet
        void addBullet(Bullet* bullet);
        void delBullet(Bullet* bullet);
        void bulletDie(int seatId, int bulletId = 0);
        Bullet* getBullet(int seatId, int bulletId);
        cocos2d::Vec2 getBulletPos(int seatId, int bulletId);

        // Fish
        void addFish(Fish* fish);
        void delFish(Fish* fish);
        void fishDie(int fishId);
        void fishDie(const std::vector<int>& vec);
        Fish* getFish(int fishId);
        Fish* getIceDragon();
        cocos2d::Vec2 getFishPos(int fishId);
        int getFishAtPosition(const cocos2d::Vec2& pt);
        void getFishesInRange(std::vector<int>& vec, const cocos2d::Vec2& center, float radius);
        void getNormalFishesInRange(std::vector<int>& vec, const cocos2d::Vec2& center, float radius);
        void getNearbyFishes(std::vector<int>& vec, const cocos2d::Vec2& center, int count);
        void getRandomFishes(std::vector<int>& vec, int fishId, int count);

        // Freeze
        void freeze(double frozenTime);
        void unfreeze(double frozenTime, double unfrozenTime);

        // Laser
        void addLaser(int seatId, Laser* laser);
        void delLaser(Laser* laser);
        void laserEnd(int seatId);
        void getLaserPath(int seatId, std::vector<cocos2d::Vec2>& path);
        void getLaserDists(int seatId, std::vector<float>& dists);
        void getLaserAngles(int seatId, std::vector<float>& angles);

        // Piton
        void addPiton(Piton* piton);
        void delPiton(Piton* piton);
        cocos2d::Vec2 getPitonPos(int pitonId);
        void pitonDie(int pitonId);

        // Lock
        void bulletLockFish(Bullet* bullet, Fish* fish);
        void checkReleaseLock(Bullet* bullet);
        void checkReleaseLock(Fish* fish);

    private:
        std::map<int, Bullet*> bullets;
        std::map<int, Fish*> fishes;
        std::map<int, std::set<Fish*> > fishHierarchy;
        std::map<int, Laser*> lasers;
        std::map<int, Piton*> pitons;

        std::map<Fish*, std::set<Bullet*> > locks;
    };

} // namespace FishGameView

#endif // __FISH_GAME_VIEW_MANAGER_H__
