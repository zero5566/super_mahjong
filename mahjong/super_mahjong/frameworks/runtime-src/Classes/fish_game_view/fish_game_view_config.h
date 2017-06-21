#ifndef __FISH_GAME_VIEW_CONFIG_H__
#define __FISH_GAME_VIEW_CONFIG_H__

namespace FishGameView
{
    // 武器配置
    struct WeaponConfig
    {
        int weaponId;                   // 武器ID
        int bulletSpeed;                // 弹失速度
    };

    // Path信息
    struct Path
    {
        int pathId;
        std::vector<cocos2d::Vec2> points;
        std::vector<cocos2d::Vec4> ctrls;
        std::vector<float> speeds;
    };

    // Wave信息
    struct Wave
    {
        int fishResId;
        float enterTime;
        float exitTime;
        int pathId;
    };

    // 鱼的碰撞盒，一条鱼可以有多个碰撞盒
    struct Box
    {
        struct RC
        {
            float maxInnerDistSquared;  // Anchor到Box边界的最大距离的平方
            cocos2d::Rect rc;           // 碰撞包围盒
        };

        cocos2d::Vec2 sz;               // 大小
        cocos2d::Vec2 center;           // 中心点
        cocos2d::Vec2 head;             // 头：相对中心点的位置
        cocos2d::Vec2 tail;             // 尾：相对中心点的位置
        cocos2d::Vec2 bottom;           // 底：相对中心点的位置
        cocos2d::Vec2 top;              // 顶：相对中心点的位置
        std::vector<RC> rcs;
    };

    /**
    * 配置表
    */
    class Config
    {
    public:
        Config();
        const WeaponConfig* getWeaponConfig(int weaponId);

        Path* getPath(int pathId);
        bool getTrack(const std::string& file, std::vector<Wave>& track);
        bool getCollisions(int fishResId, Box& colliders);

    private:
        void loadWeaponConfig();

        void loadPath();
        void loadTrack();
        void loadCollisions();

        /**
        @brief 分割字符串，按字符分割字符串，将分割后的字符串存储到vector中
        @param str 需要分割的字符串
        @param seps 分割符，这里不是当作字符串看待，而是字符集合，各个字符都参与分割
        @param vec 输出目标vector
        */
        void splitString(const std::string& str, const char* seps, std::vector<std::string>& vec);

        /**
        @brief 字符串转换为Vec2，Vec4，Rect
        @param str 字符串，格式："111.111,222.222"，"111.111,222.222,333.333,444.444"
        @return Vec2，Vec4
        */
        cocos2d::Vec2 str2Vec2(const std::string& str);
        cocos2d::Vec4 str2Vec4(const std::string& str);
        cocos2d::Rect str2Rect(const std::string& str);

        /**
        @brief 字符串转换为数组
        @param str 字符串，格式："11.1,22.2;33.3,44.4;55.5,66.6;77.7,88.8"，"11.1,22.2,33.3,44.4;55.5,66.6,77.7,88.8"，"111.111,222.222,333.333,444.444"
        @param vec 输出
        */
        void str2Vec2Array(const std::string& str, std::vector<cocos2d::Vec2>& vec);
        void str2Vec4Array(const std::string& str, std::vector<cocos2d::Vec4>& vec);
        void str2FloatArray(const std::string& str, std::vector<float>& vec);
        void str2IntArray(const std::string& str, std::vector<int>& vec);

        std::map<int, WeaponConfig> weaponConfig;
        std::map<int, Path> paths;
        std::unordered_map<std::string, std::vector<Wave> > tracks;
        std::map<int, Box> colliders;
    };

} // namespace FishGameView

#endif // __FISH_GAME_VIEW_CONFIG_H__
