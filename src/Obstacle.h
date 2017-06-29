#pragma once
#include "Collidable.h"
#include "Grid.h"

/*
 * Obstacle Class.
 * Written by Bryce Summers on Jun.28.2017
 * 
 * Specifies Polygon obstacles that are meant to statically sit on the screen and repel dynamic moving objects, like letters.
 */

class Obstacle : public Collidable
{
public:
    Obstacle(ofPolyline bounds, Grid * grid);
    ~Obstacle();

    void draw();

private:
    ofPolyline pline;

public:
    // -- Methods from Collidable.

    virtual bool detect_collision_with_rectangle(ofRectangle rect);

    // Returns true iff this collidable is capable of movement.
    virtual bool isDynamic();

    // Updates this object's internal physics values to point 
    // it away from the direction of collision.
    virtual bool resolve_collision(ofVec3f direction);

    // Returns the axis aligned bounding box for this collidable object.
    // Assumed to be in standard form, which the position at its top left corner.
    virtual ofRectangle getBoundingBox();
};