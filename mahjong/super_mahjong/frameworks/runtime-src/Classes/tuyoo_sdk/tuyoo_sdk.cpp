#include "cocos2d.h"
#include "tuyoo_sdk.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "AndroidUtils.h"
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
extern void iosInitTuyooSDK(int appId, std::string clientId, std::string serverUrl);
extern void iosQuickStart();
extern void iosLoginByPhoneWithParent();
extern void iosSetPlatformUserInfoByNickName(std::string nickName, std::string sex);
extern void iosSetUserAvatarByPrePicUrl(std::string picUrl);
extern void iosLoadAppStoreWithProductIds(std::vector<std::string> pids, std::string appInfo);
extern void iosBuyCoinDirect(std::string prodId, std::string prodName, int prodCount, int prodPrice, std::string appInfo, std::string prodOrderId, int otherPayStatus);
extern void iosPayDiamondWithParent(std::string diamondId, std::string diamondName, int diamondCount, int diamondPrice, std::string appInfo, std::string prodOrderId, int otherPayStatus);
#endif

TuyooSdk* TuyooSdk::getInstance()
{
    static TuyooSdk inst;
    return &inst;
}

void TuyooSdk::initTuyooSDK(int appId, std::string clientId, std::string serverUrl)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    //iosInitTuyooSDK(appId, clientId, serverUrl);
#endif
}

void TuyooSdk::quickStart()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    //iosQuickStart();
#endif
}

void TuyooSdk::loginByPhoneWithParent()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    //iosLoginByPhoneWithParent();
#endif
}

void TuyooSdk::setPlatformUserInfoByNickName(std::string nickName, std::string sex)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    //iosSetPlatformUserInfoByNickName(nickName, sex);
#endif
}

void TuyooSdk::setUserAvatarByPrePicUrl(std::string picUrl)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    //iosSetUserAvatarByPrePicUrl(picUrl);
#endif
}

void TuyooSdk::loadAppStoreWithProductIds(std::vector<std::string> pids, std::string appInfo)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    //iosLoadAppStoreWithProductIds(pids, appInfo);
#endif
}

void TuyooSdk::buyCoinDirect(std::string prodId, std::string prodName, int prodCount, int prodPrice, std::string appInfo, std::string prodOrderId, int otherPayStatus)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    //iosBuyCoinDirect(prodId, prodName, prodCount, prodPrice, appInfo, prodOrderId, otherPayStatus);
#endif
}

void TuyooSdk::payDiamondWithParent(std::string diamondId, std::string diamondName, int diamondCount, int diamondPrice, std::string appInfo, std::string prodOrderId, int otherPayStatus)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)

#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    //iosPayDiamondWithParent(diamondId, diamondName, diamondCount, diamondPrice, appInfo, prodOrderId, otherPayStatus);
#endif
}
