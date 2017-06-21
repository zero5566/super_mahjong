#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>

class Utils
{
public:
    static std::string desNicaiAndBase64(const std::string& str);
    static std::string md5(const std::string& str);
    static std::string fileMD5(const std::string& filePath);
    static void moveDir(const std::string& src, const std::string& dst);
    static std::string urlEncode(const std::string& url);

    static std::string getDeviceName();
    static std::string getDeviceUUID();
};

#endif // __UTILS_H__
