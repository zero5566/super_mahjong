#ifndef __FISH_GAME_VIEW_H__
#define __FISH_GAME_VIEW_H__

#include "fish_game_view_config.h"
#include "fish_game_view_utils.h"
#include "fish_game_view_bullet.h"
#include "fish_game_view_fish.h"
#include "fish_game_view_laser.h"
#include "fish_game_view_piton.h"
#include "fish_game_view_manager.h"

namespace FishGameView
{
    class Bullet;
    class Fish;

    /**
    * 捕鱼游戏主画面
    */
    class View : public cocos2d::Node
    {
    public:
        static View* create();
        virtual bool init() override;
        virtual void onEnter() override;
        virtual void onExit() override;

        // Common
        void setServerTime(double serverNowTime, double serverStartTime);
        void setMySeat(int seatId);
        cocos2d::Node* getEffectLayer();

        // Bullet
        Bullet* playerFire(int seatId, int bulletId, int weaponId, const cocos2d::Vec2& pt, int fishId);
        cocos2d::Vec2 getBulletPos(int seatId, int bulletId);
        void bulletDie(int seatId, int bulletId = 0);

        // Fish
        Fish* fishEnter(int fishResId, int fishId, int pathId, double enterTime, double frozenTime);
        cocos2d::Vec2 getFishPos(int fishId);
        void fishDie(int fishId);
        void fishDie(const std::vector<int>& vec);
        int getFishAtPosition(const cocos2d::Vec2& pt);
        void getFishesInRange(std::vector<int>& vec, const cocos2d::Vec2& center, float radius);
        void getNormalFishesInRange(std::vector<int>& vec, const cocos2d::Vec2& center, float radius);
        void getNearbyFishes(std::vector<int>& vec, const cocos2d::Vec2& center, int count);
        void getRandomFishes(std::vector<int>& vec, int fishId, int count);

        // Freeze
        void freeze(double frozenTime);
        void unfreeze(double frozenTime, double unfrozenTime);

        // Laser
        Laser* laserBegin(int seatId, int weaponId, int maxLine, const cocos2d::Vec2& pt);
        void laserEnd(int seatId);
        void getLaserPath(int seatId, std::vector<cocos2d::Vec2>& path);
        void getLaserDists(int seatId, std::vector<float>& dists);
        void getLaserAngles(int seatId, std::vector<float>& angles);

        // Piton
        Piton* castIcePiton(int pitonId, double angle);
        cocos2d::Vec2 getPitonPos(int pitonId);
        void pitonDie(int pitonId);

        // Delegate
        void onBulletHitFish(int seatId, int bulletId, int fishId);
        void onBulletHitPiton(int seatId, int bulletId, int pitonId);
        void onFishTurn(int fishId);
        void onFishOutScreen(int fishId);
        void onFishExit(int fishId);
        void onLaserUpdate(int seatId, int line, float distance, float x, float y);
        void onLaserHitFish(int seatId, int fishId, const cocos2d::Vec2& ptCollide);
        void onPitonHit(int hitBattery, int pitonId);

    private:
        cocos2d::Node* fishLayer;
        cocos2d::Node* effectLayer;
        cocos2d::Node* bulletLayer;
    };

} // namespace FishGameView

#endif // __FISH_GAME_VIEW_H__
