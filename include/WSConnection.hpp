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
#include "Poco/Net/HTTPServerParams.h"
#include "Poco/Net/HTTPRequestHandler.h"
#include "Poco/Net/HTTPRequestHandlerFactory.h"
#include "Poco/Thread.h"
#include <iostream>
#include <sstream>

using namespace std;

using Poco::Net::HTTPRequest;
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

typedef void(*ActionHandler)(string& action, ostringstream& stream);
typedef void(*onCloseConnectionHandler)(int id);

class GameConnecton;

class ConnectionsPoll
{
private:
	map<int, GameConnecton&> connections;
public:
	void addThread(int id, WebSocket &ws, onCloseConnectionHandler h, ActionHandler ah);
	void removeConnection(int id);
	void CloseConnection(int id);
	void sendMessage(string message, int id);
	GameConnecton& getConnection(int id);

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
	GameConnecton* mConnection;
	void onSocketShutdown(const AutoPtr<ShutdownNotification>& pNf);
	WebSocketHandler(
		Poco::Net::WebSocket& socket, 
		SocketReactor& reactor, 
		int id, 
		ActionHandler handler, 
		GameConnecton* connection
	);

	~WebSocketHandler();

	void onSocketReadable(const AutoPtr<ReadableNotification>& pNf);
	void onSocketWritable(const AutoPtr<WritableNotification>& pNf);
	void onFIFOOutReadable(bool& b);
	void onFIFOInWritable(bool& b);
	void sendMessage(string message);

private:
	const int BUFFER_SIZE = 1024;
	WebSocket& mSocket;
	SocketReactor& mReactor;
	FIFOBuffer	mFifoIn;
	int mId;
	FIFOBuffer	mFifoOut;
};



class GameConnecton
{
private:
	SocketReactor reactor;
	Thread* thread;
	WebSocketHandler connection;
	onCloseConnectionHandler mOnCloseHandler;
	onCloseConnectionHandler mOnCloseConnection;
	int mId;

public:
	void sendMessage(string message){
		connection.sendMessage(message);
	}

	void start()
	{
		thread->start(reactor);
		thread->join();
	}

	GameConnecton(WebSocket& ws, int id, onCloseConnectionHandler h, ActionHandler ah, onCloseConnectionHandler clscn):
		connection(ws, reactor, id, ah, this),
		mOnCloseHandler(h),
		mOnCloseConnection(clscn),
		mId(id)
	{
		thread = new Thread();
	};

	~GameConnecton() {
		delete thread;
	}

	void onCloseConnection()
	{
		
		mOnCloseHandler(mId);
		thread->yield();
		ConnectionsPoll::instance().removeConnection(mId);
	}
};









