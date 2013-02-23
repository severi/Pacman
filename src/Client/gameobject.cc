#include <iostream>
#include <SFML/Network.hpp>
#include <string>
#include <map>
#include <utility>
#include "gameobject.hh"


enum directions {
	right,
	up,
	left,
	down
};

void GameObject::change_speed(int s)
{
	speed=s;
}

int GameObject::get_Id() const {
	return Id;  
}

float GameObject::get_x_coordinate() const
{
	return x_coordinate;
}
float GameObject::get_y_coordinate() const {
	return y_coordinate;
}
int GameObject::get_current_direction() const {
	return current_direction;
}
int GameObject::get_next_direction() const {
	return next_direction;
}
int GameObject::get_state() const {
	return state;
}
int GameObject::get_previousState() const {
	return previousState;
}
void GameObject::change_Id(int newId) {
	Id = newId;
	if (newId ==PACMAN) {
		speed = PACMAN_SPEED;
	}
	else {
		speed = GHOST_SPEED;
	}

}
void GameObject::change_x_coordinate(float x){
	x_coordinate = x;
}
void GameObject::change_y_coordinate(float y) {
	y_coordinate = y;
}
void GameObject::change_current_direction(int direction) {
	current_direction = direction;
}
void GameObject::change_next_direction(int direction) {
	next_direction = direction;
}
void GameObject::change_state(int newstate) {
	state = newstate;
}
void GameObject::change_previousState(int new_previousState) {
	previousState = new_previousState;
}

int GameObject::get_score() const
{
	return score;
}
int GameObject::update_score(int amount)
{
	score+=amount;
	return score;
}
int GameObject::change_score(int nScore)
{
	score=nScore;
	return score;
}


bool isDead(GameObject object)
{
	return ( object.get_state() == DEAD);
}

bool isWall(GameObject object)   // 4 seuraavaa pystyy "paketoimaan"
{
	if (isDead(object)) {
		return false;
	}
	return ( (object.get_Id()<=WALL_MAX) && (object.get_Id()>=WALL_MIN) );
	
}
bool isFruit(GameObject object)
{
	if (isDead(object)) {
		return false;
	}
	return ( (object.get_Id()<= FRUIT_MAX) && (object.get_Id()>= FRUIT_MIN) );
}
bool isGhost(GameObject object)
{
	if (isDead(object)) {
		return false;
	}
	return ( (object.get_Id() <= GHOST_TERMINATOR) && (object.get_Id() >= GHOST_RED) ) ;
}
bool isPacman(GameObject object)
{
	if (isDead(object)) {
		return false;
	}
	return ( (object.get_Id() == PACMAN) );
}
bool isSuperFruit( GameObject object)
{
	if (isDead(object)) {
		return false;
	}
	return ( (object.get_Id() <= SUPER_FRUIT_MAX) && (object.get_Id() >= SUPER_FRUIT_MIN ) );
}
bool isTeleport( GameObject object)
{
	if (isDead(object)) {
		return false;
	}
	return ( (object.get_Id() <= TELEPORT_MAX) && (object.get_Id() >= TELEPORT_MIN ) );
	
}





std::vector<GameObject> GameObject::move(float ElapsedTime, std::vector<GameObject> map, int x_max, int y_max)
{
	
	std::vector<GameObject> muutokset;
	
	
	float nextX = x_coordinate;
	float nextY = y_coordinate;
	
	float nextX2 = nextX;
	float nextY2 = nextY;
	
	switch (next_direction) 
	{
		case left:
			nextX -= speed * ElapsedTime;
			nextX2=nextX-5;
			break;
		case right:
			nextX += speed * ElapsedTime;
			nextX2=nextX+5;
			break;
		case up:
			nextY -= speed * ElapsedTime;
			nextY2 = nextY-5;
			break;
		case down:
			nextY += speed * ElapsedTime;
			nextY2 = nextY+5;
			break;
	}
	//ylimeneminen, jos jostain kumman syystä menee yli alueen esim yhteys lagaa, niin ei tee mitään-->tyhjä lista
	if(nextX>x_max || nextY>y_max || nextX<0 || nextY<0) return muutokset;
	
	//collision check
	
	bool allowMove=true;
	
	GameObject tmp(Id, nextX2, nextY2);
	
	AABB my_aabb = tmp.getAABB();
	
	for (std::vector<GameObject>::iterator iter = map.begin(); iter!=map.end(); iter++)
	{
		if ( AABBvsAABB(my_aabb, iter->getAABB()) && (  iter->get_Id()!=Id )) //jos törmäys tapahtuu
		{
			
			if (isWall(*iter) )
			{
				allowMove=false;
				break;
			}
			if (isGhost(*this) && isTeleport(*iter))
			{
				allowMove=false;
				break;
			}
			
		}
	}
	
		
	
	
	// tässä vaiheessa voi tehä collision checkin
	
	if (allowMove)
	{
		for (std::vector<GameObject>::iterator iter = map.begin(); iter!=map.end(); iter++)
		{
			if ( AABBvsAABB(my_aabb, iter->getAABB()) && (  iter->get_Id()!=Id )) //jos törmäys tapahtuu
			{
				if (isPacman(*this) && isFruit(*iter)) {

					iter->change_state(DEAD);
					score+=SCORE_FROM_FRUIT;
					muutokset.push_back(*this);
					muutokset.push_back(*iter);
					// lisää pistelaskuria
					break;
				}
				if (isPacman(*this) && isGhost(*iter)) 
				{
					if (this->get_state() != SUPER_MODE) //jos pacman kohtaa ghostin ja ei itse ole supermodessa
					{
						if (this->get_state() != FREEZE)
						{
							this->change_previousState(this->get_state()-1);
							this->change_state(FREEZE);
							muutokset.push_back(*this);
							return muutokset;
						}
					}
					//JOS PACMAN ==FREEZE, OWNAA GHOSTIN VAIKKA EI PITÄISI :O
					if (this->get_state()==FREEZE) 
					{
						return muutokset;
					}
					iter->change_state(DEAD);
					score+=SCORE_FROM_GHOST;
					muutokset.push_back(*this);
					muutokset.push_back(*iter);
					break;
				}
				if (isGhost(*this) && isPacman(*iter)) {
					if (iter->get_state() == SUPER_MODE) { //Jos ghost kohtaa pacmanin joka on supermodessa
						this->change_state(DEAD);
						iter->update_score(SCORE_FROM_GHOST);
						muutokset.push_back(*iter);
						return muutokset;
					}
					if (iter->get_state() != FREEZE) { //SELITÄ / DOCUMENTOI
						iter->change_previousState(iter->get_state()-1);//THIS->ITER TODO
						iter->change_state(FREEZE);
						muutokset.push_back(*iter);
					}
					if (iter->get_state() == FREEZE)
					{
						return muutokset;
					}//TAMA LAITETTIIN TODO
					break;
				}
				if (isPacman(*this) && isSuperFruit(*iter)) { 
					iter->change_state(DEAD);
					muutokset.push_back(*iter);
					score+=SCORE_FROM_SUPER;
					if (this->get_state() != 9) {
						this->change_previousState(this->get_state());
						this->change_state(SUPER_MODE);
					}
					break;
				}
				if (isTeleport(*iter)) { // Teleporttaus
					if (iter->get_Id() == TELEPORT_MIN) {
						this->change_x_coordinate(iter->get_state());
						this->change_y_coordinate(iter->get_previousState());
						muutokset.push_back(*this);
						return muutokset;
					}
					if (iter->get_Id() == TELEPORT_MIN +1) {
						this->change_x_coordinate(iter->get_state());
						this->change_y_coordinate(iter->get_previousState());
						muutokset.push_back(*this);
						return muutokset;
					}
				}
				
			}
			
		}
		
		current_direction = next_direction;
		x_coordinate = nextX;
		y_coordinate = nextY;
		muutokset.push_back(*this);
		
	}
	
	
	else {
		
		
		
		nextX = x_coordinate;
		nextY = y_coordinate;
		
		switch (current_direction) 
		{
			case left:
				nextX -= speed * ElapsedTime;
				break;
			case right:
				nextX += speed * ElapsedTime;
				break;
			case up:
				nextY -= speed * ElapsedTime;
				break;
			case down:
				nextY += speed * ElapsedTime;
				break;
		}
		
		
		//collision check
		
		allowMove=true;
		
		GameObject tmp(Id, nextX, nextY);
		
		AABB my_aabb = tmp.getAABB();
		
		
		
		for (std::vector<GameObject>::iterator iter = map.begin(); iter!=map.end(); iter++)
		{
			if ( AABBvsAABB(my_aabb, iter->getAABB()) && (  iter->get_Id()!=Id )) //jos törmäys tapahtuu
			{
				
				if (isWall(*iter) )
				{
					allowMove=false;
					break;
				}
				if (isGhost(*this) && isTeleport(*iter))
				{
					allowMove=false;
					break;
				}
				if (isPacman(*this) && isFruit(*iter)) {
					iter->change_state(DEAD);
					muutokset.push_back(*iter);
					// lisää pistelaskuria
					break;
				}
				if (isPacman(*this) && isGhost(*iter)) {
					if (this->get_state() != SUPER_MODE) { //jos pacman kohtaa ghostin ja ei itse ole supermodessa
						if (this->get_state() != FREEZE) {
							this->change_previousState(this->get_state()-1);
							this->change_state(FREEZE);
							muutokset.push_back(*this);
							return muutokset;
						}
					}
					//TAPPAA GHOSTIN VAIKA ITSE FREEZE? ALLOWMOVE FALSEKSI?
					if (this->get_state()==FREEZE) {
						return muutokset;
					}
					iter->change_state(DEAD);
					muutokset.push_back(*iter);
					break;
				}
				if (isGhost(*this) && isPacman(*iter)) {
					if (iter->get_state() == SUPER_MODE) { //Jos ghost kohtaa pacmanin joka on supermodessa,
						this->change_state(DEAD);
						muutokset.push_back(*iter);
						return muutokset;
					}
					if (iter->get_state() != FREEZE) { //SELITÄ / DOCUMENTOI
						iter->change_previousState(this->get_state()-1);
						iter->change_state(FREEZE);
						muutokset.push_back(*iter);
					}
					if (iter->get_state() == FREEZE)
					{
						return muutokset;
					}
					//TODO; NYT JOS SE OLIKIN FREEZE ANNETAANKO GHOSTIN LIIKKUA?
					break;
				}
				if (isPacman(*this) && isSuperFruit(*iter)) {
					iter->change_state(DEAD);
					muutokset.push_back(*iter);
					if (this->get_state() != 9) {
						this->change_previousState(this->get_state());
						this->change_state(SUPER_MODE);
					}
					break;
				}
				if (isTeleport(*iter)) { // Teleporttaus
					if (iter->get_Id() == TELEPORT_MIN) {
						this->change_x_coordinate(iter->get_state());
						this->change_y_coordinate(iter->get_previousState());
						muutokset.push_back(*this);
						return muutokset;
					}
					if (iter->get_Id() == TELEPORT_MIN +1) {
						this->change_x_coordinate(iter->get_state());
						this->change_y_coordinate(iter->get_previousState());
						muutokset.push_back(*this);
						return muutokset;
					}
				}
				
			}
			
		}
		
		// tässä vaiheessa voi tehä collision checkin
		
		if (allowMove)
		{
			
			//current_direction = next_direction;
			x_coordinate = nextX;
			y_coordinate = nextY;
			muutokset.push_back(*this);
			
		}
	}

	
	
	return muutokset;
	
}


AABB GameObject::getAABB()
{
	AABB aabb;
	aabb.minx = get_x_coordinate()-(float(TILE_SIDE/2));
	aabb.maxx = get_x_coordinate()+(float(TILE_SIDE/2));
	aabb.miny = get_y_coordinate()-(float(TILE_SIDE/2));
	aabb.maxy = get_y_coordinate()+(float(TILE_SIDE/2));
	return aabb;
}




 

sf::Packet& operator <<(sf::Packet& packet, const GameObject::GameObject& object)
{
	return packet << object.get_Id() << object.get_x_coordinate() << object.get_y_coordinate()<< object.get_current_direction()<< object.get_next_direction() << object.get_state() << object.get_score(); //lisätty
}

sf::Packet& operator >>(sf::Packet& packet, GameObject::GameObject& object)
{
	int idd; 
	float x , y;
	int c,n;
	int s, p;
	packet >> idd>> x >> y>>c>>n >> s >>p;
	object.change_Id(idd);
	object.change_x_coordinate(x);
	object.change_y_coordinate(y);
	object.change_current_direction(c);
	object.change_next_direction(n);
	object.change_state(s); // lisätty
	object.change_score(p);
	
	return packet;
}
