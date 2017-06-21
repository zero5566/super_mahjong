#include "cocos2d.h"
#include "tcp_socket.h"
#include "json/document.h"
#include "json/writer.h"
#include "json/stringbuffer.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <winsock.h>
#else
#include <netinet/in.h>
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_WIN32

#define CMD_FIRE_VERIFY     1
#define CMD_FIRE_RESPONSE   2
#define CMD_FIRE_BROADCAST  3
#define CMD_CATCH_VERIFY    4
#define CMD_CATCH_SUCCESS   5
#define CMD_CATCH_FAILED    6

#pragma pack(1)
struct fire_verify
{
    char     flag[3];
    int8_t   cmd;
    uint64_t userId;
    int64_t  tableId;
    int8_t   seatId;
    int16_t  weaponId;
    float    x;
    float    y;
    int8_t   bulletId;
};
struct fire_response
{
    char     flag[3];
    int8_t   cmd;
    int8_t   bulletId;
    int16_t  weaponId;
    int8_t   reason;
    uint64_t gold;
};
struct fire_broadcast
{
    char     flag[3];
    int8_t   cmd;
    int8_t   seatId;
    int8_t   bulletId;
    int16_t  weaponId;
    float    x;
    float    y;
    uint64_t gold;
};
struct catch_verify
{
    char     flag[3];
    int8_t   cmd;
    uint64_t userId;
    uint64_t tableId;
    int8_t   seatId;
    int8_t   bulletId;
    int16_t  weaponId;
    uint16_t fishId;
};
struct catch_success_item
{
    int16_t itemId;
    int32_t itemNum;
};
struct catch_success
{
    char     flag[3];
    int8_t   cmd;
    int8_t   seatId;
    int8_t   bulletId;
    uint16_t fishId;
    int8_t   count;

    // count变长，多个物品：
    // catch_success_item items[];
};
struct catch_failed
{
    char     flag[3];
    int8_t   cmd;
    int8_t   bulletId;
    uint16_t fishId;
    int8_t   reason;
};
#pragma pack()

uint64_t hton64(uint64_t host)
{
    uint64_t high = (host >> 32) & 0xFFFFFFFF;
    uint64_t low = host & 0xFFFFFFFF;

    low = htonl(low);
    high = htonl(high);

    uint64_t ret = low << 32;
    ret |= high;
    return ret;
}

uint64_t ntoh64(uint64_t net)
{
    uint64_t high = (net >> 32) & 0xFFFFFFFF;
    uint64_t low = net & 0xFFFFFFFF;

    low = ntohl(low);
    high = ntohl(high);

    uint64_t ret = low << 32;
    ret |= high;
    return ret;
}

static int json_2_fire_verify(const char* msg, char* binaryBuffer)
{
    rapidjson::Document doc;
    doc.Parse<0>(msg);
    if (doc.HasParseError())
        return -1;
    if (!doc.IsObject())
        return -1;
    if (!doc.HasMember("userId") || !doc.HasMember("tableId") || !doc.HasMember("seatId") ||
        !doc.HasMember("weaponId") || !doc.HasMember("x") || !doc.HasMember("y") || !doc.HasMember("bulletId"))
        return -1;

    fire_verify* pkg = (fire_verify*)binaryBuffer;
    pkg->flag[0] = '$';
    pkg->flag[1] = 'f';
    pkg->flag[2] = 's';
    pkg->cmd = CMD_FIRE_VERIFY;
    pkg->userId = hton64(doc["userId"].GetUint64());
    pkg->tableId = hton64(doc["tableId"].GetInt64());
    pkg->seatId = doc["seatId"].GetInt();
    pkg->weaponId = htons(doc["weaponId"].GetInt());
    pkg->x = doc["x"].GetDouble();
    pkg->y = doc["y"].GetDouble();
    pkg->bulletId = doc["bulletId"].GetInt();

    return sizeof(fire_verify);
}

static int json_2_catch_verify(const char* msg, char* binaryBuffer)
{
    rapidjson::Document doc;
    doc.Parse<0>(msg);
    if (doc.HasParseError())
        return -1;
    if (!doc.IsObject())
        return -1;
    if (!doc.HasMember("userId") || !doc.HasMember("tableId") || !doc.HasMember("seatId") ||
        !doc.HasMember("bulletId") || !doc.HasMember("weaponId") || !doc.HasMember("fishId"))
        return -1;

    catch_verify* pkg = (catch_verify*)binaryBuffer;
    pkg->flag[0] = '$';
    pkg->flag[1] = 'f';
    pkg->flag[2] = 's';
    pkg->cmd = CMD_CATCH_VERIFY;
    pkg->userId = hton64(doc["userId"].GetUint64());
    pkg->tableId = hton64(doc["tableId"].GetInt64());
    pkg->seatId = doc["seatId"].GetInt();
    pkg->bulletId = doc["bulletId"].GetInt();
    pkg->weaponId = htons(doc["weaponId"].GetInt());
    pkg->fishId = htons(doc["fishId"].GetUint());

    return sizeof(catch_verify);
}

static int fire_response_2_json(const char* msg, char* jsonBuffer)
{
    fire_response* rsp = (fire_response*)msg;
    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    doc.AddMember("cmd", "fire_response", allocator);
    doc.AddMember("bulletId", rsp->bulletId, allocator);
    doc.AddMember("weaponId", (int16_t)ntohs(rsp->weaponId), allocator);
    doc.AddMember("reason", rsp->reason, allocator);
    doc.AddMember("gold", ntoh64(rsp->gold), allocator);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    const char* pszJson = buffer.GetString();
    strcpy(jsonBuffer, pszJson);
    return strlen(pszJson) + 1;
}

static int fire_broadcast_2_json(const char* msg, char* jsonBuffer)
{
    fire_broadcast* broadcast = (fire_broadcast*)msg;
    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    doc.AddMember("cmd", "fire_broadcast", allocator);
    doc.AddMember("seatId", broadcast->seatId, allocator);
    doc.AddMember("bulletId", broadcast->bulletId, allocator);
    doc.AddMember("weaponId", (int16_t)ntohs(broadcast->weaponId), allocator);
    doc.AddMember("x", broadcast->x, allocator);
    doc.AddMember("y", broadcast->y, allocator);
    doc.AddMember("gold", ntoh64(broadcast->gold), allocator);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    const char* pszJson = buffer.GetString();
    strcpy(jsonBuffer, pszJson);
    return strlen(pszJson) + 1;
}

static int catch_success_2_json(const char* msg, char* jsonBuffer)
{
    catch_success* rsp = (catch_success*)msg;
    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    doc.AddMember("cmd", "catch_success", allocator);
    doc.AddMember("seatId", rsp->seatId, allocator);
    doc.AddMember("bulletId", rsp->bulletId, allocator);
    doc.AddMember("fishId", (uint16_t)ntohs(rsp->fishId), allocator);

    catch_success_item* item = (catch_success_item*)(rsp + 1);

    rapidjson::Value array(rapidjson::kArrayType);
    for (int i = 0; i < rsp->count; ++i)
    {
        rapidjson::Value object(rapidjson::kObjectType);
        object.AddMember("itemId", (int16_t)ntohs(item->itemId), allocator);
        object.AddMember("itemNum", (int32_t)ntohl(item->itemNum), allocator);
        array.PushBack(object, allocator);
        ++item;
    }
    doc.AddMember("items", array, allocator);

    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    const char* pszJson = buffer.GetString();
    strcpy(jsonBuffer, pszJson);
    return strlen(pszJson) + 1;
}

static int catch_failed_2_json(const char* msg, char* jsonBuffer)
{
    catch_failed* rsp = (catch_failed*)msg;
    rapidjson::Document doc;
    doc.SetObject();
    rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
    doc.AddMember("cmd", "catch_failed", allocator);
    doc.AddMember("bulletId", rsp->bulletId, allocator);
    doc.AddMember("fishId", (uint16_t)ntohs(rsp->fishId), allocator);
    doc.AddMember("reason", rsp->reason, allocator);
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
    doc.Accept(writer);

    const char* pszJson = buffer.GetString();
    strcpy(jsonBuffer, pszJson);
    return strlen(pszJson) + 1;
}

int TCPSocket::checkJson2Binary(const char* msg, char* binaryBuffer)
{
    if (strstr(msg, "fire_verify") != nullptr)
    {
        return json_2_fire_verify(msg + 3, binaryBuffer);
    }
    else if (strstr(msg, "catch_verify") != nullptr)
    {
        return json_2_catch_verify(msg + 3, binaryBuffer);
    }
    return 0;
}

int TCPSocket::checkBinary2Json(const char* msg, char* jsonBuffer)
{
    // $fs + CMD
    int8_t cmd = msg[3];
    switch (cmd)
    {
    case CMD_FIRE_RESPONSE:
        return fire_response_2_json(msg, jsonBuffer);
    case CMD_FIRE_BROADCAST:
        return fire_broadcast_2_json(msg, jsonBuffer);
    case CMD_CATCH_SUCCESS:
        return catch_success_2_json(msg, jsonBuffer);
    case CMD_CATCH_FAILED:
        return catch_failed_2_json(msg, jsonBuffer);
    }
    return 0;
}
