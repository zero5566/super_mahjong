#include "cocos2d.h"
#include "scripting/js-bindings/manual/cocos2d_specifics.hpp"

#include "jsb_fish_game_view.h"
#include "fish_game_view.h"

using namespace cocos2d;
using namespace FishGameView;

JSClass* js_fish_game_view_class;
JSObject* js_fish_game_view_prototype;
const std::string JS_BINDING_NAME = "JSB_FISH_GAME_VIEW";

void View::onBulletHitFish(int seatId, int bulletId, int fishId)
{
    js_proxy_t* p = jsb_get_native_proxy(this);
    if (!p) return;

    JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();

    jsval args[3];
    args[0] = int32_to_jsval(cx, seatId);
    args[1] = int32_to_jsval(cx, bulletId);
    args[2] = int32_to_jsval(cx, fishId);

    ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(p->obj), "onBulletHitFish", 3, args);
}

void View::onBulletHitPiton(int seatId, int bulletId, int pitonId)
{
    js_proxy_t* p = jsb_get_native_proxy(this);
    if (!p) return;

    JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();

    jsval args[3];
    args[0] = int32_to_jsval(cx, seatId);
    args[1] = int32_to_jsval(cx, bulletId);
    args[2] = int32_to_jsval(cx, pitonId);

    ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(p->obj), "onBulletHitPiton", 3, args);
}

void View::onFishTurn(int fishId)
{
    js_proxy_t* p = jsb_get_native_proxy(this);
    if (!p) return;

    JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
    jsval jsret = int32_to_jsval(cx, fishId);

    ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(p->obj), "onFishTurn", 1, &jsret);
}

void View::onFishOutScreen(int fishId)
{
    js_proxy_t* p = jsb_get_native_proxy(this);
    if (!p) return;

    JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
    jsval jsret = int32_to_jsval(cx, fishId);

    ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(p->obj), "onFishOutScreen", 1, &jsret);
}

void View::onFishExit(int fishId)
{
    js_proxy_t* p = jsb_get_native_proxy(this);
    if (!p) return;

    JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();
    jsval jsret = int32_to_jsval(cx, fishId);

    ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(p->obj), "onFishExit", 1, &jsret);
}

void View::onLaserUpdate(int seatId, int line, float distance, float x, float y)
{
    js_proxy_t* p = jsb_get_native_proxy(this);
    if (!p) return;

    JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();

    jsval args[5];
    args[0] = int32_to_jsval(cx, seatId);
    args[1] = int32_to_jsval(cx, line);
    args[2] = DOUBLE_TO_JSVAL(distance);
    args[3] = DOUBLE_TO_JSVAL(x);
    args[4] = DOUBLE_TO_JSVAL(y);

    ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(p->obj), "onLaserUpdate", 5, args);
}

void View::onLaserHitFish(int seatId, int fishId, const Vec2& ptCollide)
{
    js_proxy_t* p = jsb_get_native_proxy(this);
    if (!p) return;

    JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();

    jsval args[4];
    args[0] = int32_to_jsval(cx, seatId);
    args[1] = int32_to_jsval(cx, fishId);
    args[2] = DOUBLE_TO_JSVAL(ptCollide.x);
    args[3] = DOUBLE_TO_JSVAL(ptCollide.y);

    ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(p->obj), "onLaserHitFish", 4, args);
}

void View::onPitonHit(int hitBattery, int pitonId)
{
    js_proxy_t* p = jsb_get_native_proxy(this);
    if (!p) return;

    JSContext* cx = ScriptingCore::getInstance()->getGlobalContext();

    jsval args[2];
    args[0] = int32_to_jsval(cx, hitBattery);
    args[1] = int32_to_jsval(cx, pitonId);

    ScriptingCore::getInstance()->executeFunctionWithOwner(OBJECT_TO_JSVAL(p->obj), "onPitonHitBattery", 2, args);
}

bool js_fish_game_view_constructor(JSContext* cx, uint32_t argc, jsval* vp)
{
    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
    bool ok = true;
    View* cobj = View::create();
    cocos2d::Ref *_ccobj = dynamic_cast<cocos2d::Ref *>(cobj);
    if (_ccobj) {
        _ccobj->autorelease();
    }
    TypeTest<View> t;
    js_type_class_t *typeClass = nullptr;
    std::string typeName = t.s_name();
    auto typeMapIter = _js_global_type_map.find(typeName);
    CCASSERT(typeMapIter != _js_global_type_map.end(), "Can't find the class type!");
    typeClass = typeMapIter->second;
    CCASSERT(typeClass, "The value is null.");
    // JSObject *obj = JS_NewObject(cx, typeClass->jsclass, typeClass->proto, typeClass->parentProto);
    JS::RootedObject proto(cx, typeClass->proto.get());
    JS::RootedObject parent(cx, typeClass->parentProto.get());
    JS::RootedObject obj(cx, JS_NewObject(cx, typeClass->jsclass, proto, parent));
    args.rval().set(OBJECT_TO_JSVAL(obj));
    // link the native object with the javascript object
    js_proxy_t* p = jsb_new_proxy(cobj, obj);
    AddNamedObjectRoot(cx, &p->obj, "View");
    return true;
}

void js_fish_game_view_finalize(JSFreeOp* fop, JSObject* obj)
{
    CCLOG("jsbindings: finalizing JS object %p (%s)", obj, JS_BINDING_NAME.c_str());
}

bool js_fish_game_view_setServerTime(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    JS::CallArgs args = JS::CallArgsFromVp(argc, vp);

    if (argc == 2)
    {
        double serverNowTime = 0;
        JS::ToNumber(cx, args.get(0), &serverNowTime);
        double serverStartTime = 0;
        JS::ToNumber(cx, args.get(1), &serverStartTime);

        cobj->setServerTime(serverNowTime, serverStartTime);

        args.rval().set(JSVAL_VOID);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 2", argc);
    return false;
}

bool js_fish_game_view_setMySeat(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int seatId;
        jsval_to_int32(cx, args.get(0), &seatId);

        cobj->setMySeat(seatId);

        args.rval().set(JSVAL_VOID);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 1", argc);
    return false;
}

bool js_fish_game_view_getEffectLayer(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 0)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);

        cocos2d::Node* node = cobj->getEffectLayer();

        jsval jsret = JSVAL_NULL;
        if (node != nullptr)
        {
            js_proxy_t* jsProxy = js_get_or_create_proxy<cocos2d::Node>(cx, (cocos2d::Node*)node);
            jsret = OBJECT_TO_JSVAL(jsProxy->obj);
        }
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 0", argc);
    return false;
}

bool js_fish_game_view_playerFire(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 4)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int seatId;
        jsval_to_int32(cx, args.get(0), &seatId);
        int bulletId;
        jsval_to_int32(cx, args.get(1), &bulletId);
        int weaponId;
        jsval_to_int32(cx, args.get(2), &weaponId);

        int fishId;
        Bullet* bullet = nullptr;
        if (jsval_to_int32(cx, args.get(3), &fishId))
        {
            bullet = cobj->playerFire(seatId, bulletId, weaponId, Vec2::ZERO, fishId);
        }
        else
        {
            Vec2 pt;
            jsval_to_vector2(cx, args.get(3), &pt);
            bullet = cobj->playerFire(seatId, bulletId, weaponId, pt, 0);
        }

        jsval jsret = JSVAL_NULL;
        if (bullet != nullptr)
        {
            js_proxy_t* jsProxy = js_get_or_create_proxy<cocos2d::Node>(cx, (cocos2d::Node*)bullet);
            jsret = OBJECT_TO_JSVAL(jsProxy->obj);
        }
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 4", argc);
    return false;
}

bool js_fish_game_view_getBulletPos(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 2)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int seatId;
        jsval_to_int32(cx, args.get(0), &seatId);
        int bulletId;
        jsval_to_int32(cx, args.get(1), &bulletId);

        Vec2 pt = cobj->getBulletPos(seatId, bulletId);

        jsval jsret = vector2_to_jsval(cx, pt);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 2", argc);
    return false;
}

bool js_fish_game_view_bulletDie(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 2)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int seatId;
        jsval_to_int32(cx, args.get(0), &seatId);
        int bulletId;
        jsval_to_int32(cx, args.get(1), &bulletId);

        cobj->bulletDie(seatId, bulletId);

        args.rval().set(JSVAL_VOID);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 2", argc);
    return false;
}

bool js_fish_game_view_fishEnter(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 5)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int fishResId;
        jsval_to_int32(cx, args.get(0), &fishResId);
        int fishId;
        jsval_to_int32(cx, args.get(1), &fishId);
        int pathId;
        jsval_to_int32(cx, args.get(2), &pathId);
        double enterTime = 0;
        JS::ToNumber(cx, args.get(3), &enterTime);
        double frozenTime = 0;
        JS::ToNumber(cx, args.get(4), &frozenTime);

        Fish* fish = cobj->fishEnter(fishResId, fishId, pathId, enterTime, frozenTime);

        jsval jsret = JSVAL_NULL;
        if (fish != nullptr)
        {
            js_proxy_t* jsProxy = js_get_or_create_proxy<cocos2d::Node>(cx, (cocos2d::Node*)fish);
            jsret = OBJECT_TO_JSVAL(jsProxy->obj);
        }
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 5", argc);
    return false;
}

bool js_fish_game_view_getFishPos(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int fishId;
        jsval_to_int32(cx, args.get(0), &fishId);

        Vec2 pt = cobj->getFishPos(fishId);

        jsval jsret = vector2_to_jsval(cx, pt);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 1", argc);
    return false;
}

bool js_fish_game_view_fishDie(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int fishId;
        if (jsval_to_int32(cx, args.get(0), &fishId))
        {
            cobj->fishDie(fishId);
        }
        else
        {
            std::vector<int> fishes;
            jsval_to_std_vector_int(cx, args.get(0), &fishes);
            cobj->fishDie(fishes);
        }

        args.rval().set(JSVAL_VOID);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 1", argc);
    return false;
}

bool js_fish_game_view_getFishAtPosition(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        Vec2 pt;
        jsval_to_vector2(cx, args.get(0), &pt);

        int fishId = cobj->getFishAtPosition(pt);

        jsval jsret = int32_to_jsval(cx, fishId);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 1", argc);
    return false;
}

bool js_fish_game_view_getFishesInRange(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 2)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        Vec2 center;
        jsval_to_vector2(cx, args.get(0), &center);
        double radius = 0;
        JS::ToNumber(cx, args.get(1), &radius);

        std::vector<int> vec;
        cobj->getFishesInRange(vec, center, radius);

        jsval jsret = std_vector_int_to_jsval(cx, vec);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 2", argc);
    return false;
}

bool js_fish_game_view_getNormalFishesInRange(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 2)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        Vec2 center;
        jsval_to_vector2(cx, args.get(0), &center);
        double radius = 0;
        JS::ToNumber(cx, args.get(1), &radius);

        std::vector<int> vec;
        cobj->getNormalFishesInRange(vec, center, radius);

        jsval jsret = std_vector_int_to_jsval(cx, vec);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 2", argc);
    return false;
}

bool js_fish_game_view_getNearbyFishes(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 2)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        Vec2 center;
        jsval_to_vector2(cx, args.get(0), &center);
        int count = 0;
        jsval_to_int32(cx, args.get(1), &count);

        std::vector<int> vec;
        cobj->getNearbyFishes(vec, center, count);

        jsval jsret = std_vector_int_to_jsval(cx, vec);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 2", argc);
    return false;
}

bool js_fish_game_view_getRandomFishes(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 2)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int fishId = 0;
        jsval_to_int32(cx, args.get(0), &fishId);
        int count = 0;
        jsval_to_int32(cx, args.get(1), &count);

        std::vector<int> vec;
        cobj->getRandomFishes(vec, fishId, count);

        jsval jsret = std_vector_int_to_jsval(cx, vec);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 2", argc);
    return false;
}

bool js_fish_game_view_freeze(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        double frozenTime = 0;
        JS::ToNumber(cx, args.get(0), &frozenTime);

        cobj->freeze(frozenTime);

        args.rval().set(JSVAL_VOID);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 1", argc);
    return false;
}

bool js_fish_game_view_unfreeze(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 2)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        double frozenTime = 0;
        JS::ToNumber(cx, args.get(0), &frozenTime);
        double unfrozenTime = 0;
        JS::ToNumber(cx, args.get(1), &unfrozenTime);

        cobj->unfreeze(frozenTime, unfrozenTime);

        args.rval().set(JSVAL_VOID);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 2", argc);
    return false;
}

bool js_fish_game_view_laserBegin(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 4)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int seatId;
        jsval_to_int32(cx, args.get(0), &seatId);
        int weaponId;
        jsval_to_int32(cx, args.get(1), &weaponId);
        int maxLine;
        jsval_to_int32(cx, args.get(2), &maxLine);
        Vec2 pt;
        jsval_to_vector2(cx, args.get(3), &pt);

        Laser* laser = cobj->laserBegin(seatId, weaponId, maxLine, pt);

        jsval jsret = JSVAL_NULL;
        if (laser != nullptr)
        {
            js_proxy_t* jsProxy = js_get_or_create_proxy<cocos2d::Node>(cx, (cocos2d::Node*)laser);
            jsret = OBJECT_TO_JSVAL(jsProxy->obj);
        }
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 4", argc);
    return false;
}

bool js_fish_game_view_getLaserPath(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int seatId;
        jsval_to_int32(cx, args.get(0), &seatId);

        std::vector<cocos2d::Vec2> path;
        cobj->getLaserPath(seatId, path);

        jsval jsret = vector_vec2_to_jsval(cx, path);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 1", argc);
    return false;
}

bool js_fish_game_view_getLaserAngles(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int seatId;
        jsval_to_int32(cx, args.get(0), &seatId);

        std::vector<float> angles;
        cobj->getLaserAngles(seatId, angles);

        jsval jsret = std_vector_float_to_jsval(cx, angles);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 1", argc);
    return false;
}

bool js_fish_game_view_getLaserDists(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int seatId;
        jsval_to_int32(cx, args.get(0), &seatId);

        std::vector<float> dists;
        cobj->getLaserDists(seatId, dists);

        jsval jsret = std_vector_float_to_jsval(cx, dists);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 1", argc);
    return false;
}

bool js_fish_game_view_laserEnd(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int seatId;
        jsval_to_int32(cx, args.get(0), &seatId);

        cobj->laserEnd(seatId);

        args.rval().set(JSVAL_VOID);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 1", argc);
    return false;
}

bool js_fish_game_view_castIcePiton(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 2)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int pitonId;
        jsval_to_int32(cx, args.get(0), &pitonId);
        double angle = 0;
        JS::ToNumber(cx, args.get(1), &angle);

        Piton* piton = cobj->castIcePiton(pitonId, angle);

        jsval jsret = JSVAL_NULL;
        if (piton != nullptr)
        {
            js_proxy_t* jsProxy = js_get_or_create_proxy<cocos2d::Node>(cx, (cocos2d::Node*)piton);
            jsret = OBJECT_TO_JSVAL(jsProxy->obj);
        }
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 2", argc);
    return false;
}

bool js_fish_game_view_getPitonPos(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int pitonId;
        jsval_to_int32(cx, args.get(0), &pitonId);

        Vec2 pt = cobj->getPitonPos(pitonId);

        jsval jsret = vector2_to_jsval(cx, pt);
        args.rval().set(jsret);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 1", argc);
    return false;
}

bool js_fish_game_view_pitonDie(JSContext* cx, uint32_t argc, jsval* vp)
{
    JSObject* obj_this = JS_THIS_OBJECT(cx, vp);
    js_proxy_t* proxy = jsb_get_js_proxy(obj_this);
    View* cobj = (View*)(proxy ? proxy->ptr : nullptr);
    JSB_PRECONDITION2(cobj, cx, false, "Invalid Native Object");

    if (argc == 1)
    {
        JS::CallArgs args = JS::CallArgsFromVp(argc, vp);
        int pitonId;
        jsval_to_int32(cx, args.get(0), &pitonId);

        cobj->pitonDie(pitonId);

        args.rval().set(JSVAL_VOID);
        return true;
    }

    JS_ReportError(cx, "wrong number of arguments: %d, was expecting 1", argc);
    return false;
}

void register_jsb_fish_game_view(JSContext* cx, JS::HandleObject global)
{
    js_fish_game_view_class = (JSClass*)calloc(1, sizeof(JSClass));
    js_fish_game_view_class->name = JS_BINDING_NAME.c_str();
    js_fish_game_view_class->addProperty = JS_PropertyStub;
    js_fish_game_view_class->delProperty = JS_DeletePropertyStub;
    js_fish_game_view_class->getProperty = JS_PropertyStub;
    js_fish_game_view_class->setProperty = JS_StrictPropertyStub;
    js_fish_game_view_class->enumerate = JS_EnumerateStub;
    js_fish_game_view_class->resolve = JS_ResolveStub;
    js_fish_game_view_class->convert = JS_ConvertStub;
    js_fish_game_view_class->finalize = js_fish_game_view_finalize;
    js_fish_game_view_class->flags = JSCLASS_HAS_RESERVED_SLOTS(2);

    // Properties
    static JSPropertySpec properties[] =
    {
        { 0, 0, 0, 0, 0 }
    };

    // Functions
    static JSFunctionSpec funcs[] =
    {
        // Common
        JS_FN("setServerTime", js_fish_game_view_setServerTime, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("setMySeat", js_fish_game_view_setMySeat, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("getEffectLayer", js_fish_game_view_getEffectLayer, 0, JSPROP_PERMANENT | JSPROP_ENUMERATE),

        // Bullet
        JS_FN("playerFire", js_fish_game_view_playerFire, 4, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("getBulletPos", js_fish_game_view_getBulletPos, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("bulletDie", js_fish_game_view_bulletDie, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),

        // Fish
        JS_FN("fishEnter", js_fish_game_view_fishEnter, 5, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("getFishPos", js_fish_game_view_getFishPos, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("fishDie", js_fish_game_view_fishDie, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("getFishAtPosition", js_fish_game_view_getFishAtPosition, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("getFishesInRange", js_fish_game_view_getFishesInRange, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("getNormalFishesInRange", js_fish_game_view_getNormalFishesInRange, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("getNearbyFishes", js_fish_game_view_getNearbyFishes, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("getRandomFishes", js_fish_game_view_getRandomFishes, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),

        // Freeze
        JS_FN("freeze", js_fish_game_view_freeze, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("unfreeze", js_fish_game_view_unfreeze, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),

        // Laser
        JS_FN("laserBegin", js_fish_game_view_laserBegin, 4, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("laserEnd", js_fish_game_view_laserEnd, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("getLaserPath", js_fish_game_view_getLaserPath, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("getLaserDists", js_fish_game_view_getLaserDists, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("getLaserAngles", js_fish_game_view_getLaserAngles, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),

        // Piton
        JS_FN("castIcePiton", js_fish_game_view_castIcePiton, 2, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("getPitonPos", js_fish_game_view_getPitonPos, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FN("pitonDie", js_fish_game_view_pitonDie, 1, JSPROP_PERMANENT | JSPROP_ENUMERATE),
        JS_FS_END
    };

    // static Functions
    static JSFunctionSpec st_funcs[] =
    {
        JS_FS_END
    };

    TypeTest<cocos2d::Node> t1;
    js_type_class_t *typeClass = nullptr;
    std::string typeName = t1.s_name();
    auto typeMapIter = _js_global_type_map.find(typeName);
    CCASSERT(typeMapIter != _js_global_type_map.end(), "Can't find the class type!");
    typeClass = typeMapIter->second;
    CCASSERT(typeClass, "The value is null.");

    js_fish_game_view_prototype = JS_InitClass(
        cx,
        global,
        JS::NullPtr(),
        js_fish_game_view_class,
        js_fish_game_view_constructor,
        0,
        properties,
        funcs,
        nullptr,
        st_funcs);

    // add the proto and JSClass to the type->js info hash table
    TypeTest<View> t;
    js_type_class_t* p;
    typeName = t.s_name();
    if (_js_global_type_map.find(typeName) == _js_global_type_map.end())
    {
        p = (js_type_class_t*)malloc(sizeof(js_type_class_t));
        p->jsclass = js_fish_game_view_class;
        p->proto = js_fish_game_view_prototype;
        p->parentProto = typeClass->proto;
        _js_global_type_map.insert(std::make_pair(typeName, p));
    }
}
