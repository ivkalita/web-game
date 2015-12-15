#include "WSConnection.hpp"

void ConnectionsPoll::addThread(int id, WebSocket &ws, onCloseConnectionHandler h, ActionHandler ah)
{
    GameConnecton* gc = new GameConnecton(ws, id, h, ah);
    if (connections.find(id) == connections.end())
        connections.insert(pair<int, vector<GameConnecton*>>(id, vector<GameConnecton*>()));
    connections.at(id).push_back(gc);
    gc->start(connections.at(id).size()-1);
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

ConnectionsPoll::~ConnectionsPoll()
{

}

void WebSocketHandler::onSocketShutdown(const AutoPtr<ShutdownNotification>& pNf)
{
    mConnection->onCloseConnection();
}

WebSocketHandler::WebSocketHandler(Poco::Net::WebSocket& socket, SocketReactor& reactor, int id, ActionHandler handler, GameConnecton* connection)
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
        mSocket.shutdownSend();
        mConnection->onCloseConnection();
    }
    mFifoIn.drain();
}

void WebSocketHandler::sendMessage(string message)
{
    mSocket.sendFrame(message.c_str(), message.size());
}