#ifndef __FISH_GAME_VIEW_UTILS_H__
#define __FISH_GAME_VIEW_UTILS_H__

namespace FishGameView
{
    class Utils
    {
    public:
        Utils();

        /**
        @brief 服务器时间
        */
        void setServerTime(double serverNowTime, double serverStartTime);
        int64_t getServerNowTime();
        int64_t getServerStartTime();
        double getServerTimeElapse(double serverNowTime = 0);

        /**
        @brief 自己的座位号
        */
        void setMySeat(int seatId);
        int getMySeat();

        /**
        @brief 获取时钟tick
        @return tick
        */
        int64_t getTick();

        /**
        @brief 屏幕坐标值，屏幕百分比：value to percentage
        @param value 屏幕X或者Y
        @return 相对于屏幕的百分比
        */
        float vx2px(float vx);
        float vy2py(float vy);
        float px2vx(float px);
        float py2vy(float py);

        /**
        @brief 根据座位号，获取位置
        @param seatId 座位号
        @return tick
        */
        cocos2d::Vec2 getSeatPosition(int seatId);
        cocos2d::Vec2 getBatteryPosition(int seatId);
        cocos2d::Vec2 getFirePosition(int seatId);

        /**
        @brief 根据鱼的头尾检查鱼是否处于屏幕中
        @param head 头
        @param tail 尾
        @return 鱼是否处于屏幕中
        */
        bool checkInScreen(const cocos2d::Vec2& head, const cocos2d::Vec2& tail);

        /**
        @brief 获取向量dir1到向量dir2的夹角，-180至180
        @param dir1 向量一
        @param dir2 向量二
        @return 夹角
        */
        float getAngle(const cocos2d::Vec2& dir1, const cocos2d::Vec2& dir2);

        /**
        @brief 获取线段A-B，与线段C-D的交点
        @param A 线段一的起点
        @param B 线段一的终点
        @param C 线段二的起点
        @param D 线段二的终点
        @param pt 输出参数，交点
        @return 是否相交
        */
        bool getIntersectPoint(const cocos2d::Vec2& A, const cocos2d::Vec2& B, const cocos2d::Vec2& C, const cocos2d::Vec2& D, cocos2d::Vec2* pt = nullptr);

        /**
        @brief 获取线段A-B，与圆的交点
        @param A 线段一的起点
        @param B 线段一的终点
        @param center 圆的中心点
        @param radius 圆的半径
        @param pt 输出参数，交点
        @return 是否相交
        */
        bool getIntersectPoint(const cocos2d::Vec2& A, const cocos2d::Vec2& B, const cocos2d::Vec2& center, float radius, cocos2d::Vec2* pt = nullptr);

        /**
        @brief 从pt点出发，沿dir方向，计算边界碰撞点
        @param pt 起始点
        @param dir 方向
        @return 边界碰撞点
        */
        cocos2d::Vec2 getCollisionEdgePos(const cocos2d::Vec2& pt, const cocos2d::Vec2& dir);

        /**
        @brief 从A点出发，沿dir方向，计算边界碰撞点或炮台
        @param pt 起始点
        @param dir 方向
        @param pt 输出参数，边界或炮台的碰撞点
        @return 若与炮台碰撞，返回座位号，否则返回0
        */
        int getCollisionEdgePosOrBattery(const cocos2d::Vec2& A, const cocos2d::Vec2& dir, cocos2d::Vec2* pt = nullptr);

        /**
        @brief 计算贝塞尔曲线的顶点
        @param origin 起始点
        @param control 控制点
        @param destination 结束点
        @param segments 线段数量
        @param path 输出参数，保存顶点
        */
        void getCubicBezierVertices(const cocos2d::Vec2& origin, const cocos2d::Vec2& control1, const cocos2d::Vec2& control2, const cocos2d::Vec2& destination, unsigned int segments, std::vector<cocos2d::Vec2>& path);

    private:
        int64_t timeDifferences;
        int64_t serverStartTime;
        int mySeat;

        float visibleWidth;
        float visibleHeight;
        float width2design;
        float design2width;
        float height2design;
        float design2height;

        cocos2d::Vec2 minPos;
        cocos2d::Vec2 maxPos;
        cocos2d::Vec2 cornerPos[4];
        cocos2d::Vec2 seatPos[4];
        cocos2d::Vec2 batteryPos[4];
        cocos2d::Vec2 firePos[4];
    };
}

#endif // __FISH_GAME_VIEW_UTILS_H__
