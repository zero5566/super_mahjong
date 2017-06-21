#include "cocos2d.h"
#include "cocos2d_specifics.hpp"

#include "cocostudio/CocoStudio.h"
#include "jsb_utils.h"
#include "utils/utils.h"

using namespace cocos2d;

bool js_utils_md5(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSB_PRECONDITION2(argc == 1, cx, false, "Invalid number of arguments");

    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        std::string str;
        jsval_to_std_string(cx, args.get(0), &str);

        std::string md5Str = Utils::md5(str);
        jsval jsret = std_string_to_jsval(cx, md5Str);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 1", argc);
    return false;
}

bool js_utils_fileMD5(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSB_PRECONDITION2(argc == 1, cx, false, "Invalid number of arguments");

    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        std::string str;
        jsval_to_std_string(cx, args.get(0), &str);

        std::string filePath = Utils::fileMD5(str);
        jsval jsret = std_string_to_jsval(cx, filePath);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 1", argc);
    return false;
}

bool js_utils_moveDir(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSB_PRECONDITION2(argc == 2, cx, false, "Invalid number of arguments");

    if (argc == 2)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        std::string src;
        jsval_to_std_string(cx, args.get(0), &src);
        std::string dst;
        jsval_to_std_string(cx, args.get(1), &dst);

        Utils::moveDir(src, dst);

        args.rval().set(JSVAL_VOID);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 2", argc);
    return false;
}

bool js_utils_nicai_encode(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSB_PRECONDITION2(argc == 1, cx, false, "Invalid number of arguments");

    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        std::string str;
        jsval_to_std_string(cx, args.get(0), &str);

        std::string encodeStr = Utils::desNicaiAndBase64(str);
        jsval jsret = std_string_to_jsval(cx, encodeStr);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 1", argc);
    return false;
}

bool js_utils_get_device_name(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSB_PRECONDITION2(argc == 0, cx, false, "Invalid number of arguments");

    if (argc == 0)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        std::string name = Utils::getDeviceName();
        jsval jsret = std_string_to_jsval(cx, name);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 0", argc);
    return false;
}

bool js_utils_get_device_uuid(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSB_PRECONDITION2(argc == 0, cx, false, "Invalid number of arguments");

    if (argc == 0)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        std::string uuid = Utils::getDeviceUUID();
        jsval jsret = std_string_to_jsval(cx, uuid);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 0", argc);
    return false;
}

template <typename T>
std::string GenericValue2String(const T& value)
{
    switch (value.GetType())
    {
    case rapidjson::Type::kNullType:
        return "null";
    case rapidjson::Type::kTrueType:
        return "true";
    case rapidjson::Type::kFalseType:
        return "false";
    case rapidjson::Type::kNumberType:
        {
            char buffer[32] = { 0 };
            if (value.IsInt())
            {
                sprintf(buffer, "%i", value.GetInt());
                return buffer;
            }
            else if (value.IsInt64())
            {
                sprintf(buffer, "%lli", value.GetInt64());
                return buffer;
            }
        }
    case rapidjson::Type::kStringType:
        return value.GetString();
    }
    return "";
}

bool js_utils_gen_url_params(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSB_PRECONDITION2(argc == 1, cx, false, "Invalid number of arguments");

    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        std::string params;
        jsval_to_std_string(cx, args.get(0), &params);

        // json object
        rapidjson::Document doc;
        doc.Parse<0>(params.c_str());
        std::map<std::string, std::string> paramsMap;
        for (auto iter = doc.MemberonBegin(); iter != doc.MemberonEnd(); ++iter)
        {
            std::string key = GenericValue2String(iter->name);
            std::string value = GenericValue2String(iter->value);
            if (key == "mac")
                value = Utils::desNicaiAndBase64(value);
            paramsMap[key] = value;
        }

        // code
        std::string strParams;
        for (auto iter = paramsMap.begin(); iter != paramsMap.end(); ++iter)
        {
            if (!strParams.empty())
                strParams.append("&");
            strParams.append(iter->first);
            strParams.append("=");
            strParams.append(iter->second);
        }
        std::string nicai = Utils::desNicaiAndBase64(strParams);
        std::string code = Utils::md5(nicai);

        // url encode
        strParams = "";
        for (auto iter = paramsMap.begin(); iter != paramsMap.end(); ++iter)
        {
            if (!strParams.empty())
                strParams.append("&");
            strParams.append(iter->first);
            strParams.append("=");
            strParams.append(Utils::urlEncode(iter->second));
        }

        // params
        strParams.append("&code=");
        strParams.append(code);

        // return
        jsval jsret = std_string_to_jsval(cx, strParams);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 1", argc);
    return false;
}

bool js_utils_create_node(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSB_PRECONDITION2(argc == 2, cx, false, "Invalid number of arguments");

    if (argc == 2)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        std::string filename;
        jsval_to_std_string(cx, args.get(0), &filename);
        bool fullSize = JS::ToBoolean(args.get(1));

        Node* node = CSLoader::createNode(filename);
        if (node)
        {
            if (fullSize)
            {
                Size frameSize = Director::getInstance()->getVisibleSize();
                node->setContentSize(frameSize);
                ui::Helper::doLayout(node);
            }
            js_proxy_t* jsProxy = js_get_or_create_proxy<cocos2d::Node>(cx, (cocos2d::Node*)node);
            args.rval().set(OBJECT_TO_JSVAL(jsProxy->obj));
        }
        else
        {
            args.rval().set(JSVAL_NULL);
        }
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 2", argc);
    return false;
}

bool js_utils_create_timeline(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSB_PRECONDITION2(argc == 1, cx, false, "Invalid number of arguments");

    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        std::string filename;
        jsval_to_std_string(cx, args.get(0), &filename);

        auto timeline = CSLoader::createTimeline(filename);
        jsval jsret = JSVAL_NULL;
        if (timeline)
        {
            js_proxy_t* jsProxy = js_get_or_create_proxy<cocostudio::timeline::ActionTimeline>(cx, (cocostudio::timeline::ActionTimeline*)timeline);
            jsret = OBJECT_TO_JSVAL(jsProxy->obj);
        }
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 1", argc);
    return false;
}

bool js_utils_remove_timeline(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSB_PRECONDITION2(argc == 1, cx, false, "Invalid number of arguments");

    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        std::string filename;
        jsval_to_std_string(cx, args.get(0), &filename);

        cocostudio::timeline::ActionTimelineCache::getInstance()->removeAction(filename);

        args.rval().set(JSVAL_VOID);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 1", argc);
    return false;
}

bool js_utils_remove_all_timelines(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSB_PRECONDITION2(argc == 0, cx, false, "Invalid number of arguments");

    if (argc == 0)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        cocostudio::timeline::ActionTimelineCache::getInstance()->purge();
        args.rval().set(JSVAL_VOID);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 0", argc);
    return false;
}

void register_jsb_utils(JSContext* cx, JS::HandleObject global)
{
    JS_DefineFunction(cx, global, "_md5", js_utils_md5, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, global, "_fileMD5", js_utils_fileMD5, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, global, "_moveDir", js_utils_moveDir, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, global, "_nicaiEncode", js_utils_nicai_encode, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, global, "_getDeviceName", js_utils_get_device_name, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, global, "_getDeviceUUID", js_utils_get_device_uuid, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, global, "_genUrlParams", js_utils_gen_url_params, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, global, "_createNode", js_utils_create_node, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, global, "_createTimeline", js_utils_create_timeline, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, global, "_removeTimeline", js_utils_remove_timeline, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE);
    JS_DefineFunction(cx, global, "_removeAllTimelines", js_utils_remove_all_timelines, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE);
}
