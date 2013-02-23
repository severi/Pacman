#ifndef PROJECT_CLIENT
#define PROJECT_CLIENT

#include <iostream>
#include <SFML/Network.hpp>
#include <string>
#include <map>
#include <utility>
#include <vector>
#include "gameobject.hh"


#define SendPort 4000 	//Port lahettamista varten
#define SpectatorConnectPort 6000 //tms




class Client {
public:
	Client(int spectator);
	

	
	~Client()
	{
		sendSocket.Close();
		listener.Close();
		selector.Clear();
	}
	
	
	
	void sendCommand(int cmd);
	
	/*
	 *  Funktio, jonka avulla voidaan vastaan ottaa map päivitykset ja yhteys serveriltä
	 */
	
	std::vector<GameObject> receive();
	
	std::map<int,std::string>receivePlayerList();
	
	
	
private:
	
	Client& operator =(const Client & other);
	Client(const Client& other);
	
	sf::IPAddress serverAddress;
	sf::IPAddress myIP;
	std::string name;
	int myID;
	sf::SocketTCP sendSocket;
	sf::SocketTCP listener;
	sf::SelectorTCP selector;
};





#endif
