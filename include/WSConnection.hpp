#pragma once
#include <string>
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketNotification.h"
#include "Poco/NObserver.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/SingletonHolder.h"
#include "Poco/Thread.h"
#include "Poco/FIFOBuffer.h"
#include "Poco/Delegate.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Net/WebSocket.h"
#include "Poco/Net/HTTPServer.h"
#include "User.hpp"
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Thread.h"
#include <iostream>
#include <sstream>

typedef void(*ActionHandler)(std::string& action, std::ostringstream& stream);
typedef void(*onCloseConnectionHandler)(int id);

class GameConnection;

class ConnectionsPoll
{
private:
    std::map<int, std::vector<GameConnection*>> connections;
public:
    void addThread(int id, Poco::Net::WebSocket &ws, onCloseConnectionHandler h, ActionHandler ah);
    void removeConnection(int id);
    void CloseConnection(int id);
    void sendMessage(std::string message, int id);
    ~ConnectionsPoll();
    static ConnectionsPoll& instance() {
        static Poco::SingletonHolder<ConnectionsPoll> sh;
        return *sh.get();
    }
};

class WebSocketHandler
{
public:
    ActionHandler mHandler;
    GameConnection* mConnection;
    void onSocketShutdown(const Poco::AutoPtr<Poco::Net::ShutdownNotification>& pNf);
    WebSocketHandler(
        Poco::Net::WebSocket& socket, 
        Poco::Net::SocketReactor& reactor,
        int id, 
        ActionHandler handler, 
        GameConnection* connection
    );

    ~WebSocketHandler();
    void onSocketReadable(const Poco::AutoPtr<Poco::Net::ReadableNotification>& pNf);
    void sendMessage(std::string message);

private:
    const int BUFFER_SIZE = 1024;
    Poco::Net::WebSocket& mSocket;
    Poco::Net::SocketReactor& mReactor;
    Poco::FIFOBuffer mFifoIn;
    int mId;
};

class GameConnection
{
private:
    Poco::Net::SocketReactor reactor;
    Poco::Thread* thread;
    WebSocketHandler connection;
    onCloseConnectionHandler mOnCloseHandler;
    int mId;
public:
    void sendMessage(std::string message);
    void start();
    GameConnection(Poco::Net::WebSocket& ws, int id, onCloseConnectionHandler h, ActionHandler ah);
    ~GameConnection();
    void onCloseConnection();
};









