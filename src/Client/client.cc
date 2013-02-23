#include "client.hh"

Client::Client(int spectator) 
{
	int connectionPort;
	myIP = sf::IPAddress::GetPublicAddress(0);		//hakee oman ip:n
	
	if (spectator==1)
	{
		connectionPort= SpectatorConnectPort;  //määritellään sendport spectaattoriportiksi
	}
	else 
	{
		connectionPort = SendPort;	//määritellään sendport playerportiksi
	}
	
	/*
	 *	yhdistetään serveriin
	 */
	
	bool connected=true;
	std::cout << "Type your name: ";
	std::cin  >> name;
	std::cin.clear();
	std::cin.ignore(1000,'\n');
	do
	{
		std::cout << "Type the ip address of the server: ";
		std::cin  >> serverAddress;
		std::cin.clear();
		std::cin.ignore(1000,'\n');
		
	}
	while (!serverAddress.IsValid());
	
	/*
	 *
	 *	yhdistetään serveriin 
	 *
	 */
	
	
		
	if (sendSocket.Connect(connectionPort, serverAddress) != sf::Socket::Done)
	{
		connected=false;
		while(!connected)
		{
			sf::SocketTCP tmp;
			sendSocket.Close();
			sendSocket=tmp;
			std::cout<<"Trying to connect...\n";
			if (sendSocket.Connect(connectionPort, serverAddress) == sf::Socket::Done)
			{
				connected=true;
				break;
			}
			sleep(1);
		}
	}
	std::cout<<"Connected succesfully!\n";

	if (connected)
	{		
		//lisataan listener selector arrayhin
		selector.Add(sendSocket);
		
		if (spectator==0) 
		{
			sf::Packet freeObjects;
			sendSocket.Receive(freeObjects);
			int id1=0;
			int o1=0;
			int o2=0;
			int o3=0;
			int o4=0;
			int o5=0;
			
			freeObjects>>id1>>o1>>o2>>o3>>o4>>o5;
			myID = id1;

			std::cout<<"Which charecter would you like to play?"<<std::endl;
			int counter=0;
			if (o1!=-1){
				std::cout<<o1<<". Pacman"<<std::endl;
				counter++;
			}
			if (o2!=-1){
				std::cout<<o2<<". Ghost Red"<<std::endl;
				counter++;
			}
			if (o3!=-1){
				std::cout<<o3<<". Ghost Blue"<<std::endl;
				counter++;
			}
			if (o4!=-1){
				std::cout<<o4<<". Ghost Yellow"<<std::endl;
				counter++;
			}
			if (o5!=-1){
				std::cout<<o5<<". Ghost Green"<<std::endl;
				counter++;
			}
			if (counter == 0) 
			{
				//ei pitäisi tapahtua
				std::cout<<"Sorry, but the game is already full"<<std::endl;
			}
			int valinta;
			
			if (counter!=0)
			{
				bool valintaOK=false;
				do{
					std::cout<<"Type the number of the wished character: ";
					std::cin>>valinta;
					std::cin.clear();
					std::cin.ignore(1000,'\n');
					std::cin.clear();
					if (valinta!=-1 &&(valinta==o1 || valinta == o2 || valinta == o3 ||valinta == o4 || valinta==o5))
					{
						std::cout<<"You choose: "<<valinta<<std::endl;
						valintaOK = true;
					}
					else {
						std::cout<<valinta<<"  Invalid number, choose again.\n"<<std::endl;
						
					}
				}
				while (!valintaOK);
				
			}
			else {
				valinta=-1;
			}
			
			sf::Packet confirmPacket;
			confirmPacket<<myID<<valinta;
			sendSocket.Send(confirmPacket);
			confirmPacket.Clear();
			confirmPacket<<name;
			sendSocket.Send(confirmPacket);
		}
	}
}








void Client::sendCommand(int cmd)
{
	sf::Packet packet;
	
	packet<<myID<<cmd;
	
	
	if (! (sendSocket.Send(packet) == sf::Socket::Done))
	{
		std::cout<<"Could not send the packet\n";
	}
}



std::vector<GameObject> Client::receive()
{
	unsigned int lkm =selector.Wait(0.04);
	std::vector<GameObject> map_update;
	
	
	
	for (unsigned int i=0; i<lkm; i++)
	{
		sf::SocketTCP tmp_socket = selector.GetSocketReady(i);
		
		//yhteys serveriin vastaanottamista varten
		if (tmp_socket == listener)
		{
			//hyväksytään serveryhteys jo clientin alustuksessa, eli tämä voidaan ignoorata
			continue;
			
		}
		
		//jos vastaanotetaan mapin päivitys
		sf::SocketTCP receive_socket = tmp_socket;
		sf::Packet packet_received;
		
		if (receive_socket.Receive(packet_received) == sf::Socket::Done)
		{
			GameObject object(NULL,NULL,NULL);
			packet_received >> object;
			map_update.push_back(object);
		}
		
	}
	return map_update;  //palautetaan vectori joka sisältää päivitetyt GameObjectit
}


std::map<int,std::string> Client::receivePlayerList()
{
	std::map<int,std::string> list;


	while (1)
	{
		sf::Packet packet_received;
		
		if (sendSocket.Receive(packet_received) == sf::Socket::Done)
		{
			int id;
			std::string name;
			packet_received >>id>>name;
			
			if (id==-2)
			{
				break;
			}
			list[id]=name;
		}
	}
	return list;  //palautetaan vectori joka sisältää päivitetyt GameObjectit
}





