#ifndef __TCP_SOCKET_H__
#define __TCP_SOCKET_H__

class TCPSocket : public cocos2d::Ref
{
public:
    // Main thread update
    void update(float delta);

public:
    TCPSocket();
    void connect(const char* ip, int port);
    void close();
    void sendMsg(const char* msg);

private:
    bool recvMsg();
    void ConnectThread(std::string ip, int port);
    void RecvThread();

    // Json <-> Binary
    int checkJson2Binary(const char* msg, char* binaryBuffer);
    int checkBinary2Json(const char* msg, char* jsonBuffer);

    // Delegate
    void onConnected(int seed);
    void onMessage(const char* msg);
    void onClosed();

private:
    int seed;
    int sock;
    int recvDataLen;
    char recvBuffer[1024 * 64];
    char tempBuffer[1024 * 64];
    char unzipBuffer[1024 * 64];

    int64_t heartBeatTick;

    std::mutex mutex;
    std::queue<std::string> msgs;
};

#endif // __TCP_SOCKET_H__
