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
using Poco::NObserver;

using Poco::Thread;
using Poco::FIFOBuffer;
using Poco::delegate;

using Poco::AutoPtr;
typedef void(*ActionHandler)(string& action, ostringstream& stream);
typedef void(*onCloseConnectionHandler)(string accessToken);

class GameConnecton;

class ConnectionsPoll
{
private:
	map<string, GameConnecton*> connections;
public:
	void addThread(string accessToken, WebSocket &ws, onCloseConnectionHandler h, ActionHandler ah);
	void removeConnection(string accessToken);
	void CloseConnection(string accessToken);
	void sendMessage(string message, string key);
	GameConnecton& getConnection(string accesstoken);

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
		string& acctkn, 
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
	string accessToken;
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
	string accessToken;

public:
	void sendMessage(string message){
		connection.sendMessage(message);
	}

	void start()
	{
		thread->start(reactor);
		thread->join();
	}

	GameConnecton(WebSocket& ws, string accessToken, onCloseConnectionHandler h, ActionHandler ah, onCloseConnectionHandler clscn):
		connection(ws, reactor, accessToken, ah, this),
		mOnCloseHandler(h),
		mOnCloseConnection(clscn),
		accessToken(accessToken)
	{
		thread = new Thread();
	};

	~GameConnecton() {
		delete thread;
	}

	void onCloseConnection()
	{
		mOnCloseHandler(accessToken);
		thread->yield();
	}
};









