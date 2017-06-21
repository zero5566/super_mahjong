#include "cocos2d.h"
#include "fish_game_view.h"

using namespace std;
using namespace cocos2d;

namespace FishGameView
{
    extern Utils* g_utils;

    Config::Config()
    {
        loadWeaponConfig();

        loadPath();
        loadTrack();
        loadCollisions();
    }

    const WeaponConfig* Config::getWeaponConfig(int weaponId)
    {
        auto iter = weaponConfig.find(weaponId);
        if (iter == weaponConfig.end())
        {
            CCLOGWARN("get weapon config error: %i", weaponId);
            return nullptr;
        }
        return &iter->second;
    }

    Path* Config::getPath(int pathId)
    {
        auto iter = paths.find(pathId);
        if (iter == paths.end())
            return nullptr;
        return &iter->second;
    }

    bool Config::getTrack(const string& file, vector<Wave>& track)
    {
        auto iter = tracks.find(file);
        if (iter == tracks.end())
            return false;
        track = iter->second;
        return true;
    }

    bool Config::getCollisions(int fishResId, Box& _colliders)
    {
        auto iter = colliders.find(fishResId);
        if (iter == colliders.end())
            return false;
        _colliders = iter->second;
        return true;
    }

    void Config::loadWeaponConfig()
    {
        std::string content = FileUtils::getInstance()->getStringFromFile("games/fish/fish/config/ct_weapon");
        if (content.empty())
            return;
        vector<string> lines;
        splitString(content, "\r\n", lines);
        for (size_t i = 1; i < lines.size(); ++i)
        {
            string& line = lines[i];
            vector<string> segs;
            splitString(line, "\t", segs);
            if (segs.size() < 11)
                continue;
            WeaponConfig cfg;
            cfg.weaponId = atoi(segs[0].c_str());
            cfg.bulletSpeed = atoi(segs[7].c_str());
            weaponConfig[cfg.weaponId] = cfg;
        }
    }

    void Config::loadPath()
    {
        string str = FileUtils::getInstance()->getStringFromFile("games/fish/fish/config/path");
        vector<string> lines;
        splitString(str, "\r\n", lines);

        for (string& line : lines)
        {
            vector<string> segs;
            splitString(line, "|", segs);
            if (segs.size() < 5)
                continue;
            Path path;
            path.pathId = atoi(segs[0].c_str());
            str2Vec2Array(segs[2], path.points);
            str2Vec4Array(segs[3], path.ctrls);
            str2FloatArray(segs[4], path.speeds);
            paths[path.pathId] = path;
        }
    }

    void Config::loadTrack()
    {
        string content = FileUtils::getInstance()->getStringFromFile("games/fish/fish/config/tracks.json");
        vector<string> files;
        splitString(content, "\r\n[]\", ", files);

        for (string& _file : files)
        {
            string str = FileUtils::getInstance()->getStringFromFile("games/fish/fish/config/" + _file);
            if (str.empty())
                continue;

            vector<string> lines;
            splitString(str, "\r\n", lines);
            vector<Wave> _track;

            // 第一行：track total time

            // 后面行：waves
            for (size_t i = 1; i < lines.size(); ++i)
            {
                string& line = lines[i];

                vector<string> segs;
                splitString(line, "|", segs);
                if (segs.size() < 4)
                    continue;
                Wave wave;
                wave.fishResId = atoi(segs[0].c_str());
                wave.enterTime = atof(segs[1].c_str());
                wave.exitTime = atof(segs[2].c_str());
                wave.pathId = atoi(segs[3].c_str());
                _track.push_back(wave);
            }

            if (!_track.empty())
                tracks[_file] = _track;
        }
    }

    void Config::loadCollisions()
    {
        string str = FileUtils::getInstance()->getStringFromFile("games/fish/fish/config/colliders");
        vector<string> lines;
        splitString(str, "\r\n", lines);
        for (string& line : lines)
        {
            vector<string> segs;
            splitString(line, "|", segs);
            if (segs.size() < 3)
                continue;

            int fishResId = atoi(segs[0].c_str());
            Box box;
            box.sz = str2Vec2(segs[1]);
            Vec2 center = str2Vec2(segs[2]);
            box.center = Vec2(g_utils->vx2px(center.x), g_utils->vy2py(center.y));

            float minX = 0;
            float maxX = 0;
            float minY = 0;
            float maxY = 0;
            for (size_t i = 3; i < segs.size(); ++i)
            {
                vector<string> _segs;
                splitString(segs[i], ";", _segs);
                if (_segs.size() < 2)
                    continue;

                Box::RC rc;
                rc.maxInnerDistSquared = atof(_segs[0].c_str());
                rc.rc = str2Rect(_segs[1]);
                box.rcs.push_back(rc);

                if (rc.rc.origin.x < minX)
                    minX = rc.rc.origin.x;
                if (rc.rc.origin.x + rc.rc.size.width > maxX)
                    maxX = rc.rc.origin.x + rc.rc.size.width;
                if (rc.rc.origin.y < minY)
                    minY = rc.rc.origin.y;
                if (rc.rc.origin.y + rc.rc.size.height > maxY)
                    maxY = rc.rc.origin.y + rc.rc.size.height;
            }

            box.head = Vec2(minX + center.x, center.y);
            box.tail = Vec2(maxX + center.x, center.y);
            box.bottom = Vec2(center.x, minY + center.y);
            box.top = Vec2(center.x, maxY + center.y);

            colliders[fishResId] = box;
        }
    }

    void Config::splitString(const string& str, const char* seps, vector<string>& vec)
    {
        char* buf = new char[str.length() + 1];
        strcpy(buf, str.c_str());

        char* token = strtok(buf, seps);
        while (token)
        {
            vec.push_back(token);
            token = strtok(nullptr, seps);
        }
        CC_SAFE_DELETE_ARRAY(buf);
    }

    Vec2 Config::str2Vec2(const string& str)
    {
        char px[32] = { 0 };
        strcpy(px, str.c_str());
        char* py = strchr(px, ',');
        if (py != nullptr)
            *py++ = 0;
        float x = px ? atof(px) : 0;
        float y = py ? atof(py) : 0;
        return Vec2(g_utils->px2vx(x), g_utils->py2vy(y));
    }

    Vec4 Config::str2Vec4(const string& str)
    {
        char px[64] = { 0 };
        strcpy(px, str.c_str());
        char* py = strchr(px, ',');
        char* pz = nullptr;
        char* pw = nullptr;
        if (py != nullptr)
        {
            *py++ = 0;
            pz = strchr(py, ',');
            if (pz != nullptr)
            {
                *pz++ = 0;
                pw = strchr(pz, ',');
                if (pw != nullptr)
                    *pw++ = 0;
            }
        }
        float x = px ? atof(px) : 0;
        float y = py ? atof(py) : 0;
        float z = pz ? atof(pz) : 0;
        float w = pw ? atof(pw) : 0;
        return Vec4(g_utils->px2vx(x), g_utils->py2vy(y), g_utils->px2vx(z), g_utils->py2vy(w));
    }

    Rect Config::str2Rect(const string& str)
    {
        char px[64] = { 0 };
        strcpy(px, str.c_str());
        char* py = strchr(px, ',');
        char* pz = nullptr;
        char* pw = nullptr;
        if (py != nullptr)
        {
            *py++ = 0;
            pz = strchr(py, ',');
            if (pz != nullptr)
            {
                *pz++ = 0;
                pw = strchr(pz, ',');
                if (pw != nullptr)
                    *pw++ = 0;
            }
        }
        float x = px ? atof(px) : 0;
        float y = py ? atof(py) : 0;
        float width = pz ? atof(pz) : 0;
        float height = pw ? atof(pw) : 0;
        return Rect(x, y, width, height);
    }

    void Config::str2Vec2Array(const string& str, vector<Vec2>& vec)
    {
        vector<string> segs;
        splitString(str, ";", segs);
        for (string& seg : segs)
            vec.push_back(str2Vec2(seg));
    }

    void Config::str2Vec4Array(const string& str, vector<Vec4>& vec)
    {
        vector<string> segs;
        splitString(str, ";", segs);
        for (string& seg : segs)
            vec.push_back(str2Vec4(seg));
    }

    void Config::str2FloatArray(const string& str, vector<float>& vec)
    {
        vector<string> segs;
        splitString(str, ";", segs);
        for (string& seg : segs)
            vec.push_back(atof(seg.c_str()));
    }

    void Config::str2IntArray(const string& str, vector<int>& vec)
    {
        vector<string> segs;
        splitString(str, ",", segs);
        for (string& seg : segs)
            vec.push_back(atoi(seg.c_str()));
    }

} // namespace FishGameView
