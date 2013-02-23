#include "server.hh"
#include "gameobject.hh"
#include <SFML/Network.hpp>
#include <map>
#include <vector>
#include <string>
#include <utility>
#include <fstream>


#define EMPTY 0
#define IMAGESIZE 32





sf::Vector2<int> getLocation(int x,int y)
{
	sf::Vector2 <int>vector;
	vector.x= ((IMAGESIZE/2)+x*IMAGESIZE);
	vector.y= ((IMAGESIZE/2)+y*IMAGESIZE);
	return vector;
}


enum directions 
{
	right,
	up,
	left,
	down
};


/*
 *
 *	Player struct on käytännössä vain GameObject pointteri,alunperin Player sisälsi muitakin ominaisuuksia, 
 *	mutta projektin edetessä siirsimme niitä pääasiassa GameObjectille (esim next ja current direction)
 *	Jätimme kuitenkin Player structin, koska jokaisen kohdan muuttaminen GO*:ksi missä playeria on käytetty olisi
 *	ollut turhaa työtä ja periaatteessa Player structi ehkä myös helpottaa koodin lukemista :)
 *
 */

struct Player{
	
	Player(GameObject* ob, int cur, int next):object(ob)
	{
		object->change_current_direction(cur);
		object->change_next_direction(next);
	}
	
	void changeCurrentDirection(int dir)
	{
		object->change_current_direction(dir);
	}
	
	void changeNextDirection(int dir)
	{
		object->change_next_direction(dir);
	}
	
	GameObject* getObject()
	{
		return object;
	}
	
	int getCurrentDirection()
	{
		return object->get_current_direction();
	}
	
	int getNextDirection()
	{
		return object->get_next_direction();
	}
	
private:
	
	GameObject *object;	
};




/*
 *	Funktio, jonka avulal serveri tiedustelee, halutaanko peliin odottaa lisää pelaajia, vai aloitetaanko peli
 */

int waitForMorePlayers()
{
	std::string a;
	while (1) 
	{
		
		std::cout<<"Would you like to wait for more players, type y for yes and n for no.\n";
		std::cin>>a;
		std::cin.clear();
		std::cin.ignore(1000,'\n');
		if (a.length()==1 && (a[0]=='y' || a[0]=='n'))	break;
		else {
			std::cout<<"Invalid input. Please try again.\n";
		}

	}
	if(a[0]=='y')
		return 1;
	return 0;
	
}




/*
 *	Itse peli alkaa tästä
 */



int main(void)
{
	
	/*
	 *
	 * OSA 1: Pelaajat liittyvät serverille
	 *
	 */
	
	Server serveri;
	
	std::map<int,int> pelaajat; // player id - go id
	std::map<int,int> tmp_pelaajat; //go id - player id (semi-purkka)
	
	
	std::vector<GameObject> map;	
	
	/*
	 *	odotetaan uusia pelaajia/spectaattoreita
	 */
	
	do
	{
		for (int i=0; i<50; i++) 
		{
			std::map<int,int> newPlayers;
			newPlayers = serveri.receive(0);
			for (std::map<int,int>::iterator iter = newPlayers.begin(); iter!=newPlayers.end(); iter++) 
			{
				pelaajat[iter->first]=iter->second;
				tmp_pelaajat[iter->second]=iter->first;
			}
		}
	}
	while(pelaajat.size()<2 || tmp_pelaajat.find(PACMAN)==tmp_pelaajat.end() || waitForMorePlayers() ); 
	
	
	
	GameObject pacman= GameObject(PACMAN,0,0);
	GameObject ghost1(-2,0,0);
	GameObject ghost2(-3,0,0);
	GameObject ghost3(-4,0,0);
	GameObject ghost4(-5,0,0);
	GameObject ghost5(GHOST_TERMINATOR,0,0);	//testausta varten
	
	
	/*
	 *	MAP GENEROINTI
	 *	Luodaan pelin map, map.txt tiedoston mukaiseksi
	 *	tekstitiedostossa o tarkoittaa tyhjää, p pacmania, 1-4 ghosteja( 5 on debuggaukseen käytetty ghost)
	 *	t on teleportti, x on seinä, . on syötävä "piste", ja s on superfruit
	 *	- tarkoittaa "rivinvaihtoa"
	 */
	
	std::ifstream file;
	file.open("map.txt");
	
	char type;
	int x=0;
	int y=0;
	int wall=0;
	int fruit=0;
	int super=0;
	int tele=0;
	sf::Vector2<int> beginCoordinate;
	sf::Vector2<int> ghostBeginCoordinate;
	int x_max2=0;
	while (file>>type)
	{
		switch (type) {
			case '-':{
				y++;
				x_max2=x;
				x=0;
				break;}
			case 'p':{
				pacman.change_Id(PACMAN);
				pacman.change_x_coordinate(float(getLocation(x,y).x));
				pacman.change_y_coordinate(float(getLocation(x,y).y));
				beginCoordinate=getLocation(x, y);
				
				GameObject object(FRUIT_MIN+fruit,getLocation(x, y).x,getLocation(x, y).y);
				map.push_back(object);
				fruit++;
				
				x++;
				break;}
			case '1':{
				ghost1.change_Id(GHOST_RED);
				ghost1.change_x_coordinate(float(getLocation(x,y).x));
				ghost1.change_y_coordinate(float(getLocation(x,y).y));
				ghostBeginCoordinate = getLocation(x, y);
				
				GameObject object(FRUIT_MIN+fruit,getLocation(x, y).x,getLocation(x, y).y);
				map.push_back(object);
				fruit++;
				
				x++;
				break;}
			case '2':{
				ghost2.change_Id(GHOST_BLUE);
				ghost2.change_x_coordinate(float(getLocation(x,y).x));
				ghost2.change_y_coordinate(float(getLocation(x,y).y));
				
				
				GameObject object(FRUIT_MIN+fruit,getLocation(x, y).x,getLocation(x, y).y);
				map.push_back(object);
				fruit++;
				
				x++;
				break;}
			case '3':{
				ghost3.change_Id(GHOST_GREEN);
				ghost3.change_x_coordinate(float(getLocation(x,y).x));
				ghost3.change_y_coordinate(float(getLocation(x,y).y));
				
				
				GameObject object(FRUIT_MIN+fruit,getLocation(x, y).x,getLocation(x, y).y);
				map.push_back(object);
				fruit++;
				
				x++;
				break;}
			case '4':{
				ghost4.change_Id(GHOST_YELLOW);
				ghost4.change_x_coordinate(float(getLocation(x,y).x));
				ghost4.change_y_coordinate(float(getLocation(x,y).y));
				
				
				GameObject object(FRUIT_MIN+fruit,getLocation(x, y).x,getLocation(x, y).y);
				map.push_back(object);
				fruit++;
				
				x++;
				break;}
			case '5':{
				ghost5.change_Id(GHOST_TERMINATOR);
				ghost5.change_x_coordinate(float(getLocation(x,y).x));
				ghost5.change_y_coordinate(float(getLocation(x,y).y));
				
				GameObject object(FRUIT_MIN+fruit,getLocation(x, y).x,getLocation(x, y).y);
				map.push_back(object);
				fruit++;
				
				map.push_back(ghost5);
				x++;
				break;}
			case 't':{
				GameObject object(TELEPORT_MIN+tele, getLocation(x,y).x , getLocation(x,y).y);
				map.push_back(object);
				tele++;
				x++;
				break;}	
			case 'x':{
				GameObject object(WALL_MIN+wall, getLocation(x,y).x , getLocation(x,y).y);
				map.push_back(object);
				wall++;
				x++;
				break;}
			case 's':{
				GameObject object(SUPER_FRUIT_MIN+super, getLocation(x,y).x , getLocation(x,y).y);
				map.push_back(object);
				super++;
				x++;
				break;}
			case '.':{
				GameObject object(FRUIT_MIN+fruit,getLocation(x, y).x,getLocation(x, y).y);
				map.push_back(object);
				fruit++;
				x++;
				break;
			}
			default:
				x++;
				break;
		}
	}
	
	//maksimiarvot x-ja y-koordinaateille:
	int x_max = x_max2*IMAGESIZE-(1/2)*IMAGESIZE;
	int y_max = y*IMAGESIZE-(1/2)*IMAGESIZE;
	
	
	
	/*
	 *
	 *	Tämän avulla saadaan teleportit toiminaan, oletuksena että teleportteja on joko tasan 0 tai 2
	 *	Teleporttien myös oletetaan sijaitsevan ainoastaan x, suunnassa, eli vasemmalta oikealle tai vice versa
	 *	ylös alas tapahtuvaa teleporttia ei ole määritelty ja se todennäköisesti jumittaa pacmanin teleport kierteeseen
	 *
	 */
	
	std::vector<GameObject>::iterator tele1;
	std::vector<GameObject>::iterator tele2;
	
	int telelkm=0;
	
	for (std::vector<GameObject>::iterator iter=map.begin(); iter!=map.end(); iter++) 
	{
		if (iter->get_Id() ==TELEPORT_MIN)
		{
			tele1=iter;
			telelkm++;
		}
		if (iter->get_Id() ==TELEPORT_MIN+1)
		{
			tele2=iter;
			telelkm++;
		}
	}
	if (telelkm==0 || telelkm==2)
	{
		if (telelkm==2) {
			if (tele2->get_x_coordinate()>tele1->get_x_coordinate())
			{
				tele1->change_state(tele2->get_x_coordinate()-40);
				tele1->change_previousState(tele2->get_y_coordinate());
				tele2->change_state(tele1->get_x_coordinate()+40);
				tele2->change_previousState(tele1->get_y_coordinate());
			}
			else if(tele2->get_x_coordinate()>tele1->get_x_coordinate())
			{
				tele1->change_state(tele2->get_x_coordinate()+40);
				tele1->change_previousState(tele2->get_y_coordinate());
				tele2->change_state(tele1->get_x_coordinate()-40);
				tele2->change_previousState(tele1->get_y_coordinate());
			}
			else {
				std::cout<<"WARNING!\nThe chosen map is invalid, this can cause undefined behaviour!"<<std::endl;
				tele1->change_state(tele2->get_x_coordinate());
				tele1->change_previousState(tele2->get_y_coordinate());
				tele2->change_state(tele1->get_x_coordinate());
				tele2->change_previousState(tele1->get_y_coordinate());
			}

		}
	}
	else {
		std::cout<<"WARNING!\nThe chosen map is invalid, this can cause undefined behaviour!"<<std::endl;
	}
	
	file.close();
	
	
	Player p_pacman(&pacman,right,right);
	Player p_ghost1(&ghost1,right,right); 
	Player p_ghost2(&ghost2,right,right);
	Player p_ghost3(&ghost3,right,right);
	Player p_ghost4(&ghost4,right,right);
	int ghost_count=0;
	
	
	std::map<int,Player*> players;	
	
	if (tmp_pelaajat.find(PACMAN) != tmp_pelaajat.end())
	{
		players[tmp_pelaajat[PACMAN]]=&p_pacman;
	}
	
	if (tmp_pelaajat.find(GHOST_RED) != tmp_pelaajat.end())
	{
		players[tmp_pelaajat[GHOST_RED]]= &p_ghost1;
		ghost_count++;
	}

	
	
	if (tmp_pelaajat.find(GHOST_BLUE) != tmp_pelaajat.end())
	{
		players[tmp_pelaajat[GHOST_BLUE]]= &p_ghost2;
		ghost_count++;
	}

	
	if (tmp_pelaajat.find(GHOST_YELLOW) != tmp_pelaajat.end())
	{
		players[tmp_pelaajat[GHOST_YELLOW]]= &p_ghost3;
		ghost_count++;
	}
	
	if (tmp_pelaajat.find(GHOST_GREEN) != tmp_pelaajat.end())
	{
		players[tmp_pelaajat[GHOST_GREEN]]= &p_ghost4;
		ghost_count++;
	}

	switch (ghost_count) 
	{
		case 2:
			ghost1.change_speed(140);
			ghost2.change_speed(140);
			ghost3.change_speed(140);
			ghost4.change_speed(140);
			break;
		case 3:
			ghost1.change_speed(120);
			ghost2.change_speed(120);
			ghost3.change_speed(120);
			ghost4.change_speed(120);
			break;
		case 4:
			ghost1.change_speed(100);
			ghost2.change_speed(100);
			ghost3.change_speed(100);
			ghost4.change_speed(100);
			break;

		default:
			break;
	}
	
	/*
	 *	lähetetään clienteille lista pelaajista
	 */
	
	
	for (std::map<int, Player*>::iterator it = players.begin(); it!=players.end(); it++) 
	{
		map.push_back(*((it->second)->getObject()));
	}
	
	serveri.send(map);
	
	
	/*
	 *	lähetetään pelin aloitussignaali clienteille
	 */
	
	GameObject gameStartSignal(-1,-1,-1);
	
	
	std::vector<GameObject> tmpVec;
	tmpVec.push_back(gameStartSignal);
	serveri.send(tmpVec);
	
	serveri.sendPlayerList();
	
	
	/*
	 *
	 * OSA 2: Peli käynnistetään
	 *
	 */
	
	sf::Clock Clock;
	float pacmanSuperModeClock = 0;
	float pacmanFreezeModeClock = 0;
	int mapHasBeenSent = 0;
	
	bool gameContinues = true;
	
	int alku=0;
	while (gameContinues)
	{
		//float ElapsedTime1=0;
		
		float ElapsedTime; 
		if (alku<10) {
			ElapsedTime = 0;
			alku++;
			sleep(1);
		}
		/* tod näk turha, jääny jostain testistä	
		else if (players[tmp_pelaajat[PACMAN]]->getObject()->get_state()==FREEZE)
		{
			ElapsedTime=0;
			ElapsedTime1=Clock.GetElapsedTime();
			
			
		}
		 */
		else 
		{
			ElapsedTime = Clock.GetElapsedTime(); // Laskee kuinka paljon pacmanin pitää liikkua
			//ElapsedTime1=ElapsedTime;
		}		
		
		Clock.Reset();
		
		
		std::map<int ,int> commands;
		commands = serveri.receive(1);
		
		/*
		 *	vaihtaa clientiltä saadun suunnan mukaisen next directionin playerille/GO:lle
		 *
		 *
		 */
		
		if (players[tmp_pelaajat[PACMAN]]->getObject()->get_state() !=FREEZE) 
		{
			for (std::map<int, int>::iterator iter = commands.begin(); iter != commands.end(); iter++)
			{
				for (std::map<int, Player*>::iterator it = players.begin(); it!=players.end(); it++) 
				{
					if (iter->first == it->first) 
					{
						
						it->second->changeNextDirection(iter->second);
					}
				}
				
			}
		}
		
		std::vector<GameObject> muutokset;
		bool pacManHasWon = false;
		
		
		if (alku>=10) 
		{
			
			
			/*
			 *
			 *	liikutetaan pelaajia, ja tehdään muutokset
			 */
			
			if (players.begin()->second->getObject()->get_state() != FREEZE)
			{
				bool hasFreezeHappened=false;
				for (std::map<int, Player*>::iterator it = players.begin(); it!=players.end(); it++) 
				{
					std::vector<GameObject> tmp;
					tmp = it->second->getObject()->move(ElapsedTime, map, x_max, y_max);
					
					
					// aikalaskuri
					if (it->second->getObject()->get_state() == SUPER_MODE && pacmanSuperModeClock <= SUPER_MODE_TIME ) {
						pacmanSuperModeClock += ElapsedTime;
					}
					
					for (std::vector<GameObject>::iterator it1 = tmp.begin(); it1!= tmp.end(); it1++)
					{
						muutokset.push_back(*it1);
						if (it1->get_Id()==PACMAN && it1->get_state()==FREEZE)
						{
							hasFreezeHappened=true;
						}
					}
					if (hasFreezeHappened)
					{
						break;
					}
				}
				
			}
			if (players.begin()->second->getObject()->get_state() == FREEZE && pacmanFreezeModeClock <= FREEZE_TIME) {
				pacmanFreezeModeClock += ElapsedTime;
			}
			
			if (players.begin()->second->getObject()->get_state() == FREEZE && pacmanFreezeModeClock > FREEZE_TIME) {
				pacmanFreezeModeClock = 0;
				for (std::map<int, Player*>::iterator it = players.begin(); it!=players.end(); it++) //Ghostit loo
				 {
					 if (it->second->getObject()->get_Id() == PACMAN && it->second->getObject()->get_Id() == GHOST_TERMINATOR) {
						 continue;
					 }
					 it->second->getObject()->change_x_coordinate(ghostBeginCoordinate.x);
					 it->second->getObject()->change_y_coordinate(ghostBeginCoordinate.y);
					 it->second->getObject()->change_next_direction(right);
				 
				 }
				players.begin()->second->getObject()->change_x_coordinate(beginCoordinate.x);
				players.begin()->second->getObject()->change_y_coordinate(beginCoordinate.y);
				players.begin()->second->getObject()->change_state(players.begin()->second->getObject()->get_previousState());
				players.begin()->second->getObject()->change_next_direction(right);
			}
			
			
			/*
			 *	päivitetään map muutoksien mukaiseksi
			 */
			
			
			
			/*
			 *	tarkastetaan onko pacman muutokset listalla, jos ei ole, ja pacman on supermode-tilassa, pitää myös pacman käsitellä
			 */
			
			//TODO tarviiko esim freezelle tehdä kanssa näin? korjaisko bugin?
			//katso alemmat supermode tarkistukset ja testaa auttaisiko se freeeze ongelmaan
			
			bool pacmanHasChanged=false;
			for (std::vector<GameObject>::iterator it1 = muutokset.begin(); it1!= muutokset.end(); it1++)
			{
				if (it1->get_Id()==PACMAN)
				{
					pacmanHasChanged=true;
					break;
				}
			}
			
			
			if (!pacmanHasChanged && players[tmp_pelaajat[PACMAN]]->getObject()->get_state()==SUPER_MODE)
			{
				muutokset.push_back(*players[tmp_pelaajat[PACMAN]]->getObject());
				
			}
			

			
			
			/*
			 *
			 *	sijoitetaan muutokset mappiin, sekä päivitetään player-gameobjektit 
			 *
			 */
			
			for (std::vector<GameObject>::iterator it1 = muutokset.begin(); it1!= muutokset.end(); it1++)
			{
				if (it1->get_Id() >= SUPER_FRUIT_MIN && it1->get_Id() <= SUPER_FRUIT_MAX ) { // Jos Pacman syö uuden super_fruitin
					pacmanSuperModeClock = 0;
					
				}
				for (std::vector<GameObject>::iterator it2 = map.begin(); it2!= map.end(); ) 
				{
					if (it1->get_Id()==it2->get_Id()) 
					{
						*it2 = *it1;
						
						if	(tmp_pelaajat.find(it1->get_Id())!=tmp_pelaajat.end())
						{
							
							bool superModeGoesAway=false;
							if (players[tmp_pelaajat[it1->get_Id()]]->getObject()->get_Id() == PACMAN &&  pacmanSuperModeClock > SUPER_MODE_TIME)
								superModeGoesAway=true;
							
							*players[tmp_pelaajat[it1->get_Id()]]->getObject()=*it1;
							
							if (superModeGoesAway) //tarkistaa onko obj pacman
							{ 
								pacmanSuperModeClock = 0;
								players[tmp_pelaajat[it1->get_Id()]]->getObject()->change_state(it1->get_previousState());
								it1->change_state(it1->get_previousState());
								it2->change_state(it1->get_previousState());
								
							}
						}
						
						
						break;
					}
					it2++;
				}
			}
			
			/*
			 *
			 * Tarkistetaan onko peli ohi. Peli on ohi kun kaikki hedelmät on syöty tai pacman kuollut
			 *
			 */
			
			int fruitsEaten = 0;
			
			for (std::vector<GameObject>::iterator it2 = map.begin(); it2!= map.end(); it2++ ) 
			{
				
				if (FRUIT_MIN <= it2->get_Id() && it2->get_Id() <= FRUIT_MAX && it2->get_state() == DEAD) {
					fruitsEaten += 1;
					if (fruitsEaten == fruit) {
						pacManHasWon = true;
					}
				}
				
			}
		}
		
		
		
		
		/*
		 *	Vähän purkkaviritys, jostain syystä gameobjektit eivät aina näkyneet oikein clientilllä jos map lähetettiin vain kerran kokonaisuudessaan
		 *	joten tämän takia map lähetetään 2 kertaa
		 */
		
		if (mapHasBeenSent > 2) {
			serveri.send(muutokset);
		}
		else {
			serveri.send(map);
			mapHasBeenSent++;
		}
		
		if (players[tmp_pelaajat[PACMAN]]->getObject()->get_state() == DEAD || pacManHasWon)
		{
			sleep(1);
			std::vector<GameObject> endVec;
			if (pacManHasWon)
			{
				players[tmp_pelaajat[PACMAN]]->getObject()->change_state(PACMANHASWON);
			}
			
			endVec.push_back(*players[tmp_pelaajat[PACMAN]]->getObject());
			
			serveri.send(endVec);
			
			gameContinues = false;
		}
		
		/*
		 *	vähennetään liikennettä, 0,025s valikoitui siksi että se helpotti lagia, muttei myöskään aiheuttanut ongelmia 
		 *	käännöksissä (suurilla viiveillä käännöstarkastelut epäonnistuivat, ja pelaajan hahmo ei välttämättä pystyn liikkumaan eteenpäin
		 *	vaikka tilaa periaatteessa olisi ollut.
		 *
		 */
		
		float delay=0.025;	
		delay-=Clock.GetElapsedTime();
		if (delay>0)
		{
			sf::Sleep(delay);
		}
	}	
}
















