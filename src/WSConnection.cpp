#include "WSConnection.hpp"

void ConnectionsPoll::addThread(int id, WebSocket &ws, onCloseConnectionHandler h, ActionHandler ah)
{
	GameConnecton gc(ws, id, h, ah, h);
	pair<int, GameConnecton&> r = pair<int, GameConnecton&>(id, gc);
	connections.insert(r);
	gc.start();
}

void ConnectionsPoll::sendMessage(string message, int id)
{
	connections.at(id).sendMessage(message);
}

void ConnectionsPoll::removeConnection(int id)
{
	connections.erase(id);
}

GameConnecton& ConnectionsPoll::getConnection(int id) {
	return connections.at(id);
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
	mFifoOut(BUFFER_SIZE, true),
	mId(id)
{

	mReactor.addEventHandler(mSocket, NObserver<WebSocketHandler, ReadableNotification>(*this, &WebSocketHandler::onSocketReadable));
	mReactor.addEventHandler(mSocket, NObserver<WebSocketHandler, ShutdownNotification>(*this, &WebSocketHandler::onSocketShutdown));
	mFifoOut.readable += delegate(this, &WebSocketHandler::onFIFOOutReadable);
	mFifoIn.writable += delegate(this, &WebSocketHandler::onFIFOInWritable);
}

WebSocketHandler::~WebSocketHandler()
{
	mReactor.removeEventHandler(mSocket, NObserver<WebSocketHandler, ReadableNotification>(*this, &WebSocketHandler::onSocketReadable));
	mReactor.removeEventHandler(mSocket, NObserver<WebSocketHandler, WritableNotification>(*this, &WebSocketHandler::onSocketWritable));
	mReactor.removeEventHandler(mSocket, NObserver<WebSocketHandler, ShutdownNotification>(*this, &WebSocketHandler::onSocketShutdown));
}

void WebSocketHandler::onSocketReadable(const AutoPtr<ReadableNotification>& pNf) {
	try
	{
		if (mSocket.available())
		{
			int len = mSocket.receiveBytes(mFifoIn);
			ostringstream strstream;
			if (mFifoIn.isEmpty()) {
				mReactor.stop();
				return;
			}
			string buffer = string(mFifoIn.begin());
			mHandler(buffer, strstream);
			mFifoOut.write(strstream.str().c_str(), strstream.str().size());
		}
	}
	catch (...)
	{
		mSocket.shutdownSend();
		mConnection->onCloseConnection();
	}
	mFifoIn.drain();
}

void WebSocketHandler::onSocketWritable(const AutoPtr<WritableNotification>& pNf) {
	mSocket.sendFrame(mFifoOut.begin(), mFifoOut.used());
	mFifoOut.drain();
}

void WebSocketHandler::onFIFOOutReadable(bool& b) {
	if (b)
		mReactor.addEventHandler(mSocket, NObserver<WebSocketHandler, WritableNotification>(*this, &WebSocketHandler::onSocketWritable));
	else
		mReactor.removeEventHandler(mSocket, NObserver<WebSocketHandler, WritableNotification>(*this, &WebSocketHandler::onSocketWritable));
}

void WebSocketHandler::onFIFOInWritable(bool& b) {
	if (b)
		mReactor.addEventHandler(mSocket, NObserver<WebSocketHandler, ReadableNotification>(*this, &WebSocketHandler::onSocketReadable));
	else
		mReactor.removeEventHandler(mSocket, NObserver<WebSocketHandler, ReadableNotification>(*this, &WebSocketHandler::onSocketReadable));
}

void WebSocketHandler::sendMessage(string message)
{
	mFifoOut.write(message.c_str(), message.size());
}