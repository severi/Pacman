#include <SFML/Graphics.hpp>
#include <iostream>
#include <SFML/Network.hpp>
#include <string>
#include "client.hh"
#include "gameobject.hh"
#include <sstream>
#include <SFML/Audio.hpp>
#include <map>

#define HEIGTH 650 
#define WIDTH 1200

enum directions {
	RIGHT,
	UP,
	LEFT,
	DOWN
};
//freimin liikuttaminen annetuilla parametreilla
void moveRect(sf::IntRect& rect, int x, int y)
{
	rect.Offset(-rect.Left, -rect.Top);
	rect.Offset(x,y); 
}

int main()
{
	
	int last=LEFT;
	int laskin=0;
	
	std::cout<<"Welcome to play Pacman\nWould you like to play or spectate?\nType p for play, and s for spectate: ";
	std::string valinta;
	
	int mode=1;
	bool t_or_s=false;
	while (!t_or_s)
	{
		std::cin>>valinta;
		std::cin.ignore(1000,'\n');
		std::cin.clear();
		
		if (valinta.length()==1 && valinta[0]=='p')
		{
			mode=0;
			t_or_s=true;
		}
		else if(valinta.length()==1 && valinta[0]=='s')
		{
			mode=1;
			t_or_s=true;
		}
		else {
			std::cout<<"Invalid input, choose again:\n";
		}
		
		
	}
	
	Client cl(mode);	// 0==pelaaja
	
	std::vector<GameObject> objektit;
	//kun peli ei ole alkanut luupataan ja kerätään listaa osallistuvista objekteista
	
	
	bool waitingForPlayers=true;
	while(waitingForPlayers)
	{
		//vastaanotetaan serveriltä lista uusista liittyneistä pelaajista
		std::vector<GameObject> GO_update = cl.receive();
		
		//luupataan vastaanotettu lista ja lisätään 
		for(std::vector<GameObject>::iterator update_itr = GO_update.begin(); update_itr!=GO_update.end(); update_itr++)	//GO_update-luuppi
		{
			
			if (update_itr->get_Id()== -1)
			{
				waitingForPlayers=false;
				continue;
			}
			
			objektit.push_back(*update_itr);
			
		}
		
	}
	std::map<int,std::string> playerList;
	playerList = cl.receivePlayerList();
	
	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGTH, 32), "Pac-man Vs.");
	
	//luodaan tyhjä mappi, jossa avain on gameobjektin id ja arvo on gameobjektin sprite
	std::map<int, sf::Sprite> sprites;
	//luodaan tyhjä mappi, jossa avain on gameobjectin id ja arvo on boolean joka kertoo piirretäänkö spriteä
	std::map<int, bool> spriteDrawCheckList;
	
	//kuva ikkunan iconille
	sf::Image image_icon;
	if (!image_icon.LoadFromFile("pacman_icon.png"))
	{
		std::cout<< "Couldn't open pacman_icon.png.\n";
	} 
	window.SetIcon(32,32,image_icon.GetPixelsPtr());
	//luodaan kuvat kaikista gameObjekteista
	sf::Image image_11;
	if (!image_11.LoadFromFile("pacman1.tga"))
	{
		std::cout<< "Couldn't open pacman1.tga.\n";
	}
	
	sf::Image image_12(30,30,sf::Color(1,100,255));
	
	sf::Image image_13;
	if(!image_13.LoadFromFile("fruit1.tga"))
	{
		std::cout<< "Couldn't open fruit1.tga.\n";
	}
	
	
	
	sf::Image image_red;
	if(!image_red.LoadFromFile("ghost_red.tga"))
	{
		std::cout<< "Couldn't open ghost_red.tga.\n";
	}
	
	
	sf::Image image_blue;
	if(!image_blue.LoadFromFile("ghost_blue.tga"))
	{
		std::cout<< "Couldn't open ghost_blue.tga.\n";
	}
	
	
	sf::Image image_yellow;
	if(!image_yellow.LoadFromFile("ghost_yellow.tga"))
	{
		std::cout<< "Couldn't open ghost_yellow.tga.\n";
	}
	
	sf::Image image_green;
	if(!image_green.LoadFromFile("ghost_green.tga"))
	{
		std::cout<< "Couldn't open ghost_green.tga.\n";
	}
	
	
	sf::Image image_15;
	if(!image_15.LoadFromFile("superFruit.tga"))
	{
		std::cout<< "Couldn't open superFruit.tga.\n";
	}
	
	
	sf::Image image_16(30,30,sf::Color(100,200,10));
	
	
	//luo freimit objekteista
	sf::IntRect frame_pacman(1,0,30,30);
	sf::IntRect frame_red(1,0,30,30);
	sf::IntRect frame_blue(1,0,30,30);
	sf::IntRect frame_yellow(1,0,30,30);
	sf::IntRect frame_green(1,0,30,30);
	
	
	//luo spritet objekteista
	for(std::vector<GameObject>::iterator i = objektit.begin(); i!=objektit.end();i++)
	{
		//id:n perusteella ladataan hahmon kuva ja luodaan siitä sprite, joka lisätään mappiin
		if ( i->get_Id() == PACMAN)
		{	//pacman
			//ladataan pacman
			image_15.SetSmooth(false);
			sf::Sprite sprite_11(image_11);
			sf::Vector2 <float>vec1 = sprite_11.GetSize();
			//asetetaan keskipiste rotatiota varten
			sprite_11.SetCenter(vec1.x/20, vec1.y/4);
			//lisätään mappiin
			sprite_11.SetX(45);
			sprite_11.SetY(45);
			sprites[i->get_Id()] = sprite_11;
			spriteDrawCheckList[i->get_Id()] = true;
			//player_iconille sprite
			sf::Sprite sprite_11_icon(image_11);		
		}
		//seinä
		if ( i->get_Id()>=WALL_MIN  && i->get_Id()<=WALL_MAX)
		{
			image_15.SetSmooth(false);
			sf::Sprite sprite_12(image_12);
			sf::Vector2 <float>vec2 = sprite_12.GetSize();
			sprite_12.Resize(30,30);
			sprite_12.SetCenter(vec2.x/2, vec2.y/2);
			sprites[i->get_Id()] = sprite_12;
			spriteDrawCheckList[i->get_Id()] = true;	
			//player_iconille sprite
			sf::Sprite sprite_12_icon(image_12);	
			
		}
		//hedelmät
		if(i->get_Id()>= FRUIT_MIN && i->get_Id()<= FRUIT_MAX)
		{
			image_15.SetSmooth(false);
			sf::Sprite sprite_13(image_13);
			sf::Vector2 <float>vec3 = sprite_13.GetSize();
			sprite_13.Resize(30,30);
			sprite_13.SetCenter(vec3.x/2, vec3.y/2);
			sprites[i->get_Id()] = sprite_13;	
			spriteDrawCheckList[i->get_Id()] = true;
		}
		//punainnen haamu
		if (i->get_Id() == GHOST_RED) {
			image_red.SetSmooth(false);
			sf::Sprite sprite_red(image_red);
			sf::Vector2 <float>vec4 = sprite_red.GetSize();
			//sprite_red.Resize(30,30);
			sprite_red.SetCenter(vec4.x/10, vec4.y/2);
			sprites[i->get_Id()] = sprite_red;	
			spriteDrawCheckList[i->get_Id()] = true;
		}
		//sininen haamu
		if (i->get_Id() == GHOST_BLUE) {
			image_blue.SetSmooth(false);
			sf::Sprite sprite_blue(image_blue);
			sf::Vector2 <float>vec5 = sprite_blue.GetSize();
			//sprite_blue.Resize(30,30);
			sprite_blue.SetCenter(vec5.x/10, vec5.y/2);
			sprites[i->get_Id()] = sprite_blue;	
			spriteDrawCheckList[i->get_Id()] = true;
		}
		//keltainen haamu
		if (i->get_Id() == GHOST_YELLOW) {
			image_yellow.SetSmooth(false);
			sf::Sprite sprite_yellow(image_yellow);
			sf::Vector2 <float>vec6 = sprite_yellow.GetSize();
			//sprite_yellow.Resize(30,30);
			sprite_yellow.SetCenter(vec6.x/10, vec6.y/2);
			sprites[i->get_Id()] = sprite_yellow;	
			spriteDrawCheckList[i->get_Id()] = true;
		}
		//vihreä haamu
		if (i->get_Id() == GHOST_GREEN) {
			image_green.SetSmooth(false);
			sf::Sprite sprite_green(image_green);
			sf::Vector2 <float>vec7 = sprite_green.GetSize();
			//sprite_green.Resize(30,30);
			sprite_green.SetCenter(vec7.x/10, vec7.y/2);
			sprites[i->get_Id()] = sprite_green;	
			spriteDrawCheckList[i->get_Id()] = true;
		}
		//terninaattori haamu debuggausta varten
		if (i->get_Id() == GHOST_TERMINATOR) {//ainoastaan debuggaukseen
			image_green.SetSmooth(false);
			sf::Sprite sprite_green(image_red);
			sf::IntRect frame_red(1,0,30,30);
			sprite_green.SetSubRect(frame_red);
			sf::Vector2 <float>vec7 = sprite_green.GetSize();
			//sprite_green.Resize(30,30);
			sprite_green.SetCenter(vec7.x/10, vec7.y/2);
			sprites[i->get_Id()] = sprite_green;	
			spriteDrawCheckList[i->get_Id()] = true;
		}
		
		//super-fruit
		if (i->get_Id() >= SUPER_FRUIT_MIN && i->get_Id() <= SUPER_FRUIT_MAX) {
			image_15.SetSmooth(false);
			sf::Sprite sprite_15(image_15);
			sf::Vector2 <float>vec5 = sprite_15.GetSize();
			sprite_15.Resize(30,30);
			
			sprite_15.SetCenter(vec5.x/2, vec5.y/2);
			sprites[i->get_Id()] = sprite_15;	
			spriteDrawCheckList[i->get_Id()] = true;
		}
		//teleport
		if (i->get_Id() >= TELEPORT_MIN && i->get_Id() <= TELEPORT_MAX) {
			image_16.SetSmooth(false);
			sf::Sprite sprite_16(image_16);
			sf::Vector2 <float>vec6 = sprite_16.GetSize();
			sprite_16.Resize(30,30);
			
			sprite_16.SetCenter(vec6.x/2, vec6.y/2);
			sprites[i->get_Id()] = sprite_16;	
			spriteDrawCheckList[i->get_Id()] = true;
		}		
	}
	//ladataan fontti
	sf::Font Pacfont;
	if (!Pacfont.LoadFromFile("PacFont.ttf", 16)) {
		{
			std::cout<< "Couldn't open PacFont.ttf.\n";
		}
		
	}
	//ladataan fontti numeroille
	sf::Font Namcofont;
	if (!Namcofont.LoadFromFile("namco.ttf", 16)) {
		std::cout<< "Couldn't open namco.ttf.\n";
	}
	//ladataan fontti pelaajine nimille
	sf::Font Namefont;
	if (!Namefont.LoadFromFile("namefont.ttf", 16)) {
		std::cout<< "Couldn't open namefont.ttf.\n";
	}
	//pelihahmojen tekstit
	sf::String object_player_list;
	object_player_list.SetColor(sf::Color(255,255,0));
	object_player_list.SetFont(Pacfont);
	object_player_list.SetX(700);
	object_player_list.SetY(170);
	object_player_list.SetSize(16);
	
	//palaajien nimet
	sf::String name_player_list;
	name_player_list.SetColor(sf::Color(255,255,0));
	name_player_list.SetFont(Namefont);
	name_player_list.SetX(900);
	name_player_list.SetY(170);
	name_player_list.SetSize(16);
	name_player_list.SetStyle(sf::String::Bold);
	std::string player_objects = "";
	std::string player_names = "";
	
	//spritet pelaajien ikoneille
	int player_icon_y = 0;
	sf::Image pacman;
	if (!pacman.LoadFromFile("pacman2.tga"))
	{
		std::cout<< "Couldn't open pacman2.tga.\n";
	}
	
	//pelaajien nimet ja hahmojen logot:
	std::vector<sf::Sprite> icon_sprites;
	//pacman
	if (playerList.find(1) != playerList.end()) {
		player_objects += "pacman\n\n\n";
		player_names+=playerList[1];
		player_names+="\n\n\n";
		sf::Sprite sprite_11_icon(pacman);
		sprite_11_icon.SetX(655);
		sprite_11_icon.SetY(170+player_icon_y);
		sprite_11_icon.Resize(30, 30);
		player_icon_y+=48;
		icon_sprites.push_back(sprite_11_icon);
		
	}
	//punainnen haamu
	if (playerList.find(2) != playerList.end()) {
		player_objects+= "ghost red\n\n\n";
		player_names+=playerList[2];
		std::cout<<playerList[2]<<" plays as ghost red\n";
		player_names+="\n\n\n";
		sf::Sprite sprite_red_icon(image_red);
		sf::IntRect frame_red(1,0,30,30);
		sprite_red_icon.SetSubRect(frame_red);
		sprite_red_icon.SetX(655);
		sprite_red_icon.SetY(170+player_icon_y);
		sprite_red_icon.Resize(30, 30);
		player_icon_y+=48;
		icon_sprites.push_back(sprite_red_icon);
	}
	//sininen haamu
	if (playerList.find(3) != playerList.end()) {
		player_objects+= "ghost blue\n\n\n";
		player_names+=playerList[3];
		player_names+="\n\n\n";
		sf::Sprite sprite_blue_icon(image_blue);
		sf::IntRect frame_blue(1,0,30,30);
		sprite_blue_icon.SetSubRect(frame_blue);
		sprite_blue_icon.SetX(655);
		sprite_blue_icon.SetY(170+player_icon_y);
		sprite_blue_icon.Resize(30, 30);
		player_icon_y+=48;
		icon_sprites.push_back(sprite_blue_icon);
		
	}
	//keltainen haamu
	if (playerList.find(4) != playerList.end()) {
		player_objects+= "ghost yellow\n\n\n";
		player_names+=playerList[4];
		player_names+="\n\n\n";
		sf::Sprite sprite_yellow_icon(image_yellow);
		sf::IntRect frame_yellow(1,0,30,30);
		sprite_yellow_icon.SetSubRect(frame_yellow);
		sprite_yellow_icon.SetX(655);
		sprite_yellow_icon.SetY(170+player_icon_y);
		sprite_yellow_icon.Resize(30, 30);
		player_icon_y+=48;
		icon_sprites.push_back(sprite_yellow_icon);
		
	}
	//greenki haamu
	if(playerList.find(5) != playerList.end() ) {
		player_objects+= "ghost green\n\n\n";
		player_names+=playerList[5];
		player_names+="\n\n\n";
		sf::Sprite sprite_green_icon(image_green);
		sf::IntRect frame_green(1,0,30,30);
		sprite_green_icon.SetSubRect(frame_green);
		sprite_green_icon.SetX(655);
		sprite_green_icon.SetY(170+player_icon_y);
		sprite_green_icon.Resize(30, 30);
		player_icon_y+=48;
		icon_sprites.push_back(sprite_green_icon);
	}
	
	
	
	object_player_list.SetText(player_objects);
	name_player_list.SetText(player_names);
	std::cout<<"player_objects: "<< player_objects<< " player_names: " << player_names<<std::endl;
	
	sf::String Score_text;
	Score_text.SetColor(sf::Color(255,255,0));
	Score_text.SetFont(Pacfont);
	Score_text.SetX(700);
	Score_text.SetY(20);
	Score_text.SetSize(16);
	//stringstream pisteiden näyttämistä varten
	std::stringstream ss;
	std::string score_string="pacman ";
	
	//pacmanin pisteet
	int gamescore=0;
	
	sf::String Score_num;
	Score_num.SetColor(sf::Color(255,255,0));
	Score_num.SetFont(Namcofont);
	Score_num.SetX(900);
	Score_num.SetY(20);
	Score_num.SetSize(16);
	std::stringstream ss_num;
	std::string num_string="0";
	
	//elämät
	sf::String Lives_text;
	Lives_text.SetColor(sf::Color(255,255,0));
	Lives_text.SetFont(Pacfont);
	Lives_text.SetX(700);
	Lives_text.SetY(70);
	Lives_text.SetSize(16);
	
	sf::String Lives_num;
	Lives_num.SetColor(sf::Color(255,255,0));
	Lives_num.SetFont(Namcofont);
	Lives_num.SetX(900);
	Lives_num.SetY(70);
	Lives_num.SetSize(16);
	std::stringstream ss_lives;
	std::string num_lives="3";
	int lives = 3;
	
	int stop_freeze = 0;
	int f_num_freeze = 0;
	int pacman_kuolee = 0;
	int frame_number = 0;
	bool gameContinues = true;
	
	//super_moden muuttujat
	//1)teksti
	sf::String Super_mode_text;
	Super_mode_text.SetColor(sf::Color(255,255,0));
	Super_mode_text.SetFont(Pacfont);
	Super_mode_text.SetX(700);
	Super_mode_text.SetY(120);
	Super_mode_text.SetSize(16);
	//2)aika:
	sf::String Super_mode_num;
	Super_mode_num.SetColor(sf::Color(255,255,0));
	Super_mode_num.SetFont(Namcofont);
	Super_mode_num.SetX(900);
	Super_mode_num.SetY(120);
	Super_mode_num.SetSize(16);
	std::string num_super_mode = "15";
	bool super_mode = false;
	sf::Clock super_clock;
	super_clock.Reset();
	int super_time = 0;
	
	//
	
	//äänet
	sf::Music sound_fruit;
	if (!sound_fruit.OpenFromFile("sound.ogg"))
	{
	    std::cout<< "Couldn't open sound.ogg.\n";
	}
	sound_fruit.SetLoop(false);
	
	sf::Music sound_pacman_dead;
	if (!sound_pacman_dead.OpenFromFile("sad_trombone.ogg")) 
	{
	    std::cout<< "Couldn't open sad_trombone.ogg.\n";
	}	
	sound_pacman_dead.SetLoop(false);	
	
	//ghostien kuolemaäänet muuttuvat
	sf::Music sound_ghost_eaten;
	if (!sound_ghost_eaten.OpenFromFile("pacman_eatghost.ogg"))
	{
	    std::cout<< "Couldn't open pacman_eatghost.ogg.\n";
	}	
	sf::Music sound_super_fruit;
	if (!sound_super_fruit.OpenFromFile("super_fruit.ogg"))
	{
	    std::cout<< "Couldn't open super_fruit.ogg.\n";
	}
	sound_super_fruit.SetLoop(false);
	
	sf::Music sound_ghost_wins;
	if (!sound_ghost_wins.OpenFromFile("ghosts_win.ogg"))
	{
	    std::cout<< "Couldn't open ghost_win.ogg.\n";
	}
	sound_ghost_wins.SetLoop(true);
	
	sf::Music sound_pacman_wins;
	if (!sound_pacman_wins.OpenFromFile("pacman_wins.ogg"))
	{
	    std::cout<< "Couldn't open pacman_wins.ogg.\n";
	}
	sf::Music game_music;
	game_music.SetVolume(75.f);
	if (!game_music.OpenFromFile("pacmantTheme.ogg")) 
	{
	    std::cout<< "Couldn't open pacmanTheme.ogg.\n";
	}
	
	
	game_music.SetLoop(true);
	game_music.Play();
	sound_pacman_wins.SetLoop(true);
	std::string game_over = "game over\n\n";
	
	//alustetaan spritet ikkunan ulkopuolelle enne pelin alkua
	for(std::map<int, sf::Sprite>::iterator it = sprites.begin(); it != sprites.end(); it++)
	{
		it->second.SetX(-200);
		it->second.SetY(-200);
	}
	
	//pääikkunaluuppi pääikkunaluuppi pääikkunaluuppi pääikkunaluuppi pääikkunaluuppi pääikkunaluuppi pääikkunaluuppi pääikkunaluuppi pääikkunaluuppi pääikkunaluuppi pääikkunaluuppi pääikkunaluuppi pääikkunaluuppi pääikkunaluuppi
	while(window.IsOpened() && gameContinues)
	{
		
		//super_moden kello:
		if(!super_mode)
		{
			super_clock.Reset();
			super_time = 0;	
		}
		super_time = super_clock.GetElapsedTime();
		if(super_time == SUPER_MODE_TIME)
		{
			super_time = 0;
			super_clock.Reset();
		}
		frame_number = (frame_number+1)%5;
		f_num_freeze = (f_num_freeze+1) % 5;
		if (f_num_freeze%5 == 0) {
			stop_freeze =1;
			window.SetFramerateLimit(0);
		}
		//Event_käskyt
		sf::Event event;
		while (window.GetEvent(event))
		{
			//sulkeminen
			if(event.Type == sf::Event::Closed)
			{
				window.Close();
			}
			
		}
		
		//Näppäinkomennot
		//suuntanuolet
		//left
		
		if (mode==0)
		{
			if (window.GetInput().IsKeyDown(sf::Key::Left))
			{
				
				if (last!=LEFT)
				{
					laskin=0;
				}
				if (laskin<2) 
				{
					cl.sendCommand(LEFT);
					laskin++;
					last=LEFT;
				}
			}
			//right
			else if (window.GetInput().IsKeyDown(sf::Key::Right))
			{
				if (last!=RIGHT) {
					laskin=0;
				}
				
				if (laskin<2) 
				{
					cl.sendCommand(RIGHT);
					laskin++;
					last=RIGHT;
				}
			}
			//up
			else if (window.GetInput().IsKeyDown(sf::Key::Up))
			{
				if (last!=UP) {
					laskin=0;
				}
				
				if (laskin<2) 
				{
					cl.sendCommand(UP);
					laskin++;
					last=UP;
				}
				
				
			}
			
			//down
			else if (window.GetInput().IsKeyDown(sf::Key::Down))
			{
				if (last!=DOWN) {
					laskin=0;
				}
				
				if (laskin<2) 
				{
					cl.sendCommand(DOWN);
					laskin++;
					last=LEFT;
				}
				
			}
		}
		
		//uusien sijaintien vastaanotto	
		std::vector<GameObject> GO_update = cl.receive();	
		
		//uusien sijaintien päivitys gameObjekti vektoriin
		
		for(std::vector<GameObject>::iterator objektit_itr = objektit.begin(); objektit_itr != objektit.end(); objektit_itr++)	//GO-luuppi
		{
			for(std::vector<GameObject>::iterator update_itr = GO_update.begin(); update_itr!=GO_update.end(); update_itr++)	//GO_update-luuppi
			{
				if(objektit_itr->get_Id() == update_itr->get_Id())	//jos löydetään muuttunut sijainti, niin..
				{
					
					if (update_itr->get_Id() == PACMAN && update_itr->get_state()==DEAD )		//pelin lopetusehto
					{
						game_over += "team ghost won";
						std::cout<<"Team Ghost won the game!"<<std::endl;
						game_music.Stop();
						sound_ghost_wins.Play();
						gameContinues = false;
						break;
					}
					if (update_itr->get_Id() == PACMAN && update_itr->get_state()==PACMANHASWON) {
						game_over += "pacman won";
						std::cout<<"PACMAN HAS WON WOHOOO!"<<std::endl;
						game_music.Stop();
						sound_pacman_wins.Play();
						gameContinues = false;
						break;
					}
					
					
					if (update_itr->get_Id() == PACMAN)		//pelin lopetusehto
					{
						gamescore = update_itr->get_score();
						if(update_itr->get_state() == SUPER_MODE)
						{
							super_mode = true;
						}
						else {
							super_mode = false;
						}
						
						if (update_itr->get_state()>=DEAD && update_itr->get_state()<FREEZE) {
							lives = update_itr->get_state()-2;
						}
						
					}
					
					
					
					if (update_itr->get_state() == DEAD ) {
						spriteDrawCheckList[update_itr->get_Id()] = false;
						//soittaa äänen kun syödään fruit
						if(update_itr->get_Id()>=FRUIT_MIN && update_itr->get_Id()<=FRUIT_MAX)
						{
							sound_fruit.Play();
						}
						if(update_itr->get_Id()>=SUPER_FRUIT_MIN && update_itr->get_Id()<=SUPER_FRUIT_MAX)
						{
							super_clock.Reset();
							super_time = 0;	
							sound_super_fruit.Play();
						}
						if (update_itr->get_Id()>=GHOST_RED && update_itr->get_Id()<=GHOST_TERMINATOR) {
							sound_ghost_eaten.Play();
						}
						
					}
					//pacman kuolee -->FREEZE-tila, asettaa freeze_frame_numberin oikeaan kohtaan pacman kuvassa
					//asettaa frameratelimitin, jotta vain framet liikkuvat
					if (update_itr->get_Id() == PACMAN && update_itr->get_state() == FREEZE) {
						window.SetFramerateLimit(8);
						pacman_kuolee = 1;
						stop_freeze = 0;
						f_num_freeze = 0;
						sound_pacman_dead.Play();
					}
					if (update_itr->get_Id() == PACMAN && update_itr->get_state() != FREEZE && 
						update_itr->get_state() != SUPER_MODE) {
						window.SetFramerateLimit(0);
						pacman_kuolee = 0;
						
					}					
					
					//vanha suunta
					int current = objektit_itr->get_current_direction();
					//uusi suunta
					int uusi = update_itr->get_current_direction();
					//vanhan suunnan ja uuden suunnan erotus
					int erotus = current - uusi;
					
					
					//päivitetään koordinaattien sijainnit
					objektit_itr->change_current_direction(update_itr->get_current_direction());
					objektit_itr->change_next_direction(update_itr->get_next_direction());
					
					sprites[update_itr->get_Id()].SetX(update_itr->get_x_coordinate());
					sprites[update_itr->get_Id()].SetY(update_itr->get_y_coordinate());
					
					
					//naaman suuntaaminen
					
					if (update_itr->get_Id() == PACMAN)
					{	
						switch (erotus)
						{
								
							case 1:
							{
								sprites[update_itr->get_Id()].Rotate(-90);
								break;
							}
							case -1:
							{
								sprites[update_itr->get_Id()].Rotate(90);
								break;
							}
							case 2:
							{
								sprites[update_itr->get_Id()].Rotate(180);
								break;
							}
							case -2:
							{
								sprites[update_itr->get_Id()].Rotate(180);
								break;
							}
							case 3:
							{
								sprites[update_itr->get_Id()].Rotate(90);
								break;
							}
							case -3:
							{
								sprites[update_itr->get_Id()].Rotate(-90);
								break;
							}
						}
					}		
				}
				
				
			}
		}
		
		
		// Clear screen
		window.Clear();
		
		//Luupataan spritelista läpi, ja piirretään ne eli kaikki pallurat, haamut, pacman..
		for(std::map<int,sf::Sprite>::iterator i = sprites.begin(); i!=sprites.end();i++)
		{
			// Display sprite in our window
			if ( !spriteDrawCheckList[i->first]) {
				continue;
			}
			if(i->first == PACMAN)
			{
				//perusanimaatio
				if (!pacman_kuolee) 
				{
					if(!super_mode) moveRect(frame_pacman, frame_number*(frame_pacman.GetWidth()+1),0); //move rectangle
					else moveRect(frame_pacman, frame_number*(frame_pacman.GetWidth()+1),30); //move rectangle
					i->second.SetSubRect(frame_pacman); //bind rectangle to sprite-sheet to cut out correct frame 
					
				}
				//kuolema-animaatio
				else {
					if (stop_freeze!=1) {
						
						moveRect(frame_pacman, (f_num_freeze*(frame_pacman.GetWidth()+1)+150),0);	//siirtää kuvaframessa kuolema_animaation kohdalle --> +2
						i->second.SetSubRect(frame_pacman);
					}
				}
			}
			//haamumaatiot
			if (i->first == GHOST_RED) {
				moveRect(frame_red, frame_number*(frame_red.GetWidth()+1),0); //move rectangle
				i->second.SetSubRect(frame_red);
			}
			if (i->first == GHOST_BLUE) {
				moveRect(frame_blue, frame_number*(frame_blue.GetWidth()+1),0); //move rectangle
				i->second.SetSubRect(frame_blue);
			}
			if (i->first == GHOST_YELLOW) {
				moveRect(frame_yellow, frame_number*(frame_yellow.GetWidth()+1),0); //move rectangle
				i->second.SetSubRect(frame_yellow);
			}
			if (i->first == GHOST_GREEN) {
				moveRect(frame_green, frame_number*(frame_green.GetWidth()+1),0); //move rectangle
				i->second.SetSubRect(frame_green);
			}
			window.Draw((i->second));
		}
		//piirretään iconspritet
		for (std::vector<sf::Sprite>::iterator iter = icon_sprites.begin(); iter!=icon_sprites.end(); iter++) {
			window.Draw(*iter);
		}
		
		//asetetaan pistetilanne
		Score_text.SetText(score_string);
		ss<<gamescore<<std::endl;
		ss>>num_string;
		Score_num.SetText(num_string);
		window.Draw(Score_text);
		window.Draw(Score_num);
		
		//elämät
		Lives_text.SetText("Lives: ");
		ss<<lives<<std::endl;
		ss>>num_lives;
		Lives_num.SetText(num_lives);
		window.Draw(Lives_num);
		window.Draw(Lives_text);
		
		//super_moden aika:
		if(super_mode)
		{
			Super_mode_text.SetText("Super Mode!");
			ss<<(SUPER_MODE_TIME-super_time)<<std::endl;
			ss>>num_super_mode;
			Super_mode_num.SetText(num_super_mode);
			window.Draw(Super_mode_text);
			window.Draw(Super_mode_num);
		}
		window.Draw(object_player_list);
		window.Draw(name_player_list);
		// Display window contents on screen
		
		window.Display();
		//sound_fruit.Play();
		
		
	} 
	//gameOver fontti
	sf::Font PacfontBig;
	if (!PacfontBig.LoadFromFile("PacFont.ttf", 60)) {
		//TODO
	}
	//gameOver teksti
	sf::String Game_over_text;
	Game_over_text.SetColor(sf::Color(255,0,0));
	Game_over_text.SetFont(PacfontBig);
	Game_over_text.SetX(50);
	Game_over_text.SetY(300);
	Game_over_text.SetSize(60);
	Game_over_text.SetStyle(sf::String::Bold);
	Game_over_text.SetText(game_over);
	
	//gameover näkymä
	while (window.IsOpened()) {
		//Event_käskyt
		sf::Event event;
		while (window.GetEvent(event))
		{
			//sulkeminen
			if(event.Type == sf::Event::Closed)
			{
				window.Close();
			}
			
		}
		window.Clear();
		//spritet//Luupataan spritelista läpi, ja piirretään ne eli kaikki pallurat, haamut, pacman..
		for(std::map<int,sf::Sprite>::iterator i = sprites.begin(); i!=sprites.end();i++)
		{
			// Display sprite in our window
			if ( !spriteDrawCheckList[i->first]) {
				continue;
			}
			window.Draw((i->second));
		}
		//piirretään iconspritet
		for (std::vector<sf::Sprite>::iterator iter = icon_sprites.begin(); iter!=icon_sprites.end(); iter++) {
			window.Draw(*iter);
		}
		
		//asetetaan pistetilanne
		Score_text.SetText(score_string);
		ss<<gamescore<<std::endl;
		ss>>num_string;
		Score_num.SetText(num_string);
		window.Draw(Score_text);
		window.Draw(Score_num);
		
		//elämät
		Lives_text.SetText("Lives: ");
		ss<<lives<<std::endl;
		ss>>num_lives;
		Lives_num.SetText(num_lives);
		window.Draw(Lives_num);
		window.Draw(Lives_text);
		
		window.Draw(object_player_list);
		window.Draw(name_player_list);
		window.Draw(Game_over_text);
		
		window.Display();
		
	}
}







































