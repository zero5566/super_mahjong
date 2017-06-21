#include "cocos2d.h"
#include "tcp_socket.h"
#include <thread>

#include "zlib.h"
#include <time.h>

// Windows
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include <ws2tcpip.h>
#include <wspiapi.h>
#include <winsock.h>
class CSocketInit
{
public:
    CSocketInit()
    {
        WSAData wsaData;
        int nReturnCode = WSAStartup(MAKEWORD(2,2), &wsaData);
    }

    ~CSocketInit(void)
    {
        WSACleanup();
    }
};
static CSocketInit g_SocketInit;

// Android, iOS
#else
#include <sys/time.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#endif // CC_TARGET_PLATFORM == CC_PLATFORM_WIN32

using namespace cocos2d;

static unsigned int tyrand(unsigned int randseed)
{
    unsigned int randomNext;
    randomNext = randseed * 1103515245 + 12345;
    return(unsigned int)((randomNext / 65536) % 32768);
}

static void xorCodeing(int _seed, unsigned char* data, unsigned int length)
{
    unsigned int grandom = _seed;
    for (unsigned i = 0; i < length; i++)
    {
        grandom = tyrand(grandom);
        data[i] = data[i] ^ ((unsigned char)(grandom % 255));
    }
}

static bool connect_with_timeout(int& sock, const char* ip, int port, int timeout)
{
    char servname[8] = { 0 };
    sprintf(servname, "%d", port);

    struct addrinfo hints;
    struct addrinfo* result;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICHOST;

    int error = getaddrinfo(ip, servname, &hints, &result);
    if (error)
    {
        memset(&hints, 0, sizeof(hints));
        hints.ai_socktype = SOCK_STREAM;
        error = getaddrinfo(ip, servname, &hints, &result);
        if (error)
        {
            CCLOGERROR("getaddrinfo error: %s", gai_strerror(error));
            return false;
        }
        char host[NI_MAXHOST] = { 0 };
        char serv[NI_MAXSERV] = { 0 };
        error = getnameinfo(result->ai_addr, result->ai_addrlen, host, sizeof(host), serv, sizeof(serv), NI_NUMERICHOST | NI_NUMERICSERV);
        if (error)
        {
            CCLOGERROR("getnameinfo error: %s", gai_strerror(error));
            return false;
        }
    }

    sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (sock == -1)
        return false;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    unsigned long arg = 1;
    if (ioctlsocket(sock, FIONBIO, &arg) != 0)
        return false;
#else
    int flags = ::fcntl(sock, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(sock, F_SETFL, flags);
#endif

    connect(sock, result->ai_addr, result->ai_addrlen);
    freeaddrinfo(result);

    fd_set writefds;
    FD_ZERO(&writefds);
    FD_SET(sock, &writefds);

    timeval timeval;
    timeval.tv_sec = timeout;
    timeval.tv_usec = 0;

    if (select(sock + 1, 0, &writefds, 0, &timeval) <= 0)
    {
        shutdown(sock, 2);
        return false;
    }

    int optVal;
    int optLen = sizeof(int);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS || CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    getsockopt(sock, SOL_SOCKET, SO_ERROR, (void*)&optVal, (socklen_t*)&optLen);
#else
    getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)&optVal, &optLen);
#endif
    if (optVal != 0)
    {
        shutdown(sock, 2);
        return false;
    }

    return true;
}

static void _strftime(char buf[16])
{
    time_t _t;
    time(&_t);
    tm* _tm = localtime(&_t);
    strftime(buf, 16, "%H:%M:%S", _tm);
}

TCPSocket::TCPSocket()
{
    seed = 0;
    recvDataLen = 0;
    cocos2d::Director::getInstance()->getScheduler()->scheduleUpdate(this, 0, false);
}

void TCPSocket::connect(const char* ip, int port)
{
    seed = 0;
    std::thread* connect_thread = new std::thread(&TCPSocket::ConnectThread, this, std::string(ip), port);
    connect_thread->detach();
}

void TCPSocket::close()
{
    shutdown(sock, 2);
}

void TCPSocket::sendMsg(const char* msg)
{
#if (COCOS2D_DEBUG > 0)
    char timebuf[16] = { 0 };
    _strftime(timebuf);
    CCLOG("%s - NETMSG SEND %s", timebuf, msg);
#endif

    int binaryLen = checkJson2Binary(msg, tempBuffer + 4);
    if (binaryLen < 0)
        return;

    if (binaryLen == 0)
    {
        binaryLen = (int)strlen(msg);
        memcpy(tempBuffer + 4, msg, binaryLen);
    }

    // send heart beat
    if (strstr(msg, "heart_beat") != nullptr)
    {
        timeval tv;
        gettimeofday(&tv, nullptr);
        heartBeatTick = ((int64_t)tv.tv_sec * 1000) + ((int64_t)tv.tv_usec / 1000);
    }

    char szLen[5] = { 0 };
    sprintf(szLen, "%04lx", binaryLen);
    memcpy(tempBuffer, szLen, 4);
    xorCodeing(seed + binaryLen, (unsigned char*)(tempBuffer + 4), binaryLen);
    send(sock, tempBuffer, 4 + binaryLen, 0);
}

bool TCPSocket::recvMsg()
{
    int recvLen = recv(sock, recvBuffer + recvDataLen, sizeof(recvBuffer) - recvDataLen, 0);
    if (recvLen <= 0)
        return false;

    if (seed == 0 && recvLen == 4)
    {
        sscanf(recvBuffer, "%4x", &seed);
        mutex.lock();
        msgs.push(cocos2d::StringUtils::format("c%i", seed));
        mutex.unlock();
        return true;
    }

    recvDataLen += recvLen;
    int readPos = 0;
    while (readPos < recvDataLen)
    {
        int msgLen = 0;
        sscanf(recvBuffer + readPos, "%4x", &msgLen);
        if (recvDataLen - readPos < 4 + msgLen)
            break;

        xorCodeing(seed + msgLen, (unsigned char*)(recvBuffer + readPos + 4), msgLen);

        uLongf debuflen = sizeof(unzipBuffer);
        memset(unzipBuffer, 0, debuflen);
        uncompress((Bytef*)unzipBuffer, &debuflen, (Bytef*)(recvBuffer + readPos + 4), msgLen);

        const char* buffer = unzipBuffer;
        if (unzipBuffer[0] == '$' && unzipBuffer[1] == 'f' && unzipBuffer[2] == 's')
        {
            checkBinary2Json(unzipBuffer, tempBuffer);
            buffer = tempBuffer;
        }

        // heart beat response
        if (recvLen == 4)
        {
            timeval tv;
            gettimeofday(&tv, nullptr);
            int64_t now = ((int64_t)tv.tv_sec * 1000) + ((int64_t)tv.tv_usec / 1000);
            sprintf((char*)buffer, "{\"cmd\":\"heart_beat\",\"delay\":\"%i\"}", int(now - heartBeatTick));
        }

        mutex.lock();
        msgs.push(buffer);
        mutex.unlock();

#if (COCOS2D_DEBUG > 0)
        char timebuf[16] = { 0 };
        _strftime(timebuf);
        CCLOG("%s - NETMSG RECV %s", timebuf, buffer);
#endif

        readPos += (4 + msgLen);
    }
    recvDataLen -= readPos;
    if (recvDataLen > 0 && readPos > 0)
        memmove(recvBuffer, recvBuffer + readPos, recvDataLen);
    return true;
}

void TCPSocket::ConnectThread(std::string ip, int port)
{
    if (connect_with_timeout(sock, ip.c_str(), port, 5))
    {
        std::thread* recv_thread = new std::thread(&TCPSocket::RecvThread, this);
        recv_thread->detach();
    }
    else
    {
        mutex.lock();
        msgs.push("c0");
        mutex.unlock();
    }
}

void TCPSocket::RecvThread()
{
    while (true)
    {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);
        if (select(sock + 1, &readfds, nullptr, nullptr, nullptr) < 0)
            break;

        if (FD_ISSET(sock, &readfds))
        {
            if (!recvMsg())
                break;
        }
    }

    shutdown(sock, 2);
    sock = -1;
    mutex.lock();
    msgs.push("d");
    mutex.unlock();
}

void TCPSocket::update(float delta)
{
    mutex.lock();
    if (msgs.size() == 0)
    {
        mutex.unlock();
        return;
    }

    std::string msg = msgs.front();
    msgs.pop();
    mutex.unlock();

    const char* pmsg = msg.c_str();

    // "c2989": connect + seed
    if (pmsg[0] == 'c')
    {
        int seed = atoi(pmsg + 1);
        onConnected(seed);
    }
    // "d": disconnect
    else if (pmsg[0] == 'd')
    {
        onClosed();
    }
    // normal msg: {"cmd":...}
    else
    {
        onMessage(pmsg);
    }
}
