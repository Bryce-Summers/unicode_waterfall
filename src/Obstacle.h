#pragma once

#include "Body.h"
#include "Grid.h"
#include "ConvexHull.h"

/*
 * Obstacle Class.
 * Written by Bryce Summers on Jun.28.2017
 * 
 * Specifies Polygon obstacles that are meant to statically sit on the screen and repel dynamic moving objects, like letters.
 */

class Obstacle : public Body
{
public:
    Obstacle(ofPolyline bounds, Grid * grid);
    ~Obstacle();

    void draw();

private:
    ConvexHull * collidable;
    ofPolyline pline;

    ofPolyline convexHull(ofPolyline pline);

public:

    virtual bool isDynamic(){ return true;}
    virtual Collidable * getCollidable(){return collidable;}
    virtual void updatePositionFromCollidable();

    // Dummy function, because obstacles are static.
    virtual void revertToPrevious();
};