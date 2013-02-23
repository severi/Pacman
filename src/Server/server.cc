#include "server.hh"
#define PLAYER_BASE_ID 2010



Server::Server() :nextPlayerID(0),numberOfFreeGameObjects(5)
{
	for (int i=1; i<6; i++)
	{
		freeObjects.push_back(i);
	}
	objectNames[1]="Pacman";
	objectNames[2]="Ghost 1";
	objectNames[3]="Ghost 2";
	objectNames[4]="Ghost 3";
	objectNames[5]="Ghost 4";
	
	serverIP = sf::IPAddress::GetPublicAddress(0);
	std::cout<<"Creating server...\nServer ip address is: "<<serverIP<<std::endl;
	
	if (!listenerPlayer.Listen(SERVER_ReceivePort))
	{
		listenerPlayer.Close();
		return;  //ERRORHANDLING
	}
	
	if (!listenerSpectator.Listen(SERVER_SpectatorReceivePort))
	{
		listenerPlayer.Close();
		listenerSpectator.Close();
		return;  //ERRORHANDLING 
	}
	
	std::cout<<"Listening to port "<<SERVER_ReceivePort<<" for players, and to "<<SERVER_SpectatorReceivePort<<" for spectators."<<std::endl;
	
	selectorPlayer.Add(listenerPlayer);
	selectorSpectator.Add(listenerSpectator);
	
	std::cout<<"Server was created successfully."<<std::endl;
	
}




std::map<int,int> Server::receive(int gameStarted)
{
	std::map<int,int> clientList;
	
	unsigned int nmb = selectorPlayer.Wait(0.02);
	
	for (unsigned int i = 0; i < nmb; ++i)
	{
		sf::SocketTCP tmpSocket = selectorPlayer.GetSocketReady(i);
		
		
		/*
		 *	jos peli ei ole alkanut ja vapaita GO:ta on olemassa, odotetaan uusia pelaajia
		 */
		
		if ( (tmpSocket == listenerPlayer) && (gameStarted == 0 ) && numberOfFreeGameObjects>0)
		{
			
			//sf::SocketTCP sendSocket;
			sf::IPAddress addressClient;
			sf::SocketTCP client;
			listenerPlayer.Accept(client, &addressClient);
			
			std::cout << "Player connected ! (" << addressClient << ")" << std::endl;
			
			
			/*
			 *	jos ei pystytä yhdistämään serveriin, koitetaan 5 kertaa uudestaan, jonka jälkeen hylätään player jos ei edelleenkään onnistu
			 *
			 */
			
			bool connected=true;
			
			/*
			 *	yhdistäminen clienttiin onnistui
			 */
			
			if (connected)
			{
				
				/*
				 * lähetetään clientille vapaat GO:t 1-5; jos ei ole vapaa -1;
				 */
				
				sf::Packet idPacket;
				idPacket<<nextPlayerID<<freeObjects[0]<<freeObjects[1]<<freeObjects[2]<<freeObjects[3]<<freeObjects[4];
				
				client.Send(idPacket);
				
				/*
				 * ja vastaanotetaan clientin valinta
				 */
				
				int valinta;
				int tmpID;
				sf::Packet checkPacket;
				
				client.Receive(checkPacket);
				checkPacket>>tmpID>>valinta;
				
				/*
				 *	tarkistetaan ymmärsikö playerID:n oikein, jos ei suljetaan yhteys
				 */
				
				if (tmpID!=nextPlayerID) 
				{
					std::cout<<tmpID<<" player id was false, it should have been "<<nextPlayerID<<std::endl;
					//sendSocket.Close();
					client.Close();
										
				}
				
				
				else 
				{
					std::string clientName;
					checkPacket.Clear();
					client.Receive(checkPacket);
					checkPacket>>clientName;
					
					std::cout<<clientName<<" connected succesfully to "<<addressClient<<std::endl;
					selectorPlayer.Add(client);
					sendList.push_back(client);
					
					/*
					 *	ja poistetaan valittu GOid poist listasta ja korvataan -1
					 */
					
					for (std::vector<int>::iterator j = freeObjects.begin(); j!=freeObjects.end(); j++)
					{
						if (*j == valinta)
						{
							*j=-1;
							break;
						}
					}
					
					std::cout<<"List of free gameobjects:\n";
					for (std::vector<int>::iterator j = freeObjects.begin(); j!=freeObjects.end(); j++)
					{
						if (*j!=-1) 
						{
							std::cout<<(*j)<<". "<<objectNames[*j]<<std::endl;
						}
						
					}
					
					clientList[nextPlayerID]=(valinta+PLAYER_BASE_ID);	//alustetaan playerID GOid:tä vastaavaksi
					nextPlayerID++;
					playerNameList[valinta]=clientName;
				}	
			}
		}
		
		else if( (tmpSocket!=listenerPlayer) && (gameStarted == 1))  //jos peli on alkanut kuunnellaan pelaajien lähettämie komentoja
		{
			sf::Packet paketti;
			if (tmpSocket.Receive(paketti) == sf::Socket::Done)
			{
				
				int clientID;
				int cmd;
				
				//vastsaanottaa lähettäjän id:n ja suuntakäskyn
				paketti>>clientID>>cmd;
				
				clientList[clientID]=cmd;
			}
		}
	}
	
	if (gameStarted==0)
	{
		/*
		 *	sama spectaattoreille
		 */
		
		unsigned int nmb_s = selectorSpectator.Wait(0.01);
		
		
		for (unsigned int i = 0; i < nmb_s; ++i)
		{
			
			sf::SocketTCP tmpSocket = selectorSpectator.GetSocketReady(i);
			
			if ( (tmpSocket == listenerSpectator) && (gameStarted == 0 ))	//jos peli ei ole alkanut, odotetaan uusia spectaattoreita
			{
				//sf::SocketTCP sendSocket;
				sf::IPAddress addressClient;
				sf::SocketTCP client;
				listenerSpectator.Accept(client, &addressClient);
				std::cout << "Spectator connected ! (" << addressClient << ")" << std::endl;
				
				bool connected = true;
				
				/*
				 *	jos spectator onnistui yhdistämään
				 */
				
				if (connected) 
				{
					selectorSpectator.Add(client);
					
					std::cout<<"Connected succesfully to "<<addressClient<<std::endl;
					sendList.push_back(client);
					//Toisin kuin player, spectattoria ei palauteta, sillä game_enginen ei tarvitse käsitellä spectaattoreita
				}
			}
		}
	}
	
	return clientList;
}





void Server::send(std::vector<GameObject> objects)  //id määräytyy sen mukaan mikä hahmo pelaaja on, pacman = 1, ghost 2-5
{
	for (std::vector<GameObject>::const_iterator j=objects.begin(); j!=objects.end(); j++)
	{	
		for (std::vector<sf::SocketTCP>::iterator iter= sendList.begin(); iter!=sendList.end(); iter++)
		{
		
			sf::Packet derPacket;
			derPacket<<(*j);
			
			if (!iter->IsValid() ||(iter->Send(derPacket) != sf::Socket::Done))
			{
				std::cout<<"Failed sending the packet\n";
			}
			
			
		}
	}
}


void Server::sendPlayerList()
{
	for (std::vector<sf::SocketTCP>::iterator iter= sendList.begin(); iter!=sendList.end(); iter++)
	{
		for (std::map<int, std::string>::iterator iter1 = playerNameList.begin(); iter1!=playerNameList.end(); iter1++)
		{
			
			sf::Packet derPacket;
			derPacket<<iter1->first<<iter1->second;
			
			if (!iter->IsValid() ||(iter->Send(derPacket) != sf::Socket::Done))
			{
				std::cout<<"Failed sending the packet\n";
			}
			std::cout<<iter1->first<<iter1->second<<std::endl;
		}
		sf::Packet paketti;
		
		paketti<<-2<<"";
		iter->Send(paketti);
	}
}

		
		
		




