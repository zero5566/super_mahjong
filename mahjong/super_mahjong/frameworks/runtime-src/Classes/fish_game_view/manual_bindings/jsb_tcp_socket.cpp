#include "cocos2d.h"
#include "cocos2d_specifics.hpp"

#include "jsb_tcp_socket.h"
#include "tcp_socket/tcp_socket.h"

JSClass* js_tcp_socket_class;
JSObject* js_tcp_socket_prototype;
const std::string JS_BINDING_NAME = "JSB_TCP_SOCKET";

void TCPSocket::onConnected(int seed)
{
    js_proxy_t* p = jsb_get_native_proxy(this);
    if (!p) return;

    JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
    jsval args = int32_to_jsval(cx, seed);
    ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(p->obj), "onConnected", 1, &args);
}

void TCPSocket::onMessage(const char* msg)
{
    js_proxy_t* p = jsb_get_native_proxy(this);
    if (!p) return;

    JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
    jsval args;
    if (nullptr == msg || strlen(msg) == 0)
        args = JSVAL_NULL;
    else
        args = c_string_to_jsval(cx, msg);
    ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(p->obj), "onMessage", 1, &args);
}

void TCPSocket::onClosed()
{
    js_proxy_t* p = jsb_get_native_proxy(this);
    if (!p) return;

    jsval args = JSVAL_NULL;
    ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(p->obj), "onClose", 0, &args);
}

bool jsb_tcp_socket_constructor(JSContext* cx, uint32_t argc, jsval* vp)
{
    return false;
}

void js_tcp_socket_finalize(JSFreeOp* fop, JSObject* obj)
{
    CCLOG("jsbindings: finalizing JS object %p (%s)", obj, JS_BINDING_NAME.c_str());
}

bool js_tcp_socket_create(JSContext* cx, uint32_t argc, jsval* vp)
{
    if (argc == 0)
    {
        TCPSocket* ret = new TCPSocket;
        js_proxy_t* jsProxy = js_get_or_create_proxy<TCPSocket>(cx, (TCPSocket*)ret);
        jsval jsret = OBJECT_TO_JSVAL(jsProxy->obj);

        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        args.rval().set(jsret);
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 0", argc);
    return false;
}

bool js_tcp_socket_connect(JSContext* cx, uint32_t argc, jsval* vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t* proxy = jsb_get_js_proxy(obj);
    TCPSocket* cobj = (TCPSocket*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 2)
    {
        std::string strIP;
        jsval_to_std_string(cx, args.get(0), &strIP);
        int port;
        jsval_to_int32(cx, args.get(1), &port);
        cobj->connect(strIP.c_str(), port);
        args.rval().set(JSVAL_VOID);
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 2", argc);
    return false;
}

bool js_tcp_socket_send(JSContext* cx, uint32_t argc, jsval* vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t* proxy = jsb_get_js_proxy(obj);
    TCPSocket* cobj = (TCPSocket*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 1)
    {
        std::string strMsg;
        jsval_to_std_string(cx, args.get(0), &strMsg);
        cobj->sendMsg(strMsg.c_str());
        args.rval().set(JSVAL_VOID);
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 1", argc);
    return false;
}

bool js_tcp_socket_close(JSContext* cx, uint32_t argc, jsval* vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    JS::RootedObject obj(cx, args.thisv().toObjectOrNull());
    js_proxy_t* proxy = jsb_get_js_proxy(obj);
    TCPSocket* cobj = (TCPSocket*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 0)
    {
        cobj->close();
        args.rval().set(JSVAL_VOID);
        return true;
    }
    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 0", argc);
    return false;
}

void register_jsb_tcp_socket(JSContext* cx, JS::HandleObject global)
{
    js_tcp_socket_class = (JSClass*)calloc(1, sizeof(JSClass));
    js_tcp_socket_class->name = JS_BINDING_NAME.c_str();
    js_tcp_socket_class->addProperty = JS_PropertyStub;
    js_tcp_socket_class->delProperty = JS_DeletePropertyStub;
    js_tcp_socket_class->getProperty = JS_PropertyStub;
    js_tcp_socket_class->setProperty = JS_StrictPropertyStub;
    js_tcp_socket_class->enumerate = JS_EnumerateStub;
    js_tcp_socket_class->resolve = JS_ResolveStub;
    js_tcp_socket_class->convert = JS_ConvertStub;
    js_tcp_socket_class->finalize = js_tcp_socket_finalize;
    js_tcp_socket_class->flags = JSCLASS_HAS_RESERVED_SLOTS(2);

    // Properties
    static JSPropertySpec properties[] =
    {
        { 0, 0, 0, 0, 0 }
    };

    // Functions
    static JSFunctionSpec funcs[] =
    {
        JS_FN("connect", js_tcp_socket_connect, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("send", js_tcp_socket_send, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("close", js_tcp_socket_close, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FS_END
    };

    // static Functions
    static JSFunctionSpec st_funcs[] =
    {
        JS_FN("create", js_tcp_socket_create, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FS_END
    };

    js_tcp_socket_prototype = JS_InitClass(
        cx,
        global,
        JS::NullPtr(),
        js_tcp_socket_class,
        jsb_tcp_socket_constructor,
        0,
        properties,
        funcs,
        nullptr,
        st_funcs);

    // add the proto and JSClass to the type->js info hash table
    TypeTest<TCPSocket> t;
    js_type_class_t* p;
    std::string typeName = t.s_name();
    if (_js_global_type_map.find(typeName) == _js_global_type_map.end())
    {
        p = (js_type_class_t *)malloc(sizeof(js_type_class_t));
        p->jsclass = js_tcp_socket_class;
        p->proto = js_tcp_socket_prototype;
        p->parentProto = NULL;
        _js_global_type_map.insert(std::make_pair(typeName, p));
    }
}
