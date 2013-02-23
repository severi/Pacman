#include "aabb.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <SFML/Network.hpp>

/*
 *	Tarkistus, osuuko Gameobjekti toiseen.
 */
bool AABBvsAABB(const AABB i, const AABB j)
{
	sf::Rect<float> rec1(i.minx,i.miny,i.maxx,i.maxy);
	sf::Rect<float> rec2(j.minx,j.miny,j.maxx,j.maxy);
	return rec1.Intersects(rec2);
}
