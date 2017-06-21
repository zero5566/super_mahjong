#include "cocos2d.h"
#include "cocos2d_specifics.hpp"

#include "jsb_tuyoo_sdk.h"
#include "tuyoo_sdk/tuyoo_sdk.h"

JSClass* js_tuyoo_sdk_class;
JSObject* js_tuyoo_sdk_prototype;
const std::string JS_BINDING_NAME = "JSB_TUYOO_SDK";

void TuyooSdk::onComplete(const char* data)
{
    js_proxy_t* p = jsb_get_native_proxy(this);
    if (!p) return;

    JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
    jsval args = c_string_to_jsval(cx, data);
    ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(p->obj), "onComplete", 1, &args);
}

bool jsb_tuyoo_sdk_constructor(JSContext* cx, uint32_t argc, jsval* vp)
{
    return false;
}

void js_tuyoo_sdk_finalize(JSFreeOp* fop, JSObject* obj)
{
    CCLOG("jsbindings: finalizing JS object %p (%s)", obj, JS_BINDING_NAME.c_str());
}

bool js_tuyoo_sdk_getInstance(JSContext* cx, uint32_t argc, jsval* vp)
{
    if (argc == 0)
    {
        TuyooSdk* ret = TuyooSdk::getInstance();
        js_proxy_t* jsProxy = js_get_or_create_proxy<TuyooSdk>(cx, (TuyooSdk*)ret);
        jsval jsret = OBJECT_TO_JSVAL(jsProxy->obj);

        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        args.rval().set(jsret);
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 0", argc);
    return false;
}

bool js_tuyoo_sdk_initTuyooSDK(JSContext* cx, uint32_t argc, jsval* vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t* proxy = jsb_get_js_proxy(obj);
    TuyooSdk* cobj = (TuyooSdk*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 3)
    {
        int appId;
        jsval_to_int32(cx, args.get(0), &appId);
        std::string clientId;
        jsval_to_std_string(cx, args.get(1), &clientId);
        std::string serverUrl;
        jsval_to_std_string(cx, args.get(2), &serverUrl);

        cobj->initTuyooSDK(appId, clientId, serverUrl);

        args.rval().set(JSVAL_VOID);
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 3", argc);
    return false;
}

bool js_tuyoo_sdk_quickStart(JSContext* cx, uint32_t argc, jsval* vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t* proxy = jsb_get_js_proxy(obj);
    TuyooSdk* cobj = (TuyooSdk*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 0)
    {
        cobj->quickStart();

        args.rval().set(JSVAL_VOID);
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 0", argc);
    return false;
}

bool js_tuyoo_sdk_loginByPhoneWithParent(JSContext* cx, uint32_t argc, jsval* vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t* proxy = jsb_get_js_proxy(obj);
    TuyooSdk* cobj = (TuyooSdk*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 0)
    {
        cobj->loginByPhoneWithParent();

        args.rval().set(JSVAL_VOID);
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 0", argc);
    return false;
}

bool js_tuyoo_sdk_setPlatformUserInfoByNickName(JSContext* cx, uint32_t argc, jsval* vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t* proxy = jsb_get_js_proxy(obj);
    TuyooSdk* cobj = (TuyooSdk*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 2)
    {
        std::string nickName;
        jsval_to_std_string(cx, args.get(0), &nickName);
        std::string sex;
        jsval_to_std_string(cx, args.get(1), &sex);

        cobj->setPlatformUserInfoByNickName(nickName, sex);

        args.rval().set(JSVAL_VOID);
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 2", argc);
    return false;
}

bool js_tuyoo_sdk_setUserAvatarByPrePicUrl(JSContext* cx, uint32_t argc, jsval* vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t* proxy = jsb_get_js_proxy(obj);
    TuyooSdk* cobj = (TuyooSdk*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 1)
    {
        std::string picUrl;
        jsval_to_std_string(cx, args.get(0), &picUrl);

        cobj->setUserAvatarByPrePicUrl(picUrl);

        args.rval().set(JSVAL_VOID);
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 1", argc);
    return false;
}

bool js_tuyoo_sdk_loadAppStoreWithProductIds(JSContext* cx, uint32_t argc, jsval* vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t* proxy = jsb_get_js_proxy(obj);
    TuyooSdk* cobj = (TuyooSdk*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 2)
    {
        std::vector<std::string> pids;
        jsval_to_std_vector_string(cx, args.get(0), &pids);
        std::string appInfo;
        jsval_to_std_string(cx, args.get(1), &appInfo);

        cobj->loadAppStoreWithProductIds(pids, appInfo);

        args.rval().set(JSVAL_VOID);
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 2", argc);
    return false;
}

bool js_tuyoo_sdk_buyCoinDirect(JSContext* cx, uint32_t argc, jsval* vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t* proxy = jsb_get_js_proxy(obj);
    TuyooSdk* cobj = (TuyooSdk*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 7)
    {
        std::string prodId;
        jsval_to_std_string(cx, args.get(0), &prodId);
        std::string prodName;
        jsval_to_std_string(cx, args.get(1), &prodName);
        int prodCount;
        jsval_to_int32(cx, args.get(2), &prodCount);
        int prodPrice;
        jsval_to_int32(cx, args.get(3), &prodPrice);
        std::string appInfo;
        jsval_to_std_string(cx, args.get(4), &appInfo);
        std::string prodOrderId;
        jsval_to_std_string(cx, args.get(5), &prodOrderId);
        int otherPayStatus;
        jsval_to_int32(cx, args.get(6), &otherPayStatus);

        cobj->buyCoinDirect(prodId, prodName, prodCount, prodPrice, appInfo, prodOrderId, otherPayStatus);

        args.rval().set(JSVAL_VOID);
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 7", argc);
    return false;
}

bool js_tuyoo_sdk_payDiamondWithParent(JSContext* cx, uint32_t argc, jsval* vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t* proxy = jsb_get_js_proxy(obj);
    TuyooSdk* cobj = (TuyooSdk*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 7)
    {
        std::string diamondId;
        jsval_to_std_string(cx, args.get(0), &diamondId);
        std::string diamondName;
        jsval_to_std_string(cx, args.get(1), &diamondName);
        int diamondCount;
        jsval_to_int32(cx, args.get(2), &diamondCount);
        int diamondPrice;
        jsval_to_int32(cx, args.get(3), &diamondPrice);
        std::string appInfo;
        jsval_to_std_string(cx, args.get(4), &appInfo);
        std::string prodOrderId;
        jsval_to_std_string(cx, args.get(5), &prodOrderId);
        int otherPayStatus;
        jsval_to_int32(cx, args.get(6), &otherPayStatus);

        cobj->payDiamondWithParent(diamondId, diamondName, diamondCount, diamondPrice, appInfo, prodOrderId, otherPayStatus);

        args.rval().set(JSVAL_VOID);
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 7", argc);
    return false;
}

void register_jsb_tuyoo_sdk(JSContext* cx, JS::HandleObject global)
{
    js_tuyoo_sdk_class = (JSClass*)calloc(1, sizeof(JSClass));
    js_tuyoo_sdk_class->name = JS_BINDING_NAME.c_str();
    js_tuyoo_sdk_class->addProperty = JS_PropertyStub;
    js_tuyoo_sdk_class->delProperty = JS_DeletePropertyStub;
    js_tuyoo_sdk_class->getProperty = JS_PropertyStub;
    js_tuyoo_sdk_class->setProperty = JS_StrictPropertyStub;
    js_tuyoo_sdk_class->enumerate = JS_EnumerateStub;
    js_tuyoo_sdk_class->resolve = JS_ResolveStub;
    js_tuyoo_sdk_class->convert = JS_ConvertStub;
    js_tuyoo_sdk_class->finalize = js_tuyoo_sdk_finalize;
    js_tuyoo_sdk_class->flags = JSCLASS_HAS_RESERVED_SLOTS(2);

    // Properties
    static JSPropertySpec properties[] =
    {
        { 0, 0, 0, 0, 0 }
    };

    // Functions
    static JSFunctionSpec funcs[] =
    {
        JS_FN("initTuyooSDK", js_tuyoo_sdk_initTuyooSDK, 3, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("quickStart", js_tuyoo_sdk_quickStart, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("loginByPhoneWithParent", js_tuyoo_sdk_loginByPhoneWithParent, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("setPlatformUserInfoByNickName", js_tuyoo_sdk_setPlatformUserInfoByNickName, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("setUserAvatarByPrePicUrl", js_tuyoo_sdk_setUserAvatarByPrePicUrl, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("loadAppStoreWithProductIds", js_tuyoo_sdk_loadAppStoreWithProductIds, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("buyCoinDirect", js_tuyoo_sdk_buyCoinDirect, 7, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("payDiamondWithParent", js_tuyoo_sdk_payDiamondWithParent, 7, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FS_END
    };

    // static Functions
    static JSFunctionSpec st_funcs[] =
    {
        JS_FN("getInstance", js_tuyoo_sdk_getInstance, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FS_END
    };

    js_tuyoo_sdk_prototype = JS_InitClass(
        cx,
        global,
        JS::NullPtr(),
        js_tuyoo_sdk_class,
        jsb_tuyoo_sdk_constructor,
        0,
        properties,
        funcs,
        nullptr,
        st_funcs);

    // add the proto and JSClass to the type->js info hash table
    TypeTest<TuyooSdk> t;
    js_type_class_t* p;
    std::string typeName = t.s_name();
    if (_js_global_type_map.find(typeName) == _js_global_type_map.end())
    {
        p = (js_type_class_t *)malloc(sizeof(js_type_class_t));
        p->jsclass = js_tuyoo_sdk_class;
        p->proto = js_tuyoo_sdk_prototype;
        p->parentProto = NULL;
        _js_global_type_map.insert(std::make_pair(typeName, p));
    }
}
