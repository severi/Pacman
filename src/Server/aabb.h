#ifndef AABB_H
#define AABB_H

struct AABB 
{
	float minx, maxx, miny, maxy;
};

bool AABBvsAABB (const AABB i, const AABB j);


#endif