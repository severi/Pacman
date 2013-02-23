/*
 *  aabb.h
 *  ServerClientTest
 *
 *  Created by Elmeri Poikolainen on 20.11.2011.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef AABB_H
#define AABB_H
struct AABB 
{
	float minx, maxx, miny, maxy;
};

bool AABBvsAABB (const AABB i, const AABB j);


#endif
