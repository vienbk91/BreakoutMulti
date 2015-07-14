/*
 * NodeJSServer.h
 *
 *  Created on: 13 Jul 2015
 *      Author: chu
 */

#ifndef NODEJSSERVER_H_
#define NODEJSSERVER_H_

#include "cocos2d.h"
#include "network/SocketIO.h"

USING_NS_CC;
using namespace network;
using namespace std;


class NodeServer : public Ref , public SocketIO::SIODelegate
{
public:

	static NodeServer* getInstance();
	static void destroyInstance();
	virtual bool init();

	SIOClient* _socketClient;
	string _name;


	SIOClient* getClient();

	void startConnectWithHandle(string name, SIOEvent event);
	void sendMessageWithName(string name, string message);

	virtual void onConnect(SIOClient* client);
	virtual void onMessage(SIOClient* client, const string& data);
	virtual void onClose(SIOClient* client);
	virtual void onError(SIOClient* client, const string& data);

private:
	static NodeServer* _nodeServer;


};


#endif /* NODEJSSERVER_H_ */
