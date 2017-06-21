#ifndef __TUYOO_SDK_H__
#define __TUYOO_SDK_H__

#include <string>
#include <vector>

class TuyooSdk
{
public:
    static TuyooSdk* getInstance();
    void onComplete(const char* data);

    void initTuyooSDK(int appId, std::string clientId, std::string serverUrl);
    void quickStart();
    void loginByPhoneWithParent();
    void setPlatformUserInfoByNickName(std::string nickName, std::string sex);
    void setUserAvatarByPrePicUrl(std::string picUrl);
    void loadAppStoreWithProductIds(std::vector<std::string> pids, std::string appInfo);
    void buyCoinDirect(std::string prodId, std::string prodName, int prodCount, int prodPrice, std::string appInfo, std::string prodOrderId, int otherPayStatus);
    void payDiamondWithParent(std::string diamondId, std::string diamondName, int diamondCount, int diamondPrice, std::string appInfo, std::string prodOrderId, int otherPayStatus);
};

#endif // __TUYOO_SDK_H__
