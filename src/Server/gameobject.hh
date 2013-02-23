#ifndef PROJECT_GAMEOBJECT
#define PROJECT_GAMEOBJECT

#include "aabb.h"
#include <iostream>
#include <SFML/Network.hpp>
#include <string>
#include <map>
#include <utility>

#define PACMAN 2011
#define GHOST_RED 2012
#define GHOST_BLUE 2013
#define GHOST_YELLOW 2014
#define GHOST_GREEN 2015
#define GHOST_TERMINATOR 2016
#define TELEPORT_MIN 90
#define TELEPORT_MAX 99
#define WALL_MIN 500
#define WALL_MAX 999
#define FRUIT_MIN 100
#define FRUIT_MAX 399
#define SUPER_FRUIT_MIN 400
#define SUPER_FRUIT_MAX 499
#define PACMAN_SCORE 1000
#define SCORE_START 0
#define DEAD 1
#define ALIVE 2
#define LIVES_1 3
#define LIVES_2 4
#define LIVES_3 5
#define FREEZE 6
#define SUPER_MODE 9
#define TILE_SIDE 30
#define PACMAN_SPEED 170
#define GHOST_SPEED 170
#define SCORE_FROM_GHOST 300
#define SCORE_FROM_FRUIT 10
#define SCORE_FROM_SUPER 50
#define FREEZE_TIME 3
#define SUPER_MODE_TIME 5
#define PACMANHASWON 20


class GameObject  {
	
	
public:
	GameObject(int ID, int x, int y) : Id(ID),x_coordinate(x), y_coordinate(y), current_direction(-1), next_direction(-1), score(0)
	{ 
		speed = 80;
		if (ID == PACMAN) {
			state = LIVES_3;
			speed = PACMAN_SPEED;
		}
		else {
			state = ALIVE;
			speed = GHOST_SPEED;
		}
		previousState = -1;
		
	}
	void change_speed(int s);
	int get_Id() const;
	float get_x_coordinate() const;
	float get_y_coordinate() const;
	int get_current_direction() const;
	int get_next_direction() const;
	int get_state() const; //lisätty
	int get_previousState() const; //lisätty
	void change_Id(int);
	void change_x_coordinate(float);
	void change_y_coordinate(float);
	std::vector<GameObject> move(float ElapsedTime, std::vector<GameObject> map,int x_max, int y_max);
	void change_current_direction(int direction);
	void change_next_direction(int direction);
	void change_state(int state); //lisätty
	void change_previousState(int previousState);
	int get_score() const;
	int update_score(int amount);
	int change_score(int nScore);
	AABB getAABB();
	
	
	
private:
	int Id;
	float x_coordinate;
	float y_coordinate;
	float size_x, size_y;
	float halfSize_x, halfSize_y;
	int speed;
	int current_direction;
	int next_direction;
	int state; //lisätty
	int previousState;
	int score;
	
};


bool isWall(GameObject);

sf::Packet& operator <<(sf::Packet& packet, const GameObject& object);

sf::Packet& operator >>(sf::Packet& packet, GameObject& object);

#endif
