#include "cocos2d.h"
#include "utils.h"
#include "base/CCUserDefault.h"

#define DES_LONG unsigned int
#define DES_ENCRYPT 1
#define DES_DECRYPT 0
#define MD5_DIGEST_LENGTH 16

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "AndroidUtils.h"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
extern std::string getMobileDeviceName();
extern std::string getMobileDeviceUUID();
#endif

using namespace cocos2d;

typedef unsigned char DES_cblock[8];
typedef unsigned char const_DES_cblock[8];
typedef struct DES_ks
{
    union
    {
        DES_cblock cblock;
        DES_LONG deslong[2];
    } ks[16];
} DES_key_schedule;

extern "C" void DES_set_key_unchecked(const_DES_cblock* key, DES_key_schedule* schedule);
extern "C" void DES_ecb_encrypt(const_DES_cblock* input, DES_cblock* output, DES_key_schedule* ks, int enc);
extern "C" int EVP_EncodeBlock(unsigned char* t, const unsigned char* f, int n);
extern "C" int EVP_DecodeBlock(unsigned char* t, const unsigned char* f, int n);
extern "C" unsigned char* MD5(const unsigned char* d, size_t n, unsigned char* md);

static unsigned char* des(unsigned char* src, unsigned srclen, unsigned char* key, int& outLen)
{
    size_t len = srclen / 8 * 8 + 8;
    outLen = len;

    unsigned char* input = new unsigned char[len + 1];
    memset(input, '0', len + 1);
    memcpy(input, src, srclen);

    unsigned char* output = new unsigned char[len + 1];
    memset(output, 0, len + 1);

    DES_key_schedule schedule;
    DES_set_key_unchecked((const_DES_cblock*)key, &schedule);
    for (size_t i = 0; i < len / 8; i++)
        DES_ecb_encrypt((const_DES_cblock*)(input + i * 8), (DES_cblock*)(output + i * 8), &schedule, DES_ENCRYPT);

    delete[] input;
    return output;
}

static unsigned char* tuyooDes(const char* str, int& len)
{
    std::string data = "TUYOO~";
    data += str;
    data += "~POKER201309031548";

    unsigned char* out = des((unsigned char*)(data.c_str()), data.length(), (unsigned char*)("tuyoocom"), len);
    return out;
}

static bool base64(const unsigned char* in, unsigned char* out, const int len, bool encode)
{
    if (encode)
    {
        EVP_EncodeBlock(out, in, len);
    }
    else
    {
        int n = EVP_DecodeBlock(out, in, len);
        for (int i = n - 1; i > 0; i--)
        {
            if (out[i] != '\0')
            {
                n = i + 1;
                break;
            }
        }
    }
    return true;
}

static unsigned char toHex(unsigned char x)
{
    return  x > 9 ? x + 55 : x + 48;
}

std::string Utils::desNicaiAndBase64(const std::string& str)
{
    // step 1: tuyooDes
    int len = str.length();
    unsigned char* pTuyooDes = tuyooDes(str.c_str(), len);

    // step 2: base64
    unsigned char* pBase64 = new unsigned char[len * 2];
    memset(pBase64, 0, len * 2);
    base64(pTuyooDes, pBase64, len, true);

    delete[] pTuyooDes;
    std::string ret((char*)pBase64);
    delete[] pBase64;
    return ret;
}

std::string Utils::md5(const std::string& str)
{
    if (str.empty())
        return "";
    const unsigned char* d = (unsigned char*)str.c_str();
    int n = (int)str.length();
    unsigned char md[MD5_DIGEST_LENGTH] = { 0 };
    MD5(d, n, md);

    char sz[3] = { 0 };
    std::string hash = "";
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
        sprintf(sz, "%02X", md[i]);
        hash += sz;
    }
    return hash;
}

std::string Utils::fileMD5(const std::string& filePath)
{
    if (filePath.empty())
        return "";

    FILE *fp;
    fp = fopen(filePath.c_str(), "rb");
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char* buf = new char[size + 1];
    size_t read = fread(buf, 1, size, fp);
    fclose(fp);

    const unsigned char* d = (unsigned char*)buf;
    int n = (int)size;
    unsigned char md[MD5_DIGEST_LENGTH] = { 0 };
    MD5(d, n, md);

    delete buf;

    char sz[3] = { 0 };
    std::string hash = "";
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
    {
        sprintf(sz, "%02X", md[i]);
        hash += sz;
    }

    return hash;
}

static std::string removeSlash(const std::string& path)
{
    if (*path.rbegin() == '/')
        return path.substr(0, path.length() - 1);
    return path;
}

void Utils::moveDir(const std::string& src, const std::string& dst)
{
    if (!FileUtils::getInstance()->isDirectoryExist(dst))
        FileUtils::getInstance()->createDirectory(dst);
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
    std::string command = "cmd /c xcopy /e /i /y ";
    command += "\"" + removeSlash(src) + "\"" + " \"" + removeSlash(dst) + "\"";
    WinExec(command.c_str(), SW_HIDE);
#else
    std::string command = "cp -RP ";
    command += src + "/* " + dst;
    system(command.c_str());
#endif
}

std::string Utils::urlEncode(const std::string& url)
{
    std::string str = "";
    size_t length = url.length();
    for (size_t i = 0; i < length; i++)
    {
        unsigned char c = (unsigned char)url[i];
        if (isalnum(c) || (c == '-') || (c == '_') || (c == '.') || (c == '~'))
        {
            str += c;
        }
        else if (c == ' ')
        {
            str += "+";
        }
        else
        {
            str += '%';
            str += toHex(c >> 4);
            str += toHex(c % 16);
        }
    }
    return str;
}

std::string Utils::getDeviceName()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    return "windows";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    return "mac";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    return getMobileDeviceName();
#endif
}

std::string Utils::getDeviceUUID()
{
    std::string uuid = UserDefault::getInstance()->getStringForKey("uuid");
    if (uuid.empty())
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
        char sz[16] = { 0 };
        srand(time(0));
        sprintf(sz, "%i", rand());
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        std::string sz = getMobileDeviceUUID();
#endif
        std::string strMD5 = md5(sz);
        uuid = strMD5.substr(10, 12);
        UserDefault::getInstance()->setStringForKey("uuid", uuid);
    }
    return uuid;
}
