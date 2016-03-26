#include "WSConnection.hpp"

using namespace std;

using Poco::Net::HTTPResponse;
using Poco::Net::WebSocket;
using Poco::Net::SocketReactor;
using Poco::Net::ReadableNotification;
using Poco::Net::WritableNotification;
using Poco::Net::ShutdownNotification;
using Poco::Net::ErrorNotification;
using Poco::NObserver;

using Poco::Thread;
using Poco::FIFOBuffer;
using Poco::delegate;
using Poco::AutoPtr;

void GameConnection::start()
{
    thread->start(reactor);
    thread->join();
}

void ConnectionsPoll::sendMessage(string message, int id)
{
    auto currentConnections = connections.at(id);
    for (auto it = currentConnections.begin(); it != currentConnections.end(); ++it)
        (*it)->sendMessage(message);
}

void ConnectionsPoll::removeConnection(int id)
{
    connections.erase(id);
}


void ConnectionsPoll::CloseConnection(int id)
{
    connections.erase(id);
}

ConnectionsPoll::~ConnectionsPoll() {}

void WebSocketHandler::onSocketShutdown(const AutoPtr<ShutdownNotification>& pNf)
{
    mConnection->onCloseConnection();
}

WebSocketHandler::WebSocketHandler(Poco::Net::WebSocket& socket, SocketReactor& reactor, int id, ActionHandler handler, GameConnection* connection)
    :
    mSocket(socket),
    mReactor(reactor),
    mHandler(handler),
    mConnection(connection),
    mFifoIn(BUFFER_SIZE, true),
    mId(id)
{
    mReactor.addEventHandler(mSocket, NObserver<WebSocketHandler, ReadableNotification>(*this, &WebSocketHandler::onSocketReadable));
    mReactor.addEventHandler(mSocket, NObserver<WebSocketHandler, ShutdownNotification>(*this, &WebSocketHandler::onSocketShutdown));
}

WebSocketHandler::~WebSocketHandler()
{
    mReactor.removeEventHandler(mSocket, NObserver<WebSocketHandler, ReadableNotification>(*this, &WebSocketHandler::onSocketReadable));
    mReactor.removeEventHandler(mSocket, NObserver<WebSocketHandler, ShutdownNotification>(*this, &WebSocketHandler::onSocketShutdown));
}

void WebSocketHandler::onSocketReadable(const AutoPtr<ReadableNotification>& pNf){
    try
    {
        if (mSocket.available())
        {
            int len = mSocket.receiveBytes(mFifoIn);
            ostringstream strstream;
            if (mFifoIn.isEmpty())
            {
                mReactor.stop();
                return;
            }
            string buffer = string(mFifoIn.begin());
            mHandler(buffer, strstream);
            ConnectionsPoll::instance().sendMessage(strstream.str(), mId);
        }
    }
    catch (exception &e)
    {
        cout << "Error: " << e.what() << endl;
    }
    mFifoIn.drain();
}

void WebSocketHandler::sendMessage(string message)
{
    mSocket.sendFrame(message.c_str(), message.size());
}

void ConnectionsPoll::addThread(int id, WebSocket &ws, onCloseConnectionHandler h, ActionHandler ah)
{
    GameConnection* gc = new GameConnection(ws, id, h, ah);
    if (connections.find(id) == connections.end())
        connections.insert(pair<int, vector<GameConnection*>>(id, vector<GameConnection*>()));
    connections.at(id).push_back(gc);
    gc->start();
}

void GameConnection::sendMessage(string message)
{
    connection.sendMessage(message);
}

GameConnection::GameConnection(WebSocket& ws, int id, onCloseConnectionHandler h, ActionHandler ah) :
    connection(ws, reactor, id, ah, this),
    mOnCloseHandler(h),
    mId(id)
{
    thread = new Thread();
};

GameConnection::~GameConnection()
{
    delete thread;
}

void GameConnection::onCloseConnection()
{
    mOnCloseHandler(mId);
    reactor.stop();
    thread->yield();
    ConnectionsPoll::instance().removeConnection(mId);
}
