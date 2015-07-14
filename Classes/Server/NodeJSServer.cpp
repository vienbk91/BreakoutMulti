/*
 * NodeJSServer.cpp
 *
 *  Created on: 13 Jul 2015
 *      Author: chu
 */

#include "NodeJSServer.h"

NodeServer* NodeServer::_nodeServer = nullptr;

NodeServer* NodeServer::getInstance()
{
	if(!_nodeServer){
		_nodeServer = new (std::nothrow) NodeServer();
		_nodeServer->init();
	}

	return _nodeServer;
}

void NodeServer::destroyInstance(){
	if(_nodeServer->getClient() != nullptr){
		_nodeServer->getClient()->disconnect();
	}

	_nodeServer = nullptr;
}

SIOClient* NodeServer::getClient(){
	if(_socketClient != nullptr){
		return _socketClient;
	}

	return nullptr;
}

bool NodeServer::init()
{
	//============================================
	// Connect server
	std::string url = "ws://127.0.0.1:8080/";
	_socketClient = SocketIO::connect(url , *this);
	CCASSERT(_socketClient , "==============>Error! Can not create socketIO");
	log("============> Connect to nodejs server : %s" , url.c_str() );

	return true;
}


void NodeServer::startConnectWithHandle(std::string name, SIOEvent event)
{
	_name = name;

	if (_socketClient){
		_socketClient->on(name.c_str() , event);
		return;
	}

	log("Start Connect Failed");

}

void NodeServer::sendMessageWithName(string name, string message){
	if (_socketClient){
		_socketClient->emit(name.c_str(), message.c_str());
	}
	else{
		log("Send Message Failed");
	}
}


void NodeServer::onConnect(SIOClient* client){
	log("================================> onConnect");
}

void NodeServer::onMessage(SIOClient* client, const string& data){

}

void NodeServer::onClose(SIOClient* client){
	log("===============================> onClose");
}

void NodeServer::onError(SIOClient* client, const string& data){
	log("===============================> onError");
}
