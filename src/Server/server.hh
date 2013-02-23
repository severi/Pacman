#ifndef PROJECT_SERVER
#define PROJECT_SERVER

#include <iostream>
#include <SFML/Network.hpp>
#include <string>
#include <map>
#include <utility>
#include <vector>
#include "gameobject.hh"


//#define SERVER_SendPort 5000 	//Port lahettamista varten
#define SERVER_ReceivePort 4000 	//Port vastaanottamista varten
#define SERVER_SpectatorReceivePort 6000


class Server {
public:
	Server();
	
	~Server()
	{
		listenerSpectator.Close();
		listenerPlayer.Close();
		for (std::vector<sf::SocketTCP>::iterator iter=sendList.begin(); iter!=sendList.end(); iter++)
		{
			iter->Close();
		}
		selectorPlayer.Clear();
		selectorSpectator.Clear();
	}
	
	//vastaanottaa erikseen specatorportista ja playerportista tulleet tiedot ja lähettää map päivitykset molempiin
	// int gamestarted määrittää odotetaanko uusia pelaajia vai onko peli jo alkanut ja odotetaan komentoja
	
	std::map<int,int> receive(int gameStarted);  // playerID	
	
	
	
	
	
	void send(std::vector<GameObject> objects);
	void sendPlayerList();
	
	
	
private:
	Server& operator =(const Server & other);
	Server(const Server& other);
	
	
	int nextPlayerID;
	int numberOfFreeGameObjects;
	std::vector<int> freeObjects;
	std::map<int,std::string> objectNames;
	sf::IPAddress serverIP;
	sf::SocketTCP listenerPlayer;
	sf::SocketTCP listenerSpectator;
	sf::SelectorTCP selectorPlayer;
	sf::SelectorTCP selectorSpectator;
	std::vector<sf::SocketTCP> sendList;
	std::map<int, std::string> playerNameList;
	
	
};




#endif


